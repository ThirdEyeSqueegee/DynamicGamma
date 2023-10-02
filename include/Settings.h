#pragma once

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;

    inline static bool debug_logging{};

    inline static int                              every_x_frames{};
    inline static std::unordered_map<float, float> exterior_map;
    inline static std::unordered_map<float, float> interior_map;
};
