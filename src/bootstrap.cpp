#include <bootstrap.h>

#include <event_bus.h>

void bootstrap::init() {
    event_bus::run_init();
}
