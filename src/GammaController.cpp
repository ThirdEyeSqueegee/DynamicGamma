#include "GammaController.h"

#include "Settings.h"

std::int32_t GammaController::Thunk() {
    frame_counter++;

    if (frame_counter < Settings::every_x_frames)
        return func();

    frame_counter = 0;

    if (const auto ui = RE::UI::GetSingleton(); ui->IsApplicationMenuOpen() || ui->IsItemMenuOpen() || ui->IsModalMenuOpen()) {
        if (gamma_setting->GetFloat() != original_gamma) {
            logger::debug("Open menu detected, resetting gamma");
            gamma_setting->data.f = original_gamma;
        }

        return func();
    }

    if (control_global->value == 0.0f) {
        if (gamma_setting->GetFloat() != original_gamma) {
            logger::debug("Global set to 0, resetting gamma");
            gamma_setting->data.f = original_gamma;
        }

        return func();
    }

    if (const auto calendar = RE::Calendar::GetSingleton()) {
        const auto game_hour  = calendar->GetHour();
        const auto trunc_time = std::floor(game_hour * 100) / 100;
        logger::debug("Current time: {}", trunc_time);
        logger::debug("Current gamma: {}", gamma_setting->GetFloat());

        SetGlobal(trunc_time, game_hour, 10);
    }

    return func();
}

void GammaController::Init() {
    const auto ini_settings = RE::INIPrefSettingCollection::GetSingleton();

    gamma_setting  = ini_settings->GetSetting("fGamma:Display"sv);
    original_gamma = gamma_setting->GetFloat();
    logger::debug("Original gamma: {}", original_gamma);

    const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESGlobal>();

    control_global        = factory->Create();
    control_global->type  = RE::TESGlobal::Type::kFloat;
    control_global->value = 1.0f;

    control_global->SetFormEditorID("gammaglobal");

    const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
    RE::BSWriteLockGuard locker{ lock };
    if (map)
        map->emplace(control_global->GetFormEditorID(), control_global);

    if (!control_global)
        logger::error("ERROR: Failed to cache global");
    else
        logger::info("Cached global {} with value {}", control_global->GetFormEditorID(), control_global->value);

    ENBAPI::FindENB();
    if (ENBAPI::enb_found) {
        logger::info("Found ENB. Linking ENB functions...");
        ENBAPI::LinkENBFunctions();
        logger::info("Linked ENB functions");
        const auto version = ENBAPI::get_enb_version();
        logger::info("Current ENB version: v0.{}", version);
        ENBAPI::set_enb_callback(ENBCallback);
        logger::info("Registered ENB callback");
    }
}

void WINAPI GammaController::ENBCallback(ENBCallbackType callback_type) {
    if (callback_type == ENBCallbackType::BeginFrame) {
        call_counter++;

        if (call_counter < Settings::every_x_frames)
            return;

        call_counter = 0;

        if (const auto calendar = RE::Calendar::GetSingleton()) {
            const auto game_hour  = calendar->GetHour();
            const auto trunc_time = std::floor(game_hour * 100) / 100;
            SetGlobal(trunc_time, game_hour, 10);
        }
    }
}

void GammaController::SetGlobal(float a_time, const float a_gameHour, int a_factor) {
    if (const auto player = RE::PlayerCharacter::GetSingleton()) {
        const auto* map = &Settings::exterior_map;
        if (const auto parent_cell = player->GetParentCell()) {
            if (parent_cell->IsInteriorCell()) {
                logger::debug("Player is in interior cell {}", parent_cell->GetName());
                map = &Settings::interior_map;
            }
        } else {
            logger::debug("Player is in exterior cell");
            map = &Settings::exterior_map;
        }
        if (!map->contains(a_time)) {
            logger::debug("{} not found in map, truncating...", a_time);
            a_time = std::floor(a_gameHour * a_factor) / a_factor;
            logger::debug("New trunc time: {}", a_time);
            SetGlobal(a_time, a_gameHour, a_factor / 10);
        } else {
            const auto new_gamma = map->at(a_time);
            if (ENBAPI::enb_found) {
                ENBParameter brightness;
                ENBAPI::get_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
                std::memcpy(&brightness.data, &new_gamma, sizeof new_gamma);
                ENBAPI::set_enb_param(nullptr, "ENBEFFECT.FX", "CC: Brightness", &brightness);
                logger::debug("Set new brightness {} for {}", new_gamma, a_time);
            } else {
                gamma_setting->data.f = new_gamma;
                logger::debug("Set new gamma {} for {}", new_gamma, a_time);
            }
        }
    }
}
