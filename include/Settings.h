#pragma once

class Settings {
protected:
    Settings() = default;
    ~Settings() = default;

public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings& operator=(Settings&&) = delete;

    static Settings* GetSingleton();
    static void LoadSettings();

    inline static bool debug_logging = false;

    inline static std::unordered_map<float, float> exterior_map;
    inline static std::unordered_map<float, float> interior_map;
    inline static int every_x_frames = 0;
};
