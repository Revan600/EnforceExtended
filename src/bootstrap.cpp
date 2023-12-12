#include <bootstrap.h>

#include <MinHook.h>
#include <event_bus.h>

HMODULE bootstrap::module_handle;

void bootstrap::init() {
    event_bus::run_init();
    MH_EnableHook(nullptr);
}
