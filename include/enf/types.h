#pragma once

#include <cstdint>

#pragma pack(push, 1)

template <typename T>
class enf_array {
    void* m_array_vtbl;
    void* m_type;
    void* m_allocator_vtbl;
    void* m_unk1;
    void* m_ptr_tracker;
    T* m_data;
    int32_t m_capacity;
    int32_t m_size;

public:
    int32_t get_size() const {
        return m_size;
    }

    int32_t get_capacity() const {
        return m_capacity;
    }

    const T& get(size_t index) {
        return m_data[index];
    }

    void set(const T&& data, size_t index) {
        m_data[index] = data;
    }
};

using enf_string = const char*;
using enf_int = int32_t;
using enf_string_dynamic_array = enf_array<enf_string>;
using enf_int_dynamic_array = enf_array<enf_int>;
using enf_string_static_array = enf_string*;
using enf_int_static_array = enf_int*;

#pragma pack(pop)