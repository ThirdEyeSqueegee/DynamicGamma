#pragma once

#include "ENBAPI.h"

class GammaController : public Singleton<GammaController>
{
public:
    static void Init() noexcept;

    static void WINAPI ENBCallback(ENBCallbackType callback_type) noexcept;

    inline static std::chrono::steady_clock::time_point last_run{};

    inline static float original_gamma{};

    inline static float original_enb_brightness{};

    inline static std::atomic_bool cached_original_enb_brightness{};

    inline static std::atomic_bool is_procedural_correction_disabled{};

    inline static RE::TESGlobal* control_global{};

    inline static RE::Setting* gamma_setting{};

    inline static RE::BSReadWriteLock lock{};
};
