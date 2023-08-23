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
    unsigned char    data[16] = { 0 };
    unsigned long    size     = 0;
    ENBParameterType type     = ENBParameterType::None;
};

class ENBRenderInfo {
public:
    ID3D11Device*        d3d11_device         = nullptr;
    ID3D11DeviceContext* d3d11_device_context = nullptr;
    IDXGISwapChain*      dxgi_swapchain       = nullptr;
    DWORD                screen_x             = 0;
    DWORD                screen_y             = 0;
};

using enbsdkversion_t    = long (*)();
using enbversion_t       = long (*)();
using enbcallback_t      = void(WINAPI*)(ENBCallbackType a_callback_type);
using enbsetcallback_t   = void (*)(enbcallback_t a_callback_func);
using enbgetparam_t      = BOOL (*)(const char* a_filename, const char* a_category, const char* a_keyname, ENBParameter* a_out);
using enbsetparam_t      = BOOL (*)(const char* a_filename, const char* a_category, const char* a_keyname, ENBParameter* a_in);
using enbgetrenderinfo_t = ENBRenderInfo* (*)();

class ENBAPI : public Singleton<ENBAPI> {
public:
    inline static bool               linked_enb_functions = false;
    inline static HMODULE            d3d11_handle         = nullptr;
    inline static enbsdkversion_t    get_enb_sdk_version  = nullptr;
    inline static enbversion_t       get_enb_version      = nullptr;
    inline static enbsetcallback_t   set_enb_callback     = nullptr;
    inline static enbgetparam_t      get_enb_param        = nullptr;
    inline static enbsetparam_t      set_enb_param        = nullptr;
    inline static enbgetrenderinfo_t get_enb_render_info  = nullptr;

    static void FindD3D11() { d3d11_handle = GetModuleHandle(L"d3d11"); }

    static bool LinkENBFunctions() {
        auto found1 = false, found2 = false, found3 = false, found4 = false, found5 = false, found6 = false;
        if (const auto handle1 = GetProcAddress(d3d11_handle, "ENBGetSDKVersion")) {
            logger::info("Linked ENBGetSDKVersion");
            get_enb_sdk_version = reinterpret_cast<enbsdkversion_t>(handle1);
            found1              = true;
        }
        if (const auto handle2 = GetProcAddress(d3d11_handle, "ENBGetVersion")) {
            logger::info("Linked ENBGetVersion");
            get_enb_version = reinterpret_cast<enbversion_t>(handle2);
            found2          = true;
        }
        if (const auto handle3 = GetProcAddress(d3d11_handle, "ENBSetCallbackFunction")) {
            logger::info("Linked ENBSetCallbackFunction");
            set_enb_callback = reinterpret_cast<enbsetcallback_t>(handle3);
            found3           = true;
        }
        if (const auto handle4 = GetProcAddress(d3d11_handle, "ENBGetParameter")) {
            logger::info("Linked ENBGetParameter");
            get_enb_param = reinterpret_cast<enbgetparam_t>(handle4);
            found4        = true;
        }
        if (const auto handle5 = GetProcAddress(d3d11_handle, "ENBSetParameter")) {
            logger::info("Linked ENBSetParameter");
            set_enb_param = reinterpret_cast<enbsetparam_t>(handle5);
            found5        = true;
        }
        if (const auto handle6 = GetProcAddress(d3d11_handle, "ENBGetRenderInfo")) {
            logger::info("Linked ENBGetRenderInfo");
            get_enb_render_info = reinterpret_cast<enbgetrenderinfo_t>(handle6);
            found6              = true;
        }
        if (found1 && found2 && found3 && found4 && found5 && found6)
            return true;

        return false;
    }
};
