#include "ExternalObjects.h"

#include "CommandCenter.h"
#include "LoggingProxy.h"
#include "Module.h"
#include "Operators.h"
#include "Strategies.h"

using LS = LogSource;

void SolarStorm::interact(CommandCenter& center, Module& module) {
    const double effective = strength_ / (1.0 + 0.1 * center.getSafetyLevel());
    auto strategy = selectStrategy(module, EffectKind::Radiation, effective);
    LoggingProxy::log(LS::OBJECT, "SolarStorm → " + module.label() + " [" + strategy->name() +
        ", интенсивность " + LoggingProxy::fmt(effective, 2) + "]");
    strategy->apply(module);  
}

void MeteorShower::interact(CommandCenter& center, Module& module) {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    if (d(rng_) > 0.6) {
        LoggingProxy::log(LS::OBJECT, "MeteorShower: метеориты прошли мимо " + module.label());
        return;
    }
    const double amount = strength_ * (0.5 + d(rng_)) / (1.0 + 0.05 * center.getSafetyLevel());
    LoggingProxy::log(LS::OBJECT, "MeteorShower: попадание в " + module.label());
    (void)(module - Damage{ amount, DamageType::Physical });
}

void CargoContainer::interact(CommandCenter& center, Module& module) {
    LoggingProxy::log(LS::OBJECT, "CargoContainer: стыковка у " + module.label() + ", +" +
        LoggingProxy::fmt(cargo_.energy) + " энергии, +" +
        LoggingProxy::fmt(cargo_.oxygen) + " кислорода");
    (void)(center + cargo_);
}

void SpaceAnomaly::interact(CommandCenter& center, Module& module) {
    AnomalyEffect effect = forced_;
    if (effect == AnomalyEffect::Random) {
        std::uniform_int_distribution<int> d(1, 3);
        effect = static_cast<AnomalyEffect>(d(rng_));
    }
    switch (effect) {
    case AnomalyEffect::Teleport: {
        std::uniform_int_distribution<int> sector(1, 6);
        const int s = sector(rng_);
        LoggingProxy::log(LS::OBJECT, "SpaceAnomaly: телепортация " + module.label() + " в сектор " + std::to_string(s));
        module.setSector(s);
        (void)(module * 1.2);
        break;
    }
    case AnomalyEffect::DoubleEnergy:
        LoggingProxy::log(LS::OBJECT, "SpaceAnomaly: удвоение энергии станции");
        (void)(center + ResourcePackage{ center.getEnergy(), 0.0, 0 });
        (void)(module * 1.5);
        break;
    case AnomalyEffect::Shutdown:
        LoggingProxy::log(LS::OBJECT, "SpaceAnomaly: отключение " + module.label());
        (void)(module * 0.5);
        module.deactivate();
        break;
    case AnomalyEffect::Random:
        break;
    }
}