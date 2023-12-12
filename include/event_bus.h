#pragma once

#include <vector>
#include <string>
#include <map>

class event_func_base;

class init_func;

class event_bus {
public:
    static void register_init(init_func* func);

    static void run_init();

    static void run_init(const std::string& name);

private:
    static event_bus& instance();

    event_bus();

    event_bus(const event_bus&) = delete;

    uint32_t run_init(init_func* func);

    std::map<std::string, init_func*> m_init_funcs;
};

class event_func_base {
public:
    virtual ~event_func_base() = default;

    event_func_base(void(* func)(), const std::string& name) {
        this->m_func = func;
        this->name = name;
    }

    event_func_base(void(* func)(), const char* name) {
        this->m_func = func;
        this->name = name;
    }

    event_func_base(const event_func_base&) = delete;

    virtual bool run();

    std::string name;
private:
    void (* m_func)();
};

class init_func final : public event_func_base {
public:
    init_func(void (* func)(), const std::string& name, std::vector<std::string> dependencies = {});

    init_func(void (* func)(), const char* name, std::vector<const char*> dependencies = {});

    init_func(const init_func&) = delete;

    bool run() override;

    std::vector<std::string> dependencies;

private:
    bool m_has_fired;
};
