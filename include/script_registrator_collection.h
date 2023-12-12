#pragma once

#include <string>
#include <map>
#include <type_traits>

#include <ext/ext_script_registrator_base.h>

class script_registrator_collection {
public:
    static void add(ext_script_registrator_base* registrator);

    template <typename TRegistrator, typename ...TArg>
    static void add(TArg... args) {
        add(new TRegistrator(std::forward<TArg>(args)...));
    }
};
