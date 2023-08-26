#pragma once

enum class ENBCallbackType : long {
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

enum class ENBParameterType : long {
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

class ENBParameter {
public:
    unsigned char    data[16]{};
    unsigned long    size{};
    ENBParameterType type{};
};

class ENBRenderInfo {
public:
    ID3D11Device*        d3d11_device{};
    ID3D11DeviceContext* d3d11_device_context{};
    IDXGISwapChain*      dxgi_swapchain{};
    DWORD                screen_x{};
    DWORD                screen_y{};
};

using ENBSDKVersion    = long (*)();
using ENBVersion       = long (*)();
using ENBCallback      = void(WINAPI*)(ENBCallbackType a_callback_type);
using ENBSetCallback   = void (*)(ENBCallback a_callback_func);
using ENBGetParameter  = BOOL (*)(const char* a_filename, const char* a_category, const char* a_keyname, ENBParameter* a_out);
using ENBSetParameter  = BOOL (*)(const char* a_filename, const char* a_category, const char* a_keyname, ENBParameter* a_in);
using ENBGetRenderInfo = ENBRenderInfo* (*)();

class ENBAPI : public Singleton<ENBAPI> {
public:
    inline static bool             linked_enb_functions{};
    inline static HMODULE          d3d11_handle{};
    inline static ENBSDKVersion    get_enb_sdk_version{};
    inline static ENBVersion       get_enb_version{};
    inline static ENBSetCallback   set_enb_callback{};
    inline static ENBGetParameter  get_enb_param{};
    inline static ENBSetParameter  set_enb_param{};
    inline static ENBGetRenderInfo get_enb_render_info{};

    static void FindD3D11() {
        d3d11_handle = GetModuleHandle(L"d3d11");
    }

    static void LinkENBFunctions() {
        bool found1{}, found2{}, found3{}, found4{}, found5{}, found6{};
        if (const auto handle1{ GetProcAddress(d3d11_handle, "ENBGetSDKVersion") }) {
            logger::info("Linked ENBGetSDKVersion");
            get_enb_sdk_version = reinterpret_cast<ENBSDKVersion>(handle1);
            found1              = true;
        }
        if (const auto handle2{ GetProcAddress(d3d11_handle, "ENBGetVersion") }) {
            logger::info("Linked ENBGetVersion");
            get_enb_version = reinterpret_cast<ENBVersion>(handle2);
            found2          = true;
        }
        if (const auto handle3{ GetProcAddress(d3d11_handle, "ENBSetCallbackFunction") }) {
            logger::info("Linked ENBSetCallbackFunction");
            set_enb_callback = reinterpret_cast<ENBSetCallback>(handle3);
            found3           = true;
        }
        if (const auto handle4{ GetProcAddress(d3d11_handle, "ENBGetParameter") }) {
            logger::info("Linked ENBGetParameter");
            get_enb_param = reinterpret_cast<ENBGetParameter>(handle4);
            found4        = true;
        }
        if (const auto handle5{ GetProcAddress(d3d11_handle, "ENBSetParameter") }) {
            logger::info("Linked ENBSetParameter");
            set_enb_param = reinterpret_cast<ENBSetParameter>(handle5);
            found5        = true;
        }
        if (const auto handle6{ GetProcAddress(d3d11_handle, "ENBGetRenderInfo") }) {
            logger::info("Linked ENBGetRenderInfo");
            get_enb_render_info = reinterpret_cast<ENBGetRenderInfo>(handle6);
            found6              = true;
        }
        if (found1 && found2 && found3 && found4 && found5 && found6) {
            linked_enb_functions = true;
        }
    }
};
