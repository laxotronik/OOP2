#include "ModuleFactory.h"

#include "Modules.h"

double ModuleFactory::cost() const { return ModuleTypeRegistry::get(typeName())->cost; }

std::unique_ptr<Module> EnergyModuleFactory::create(int id) const { return std::make_unique<EnergyModule>(id); }
std::unique_ptr<Module> ReactorModuleFactory::create(int id) const { return std::make_unique<ReactorModule>(id); }
std::unique_ptr<Module> CrewModuleFactory::create(int id) const { return std::make_unique<CrewModule>(id); }
std::unique_ptr<Module> ResearchModuleFactory::create(int id) const { return std::make_unique<ResearchModule>(id); }