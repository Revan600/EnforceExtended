#ifdef __linux__

#include <bootstrap.h>
#include <rcmp.hpp>
#include <common/signscan.h>

static void* engine_startup(void* (* original)()) {
    bootstrap::init();
    return original();
}

__attribute__((constructor))
static void entry_point(int argc, char** argv, char** envp) {
    constexpr auto pattern = "48 83 EC 28 64 48 8B 04 25 ? ? ? ? 48 89 44 24 ? 31 C0 0F AE 5C 24 ? 8B 44 24 0C 25 ? ? ? ? 0D ? ? ? ? 89 44 24 0C";
    auto ptr = scan_pattern(pattern);
    printf("before hook\n");
    rcmp::hook_function<void*()>(ptr.value(), engine_startup);
    printf("before hook\n");
}

#endif