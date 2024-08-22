#pragma once

namespace Hooks
{
    void Install() noexcept;

    class MainUpdate : public Singleton<MainUpdate>
    {
    public:
        static i32 Thunk() noexcept;

        inline static REL::Relocation<decltype(&Thunk)> func;

        inline static const REL::Relocation target{ RELOCATION_ID(35565, 36564), REL::Relocate(0x748, 0xc26, 0x7ee) };

        inline static const auto address{ target.address() };
    };
} // namespace Hooks
