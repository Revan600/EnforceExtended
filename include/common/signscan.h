#pragma once

#include <iostream>
#include <Windows.h>
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

[[nodiscard]] std::optional<std::uintptr_t> scan_pattern(const std::string_view& pattern, std::intptr_t offset = 0) {
    static const auto module_base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    const auto dos_header = get_rva<IMAGE_DOS_HEADER>(0, module_base_address);
    const auto nt_header = get_rva<IMAGE_NT_HEADERS>(dos_header->e_lfanew, module_base_address);

    const auto end = module_base_address + nt_header->OptionalHeader.SizeOfCode;
    auto start = module_base_address;

    std::vector<std::pair<bool, std::uint8_t>> byte_pattern{};
    for (auto iterator_start = pattern.begin(); iterator_start < pattern.end(); ++iterator_start) {
        switch (*iterator_start) {
            case ' ':
                continue;
            case '?':
                byte_pattern.emplace_back(std::make_pair<bool, std::uint8_t>(true, 0x00));
                continue;
            default:
                std::string cached_byte_string(iterator_start - 1, (++iterator_start) + 1);
                byte_pattern.emplace_back(std::make_pair<bool, std::uint8_t>(false,
                                                                             static_cast<std::uint8_t>(std::stoul(
                                                                                     cached_byte_string, nullptr,
                                                                                     16))));
        }
    }

    for (auto cur = start; cur < end; ++cur) {
        auto found_pattern = true;

        auto curCopy = cur;

        for (const auto& [wildcard, byte]: byte_pattern) {
            if (wildcard) {
                ++curCopy;
                continue;
            }

            if (*reinterpret_cast<std::uint8_t*>(curCopy) != byte) {
                found_pattern = false;
                break;
            }

            ++curCopy;
        }

        if (found_pattern)
            return curCopy - byte_pattern.size() + offset;
    }

    return std::nullopt;
}