#pragma once
#include <memory>
#include <string>

// Легковес: разделяемые (неизменяемые) характеристики типа модуля.
struct ModuleTypeInfo {
    std::string name;
    double maxHealth;
    double baseEnergyConsumption;
    double baseEfficiency;
    double cost;
};

class ModuleTypeRegistry {
public:
    static std::shared_ptr<const ModuleTypeInfo> get(const std::string& name);
    // Новые типы можно регистрировать без правки CommandCenter.
    static void registerType(const ModuleTypeInfo& info);
};