#include "Hooks.h"

#include "GammaController.h"

namespace Hooks {
    void Install() {
        const REL::Relocation<std::uintptr_t> target{ REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xc26, 0x7ee) };
        stl::write_thunk_call<GammaController>(target.address());
        logger::info("Installed main update hook");
    }
}
