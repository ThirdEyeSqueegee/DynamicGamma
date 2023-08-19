#pragma once

class Settings : public Singleton<Settings> {
public:
    static void LoadSettings();

    inline static bool debug_logging = false;

    inline static std::unordered_map<float, float> exterior_map;
    inline static std::unordered_map<float, float> interior_map;
    inline static int every_x_frames = 0;
};
