#include <common/signscan.h>

#ifdef WIN32

#include <Windows.h>

std::pair<std::uintptr_t, std::uintptr_t> get_module_bounds() {
    static const auto module_base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    const auto dos_header = get_rva<IMAGE_DOS_HEADER>(0, module_base_address);
    const auto nt_header = get_rva<IMAGE_NT_HEADERS>(dos_header->e_lfanew, module_base_address);

    const auto end = module_base_address + nt_header->OptionalHeader.SizeOfCode;

    return {module_base_address, end};
}

#endif

#ifdef __linux__

#include <dlfcn.h>
#include <link.h>
#include <cstring>

// https://github.com/torvalds/linux/blob/master/fs/binfmt_elf.c
inline std::size_t total_mapping_size(const ElfW(Phdr)* cmds, std::size_t count) {
    std::size_t first_idx = SIZE_MAX, last_idx = SIZE_MAX;

    for (std::size_t i = 0; i < count; ++i) {
        if (cmds[i].p_type == PT_LOAD) {
            last_idx = i;

            if (first_idx == SIZE_MAX)
                first_idx = i;
        }
    }

    if (first_idx == SIZE_MAX)
        return 0;

    return cmds[last_idx].p_vaddr + cmds[last_idx].p_memsz -
           (cmds[first_idx].p_vaddr & ~(cmds[first_idx].p_align - 1));
}

struct dl_iterate_query {
    const char* name{nullptr};
    void* result{nullptr};
};

inline int dl_iterate_callback(struct dl_phdr_info* info, std::size_t size, void* data) {
    (void) size;

    auto* search_info = static_cast<dl_iterate_query*>(data);

    const char* file_path = info->dlpi_name;
    const char* file_name = std::strrchr(file_path, '/');

    if (file_name) {
        ++file_name;
    } else {
        file_name = file_path;
    }

    if (!std::strcmp(search_info->name, file_name)) {
        for (int i = 0; i < info->dlpi_phnum; ++i) {
            if (info->dlpi_phdr[i].p_type == PT_LOAD) {
                search_info->result = reinterpret_cast<void*>(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);

                return 1;
            }
        }

        return 2;
    }

    return 0;
}

static std::uintptr_t get_module_address() {
    dl_iterate_query search;

    search.name = "";

    if (dl_iterate_phdr(&dl_iterate_callback, &search) == 1) {
        return reinterpret_cast<uintptr_t>(search.result);
    }

    return 0;
}

std::pair<std::uintptr_t, std::uintptr_t> get_module_bounds() {
    static const auto module_base_address = get_module_address();
    static const auto* ehdr = reinterpret_cast<ElfW(Ehdr)*>(module_base_address);
    static const auto* phdr = reinterpret_cast<ElfW(Phdr)*>(module_base_address + ehdr->e_phoff);
    static const auto mapping_size = total_mapping_size(phdr, ehdr->e_phnum);

    const auto end = module_base_address + mapping_size;
    return {module_base_address, end};
}

#endif

std::optional<std::uintptr_t> scan_pattern(const std::string_view& pattern, std::intptr_t offset) {
    static const auto bounds = get_module_bounds();
    const auto end = bounds.second;
    auto start = bounds.first;

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

    printf("Pattern %s was not found!\n", pattern.data());

    return std::nullopt;
}