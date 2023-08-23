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
    inline static bool               enb_found           = false;
    inline static HMODULE            enb_handle          = nullptr;
    inline static enbsdkversion_t    get_enb_sdk_version = nullptr;
    inline static enbversion_t       get_enb_version     = nullptr;
    inline static enbsetcallback_t   set_enb_callback    = nullptr;
    inline static enbgetparam_t      get_enb_param       = nullptr;
    inline static enbsetparam_t      set_enb_param       = nullptr;
    inline static enbgetrenderinfo_t get_enb_render_info = nullptr;

    static void FindENB() {
        enb_handle = GetModuleHandle(L"d3d11");

        if (enb_handle)
            enb_found = true;
    }

    static void LinkENBFunctions() {
        get_enb_sdk_version = reinterpret_cast<enbsdkversion_t>(GetProcAddress(enb_handle, "ENBGetSDKVersion"));
        get_enb_version     = reinterpret_cast<enbversion_t>(GetProcAddress(enb_handle, "ENBGetVersion"));
        set_enb_callback    = reinterpret_cast<enbsetcallback_t>(GetProcAddress(enb_handle, "ENBSetCallbackFunction"));
        get_enb_param       = reinterpret_cast<enbgetparam_t>(GetProcAddress(enb_handle, "ENBGetParameter"));
        set_enb_param       = reinterpret_cast<enbsetparam_t>(GetProcAddress(enb_handle, "ENBSetParameter"));
        get_enb_render_info = reinterpret_cast<enbgetrenderinfo_t>(GetProcAddress(enb_handle, "ENBGetRenderInfo"));
    }
};
