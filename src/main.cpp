#include "menu.hpp"
#include <cmath>
#include <cstddef>
#include <dlfcn.h>
#include <functional>
#include <glaze/json.hpp>
#include <libhat.hpp>
#include <link.h>
#include <memory>
#include <print>
#include <span>
#include <stdlib.h>
#include <thread>

struct Config {
    bool enable = true;
};

static Config config;

static glz::sv configPath = "/data/data/com.mojang.minecraftpe/nohurtcam_config.json";

static void loadConfig() {
    std::string buffer;
    if (auto ec = glz::read_file_json(config, configPath, buffer))
        std::println(stderr, "Failed to load no hurt cam config: {}", glz::format_error(ec, buffer));
}

static void saveConfig() {
    std::string buffer;
    if (auto ec = glz::write_file_json<glz::opts{.prettify = true}>(config, configPath, buffer))
        std::println(stderr, "Failed to save no hurt cam config: {}", glz::format_error(ec, buffer));
}

extern "C" [[gnu::visibility("default")]] void mod_preinit() {
    loadConfig();
    saveConfig();

    auto menuLib = dlopen("libmcpelauncher_menu.so", 0);

    addMenu     = reinterpret_cast<decltype(addMenu)>(dlsym(menuLib, "mcpelauncher_addmenu"));
    showWindow  = reinterpret_cast<decltype(showWindow)>(dlsym(menuLib, "mcpelauncher_show_window"));
    closeWindow = reinterpret_cast<decltype(closeWindow)>(dlsym(menuLib, "mcpelauncher_close_window"));

    MenuEntryABI menuEntry{
        .name     = "No hurt cam",
        .selected = [](void*) { return config.enable; },
        .click    = [](void*) {
            config.enable = !config.enable;
            saveConfig(); }};

    addMenu(1, &menuEntry);
}

extern "C" [[gnu::visibility("default")]] void mod_init() {
    auto mcLib = dlopen("libminecraftpe.so", 0);

    std::span<std::byte> range1, range2;

    auto callback = [&](const dl_phdr_info& info) {
        if (auto h = dlopen(info.dlpi_name, RTLD_NOLOAD); dlclose(h), h != mcLib)
            return 0;
        range1 = {reinterpret_cast<std::byte*>(info.dlpi_addr + info.dlpi_phdr[1].p_vaddr), info.dlpi_phdr[1].p_memsz};
        range2 = {reinterpret_cast<std::byte*>(info.dlpi_addr + info.dlpi_phdr[2].p_vaddr), info.dlpi_phdr[2].p_memsz};
        return 1;
    };

    dl_iterate_phdr(
        [](dl_phdr_info* info, size_t, void* data) {
            return (*static_cast<decltype(callback)*>(data))(*info);
        },
        &callback);

    auto VanillaCameraAPI_typeinfo_name             = hat::find_pattern(range1, hat::object_to_signature("16VanillaCameraAPI")).get();
    auto VanillaCameraAPI_typeinfo                  = hat::find_pattern(range2, hat::object_to_signature(VanillaCameraAPI_typeinfo_name)).get() - sizeof(void*);
    auto VanillaCameraAPI_vtable                    = hat::find_pattern(range2, hat::object_to_signature(VanillaCameraAPI_typeinfo)).get() + sizeof(void*);
    auto VanillaCameraAPI_tryGetDamageBobParameters = reinterpret_cast<float (**)(void**, void*, float)>(VanillaCameraAPI_vtable) + 2;

    static auto VanillaCameraAPI_tryGetDamageBobParameters_orig = *VanillaCameraAPI_tryGetDamageBobParameters;

    *VanillaCameraAPI_tryGetDamageBobParameters = [](void** self, void* traits, float a) -> float {
        if (config.enable) {
            return 0;
        }
        return VanillaCameraAPI_tryGetDamageBobParameters_orig(self, traits, a);
    };
}
