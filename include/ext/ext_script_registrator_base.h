#pragma once

#include <map>

#include <enf/script_registrator.h>

class ext_type_meta {
    std::string m_name;
    std::map<std::string, void*> m_functions;
public:
    explicit ext_type_meta(const std::string& name) : m_name(name) {};

    ext_type_meta(const ext_type_meta&) = delete;

    ext_type_meta& operator=(const ext_type_meta&) = delete;

    void register_function(const std::string& name, void* functionPtr);

    void flush(script_context* ctx) const;
};

class ext_script_registrator_base : public script_registrator {
    std::map<std::string, ext_type_meta> m_types;
    std::map<std::string, void*> m_functions;
public:
    ext_script_registrator_base();

    ~ext_script_registrator_base() override = default;

    void* vt0() override {
        return nullptr;
    }

    bool flush(script_context* ctx) override;

    int32_t vt2() override {
        return 1;
    }

protected:
    ext_type_meta& begin_type(const std::string& name);

    void register_function(const std::string& name, void* functionPtr);
};
