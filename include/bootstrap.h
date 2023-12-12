#pragma once

#include <Windows.h>

class bootstrap {
public:
    static HMODULE module_handle;

    static void init();
};