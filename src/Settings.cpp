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

    const auto settings = GetSingleton();

    CSimpleIniA::TNamesDepend keys;

    ini.GetAllKeys("General", keys);

    for (const auto& key : keys) 
        settings->hours_to_gammas.insert(
            std::pair(std::stof(key.pItem), std::stof(ini.GetValue("General", key.pItem))));

    logger::info("Loaded settings");
}
