#pragma once
#include "Module.h"

class EnergyModule : public Module {
public:
    explicit EnergyModule(int id);
    std::unique_ptr<Module> clone() const override { return std::make_unique<EnergyModule>(*this); }
    double energyProduction() const override;
};

class ReactorModule : public Module {
public:
    explicit ReactorModule(int id);
    std::unique_ptr<Module> clone() const override { return std::make_unique<ReactorModule>(*this); }
    double energyProduction() const override;
};

class CrewModule : public Module {
public:
    explicit CrewModule(int id);
    std::unique_ptr<Module> clone() const override { return std::make_unique<CrewModule>(*this); }
    bool isLiving() const override { return true; }
    double oxygenConsumption() const override { return 2.0; }
};

class ResearchModule : public Module {
public:
    explicit ResearchModule(int id);
    std::unique_ptr<Module> clone() const override { return std::make_unique<ResearchModule>(*this); }
    bool isResearch() const override { return true; }
    void work() override;
    double getResearchPoints() const { return points_; }

private:
    double points_ = 0.0;
};