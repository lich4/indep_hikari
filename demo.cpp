#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#if LLVM_VERSION_MAJOR <= 15
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#endif
using namespace llvm;

#define PASSNAME          "MyPassDemo"

// ---------------- Legacy Pass ---------------- //
#if LLVM_VERSION_MAJOR <= 14
class MyPassDemoLegacy : public ModulePass {
public:
    static char ID;
    MyPassDemoLegacy() : ModulePass(ID) {}
    virtual bool runOnModule(Module& M) override {
        errs() << "MyPassDemoLegacy\n";
        return false;
    }
};
char MyPassDemoLegacy::ID = 0;
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, 
    [](const PassManagerBuilder &, legacy::PassManagerBase &PM) {
        PM.add(new MyPassDemoLegacy());
    }
);
#endif // LLVM_VERSION_MAJOR <= 14
// ---------------- Legacy Pass ---------------- //

// ---------------- New Pass ---------------- //
#if LLVM_VERSION_MAJOR <= 13
#define OptimizationLevel PassBuilder::OptimizationLevel
#endif

class MyPassDemo : public PassInfoMixin<MyPassDemo> {
public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        errs() << "MyPassDemo\n";
        return PreservedAnalyses::all();
    };
    static bool isRequired() { return true; }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = PASSNAME,
        .PluginVersion = "1.0",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM
#if LLVM_VERSION_MAJOR >= 12
                , OptimizationLevel Level
#endif
                ) {
                    MPM.addPass(MyPassDemo());
            });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager& MPM, ArrayRef<PassBuilder::PipelineElement>) {
                    MPM.addPass(MyPassDemo());
                    return true;
            });
        }
    };
}
// ---------------- New Pass ---------------- //

__attribute__((constructor)) void onInit() {
}

