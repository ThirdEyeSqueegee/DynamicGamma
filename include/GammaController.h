#pragma once

#include "ENBAPI.h"

class GammaController : public Singleton<GammaController>
{
public:
    static void Init() noexcept;

    static std::int32_t Thunk();

    static void WINAPI ENBCallback(ENBCallbackType callback_type);

    inline static REL::Relocation<decltype(&Thunk)> func;

    inline static REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(35565, 36564), REL::Relocate(0x748, 0xc26, 0x7ee) };

    inline static const auto address{ target.address() };

    inline static int                 frame_counter{};
    inline static int                 call_counter{};
    inline static float               original_gamma{};
    inline static float               original_enb_brightness{};
    inline static RE::TESGlobal*      control_global{};
    inline static RE::Setting*        gamma_setting{};
    inline static RE::BSReadWriteLock lock{};
};
