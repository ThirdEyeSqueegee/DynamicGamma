#include "Hooks.h"

#include "GammaController.h"

namespace Hooks {
    void InstallHook() {
        auto& trampoline = SKSE::GetTrampoline();

        REL::Relocation<std::uintptr_t> func{REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xc26, 0x7ee)};

        GammaController::_OnFrameUpdate = trampoline.write_call<5>(func.address(), GammaController::OnFrameUpdate);

        logger::info("Installed hook");
    }
}
