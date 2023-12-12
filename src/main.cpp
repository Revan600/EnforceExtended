#include <Windows.h>
#include <MinHook.h>

#include <bootstrap.h>
#include <combaseapi.h>

static HMODULE g_current_module = nullptr;

static HRESULT (* g_co_initialize_ex_orig)(LPVOID pvReserved, DWORD dwCoInit) = nullptr;

static HRESULT co_initialize_ex_hook(LPVOID pvReserved, DWORD dwCoInit) {
    static auto has_initialized = false;

    auto result = g_co_initialize_ex_orig(pvReserved, dwCoInit);

    if (has_initialized) {
        return result;
    }

    has_initialized = true;

    bootstrap::module_handle = g_current_module;
    bootstrap::init();

    return result;
}

extern "C" {
__declspec(dllexport) void dummy_export() {

}
}

BOOL APIENTRY DllMain(HMODULE moduleHandle,
                      DWORD reason,
                      LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            g_current_module = moduleHandle;

            if (MH_Initialize() != MH_OK) {
                MessageBox(nullptr, "Cannot initialize hook system!", "Error", MB_OK | MB_ICONSTOP);
                return false;
            }

            if (MH_CreateHook(CoInitializeEx, co_initialize_ex_hook,
                              reinterpret_cast<LPVOID*>(&g_co_initialize_ex_orig)) != MH_OK) {
                MessageBox(nullptr, "Cannot initialize bootstrap system!", "Error", MB_OK | MB_ICONSTOP);
                return false;
            }

            if (MH_EnableHook(nullptr) != MH_OK) {
                MessageBox(nullptr, "Cannot enable bootstrap system!", "Error", MB_OK | MB_ICONSTOP);
                return false;
            }

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
