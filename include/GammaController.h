#pragma once

class GammaController : public Singleton<GammaController> {
public:
    static std::int32_t thunk();
    inline static REL::Relocation<decltype(&thunk)> func;

    inline static int frame_counter = 0;
    inline static RE::TESGlobal* control_global = nullptr;
    inline static RE::Setting* gamma_setting = nullptr;
    inline static float original_gamma;

    static void Init();

    static void SetGlobal(float a_time, float a_gameHour, int a_factor);
};
