#ifdef __linux__

#include <event_bus.h>
#include <rcmp.hpp>
#include <common/signscan.h>

static constexpr int32_t g_stable_appid = 221100;

static void patch_integer_appid(const std::string_view& pattern, std::intptr_t offset) {
    auto ptr = scan_pattern(pattern, offset);

    if (!ptr) {
        printf("ptr %s not found\n", pattern.data());
    }

    rcmp::unprotect_memory(ptr.value(), sizeof(int32_t));
    *reinterpret_cast<int32_t*>(ptr.value()) = g_stable_appid;
}

static init_func fn([]() {
    static constexpr const char* appIdStr = "SteamAppId=221100";
    printf("Making experimental server great again (allowing connection from stable client)\n");

    patch_integer_appid("C7 07 14 A0 0F 00", 2);
    patch_integer_appid("BE 14 A0 0F 00", 1);
    patch_integer_appid("C7 00 14 A0 0F 00", 2);

    // Patch "SteamAppId=1024020" string
    constexpr auto pattern = "B9 ? ? ? ? E8 ? ? ? ? 48 8D 3D ? ? ? ? E8 ? ? ? ?";
    auto ptr = scan_pattern(pattern, 13);
    auto addr = get_address<char*>(ptr.value());
    auto sz = strlen(addr);
    rcmp::unprotect_memory(addr, sz + 1);
    memset(addr, 0, sz);
    strcpy(addr, appIdStr);
}, "appid-patch");

#endif