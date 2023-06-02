#pragma once

#include "Settings.h"

class GammaController {
protected:
    GammaController() = default;

public:
    GammaController(const GammaController&) = delete;
    GammaController(GammaController&&) = delete;
    GammaController& operator=(const GammaController&) = delete;
    GammaController& operator=(GammaController&&) = delete;

    static GammaController* GetSingleton();

    static std::int32_t OnFrameUpdate();

    inline static REL::Relocation<decltype(OnFrameUpdate)> _OnFrameUpdate;
    inline static Settings* settings = Settings::GetSingleton();
    inline static RE::INIPrefSettingCollection* ini_settings = RE::INIPrefSettingCollection::GetSingleton();
};
