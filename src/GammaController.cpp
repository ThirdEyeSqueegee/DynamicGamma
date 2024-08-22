#include "GammaController.h"

#include "Settings.h"

void GammaController::Init() noexcept
{
    const auto ini_settings{ RE::INIPrefSettingCollection::GetSingleton() };
    gamma_setting  = ini_settings->GetSetting("fGamma:Display");
    original_gamma = gamma_setting->GetFloat();
    logger::info("Original gamma: {:.2f}", original_gamma);

    const auto factory{ RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESGlobal>() };
    control_global        = factory->Create();
    control_global->type  = RE::TESGlobal::Type::kFloat;
    control_global->value = 1.0;
    control_global->SetFormEditorID("gammaglobal");

    if (const auto& [map, map_lock]{ RE::TESForm::GetAllFormsByEditorID() }; map) {
        const RE::BSWriteLockGuard guard{ map_lock };
        map->emplace(control_global->GetFormEditorID(), control_global);
    }

    if (!control_global) {
        logger::error("ERROR: Failed to cache global");
    }
    else {
        logger::info("Cached global {} with value {}", control_global->GetFormEditorID(), control_global->value);
    }
    logger::info("");
}

void WINAPI GammaController::ENBCallback(ENBCallbackType callback_type) noexcept
{
    if (callback_type == ENBCallbackType::BeginFrame) {
        if (is_procedural_correction_disabled) {
            return;
        }

        if (!cached_original_enb_brightness) {
            if (const auto procedural_correction{ ENBAPI::GetENBParameter<bool>("enbseries.ini", "COLORCORRECTION", "UseProceduralCorrection") };
                procedural_correction.has_value() && !procedural_correction.value())
            {
                logger::error("ERROR: Procedural color correction is disabled");
                is_procedural_correction_disabled = true;
            }
            else {
                if (const auto brightness{ ENBAPI::GetENBParameter<float>(nullptr, "ENBEFFECT.FX", "CC: Brightness") }; brightness.has_value()) {
                    original_enb_brightness = brightness.value();
                    logger::info("Cached ENB brightness: {:.2f}", original_enb_brightness);
                    cached_original_enb_brightness = true;
                }
                else {
                    logger::error("ERROR: Failed to cache ENB brightness");
                }
            }
        }

        const auto now{ std::chrono::steady_clock::now() };

        if (now - last_run <= std::chrono::milliseconds(Settings::refresh_rate)) {
            return;
        }

        last_run = now;

        const auto current_brightness_opt{ ENBAPI::GetENBParameter<float>(nullptr, "ENBEFFECT.FX", "CC: Brightness") };
        if (!current_brightness_opt.has_value()) {
            logger::error("ERROR: Failed to get current ENB brightness");
            return;
        }

        const auto current_brightness{ current_brightness_opt.value() };

        if (control_global->value == 0.0) {
            if (current_brightness != original_enb_brightness) {
                logger::debug("Global set to 0, resetting brightness...");
                if (ENBAPI::SetENBParameter(nullptr, "ENBEFFECT.FX", "CC: Brightness", original_enb_brightness)) {
                    logger::debug("Reset brightness to {:2f}", original_enb_brightness);
                }
                else {
                    logger::error("ERROR: Failed to reset ENB brightness");
                }
            }
            return;
        }

        if (const auto calendar{ RE::Calendar::GetSingleton() }) {
            if (const auto player{ RE::PlayerCharacter::GetSingleton() }; player->Is3DLoaded()) {
                const HourAndMinute hour_and_minute{ .hour = static_cast<u8>(calendar->GetHour()), .minute = static_cast<u8>(calendar->GetMinutes()) };
                logger::debug("Current time: {}", hour_and_minute);
                logger::debug("Current brightness: {:.2f}", current_brightness);

                auto map{ &Settings::exterior_map };
                auto keys{ &Settings::exterior_keys_sorted };
                if (const auto parent_cell{ player->GetParentCell() }) {
                    if (parent_cell->IsInteriorCell()) {
                        logger::debug("Player is in interior cell {}", parent_cell->GetName());
                        map  = &Settings::interior_map;
                        keys = &Settings::interior_keys_sorted;
                    }
                }
                else {
                    logger::debug("Player is in exterior cell");
                }

                if (const auto lower_bound{ std::ranges::lower_bound(*keys, hour_and_minute, pred) }; lower_bound != keys->end()) {
                    logger::debug("Found nearest key {} for {}", *lower_bound, hour_and_minute);
                    if (const auto new_brightness{ map->at(*lower_bound) }; current_brightness != new_brightness) {
                        logger::debug("Found brightness {:.2f} for key {}", new_brightness, *lower_bound);
                        if (ENBAPI::SetENBParameter(nullptr, "ENBEFFECT.FX", "CC: Brightness", new_brightness)) {
                            logger::debug("Set new brightness {:.2f} for {}", new_brightness, hour_and_minute);
                        }
                        else {
                            logger::error("ERROR: Failed to set ENB brightness");
                        }
                    }
                }
            }
        }
    }

    if (callback_type == ENBCallbackType::PreSave && cached_original_enb_brightness) {
        logger::info("Resetting ENB brightness...");
        if (ENBAPI::SetENBParameter(nullptr, "ENBEFFECT.FX", "CC: Brightness", original_enb_brightness)) {
            logger::info("Reset ENB brightness to original value: {:.2f}", original_enb_brightness);
        }
    }
}
