#include "GammaController.h"

#include "Settings.h"

GammaController* GammaController::GetSingleton() {
    static GammaController singleton;
    return std::addressof(singleton);
}

std::int32_t GammaController::OnFrameUpdate() {
    if (const auto calendar = RE::Calendar::GetSingleton()) {
        const auto game_hour = calendar->GetHour();
        auto trunc_time = std::floor(game_hour * 100) / 100;
        const auto current_gamma = ini_settings->GetSetting("fGamma:Display");
        if (!settings->hours_to_gammas.contains(trunc_time)) {
            trunc_time = std::floor(game_hour * 10) / 10;
            if (const auto new_gamma = settings->hours_to_gammas.find(trunc_time)->second) {
                current_gamma->data.f = new_gamma;
                ini_settings->WriteSetting(current_gamma);
            }
        } else {
            if (const auto new_gamma = settings->hours_to_gammas.find(trunc_time)->second) {
                current_gamma->data.f = new_gamma;
                ini_settings->WriteSetting(current_gamma);
            }
        }
    }
    return _OnFrameUpdate();
}
