#pragma once

class Settings {
protected:
    Settings() = default;

public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings& operator=(Settings&&) = delete;

    static Settings* GetSingleton();
    static void LoadSettings();

    std::unordered_map<float, float> hours_to_gammas;
};
