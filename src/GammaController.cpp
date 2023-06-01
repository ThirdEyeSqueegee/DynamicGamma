#include "GammaController.h"

#include "Settings.h"

GammaController* GammaController::GetSingleton() {
    static GammaController singleton;
    return std::addressof(singleton);
}

void GammaController::OnFrameUpdate() {
    const auto game_hour = GetSingleton()->calendar->GetHour();
    const auto ini_settings = GetSingleton()->ini_settings;
    const auto settings = Settings::GetSingleton();

    const auto trunc_time = std::floor(game_hour * 100) / 100;

    const auto current_gamma = ini_settings->GetSetting("fGamma");

    logger::info("Time: {}, Gamma: {}", trunc_time, current_gamma->data.f);

    if (settings->hours_to_gammas.contains(trunc_time)) {
        current_gamma->data.f = settings->hours_to_gammas.find(trunc_time)->second;
        ini_settings->WriteSetting(current_gamma);
    }
}
