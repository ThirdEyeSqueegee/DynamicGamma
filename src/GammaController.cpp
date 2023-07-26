#include "GammaController.h"

#include "Settings.h"

GammaController* GammaController::GetSingleton() {
    static GammaController singleton;
    return std::addressof(singleton);
}

std::int32_t GammaController::OnFrameUpdate() {
    frame_counter++;

    if (frame_counter < Settings::every_x_frames)
        return _OnFrameUpdate();

    frame_counter = 0;

    const auto ini_settings = RE::INIPrefSettingCollection::GetSingleton();

    if (control_global->value == 0.0f) {
        if (const auto gamma = ini_settings->GetSetting("fGamma:Display"sv); gamma->GetFloat() != 1.0f)
            gamma->data.f = 1.0f;

        return _OnFrameUpdate();
    }

    if (const auto calendar = RE::Calendar::GetSingleton()) {
        const auto game_hour = calendar->GetHour();
        auto trunc_time = std::floor(game_hour * 100) / 100;
        logger::debug("Current time: {}", trunc_time);
        const auto current_gamma = ini_settings->GetSetting("fGamma:Display"sv);
        logger::debug("Current gamma: {}", current_gamma->GetFloat());
        if (const auto player = RE::PlayerCharacter::GetSingleton()) {
            auto& map = Settings::exterior_map;
            if (player->GetParentCell() && player->GetParentCell()->IsInteriorCell()) {
                logger::debug("Player is in interior cell");
                map = Settings::interior_map;
            }
            if (!map.contains(trunc_time)) {
                logger::debug("{} not found in map, truncating again", trunc_time);
                trunc_time = std::floor(game_hour * 10) / 10;
                logger::debug("New trunc time: {}", trunc_time);
                if (!map.contains(trunc_time)) {
                    logger::debug("{} not found in map, truncating again", trunc_time);
                    trunc_time = static_cast<float>(static_cast<int>(trunc_time));
                    logger::debug("New trunc time: {}", trunc_time);
                    if (const auto new_gamma = map.find(trunc_time); new_gamma != map.end()) {
                        current_gamma->data.f = new_gamma->second;
                        logger::debug("Set new gamma {} for {}", new_gamma->second, trunc_time);
                    }
                } else {
                    if (const auto new_gamma = map.find(trunc_time); new_gamma != map.end()) {
                        current_gamma->data.f = new_gamma->second;
                        logger::debug("Set new gamma {} for {}", new_gamma->second, trunc_time);
                    }
                }
            } else {
                if (const auto new_gamma = map.find(trunc_time); new_gamma != map.end()) {
                    current_gamma->data.f = new_gamma->second;
                    logger::debug("Set new gamma {} for {}", new_gamma->second, trunc_time);
                }
            }
        }
    }

    return _OnFrameUpdate();
}

void GammaController::InitGlobal() {
    const auto handler = RE::TESDataHandler::GetSingleton();
    control_global = handler->LookupForm<RE::TESGlobal>(0x10ab31, "Skyrim.esm"sv);
    control_global->type = RE::TESGlobal::Type::kFloat;
    control_global->value = 1.0f;
    if (control_global == nullptr)
        logger::error("Failed to cache global");
    else
        logger::info("Cached global {} with value {}", control_global->GetFormEditorID(), control_global->value);
}
