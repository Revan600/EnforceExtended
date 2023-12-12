#pragma once

#include <cstdint>

// enf::ScriptContext
class script_context;

// enf::ScriptRegistrator
class script_registrator {
public:
    virtual ~script_registrator() {};

    virtual void* vt0() = 0;

    virtual bool flush(script_context* ctx) = 0;

    virtual int32_t vt2() = 0;

    const script_registrator* next() const {
        return m_next;
    }

    void set_next(script_registrator* registrator) const {
        m_next = registrator;
    }

    void set_module_name(const char* name) {
        m_module_name = name;
    }

    const char* module_name() const {
        return m_module_name;
    }

protected:
    const char* m_module_name = nullptr;
    mutable script_registrator* m_next = nullptr;
    void* m_unk = nullptr;
};

