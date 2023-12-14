#ifdef WIN32

#include <Windows.h>

#include <bootstrap.h>
#include <combaseapi.h>
#include <rcmp.hpp>

static HRESULT co_initialize_ex_hook(HRESULT (* original)(LPVOID, DWORD),
                                     LPVOID pvReserved,
                                     DWORD dwCoInit) {
    static auto has_initialized = false;

    auto result = original(pvReserved, dwCoInit);

    if (has_initialized) {
        return result;
    }

    has_initialized = true;

    bootstrap::init();

    return result;
}

extern "C" {
__declspec(dllexport) void dummy_export() {
    // for import table hijacking
}
}

BOOL APIENTRY DllMain(HMODULE moduleHandle,
                      DWORD reason,
                      LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            rcmp::hook_function<&CoInitializeEx>(co_initialize_ex_hook);

            DisableThreadLibraryCalls(moduleHandle);

            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        default:
            break;
    }
    return true;
}
#endif