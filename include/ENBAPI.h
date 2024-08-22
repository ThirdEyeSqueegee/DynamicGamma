#pragma once

enum struct ENBCallbackType : i32
{
    EndFrame   = 1,
    BeginFrame = 2,
    PreSave    = 3,
    PostLoad   = 4,
    OnInit     = 5,
    OnExit     = 6,
    PreReset   = 7,
    PostReset  = 8,
    ForceDWORD = 0x7fffffff
};

enum struct ENBParameterType : i32
{
    None       = 0,
    Float      = 1,
    Int        = 2,
    Hex        = 3,
    Bool       = 4,
    Color3     = 5,
    Color4     = 6,
    Vec3       = 7,
    ForceDWORD = 0x7fffffff
};

struct ENBParameter
{
    std::array<unsigned char, 16> data{};
    u32                           size{};
    ENBParameterType              type{};
};

using TENBSDKVersion   = i32 (*)();
using TENBVersion      = i32 (*)();
using TENBCallback     = void(WINAPI*)(ENBCallbackType a_callback_type);
using TENBSetCallback  = void (*)(TENBCallback a_callback_func);
using TENBGetParameter = REX::W32::BOOL (*)(const char* a_filename, const char* a_category, const char* a_keyname, ENBParameter* a_out);
using TENBSetParameter = REX::W32::BOOL (*)(const char* a_filename, const char* a_category, const char* a_keyname, ENBParameter* a_in);

template <typename T>
concept is_bool_or_float_v = std::disjunction_v<std::is_same<T, bool>, std::is_same<T, float>>;

class ENBAPI : public Singleton<ENBAPI>
{
public:
    inline static bool linked_enb_functions{};

    inline static REX::W32::HMODULE d3d11_handle{};

    inline static TENBSDKVersion get_enb_sdk_version{};

    inline static TENBVersion get_enb_version{};

    inline static TENBSetCallback set_enb_callback{};

    inline static TENBGetParameter get_enb_param{};

    inline static TENBSetParameter set_enb_param{};

    inline static std::once_flag set_enb_callback_flag{};

    static auto FindD3D11() noexcept { d3d11_handle = REX::W32::GetModuleHandleW(L"d3d11"); }

    static auto LinkENBFunctions() noexcept
    {
        bool found1{};
        bool found2{};
        bool found3{};
        bool found4{};
        bool found5{};
        if (const auto handle1{ REX::W32::GetProcAddress(d3d11_handle, "ENBGetSDKVersion") }) {
            logger::info("\tLinked ENBGetSDKVersion");
            get_enb_sdk_version = reinterpret_cast<TENBSDKVersion>(handle1);
            found1              = true;
        }
        if (const auto handle2{ REX::W32::GetProcAddress(d3d11_handle, "ENBGetVersion") }) {
            logger::info("\tLinked ENBGetVersion");
            get_enb_version = reinterpret_cast<TENBVersion>(handle2);
            found2          = true;
        }
        if (const auto handle3{ REX::W32::GetProcAddress(d3d11_handle, "ENBSetCallbackFunction") }) {
            logger::info("\tLinked ENBSetCallbackFunction");
            set_enb_callback = reinterpret_cast<TENBSetCallback>(handle3);
            found3           = true;
        }
        if (const auto handle4{ REX::W32::GetProcAddress(d3d11_handle, "ENBGetParameter") }) {
            logger::info("\tLinked ENBGetParameter");
            get_enb_param = reinterpret_cast<TENBGetParameter>(handle4);
            found4        = true;
        }
        if (const auto handle5{ REX::W32::GetProcAddress(d3d11_handle, "ENBSetParameter") }) {
            logger::info("\tLinked ENBSetParameter");
            set_enb_param = reinterpret_cast<TENBSetParameter>(handle5);
            found5        = true;
        }

        linked_enb_functions = found1 && found2 && found3 && found4 && found5;
    }

    static auto Init() noexcept
    {
        FindD3D11();

        if (d3d11_handle) {
            logger::info("Found d3d11.dll. Linking ENB functions...");

            LinkENBFunctions();
            if (linked_enb_functions) {
                logger::info("Linked ENB functions");
                logger::info("");

                const auto enb_version{ get_enb_version() };
                logger::info("Current ENB version: v0.{}", enb_version);
                logger::info("");
            }
            else {
                logger::info("ENB not found. Installing main update hook...");
            }
        }
    }

    template <typename T>
    static std::optional<T> GetENBParameter(const char* filename, const char* category, const char* key) noexcept
        requires is_bool_or_float_v<T>
    {
        ENBParameter param{};
        if constexpr (std::is_same_v<T, bool>) {
            REX::W32::BOOL value{};
            if (get_enb_param(filename, category, key, &param)) {
                std::memcpy(&value, &param.data, sizeof value);
                return value;
            }
        }
        else if constexpr (std::is_same_v<T, float>) {
            float value{};
            if (get_enb_param(filename, category, key, &param)) {
                std::memcpy(&value, &param.data, sizeof value);
                return value;
            }
        }
        return std::nullopt;
    }

    template <typename T>
    static bool SetENBParameter(const char* filename, const char* category, const char* key, T value) noexcept
        requires is_bool_or_float_v<T>
    {
        ENBParameter param{};
        param.size = sizeof value;
        std::memcpy(param.data.data(), &value, param.size);
        if constexpr (std::is_same_v<T, bool>) {
            param.type = ENBParameterType::Bool;
        }
        else if constexpr (std::is_same_v<T, float>) {
            param.type = ENBParameterType::Float;
        }
        return set_enb_param(filename, category, key, &param);
    }
};
