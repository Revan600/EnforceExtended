#ifdef WIN32

#include <event_bus.h>

#include <script_registrator_collection.h>
#include <common/signscan.h>
#include <common/callable_address.h>
#include <ext/ext_script_registrator_base.h>

static const char* g_module_core = nullptr;

static callable_address<void*(const script_context*, const char*, void*, uint32_t)> register_function([]() {
    constexpr auto pattern = "E8 ? ? ? ? 48 8B F8 85 F6 74 24 48 8B 8D ? ? ? ? 45 33 C0";
    return scan_pattern(pattern, -0x15).value();
});

static callable_address<void*(const script_context*, const char*, uint8_t)> register_type([]() {
    constexpr auto pattern = "E8 ? ? ? ? 48 8B 00 48 85 C0 74 11 8B 48 08 81 E1 ? ? ? ? 81 F9 ? ? ? ? 74 02";
    return scan_pattern(pattern, -0x10).value();
});

static callable_address<void*(const script_context*, void*, const char*, void*, uint32_t,
                              uint8_t)> register_member_function([]() {
    constexpr auto pattern = "E8 ? ? ? ? 8B 54 24 60 48 8B F8 85 D2 74 22 48 8B 8E ? ? ? ? 45 33 C0";
    return scan_pattern(pattern, -0x16).value();
});

static callable_address<script_registrator**()> get_registration_head([]() {
    constexpr auto pattern = "48 85 D2 74 07 C6 82 ? ? ? ? ? 48 8B 35 ? ? ? ? E8 ? ? ? ? 48 8B 18 48 85 DB 74 21 66 0F 1F 44 00";
    auto ptr = scan_pattern(pattern, 0x13);

    return get_call(ptr.value());
});

void script_registrator_collection::add(ext_script_registrator_base* registrator) {
    const auto* head = *get_registration_head();
    do {
        head = head->next();
    } while (head->next());
    head->set_next(registrator);
}

ext_script_registrator_base::ext_script_registrator_base() {
    m_module_name = g_module_core;
}

void ext_script_registrator_base::register_function(const std::string& name, void* functionPtr) {
    m_functions.emplace(name, functionPtr);
}

bool ext_script_registrator_base::flush(script_context* ctx) {
    for (const auto& [name, cls]: m_types) {
        cls.flush(ctx);
    }

    for (const auto& [name, ptr]: m_functions) {
        ::register_function(ctx, name.c_str(), ptr, 0);
    }

    return true;
}

ext_type_meta& ext_script_registrator_base::begin_type(const std::string& name) {
    auto it = m_types.find(name);

    if (it != m_types.end()) {
        return it->second;
    }

    return m_types.emplace(name, name).first->second;
}

void ext_type_meta::flush(script_context* ctx) const {
    auto type = ::register_type(ctx, m_name.c_str(), 4);

    for (const auto& [name, ptr]: m_functions) {
        ::register_member_function(ctx, type, name.c_str(), ptr, 0, 1);
    }
}

void ext_type_meta::register_function(const std::string& name, void* functionPtr) {
    m_functions.emplace(name, functionPtr);
}

static init_func fn([]() {
    constexpr auto pattern = "48 8B 35 ? ? ? ? E8 ? ? ? ? 48 8B 38 48 85 FF 74 1B 48 39 77 08 75 0C 48 8B 07 48 8B D3 48 8B CF FF 50 10 48 8B 7F 10 48 85 FF 75 E5 48 8B 5C 24 ? 48 8B 74 24 ? 48 83 C4 40 5F C3";
    auto ptr = scan_pattern(pattern, 3);
    g_module_core = *get_address<const char**>(ptr.value());
}, "scripting");

#endif