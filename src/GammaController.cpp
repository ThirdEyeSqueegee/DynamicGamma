#include "GammaController.h"

#include "Settings.h"

GammaController* GammaController::GetSingleton() {
    static GammaController singleton;
    return std::addressof(singleton);
}

std::int32_t GammaController::OnFrameUpdate() {
    if (const auto calendar = RE::Calendar::GetSingleton()) {
        const auto game_hour = calendar->GetHour();
        if (const auto ini_settings = RE::INIPrefSettingCollection::GetSingleton()) {
            const auto trunc_time = std::floor(game_hour * 100) / 100;
            const auto current_gamma = ini_settings->GetSetting("fGamma:Display");
            if (settings->hours_to_gammas.contains(trunc_time)) {
                if (const auto new_gamma = settings->hours_to_gammas[trunc_time]) {
                    current_gamma->data.f = new_gamma;
                    ini_settings->WriteSetting(current_gamma);
                }                
            }
        }
    }
    return _OnFrameUpdate();
}
