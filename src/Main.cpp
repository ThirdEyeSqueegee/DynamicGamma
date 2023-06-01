#include "SKSE/Interfaces.h"
#include "Logging.h"
#include "Hooks.h"


SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    InitializeLogging();
    const auto* plugin = SKSE::PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    logger::info("{} {} is loading...", plugin->GetName(), version);
    Init(skse);
    SKSE::AllocTrampoline(32);
    Hooks::InstallHook();

    logger::info("{} has finished loading.", plugin->GetName());
    return true;
}
