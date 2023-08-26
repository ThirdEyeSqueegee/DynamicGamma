#include "GammaController.h"

#include "Settings.h"

std::int32_t GammaController::Thunk() {
    const auto result{ func() };

    frame_counter++;

    if (frame_counter <=> Settings::every_x_frames < 0)
        return result;

    frame_counter = 0;

    const auto gamma{ gamma_setting->GetFloat() };
    if (const auto ui{ RE::UI::GetSingleton() }) {
        if (ui->IsApplicationMenuOpen() || ui->IsItemMenuOpen() || ui->IsModalMenuOpen()) {
            if (gamma <=> original_gamma != 0) {
                logger::debug("Open menu detected, resetting gamma");
                gamma_setting->data.f = original_gamma;
            }
            return result;
        }
    }
    if (control_global->value <=> 0.0f == 0) {
        if (gamma <=> original_gamma != 0) {
            logger::debug("Global set to 0, resetting gamma");
            gamma_setting->data.f = original_gamma;
        }
        return result;
    }
    if (const auto calendar{ RE::Calendar::GetSingleton() }) {
        const auto game_hour{ calendar->GetHour() };
        auto       trunc_time{ std::floor(game_hour * 100) / 100 };
        logger::debug("Current time: {}", trunc_time);
        logger::debug("Current gamma: {}", gamma);
        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            const auto* map{ &Settings::exterior_map };
            if (const auto parent_cell{ player->GetParentCell() }) {
                if (parent_cell->IsInteriorCell()) {
                    logger::debug("Player is in interior cell {}", parent_cell->GetName());
                    map = &Settings::interior_map;
                }
            } else {
                logger::debug("Player is in exterior cell");
                map = &Settings::exterior_map;
            }
            if (!map->contains(trunc_time)) {
                logger::debug("{} not found in map, truncating...", trunc_time);
                trunc_time = std::floor(game_hour * 10) / 10;
                logger::debug("New trunc time: {}", trunc_time);
                if (map->contains(trunc_time)) {
                    const auto new_gamma{ map->at(trunc_time) };
                    gamma_setting->data.f = new_gamma;
                    logger::debug("Set new gamma {} for {}", new_gamma, trunc_time);
                }
            } else {
                const auto new_gamma{ map->at(trunc_time) };
                gamma_setting->data.f = new_gamma;
                logger::debug("Set new gamma {} for {}", new_gamma, trunc_time);
            }
        }
    }

    return result;
}

void GammaController::Init() {
    const auto ini_settings{ RE::INIPrefSettingCollection::GetSingleton() };
    gamma_setting  = ini_settings->GetSetting("fGamma:Display"sv);
    original_gamma = gamma_setting->GetFloat();
    logger::debug("Original gamma: {}", original_gamma);

    const auto factory{ RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESGlobal>() };
    control_global        = factory->Create();
    control_global->type  = RE::TESGlobal::Type::kFloat;
    control_global->value = 1.0f;
    control_global->SetFormEditorID("gammaglobal");

    const auto& [map, lock]{ RE::TESForm::GetAllFormsByEditorID() };
    if (map) {
        lock.get().LockForWrite();
        lock.get().LockForRead();
        map->emplace(control_global->GetFormEditorID(), control_global);
        lock.get().UnlockForWrite();
        lock.get().UnlockForRead();
    }

    if (!control_global)
        logger::error("ERROR: Failed to cache global");
    else
        logger::info("Cached global {} with value {:.2f}", control_global->GetFormEditorID(), control_global->value);

    ENBAPI::FindD3D11();
    if (ENBAPI::d3d11_handle) {
        logger::info("Found d3d11.dll. Attempting to link ENB functions...");
        if (ENBAPI::LinkENBFunctions()) {
            logger::info("Linked ENB functions");
            const auto version{ ENBAPI::get_enb_version() };
            logger::info("Current ENB version: v0.{}", version);
            ENBAPI::set_enb_callback(ENBCallback);
            logger::info("Registered ENB callback");
            ENBParameter brightness;
            ENBAPI::get_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
            std::memcpy(&original_enb_brightness, &brightness.data, sizeof original_enb_brightness);
            logger::info("Cached ENB brightness: {:.2f}", original_enb_brightness);
        } else
            logger::info("ENB not found. Installing main update hook...");
    }
}

void WINAPI GammaController::ENBCallback(ENBCallbackType callback_type) {
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
            const auto game_hour{ calendar->GetHour() };
            auto       trunc_time{ std::floor(game_hour * 100) / 100 };
            logger::debug("Current time: {}", trunc_time);
            logger::debug("Current brightness: {}", current_brightness);
            if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
                const auto* map{ &Settings::exterior_map };
                if (const auto parent_cell{ player->GetParentCell() }) {
                    if (parent_cell->IsInteriorCell()) {
                        logger::debug("Player is in interior cell {}", parent_cell->GetName());
                        map = &Settings::interior_map;
                    }
                } else {
                    logger::debug("Player is in exterior cell");
                    map = &Settings::exterior_map;
                }
                if (!map->contains(trunc_time)) {
                    logger::debug("{} not found in map, truncating...", trunc_time);
                    trunc_time = std::floor(game_hour * 10) / 10;
                    logger::debug("New trunc time: {}", trunc_time);
                    if (map->contains(trunc_time)) {
                        const auto new_gamma{ map->at(trunc_time) };
                        std::memcpy(&brightness.data, &new_gamma, sizeof new_gamma);
                        ENBAPI::set_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
                        logger::debug("Set new brightness {} for {}", new_gamma, trunc_time);
                    }
                } else {
                    const auto new_gamma{ map->at(trunc_time) };
                    std::memcpy(&brightness.data, &new_gamma, sizeof new_gamma);
                    ENBAPI::set_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
                    logger::debug("Set new brightness {} for {}", new_gamma, trunc_time);
                }
            }
        }
    }
}
