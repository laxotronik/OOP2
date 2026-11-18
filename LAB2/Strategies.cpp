#include "Strategies.h"

#include <algorithm>

#include "LoggingProxy.h"
#include "Module.h"
#include "Operators.h"

namespace {
    void hit(Module& m, double amount, DamageType t) { (void)(m - Damage{ amount, t }); }
    void scale(Module& m, double k) { (void)(m * k); }
}  // namespace

// Электроника солнечных панелей уязвима к радиации (сбой → падение эффективности),
// панели хрупкие к ударам.
void EnergyModuleStrategy::apply(Module& m) {
    if (kind_ == EffectKind::Radiation) {
        hit(m, 4.0 * intensity_, DamageType::Radiation);
        scale(m, 1.0 - std::min(0.5, 0.1 * intensity_));
    }
    else {
        hit(m, 6.0 * intensity_, DamageType::Physical);
    }
}

// Жилой модуль: радиация опасна для экипажа, корпус прочный.
void CrewModuleStrategy::apply(Module& m) {
    if (kind_ == EffectKind::Radiation) hit(m, 5.0 * intensity_, DamageType::Radiation);
    else                                hit(m, 3.0 * intensity_, DamageType::Physical);
}

// Исследовательский: радиация сбивает приборы, удары блокируют работу.
void ResearchModuleStrategy::apply(Module& m) {
    hit(m, 3.0 * intensity_, kind_ == EffectKind::Radiation ? DamageType::Electronic : DamageType::Physical);
    if (kind_ == EffectKind::Radiation) scale(m, 1.0 - std::min(0.6, 0.15 * intensity_));
    else                                m.block();
}

// Реактор: экранирован от радиации, но уязвим для ударов.
void ReactorModuleStrategy::apply(Module& m) {
    if (kind_ == EffectKind::Radiation) hit(m, 1.0 * intensity_, DamageType::Radiation);
    else                                hit(m, 4.0 * intensity_, DamageType::Physical);
}

std::unique_ptr<IInteractionStrategy> selectStrategy(const Module& module, EffectKind kind, double intensity) {
    switch (module.getKind()) {
    case ModuleKind::Energy:   return std::make_unique<EnergyModuleStrategy>(kind, intensity);
    case ModuleKind::Crew:     return std::make_unique<CrewModuleStrategy>(kind, intensity);
    case ModuleKind::Research: return std::make_unique<ResearchModuleStrategy>(kind, intensity);
    case ModuleKind::Reactor:  return std::make_unique<ReactorModuleStrategy>(kind, intensity);
    }
    return std::make_unique<ReactorModuleStrategy>(kind, intensity);
}