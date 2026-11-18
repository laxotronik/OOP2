#pragma once
#include <memory>
#include <string>
#include <utility>

#include "IObservable.h"
#include "ModuleTypeInfo.h"

enum class ModuleKind { Energy, Crew, Research, Reactor };

class Module : public IObservable {
public:
    Module(int id, std::shared_ptr<const ModuleTypeInfo> info, ModuleKind kind);
    Module(const Module&) = default;
    ~Module() override = default;

    virtual std::unique_ptr<Module> clone() const = 0;

    // --- состояние ---
    int getId() const { return id_; }
    ModuleKind getKind() const { return kind_; }
    const std::string& getTypeName() const { return info_->name; }
    std::string label() const;
    double getHealth() const { return health_; }
    double getMaxHealth() const { return info_->maxHealth; }
    double getHealthRatio() const { return health_ / info_->maxHealth; }
    double getEfficiency() const { return efficiency_; }
    double getEnergyConsumption() const { return info_->baseEnergyConsumption; }
    double getReserve() const { return reserve_; }
    int getSector() const { return sector_; }
    int getAge() const { return age_; }
    bool isActive() const { return active_; }
    bool isBlocked() const { return blocked_; }
    bool isDestroyed() const { return health_ <= 0.0; }
    bool isOperational() const { return active_ && !blocked_ && !isDestroyed(); }

    // --- действия ---
    void activate();
    void deactivate();
    void block();
    void unblock();
    void takeDamage(double amount);            // -> Damaged / Destroyed
    void repair(double amount);
    void scaleEfficiency(double factor);       // используется operator*
    void consumeEnergy(double envModifier);    // -> EnergyConsumed
    void addReserve(double amount);
    bool transferEnergyTo(Module& other, double amount);  // взаимодействие модулей
    void setSector(int sector) { sector_ = sector; }
    void advanceTime() { ++age_; }

    // --- полиморфное поведение ---
    virtual bool isResearch() const { return false; }
    virtual bool isLiving() const { return false; }
    virtual double energyProduction() const { return 0.0; }
    virtual double oxygenConsumption() const { return 0.0; }
    virtual void work() {}
    virtual std::string describe() const;

private:
    void emit(ModuleEventType type, double damage = 0.0, double energy = 0.0);

    static constexpr double kMaxReserve = 100.0;
    static constexpr double kMaxEfficiency = 3.0;

    int id_;
    ModuleKind kind_;
    std::shared_ptr<const ModuleTypeInfo> info_;  // Легковес
    double health_;
    double efficiency_;
    double reserve_ = 0.0;
    bool active_ = false;
    bool blocked_ = false;
    int sector_;
    int age_ = 0;
};