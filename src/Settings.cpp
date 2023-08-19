#include "Settings.h"

#include "SimpleIni.h"

void Settings::LoadSettings() {
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\DynamicGamma.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    }

    CSimpleIniA::TNamesDepend exterior_keys;
    CSimpleIniA::TNamesDepend interior_keys;

    ini.GetAllKeys("Exterior", exterior_keys);
    ini.GetAllKeys("Interior", interior_keys);

    for (const auto& key : exterior_keys)
        exterior_map.emplace(std::strtof(key.pItem, nullptr), static_cast<float>(ini.GetDoubleValue("Exterior", key.pItem)));
    for (const auto& key : interior_keys)
        interior_map.emplace(std::strtof(key.pItem, nullptr), static_cast<float>(ini.GetDoubleValue("Interior", key.pItem)));

    for (const auto& [k, v] : exterior_map)
        logger::info("Loaded exterior setting: {} {}", k, v);
    for (const auto& [k, v] : interior_map)
        logger::info("Loaded interior setting: {} {}", k, v);

    every_x_frames = static_cast<int>(ini.GetLongValue("General", "uEveryXFrames"));

    logger::info("Loaded settings");
    logger::info("\t{} exterior pairs", exterior_keys.size());
    logger::info("\t{} interior pairs", interior_keys.size());
    logger::info("\tuEveryXFrames = {}", every_x_frames);
}
