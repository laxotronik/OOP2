#pragma once
#include <memory>
#include <string>

#include "Module.h"

// Абстрактная фабрика: CommandCenter знает только этот интерфейс.
class ModuleFactory {
public:
    virtual ~ModuleFactory() = default;
    virtual std::unique_ptr<Module> create(int id) const = 0;
    virtual std::string typeName() const = 0;
    double cost() const;
};

class EnergyModuleFactory : public ModuleFactory {
public:
    std::unique_ptr<Module> create(int id) const override;
    std::string typeName() const override { return "Energy"; }
};
class ReactorModuleFactory : public ModuleFactory {
public:
    std::unique_ptr<Module> create(int id) const override;
    std::string typeName() const override { return "Reactor"; }
};
class CrewModuleFactory : public ModuleFactory {
public:
    std::unique_ptr<Module> create(int id) const override;
    std::string typeName() const override { return "Crew"; }
};
class ResearchModuleFactory : public ModuleFactory {
public:
    std::unique_ptr<Module> create(int id) const override;
    std::string typeName() const override { return "Research"; }
};