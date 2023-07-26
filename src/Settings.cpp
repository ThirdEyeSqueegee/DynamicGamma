#include "Settings.h"

#include "SimpleIni.h"

Settings* Settings::GetSingleton() {
    static Settings singleton;
    return std::addressof(singleton);
}

void Settings::LoadSettings() {
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\DynamicGamma.ini)");

    debug_logging = ini.GetValue("Log", "Debug");

    if (debug_logging) {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    }

    CSimpleIniA::TNamesDepend exterior_keys;
    CSimpleIniA::TNamesDepend interior_keys;

    ini.GetAllKeys("Exterior", exterior_keys);
    ini.GetAllKeys("Interior", interior_keys);

    for (const auto& key : exterior_keys) exterior_map.emplace(std::stof(key.pItem), std::stof(ini.GetValue("Exterior", key.pItem)));
    for (const auto& key : interior_keys) interior_map.emplace(std::stof(key.pItem), std::stof(ini.GetValue("Interior", key.pItem)));

    for (const auto& [k, v] : exterior_map) logger::info("Loaded exterior setting: {} {}", k, v);
    for (const auto& [k, v] : interior_map) logger::info("Loaded interior setting: {} {}", k, v);

    every_x_frames = std::atoi(ini.GetValue("General", "uEveryXFrames"));

    logger::info("Loaded settings");
}
