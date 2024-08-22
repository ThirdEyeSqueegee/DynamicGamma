#pragma once

#include "ankerl/unordered_dense.h"

class HourAndMinute
{
public:
    u8 hour{};

    u8 minute{};

    constexpr bool operator==(const HourAndMinute& other) const noexcept = default;

    constexpr bool operator<(const HourAndMinute& other) const noexcept
    {
        if (hour == other.hour) {
            return minute < other.minute;
        }
        return hour < other.hour;
    }
};

static constexpr auto pred{ [](const HourAndMinute x, const HourAndMinute y) {
    if (x.hour == y.hour) {
        return x.minute < y.minute;
    }
    return x.hour < y.hour;
} };

template <>
struct ankerl::unordered_dense::hash<HourAndMinute>
{
    using is_avalanching = void;

    [[nodiscard]] auto operator()(const HourAndMinute& value) const noexcept
    {
        static_assert(std::has_unique_object_representations_v<HourAndMinute>);
        return detail::wyhash::hash(&value, sizeof value);
    }
};

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;

    static HourAndMinute GetHourAndMinute(std::string s) noexcept;

    inline static bool debug_logging{};

    inline static u32 refresh_rate{};

    inline static ankerl::unordered_dense::map<HourAndMinute, float> exterior_map;

    inline static ankerl::unordered_dense::map<HourAndMinute, float> interior_map;

    inline static std::vector<HourAndMinute> exterior_keys_sorted;

    inline static std::vector<HourAndMinute> interior_keys_sorted;
};

inline auto format_as(const HourAndMinute& hour_and_minute) noexcept
{
    return fmt::format("{:02}:{:02}", hour_and_minute.hour, hour_and_minute.minute);
}
