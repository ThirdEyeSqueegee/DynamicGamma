#pragma once

class GammaController {
protected:
    GammaController() = default;
    ~GammaController() = default;

public:
    GammaController(const GammaController&) = delete;
    GammaController(GammaController&&) = delete;
    GammaController& operator=(const GammaController&) = delete;
    GammaController& operator=(GammaController&&) = delete;

    static GammaController* GetSingleton();

    static std::int32_t OnFrameUpdate();
    inline static REL::Relocation<decltype(&OnFrameUpdate)> _OnFrameUpdate;

    inline static int frame_counter = 0;
};
