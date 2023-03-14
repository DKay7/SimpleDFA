#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include <vector>

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

            FunctionType *function_logger = 
                FunctionType::get(llvm_void_t, {llvm_int64_t, llvm_int64_t}, true);

//--------------------------------------------------------------------------------

            FunctionCallee open_log_func = 
                module.getOrInsertFunction("open_log", open_close_log_func_t);
            
            FunctionCallee close_log_func = 
                module.getOrInsertFunction("close_log", open_close_log_func_t);
            
            FunctionCallee logger_func = 
                module.getOrInsertFunction("function_logger", function_logger);

//--------------------------------------------------------------------------------

            for (Function& function : module) {

                if (function.isDeclaration())
                    continue;

                if (function.getName() == "main") {
                    builder.SetInsertPoint(&*function.getEntryBlock().getFirstInsertionPt());
                    builder.CreateCall(open_log_func);

                    for (auto& basic_block : function)
                    for (auto& instr : basic_block) {
                        if (dyn_cast<ReturnInst>(&instr)) {
                            builder.SetInsertPoint(&instr);
                            builder.CreateCall(close_log_func);
                        }
                    }

                    continue;
                }

                std::vector<Value*> logger_args;


                Value* func_ptr = ConstantInt::get(llvm_int64_t, reinterpret_cast<uint64_t>(&function));
                Value* num_args = ConstantInt::get(llvm_int64_t, function.arg_size());
                logger_args.push_back(func_ptr);
                logger_args.push_back(num_args);

                errs() << "PROCESSING " << function.getName() << "\n";                
                for(auto arg = function.arg_begin(); arg != function.arg_end(); ++arg) {
                    // arg->dump();
                    // arg->getType()->dump();
                    if(arg->getType()->isIntegerTy()) {
                        // errs() << ci->getValue() << "\n";
                        logger_args.push_back(arg);
                    }
                }
                
                builder.SetInsertPoint(&*function.getEntryBlock().getFirstInsertionPt());
                builder.CreateCall(logger_func, logger_args);
            }

            return PreservedAnalyses::all();
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