#pragma once
#include <memory>

#include "IInteractionStrategy.h"

class Module;

enum class EffectKind { Radiation, Physical };

class StrategyBase : public IInteractionStrategy {
public:
    StrategyBase(EffectKind kind, double intensity) : kind_(kind), intensity_(intensity) {}
protected:
    EffectKind kind_;
    double intensity_;
};

class EnergyModuleStrategy : public StrategyBase {
public:
    using StrategyBase::StrategyBase;
    void apply(Module& module) override;
    std::string name() const override { return "EnergyModuleStrategy"; }
};
class CrewModuleStrategy : public StrategyBase {
public:
    using StrategyBase::StrategyBase;
    void apply(Module& module) override;
    std::string name() const override { return "CrewModuleStrategy"; }
};
class ResearchModuleStrategy : public StrategyBase {
public:
    using StrategyBase::StrategyBase;
    void apply(Module& module) override;
    std::string name() const override { return "ResearchModuleStrategy"; }
};
class ReactorModuleStrategy : public StrategyBase {
public:
    using StrategyBase::StrategyBase;
    void apply(Module& module) override;
    std::string name() const override { return "ReactorModuleStrategy"; }
};

// Выбор стратегии по типу модуля.
std::unique_ptr<IInteractionStrategy> selectStrategy(const Module& module, EffectKind kind, double intensity);