#pragma once

#include <cstdint>
#include <functional>

namespace details {
    template <typename TRet, typename... Args>
    class callable_address_ {
        std::uintptr_t m_function_ptr = 0;

        std::uintptr_t (* m_getter)();

    protected:
        explicit callable_address_(std::uintptr_t(* getter)()) {
            m_getter = getter;
        }

    public:
        TRet operator()(Args... args) {
            if (!m_function_ptr) {
                m_function_ptr = m_getter();
            }

            return reinterpret_cast<TRet(*)(Args...)>(m_function_ptr)(args...);
        }
    };
}

template <typename TRet>
class callable_address {
};

template <typename TRet, typename... Args>
class callable_address<TRet(Args...)> : public details::callable_address_<TRet, Args...> {
public:
    explicit callable_address(std::uintptr_t(* getter)()) : details::callable_address_<TRet, Args...>(getter) {};
};