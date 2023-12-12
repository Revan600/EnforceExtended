#include <event_bus.h>

#include <utility>

bool event_func_base::run() {
    if (!m_func)
        return false;

    m_func();
    return true;
}

void event_bus::register_init(init_func* func) {
    auto& bus = instance();

    bus.m_init_funcs.insert({ func->name, func });
}

event_bus& event_bus::instance() {
    static event_bus bus;
    return bus;
}

void event_bus::run_init() {
    auto& bus = instance();
    auto counter = 0;

    for (auto& [key, value] : bus.m_init_funcs) {
        counter += bus.run_init(value);
    }
}

void event_bus::run_init(const std::string& name) {
    auto& bus = instance();

    const auto it = bus.m_init_funcs.find(name);

    if (it == bus.m_init_funcs.end())
        return;

    bus.run_init(it->second);
}

event_bus::event_bus() {

}

uint32_t event_bus::run_init(init_func* func) {
    auto result = 0;

    for (const auto& dep : func->dependencies) {
        auto iter = m_init_funcs.find(dep);

        if (iter == m_init_funcs.end()) {
            exit(1);
        }

        result += run_init(iter->second);
    }

    if (func->run())
        result++;

    return result;
}

bool init_func::run() {
    if (m_has_fired)
        return false;

    event_func_base::run();
    m_has_fired = true;
    return true;
}

init_func::init_func(void (*func)(), const std::string& name, std::vector<std::string> dependencies)
        : event_func_base(func, name) {
    this->dependencies = std::move(dependencies);
    this->m_has_fired = false;

    event_bus::register_init(this);
}

init_func::init_func(void(*func)(), const char* name, std::vector<const char*> dependencies)
        : event_func_base(func, name) {
    for (const auto dependency : dependencies) {
        this->dependencies.emplace_back(dependency);
    }

    this->m_has_fired = false;

    event_bus::register_init(this);
}
