#include "GammaController.h"
#include "Hooks.h"
#include "Logging.h"
#include "Settings.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Settings::LoadSettings();
        ENBAPI::Init();
        GammaController::Init();
        if (!ENBAPI::linked_enb_functions) {
            Hooks::Install();
        }
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
        if (ENBAPI::linked_enb_functions) {
            std::call_once(ENBAPI::set_enb_callback_flag, ENBAPI::set_enb_callback, GammaController::ENBCallback);
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", name, version);

    Init(skse);

    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }

    logger::info("{} has finished loading.", name);
    logger::info("");

    return true;
}
