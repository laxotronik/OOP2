#include "ModuleTypeInfo.h"

#include <stdexcept>
#include <unordered_map>

namespace {
    using Pool = std::unordered_map<std::string, std::shared_ptr<const ModuleTypeInfo>>;

    Pool& pool() {
        static Pool p = [] {
            Pool m;
            auto add = [&m](ModuleTypeInfo i) {
                m[i.name] = std::make_shared<const ModuleTypeInfo>(i);
                };
            //  name        maxHP  energy eff  cost
            add({ "Energy",   100.0,  2.0, 1.0,  60.0 });
            add({ "Crew",     150.0, 10.0, 1.0, 100.0 });
            add({ "Research",  80.0, 15.0, 1.0, 120.0 });
            add({ "Reactor",  200.0,  5.0, 1.0, 200.0 });
            return m;
            }();
        return p;
    }
}  // namespace

std::shared_ptr<const ModuleTypeInfo> ModuleTypeRegistry::get(const std::string& name) {
    auto it = pool().find(name);
    if (it == pool().end()) throw std::out_of_range("Unknown module type: " + name);
    return it->second;
}

void ModuleTypeRegistry::registerType(const ModuleTypeInfo& info) {
    pool()[info.name] = std::make_shared<const ModuleTypeInfo>(info);
}