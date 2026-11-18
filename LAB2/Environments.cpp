#include "Environments.h"

#include "LoggingProxy.h"
#include "Module.h"
#include "Strategies.h"

using LS = LogSource;

void EarthOrbit::applyEffect(Module& module) {
    LoggingProxy::log(LS::ENV, getName() + ": минимальное влияние на " + module.label());
}

void DeepSpace::applyEffect(Module& module) {
    if (module.isDestroyed()) return;
    if (module.getAge() % kRadiationPeriod != 0) return;  
    auto strategy = selectStrategy(module, EffectKind::Radiation, 1.0 * getDamageModifier());
    LoggingProxy::log(LS::ENV, getName() + ": радиационный импульс → " + module.label() +
        " [" + strategy->name() + "]");
    strategy->apply(module);
}

void AsteroidField::applyEffect(Module& module) {
    if (module.isDestroyed()) return;
    auto strategy = selectStrategy(module, EffectKind::Physical, 0.5 * getDamageModifier());
    LoggingProxy::log(LS::ENV, getName() + ": удары камней → " + module.label() +
        " [" + strategy->name() + "]");
    strategy->apply(module);
}