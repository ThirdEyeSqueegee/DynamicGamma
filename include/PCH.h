#pragma once

/* +++++++++++++++++++++++++ C++23 Standard Library +++++++++++++++++++++++++ */

// Concepts library
#include <concepts>

// Coroutines library
#include <coroutine>

// Utilities library
#include <any>
#include <bitset>
#include <chrono>
#include <compare>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <expected>
#include <functional>
#include <initializer_list>
#include <optional>
#include <source_location>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <version>

// Dynamic memory management
#include <memory>
#include <memory_resource>
#include <new>
#include <scoped_allocator>

// Numeric limits
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <cstdint>
#include <limits>
#include <stdfloat>

// Error handling
#include <cassert>
#include <cerrno>
#include <exception>
#include <stacktrace>
#include <stdexcept>
#include <system_error>

// Strings library
#include <cctype>
#include <charconv>
#include <cstring>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <string>
#include <string_view>

// Containers library
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <mdspan>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Iterators library
#include <iterator>

// Ranges library
#include <ranges>

// Algorithms library
#include <algorithm>
#include <execution>

// Numerics library
#include <bit>
#include <cfenv>
#include <cmath>
#include <complex>
#include <numbers>
#include <numeric>
#include <random>
#include <ratio>
#include <valarray>

// Localization library
#include <clocale>
#include <locale>

// Input/output library
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <ostream>
#include <print>
#include <spanstream>
#include <sstream>
#include <streambuf>
#include <syncstream>

// Filesystem library
#include <filesystem>

// Regular Expressions library
#include <regex>

// Atomic Operations library
#include <atomic>

// Thread support library
#include <barrier>
#include <condition_variable>
#include <future>
#include <latch>
#include <mutex>
#include <semaphore>
#include <shared_mutex>
#include <stop_token>
#include <thread>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <REX/W32.h>
#include <SKSE/SKSE.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

using namespace std::literals;
using namespace REL::literals;

namespace logger = SKSE::log;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

template <typename T>
class Singleton
{
protected:
    constexpr Singleton() noexcept  = default;
    constexpr ~Singleton() noexcept = default;

public:
    constexpr Singleton(const Singleton&)      = delete;
    constexpr Singleton(Singleton&&)           = delete;
    constexpr auto operator=(const Singleton&) = delete;
    constexpr auto operator=(Singleton&&)      = delete;

    [[nodiscard]] static constexpr auto GetSingleton() noexcept
    {
        static T singleton;
        return std::addressof(singleton);
    }
};

template <typename TDerived, typename TEvent>
class EventSingleton : public RE::BSTEventSink<TEvent>
{
protected:
    constexpr EventSingleton() noexcept           = default;
    constexpr ~EventSingleton() noexcept override = default;

public:
    constexpr EventSingleton(const EventSingleton&) = delete;
    constexpr EventSingleton(EventSingleton&&)      = delete;
    constexpr auto operator=(const EventSingleton&) = delete;
    constexpr auto operator=(EventSingleton&&)      = delete;

    [[nodiscard]] static constexpr auto GetSingleton() noexcept
    {
        static TDerived singleton;
        return std::addressof(singleton);
    }

    static constexpr auto Register() noexcept
    {
        using TEventSource = RE::BSTEventSource<TEvent>;

        const std::string dirty_name{ typeid(TEvent).name() };
        const std::regex  p{ "class |struct |RE::|SKSE::| * __ptr64" };
        const auto        name{ std::regex_replace(dirty_name, p, "") };

        if constexpr (std::is_base_of_v<TEventSource, RE::BSInputDeviceManager>) {
            const auto manager{ RE::BSInputDeviceManager::GetSingleton() };
            manager->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_base_of_v<TEventSource, RE::UI>) {
            const auto ui{ RE::UI::GetSingleton() };
            ui->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::ActionEvent>) {
            SKSE::GetActionEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::CameraEvent>) {
            SKSE::GetCameraEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::CrosshairRefEvent>) {
            SKSE::GetCrosshairRefEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::ModCallbackEvent>) {
            SKSE::GetModCallbackEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::NiNodeUpdateEvent>) {
            SKSE::GetNiNodeUpdateEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        else if constexpr (std::is_base_of_v<TEventSource, RE::ScriptEventSourceHolder>) {
            const auto holder{ RE::ScriptEventSourceHolder::GetSingleton() };
            holder->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            logger::info("");
            return;
        }
        const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
        SKSE::stl::report_and_fail(fmt::format("{}: Failed to register {} handler", plugin->GetName(), name));
    }
};

namespace stl
{
    using namespace SKSE::stl;

    template <typename T, std::size_t Size = 5>
    constexpr auto write_thunk_call(const std::uintptr_t a_address) noexcept
    {
        SKSE::AllocTrampoline(14);
        auto& trampoline{ SKSE::GetTrampoline() };
        T::func = trampoline.write_call<Size>(a_address, T::Thunk);
    }

    template <typename T, std::size_t Size = 5>
    constexpr auto write_thunk_call() noexcept
    {
        write_thunk_call<T, Size>(T::address);
    }

    template <typename T>
    constexpr auto write_vfunc(const REL::VariantID variant_id) noexcept
    {
        REL::Relocation vtbl{ variant_id };
        T::func = vtbl.write_vfunc(T::idx, T::Thunk);
    }

    template <typename TDest, typename TSource>
    constexpr auto write_vfunc(const std::size_t a_vtableIdx = 0) noexcept
    {
        write_vfunc<TSource>(TDest::VTABLE[a_vtableIdx]);
    }

    template <typename T, std::size_t Size = 5>
    constexpr auto write_thunk_jump(const std::uintptr_t a_src) noexcept
    {
        SKSE::AllocTrampoline(14);
        auto& trampoline{ SKSE::GetTrampoline() };
        T::func = trampoline.write_branch<Size>(a_src, T::Thunk);
    }

    namespace detail
    {
        template <typename>
        struct is_chrono_duration : std::false_type
        {
        };

        template <typename Rep, typename Period>
        struct is_chrono_duration<std::chrono::duration<Rep, Period>> : std::true_type
        {
        };

        template <typename T>
        concept is_duration = is_chrono_duration<T>::value;
    } // namespace detail

    auto add_thread_task(const std::function<void()>& a_fn, const detail::is_duration auto a_wait_for) noexcept
    {
        std::jthread([=] {
            std::this_thread::sleep_for(a_wait_for);
            SKSE::GetTaskInterface()->AddTask(a_fn);
        }).detach();
    }
} // namespace stl
