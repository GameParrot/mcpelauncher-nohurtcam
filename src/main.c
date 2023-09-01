#include <dlfcn.h>
void stubbedHc() {};
void __attribute__ ((visibility ("default"))) mod_preinit() {
    int (*mcpelauncher_preinithook)(const char *, void *, void **) = (int (*)(const char *, void *, void **))dlsym(dlopen("libmcpelauncher_mod.so", 0), "mcpelauncher_preinithook");
    mcpelauncher_preinithook("_ZNK16VanillaCameraAPI25tryGetDamageBobParametersE8WeakRefTI15EntityRefTraitsEf", stubbedHc, NULL); // 1.19.40-
    mcpelauncher_preinithook("_ZThn8_NK9CameraAPI25tryGetDamageBobParametersE8WeakRefTI15EntityRefTraitsEf", stubbedHc, NULL); // 1.19.0-1.19.31
    mcpelauncher_preinithook("_ZThn8_NK9CameraAPI25tryGetDamageBobParametersE13ActorUniqueIDf", stubbedHc, NULL); // 1.18.10-1.18.32
};
