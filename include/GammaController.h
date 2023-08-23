#pragma once

#include "ENBAPI.h"

class GammaController : public Singleton<GammaController> {
public:
    static std::int32_t Thunk();

    inline static REL::Relocation<decltype(&Thunk)> func;

    static void Init();

    static void WINAPI ENBCallback(ENBCallbackType callback_type);

    static void SetGlobal(float a_time, float a_gameHour, int a_factor);

    inline static REL::Relocation<std::uintptr_t> target{ REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xc26, 0x7ee) };

    inline static const auto address = target.address();

    inline static int            frame_counter = 0;
    inline static int            call_counter  = 0;
    inline static float          original_gamma;
    inline static RE::TESGlobal* control_global = nullptr;
    inline static RE::Setting*   gamma_setting  = nullptr;
    inline static ENBParameter   original_enb_gamma;
};
