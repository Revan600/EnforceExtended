#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <optional>
#include <string>

template <typename TReturn, typename TOffset>
TReturn* get_rva(TOffset rva, std::uintptr_t begin) {
    // ReSharper disable once CppCStyleCast
    return (TReturn*) (begin + rva);
}

template <typename T>
T get_call(T address) {
    uintptr_t target = *reinterpret_cast<int32_t*>(address + 1); // +1 in order to skip E8
    target += address + 5; // 5 - full instruction size
    return (T) (target);
}

template <typename T, typename TAddr>
inline T get_address(TAddr address) {
    intptr_t target = *reinterpret_cast<int32_t*>(address);
    target += address + 4; //4 address size

    return (T) target;
}

std::pair<std::uintptr_t, std::uintptr_t> get_module_bounds();

[[nodiscard]] std::optional<std::uintptr_t> scan_pattern(const std::string_view& pattern, std::intptr_t offset = 0);