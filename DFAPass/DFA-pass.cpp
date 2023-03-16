#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <string>

//--------------------------------------------------------------------------------

using namespace llvm;

class DFAPass: public PassInfoMixin<DFAPass> {
    public:
        DFAPass() {}

        PreservedAnalyses run(Module &module, ModuleAnalysisManager &module_analysis_manager) {
            LLVMContext &llvm_context = module.getContext();

            IntegerType *llvm_int32_t   = Type::getInt32Ty(llvm_context);
            IntegerType *llvm_int64_t   = Type::getInt64Ty(llvm_context);
            PointerType *llvm_int8ptr_t = Type::getInt8PtrTy(llvm_context);
            Type        *llvm_void_t    = Type::getVoidTy(llvm_context);
            IRBuilder<> builder(llvm_context);

//--------------------------------------------------------------------------------

            FunctionType *open_close_log_func_t = 
                FunctionType::get(llvm_void_t, {llvm_void_t}, false);

            FunctionType *instr_logger = 
                FunctionType::get(llvm_void_t, {llvm_int64_t, llvm_int64_t}, false);

//--------------------------------------------------------------------------------

            FunctionCallee open_log_func = 
                module.getOrInsertFunction("open_log", open_close_log_func_t);
            
            FunctionCallee close_log_func = 
                module.getOrInsertFunction("close_log", open_close_log_func_t);
            
            FunctionCallee logger_func = 
                module.getOrInsertFunction("instruction_logger", instr_logger);

//--------------------------------------------------------------------------------
            
            draw_def_use_chain(module);

//--------------------------------------------------------------------------------
            
            for (Function& function : module) {

                if (function.isDeclaration())
                    continue;

                if (function.getName() == "main") {
                    // insert open log function
                    builder.SetInsertPoint(&*function.getEntryBlock().getFirstInsertionPt());
                    builder.CreateCall(open_log_func);

                    // insert close log function
                    for (auto& basic_block : function) {
                        for (auto& instr : basic_block) {
                            if (dyn_cast<ReturnInst>(&instr)) {
                                builder.SetInsertPoint(&instr);
                                builder.CreateCall(close_log_func);
                            }
                        }
                    }
                }

                for (auto& arg : function.args()) {
                    if (arg.getType()->isIntegerTy()) {
                        Value* arg_ptr = 
                            ConstantInt::get(llvm_int64_t, reinterpret_cast<uint64_t>(&arg));

                        builder.SetInsertPoint(&*function.getEntryBlock().getFirstInsertionPt());
                        builder.CreateCall(logger_func, {arg_ptr, &arg});
                    }
                }

                for (auto& basic_block : function) {
                    for (auto& instr : basic_block) {
                        if (instr.getType()->isIntegerTy()) {
                            Value* instr_ptr = 
                                ConstantInt::get(llvm_int64_t, reinterpret_cast<uint64_t>(&instr));

                            builder.SetInsertPoint(instr.getNextNode());
                            builder.CreateCall(logger_func, {instr_ptr, &instr});
                        }
                    }
                }
            }

            return PreservedAnalyses::all();
        }
    
    private:        
        void draw_def_use_chain(Module& module) {

            std::set<Instruction*> dumped;

            std::error_code EC;
            raw_fd_ostream dot_file(module.getName().str() + "_def-use_dump.dot", EC); 

            dot_file << "digraph {\n rankdir = TB;\n node[shape=record; style=filled]\n";

            for (auto& func : module) {
                if (func.isDeclaration())
                    continue;
                std::stringstream cluster_basename_ss;
                cluster_basename_ss << "cluster_" << reinterpret_cast<int64_t>(&func);
                
                std::string cluster_basename = cluster_basename_ss.str();

                dot_file << "subgraph " << cluster_basename + " {\n"
                        << "label=\" FUNCTION " + func.getName() << "\" ";
                
                dot_file << "subgraph " << cluster_basename + "_args {\n"
                         << "label=\" ARGUMENTS \"";
                
                for (auto arg_it = func.arg_begin(); arg_it != func.arg_end(); ++arg_it) {
                    dump_value(dot_file, *arg_it);
                    if((arg_it + 1) != func.arg_end())
                        dump_connection(dot_file, arg_it, arg_it + 1, "style=invis");
                }
                                
                dot_file << "}\n";

                dot_file << "subgraph " << cluster_basename + "_body {\n"
                         << "label=\" BODY \"";

                for (auto& basic_block : func) {
                    for (auto& instr : basic_block) {
                        dump_value(dot_file, instr);
                        if (auto next_instr = instr.getNextNode())
                            dump_connection(dot_file, &instr, next_instr, "weight=1000");

                        for (auto user : instr.users()) {
                                dump_value(dot_file, *user);
                                dump_connection(dot_file, user, &instr, "label=\"user\" fontcolor=maroon color=maroon");
                        }
                    }
                }
                
                dot_file << "}\n";
                dot_file << "}\n";

                for (auto& arg : func.args()) {
                   for (auto user : arg.users()) {
                       dump_connection(dot_file, user, &arg, "label=\"user\" fontcolor=maroon color=maroon");
                   } 
                }
            }
                
            dot_file << "\n}";
            dot_file.close();
        }
  
        void dump_value(raw_fd_ostream& dot_file, Value& val) {
                dot_file << "node_" << reinterpret_cast<int64_t>(&val) << "[" 
                    << "label=\"";
                val.print(dot_file);
                dot_file << " | VALUE=~~" << reinterpret_cast<int64_t>(&val) << "\"];\n";
        }

        void dump_connection(raw_fd_ostream& dot_file, const void* from, const void* to, std::string attributes="") {
            attributes = "[ " + attributes + " dir=\"forward\" ]";
            dot_file << "node_" << reinterpret_cast<int64_t>(from) << " -> " 
                     << "node_" << reinterpret_cast<int64_t>(to) << attributes << "\n";
        }
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "Simple-DFA-plugin", "v2.0",
            [](PassBuilder &PB) {
                using OptimizationLevel = typename PassBuilder::OptimizationLevel;
                PB.registerPipelineStartEPCallback(
                    [](ModulePassManager &MPM, OptimizationLevel O0) { 
                        MPM.addPass(DFAPass()); 
                    });
            }};
}