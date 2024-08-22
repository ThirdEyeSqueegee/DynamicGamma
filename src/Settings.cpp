#include "Settings.h"

HourAndMinute Settings::GetHourAndMinute(std::string s) noexcept
{
    if (s.length() != 4) {
        logger::error("ERROR: Invalid time format: {}", s);
        return {};
    }
    return { .hour = static_cast<u8>(std::stoul(s.substr(0, 2))), .minute = static_cast<u8>(std::stoul(s.substr(2))) };
}

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\DynamicGamma.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    CSimpleIniA::TNamesDepend exterior_keys;
    CSimpleIniA::TNamesDepend interior_keys;

    ini.GetAllKeys("Exterior", exterior_keys);
    ini.GetAllKeys("Interior", interior_keys);

    for (const auto& key : exterior_keys) {
        const auto hour_and_minute{ GetHourAndMinute(key.pItem) };
        exterior_keys_sorted.emplace_back(hour_and_minute);
        exterior_map.emplace(hour_and_minute, static_cast<float>(ini.GetDoubleValue("Exterior", key.pItem)));
    }
    for (const auto& key : interior_keys) {
        const auto hour_and_minute{ GetHourAndMinute(key.pItem) };
        interior_keys_sorted.emplace_back(hour_and_minute);
        interior_map.emplace(hour_and_minute, static_cast<float>(ini.GetDoubleValue("Interior", key.pItem)));
    }

    std::sort(std::execution::par, exterior_keys_sorted.begin(), exterior_keys_sorted.end());
    std::sort(std::execution::par, interior_keys_sorted.begin(), interior_keys_sorted.end());

    refresh_rate = static_cast<u32>(ini.GetLongValue("General", "uRefreshRate"));

    logger::info("Loaded settings");
    logger::info("\t{} exterior pairs", exterior_keys.size());
    logger::info("\t{} interior pairs", interior_keys.size());
    logger::info("\tuRefreshRate = {}", refresh_rate);
    logger::info("");
}
