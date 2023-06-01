#include "Hooks.h"

#include "GammaController.h"

namespace Hooks {
    void InstallHook() {
        auto& trampoline = SKSE::GetTrampoline();
        GammaController::GetSingleton()->_OnFrameUpdate = trampoline.write_call<5>(
            On_Update_Hook.address(), GammaController::OnFrameUpdate);

        logger::info("Installed hook");
    }
}
