#include "Hooks.h"

#include "GammaController.h"

namespace Hooks {
    void Install() {
        stl::write_thunk_call<GammaController>();
        logger::info("Installed main update hook");
    }
}
