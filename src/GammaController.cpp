#include "GammaController.h"

#include "Settings.h"

constexpr auto main_menu{ RE::MainMenu::MENU_NAME };
constexpr auto map_menu{ RE::MapMenu::MENU_NAME };
constexpr auto mist_menu{ RE::MistMenu::MENU_NAME };
constexpr auto loading_menu{ RE::LoadingMenu::MENU_NAME };

void GammaController::Init() noexcept
{
    const auto ini_settings{ RE::INIPrefSettingCollection::GetSingleton() };
    gamma_setting  = ini_settings->GetSetting("fGamma:Display"sv);
    original_gamma = gamma_setting->GetFloat();
    logger::debug("Original gamma: {:.2f}", original_gamma);

    const auto factory{ RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESGlobal>() };
    control_global        = factory->Create();
    control_global->type  = RE::TESGlobal::Type::kFloat;
    control_global->value = 1.0f;
    control_global->SetFormEditorID("gammaglobal");

    if (const auto& [map, map_lock]{ RE::TESForm::GetAllFormsByEditorID() }; map) {
        const RE::BSReadLockGuard guard{ map_lock };
        map->emplace(control_global->GetFormEditorID(), control_global);
    }

    if (!control_global)
        logger::error("ERROR: Failed to cache global");
    else
        logger::info("Cached global {} with value {}", control_global->GetFormEditorID(), control_global->value);

    ENBAPI::FindD3D11();
    if (ENBAPI::d3d11_handle) {
        logger::info("Found d3d11.dll. Attempting to link ENB functions...");
        ENBAPI::LinkENBFunctions();
        if (ENBAPI::linked_enb_functions) {
            logger::info("Linked ENB functions");
            const auto version{ ENBAPI::get_enb_version() };
            logger::info("Current ENB version: v0.{}", version);
            ENBAPI::set_enb_callback(ENBCallback);
            logger::info("Registered ENB callback");
            ENBParameter brightness;
            ENBAPI::get_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
            std::memcpy(&original_enb_brightness, &brightness.data, sizeof original_enb_brightness);
            logger::info("Cached ENB brightness: {:.2f}", original_enb_brightness);
        }
        else
            logger::info("ENB not found. Installing main update hook...");
    }
}

std::int32_t GammaController::Thunk()
{
    const auto result{ func() };

    frame_counter++;

    if (frame_counter <=> Settings::every_x_frames < 0)
        return result;

    frame_counter = 0;

    const auto current_gamma{ gamma_setting->GetFloat() };
    if (const auto ui{ RE::UI::GetSingleton() }) {
        if (ui->IsMenuOpen(main_menu) || ui->IsMenuOpen(map_menu) || ui->IsMenuOpen(mist_menu) || ui->IsMenuOpen(loading_menu)) {
            if (current_gamma <=> original_gamma != 0) {
                logger::debug("Open menu detected, resetting gamma");
                const RE::BSReadLockGuard guard{ lock };
                gamma_setting->data.f = original_gamma;
            }
            return result;
        }
    }
    if (control_global->value <=> 0.0f == 0) {
        if (current_gamma <=> original_gamma != 0) {
            logger::debug("Global set to 0, resetting gamma");
            const RE::BSReadLockGuard guard{ lock };
            gamma_setting->data.f = original_gamma;
        }
        return result;
    }
    if (const auto calendar{ RE::Calendar::GetSingleton() }) {
        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            const auto game_hour{ calendar->GetHour() };
            auto       trunc_time{ std::floor(game_hour * 100) / 100 };
            logger::debug("Current time: {:.2f}", trunc_time);
            logger::debug("Current gamma: {:.2f}", current_gamma);

            const auto* map{ &Settings::exterior_map };
            if (const auto parent_cell{ player->GetParentCell() }) {
                if (parent_cell->IsInteriorCell()) {
                    logger::debug("Player is in interior cell {}", parent_cell->GetName());
                    map = &Settings::interior_map;
                }
            }
            else
                logger::debug("Player is in exterior cell");

            if (!map->contains(trunc_time)) {
                logger::debug("{:.2f} not found in map, truncating...", trunc_time);
                trunc_time = std::floor(game_hour * 10) / 10;
                logger::debug("New trunc time: {:.2f}", trunc_time);
                if (!map->contains(trunc_time)) {
                    logger::debug("{:.2f} not found in map, truncating...", trunc_time);
                    trunc_time = std::floor(trunc_time);
                    logger::debug("New trunc time: {:.2f}", trunc_time);
                }
            }
            if (map->contains(trunc_time)) {
                if (const auto new_gamma{ map->at(trunc_time) }; current_gamma <=> new_gamma != 0) {
                    const RE::BSReadLockGuard guard{ lock };
                    gamma_setting->data.f = new_gamma;
                }
            }
        }
    }

    return result;
}

void WINAPI GammaController::ENBCallback(ENBCallbackType callback_type)
{
    if (callback_type <=> ENBCallbackType::BeginFrame == 0) {
        call_counter++;

        if (call_counter <=> Settings::every_x_frames < 0)
            return;

        call_counter = 0;

        ENBParameter brightness;
        float        current_brightness{};
        ENBAPI::get_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
        std::memcpy(&current_brightness, &brightness.data, sizeof current_brightness);

        if (control_global->value <=> 0.0f == 0) {
            if (current_brightness <=> original_enb_brightness != 0) {
                logger::debug("Global set to 0, resetting brightness...");
                std::memcpy(&brightness.data, &original_enb_brightness, sizeof original_enb_brightness);
                ENBAPI::set_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
            }
            return;
        }

        if (const auto calendar{ RE::Calendar::GetSingleton() }) {
            if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
                const auto game_hour{ calendar->GetHour() };
                auto       trunc_time{ std::floor(game_hour * 100) / 100 };
                logger::debug("Current time: {:.2f}", trunc_time);
                logger::debug("Current brightness: {:.2f}", current_brightness);

                const auto* map{ &Settings::exterior_map };
                if (const auto parent_cell{ player->GetParentCell() }) {
                    if (parent_cell->IsInteriorCell()) {
                        logger::debug("Player is in interior cell {}", parent_cell->GetName());
                        map = &Settings::interior_map;
                    }
                }
                else
                    logger::debug("Player is in exterior cell");

                if (!map->contains(trunc_time)) {
                    logger::debug("{:.2f} not found in map, truncating...", trunc_time);
                    trunc_time = std::floor(game_hour * 10) / 10;
                    logger::debug("New trunc time: {:.2f}", trunc_time);
                    if (!map->contains(trunc_time)) {
                        logger::debug("{:.2f} not found in map, truncating...", trunc_time);
                        trunc_time = std::floor(trunc_time);
                        logger::debug("New trunc time: {:.2f}", trunc_time);
                    }
                }
                const auto new_brightness{ map->at(trunc_time) };
                if (current_brightness <=> new_brightness != 0) {
                    std::memcpy(&brightness.data, &new_brightness, sizeof new_brightness);
                    ENBAPI::set_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
                    logger::debug("Set new brightness {:.2f} for {:.2f}", new_brightness, trunc_time);
                }
            }
        }
    }
}
