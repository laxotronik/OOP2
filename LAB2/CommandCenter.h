#pragma once
#include <cstddef>
#include <iosfwd>
#include <memory>
#include <vector>

#include "EnvironmentProxy.h"
#include "IObserver.h"
#include "Module.h"
#include "ResourcePackage.h"

class ExternalObject;
class ModuleFactory;

// Не зависит от конкретных типов модулей: работает только с Module / ModuleFactory.
class CommandCenter : public IObserver {
public:
    CommandCenter(double energy, double oxygen, int safetyLevel, std::size_t maxModules);
    CommandCenter(const CommandCenter& other);             // глубокое копирование модулей
    CommandCenter(CommandCenter&& other);                  // перенос + перепривязка подписок
    CommandCenter& operator=(const CommandCenter& other);
    CommandCenter& operator=(CommandCenter&& other);
    ~CommandCenter() override;

    // --- управление модулями ---
    Module* createModule(const ModuleFactory& factory);
    bool addModule(std::unique_ptr<Module> module);
    Module* findModule(int id);
    bool transferEnergy(int fromId, int toId, double amount);   // взаимодействие модулей
    void purgeDestroyed();   // удаляет уничтоженные модули (отложенно — безопасно)

    // --- окружение и внешние объекты ---
    void setEnvironment(std::shared_ptr<Environment> env);
    EnvironmentProxy& getEnvironment() { return *currentEnvironment_; }
    void handleExternalObject(ExternalObject& object, int targetModuleId = -1);

    // --- симуляция ---
    void tick();
    void receiveResources(const ResourcePackage& pkg);   // используется operator+

    // --- Observer ---
    void onModuleEvent(const ModuleEvent& event) override;

    // --- статистика ---
    double getEnergy() const { return energy_; }
    double getOxygen() const { return oxygen_; }
    int getSafetyLevel() const { return safetyLevel_; }
    std::size_t getMaxModules() const { return maxModules_; }
    std::size_t getModuleCount() const { return modules_.size(); }
    double getTotalDamage() const { return totalDamage_; }
    int getTurn() const { return turn_; }
    void printStatistics(std::ostream& os) const;

private:
    void detachAll();
    void cloneFrom(const CommandCenter& other);

    double energy_;
    double oxygen_;
    int safetyLevel_;
    std::size_t maxModules_;
    std::vector<std::unique_ptr<Module>> modules_;
    std::shared_ptr<EnvironmentProxy> currentEnvironment_;
    std::vector<int> pendingRemoval_;
    double totalDamage_ = 0.0;
    int nextModuleId_ = 1;
    int turn_ = 0;
};