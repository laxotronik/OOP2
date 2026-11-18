#include "Modules.h"

EnergyModule::EnergyModule(int id) : Module(id, ModuleTypeRegistry::get("Energy"), ModuleKind::Energy) {}
double EnergyModule::energyProduction() const {
    return isOperational() ? 30.0 * getEfficiency() * getHealthRatio() : 0.0;
}

ReactorModule::ReactorModule(int id) : Module(id, ModuleTypeRegistry::get("Reactor"), ModuleKind::Reactor) {}
double ReactorModule::energyProduction() const {
    return isOperational() ? 80.0 * getEfficiency() * getHealthRatio() : 0.0;
}

CrewModule::CrewModule(int id) : Module(id, ModuleTypeRegistry::get("Crew"), ModuleKind::Crew) {}

ResearchModule::ResearchModule(int id) : Module(id, ModuleTypeRegistry::get("Research"), ModuleKind::Research) {}
void ResearchModule::work() {
    if (isOperational()) points_ += getEfficiency() * getHealthRatio();
}