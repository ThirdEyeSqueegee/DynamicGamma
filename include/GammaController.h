#pragma once

class GammaController {
protected:
    GammaController() = default;

public:
    GammaController(const GammaController&) = delete;
    GammaController(GammaController&&) = delete;
    GammaController& operator=(const GammaController&) = delete;
    GammaController& operator=(GammaController&&) = delete;

    static GammaController* GetSingleton();

    static void OnFrameUpdate();

    RE::Calendar* calendar = RE::Calendar::GetSingleton();
    RE::INISettingCollection* ini_settings = RE::INISettingCollection::GetSingleton();
};