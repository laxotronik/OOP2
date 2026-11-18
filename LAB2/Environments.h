#pragma once
#include "Environment.h"

// Орбита Земли: низкий риск, нейтральный эффект.
class EarthOrbit : public Environment {
public:
    void applyEffect(Module& module) override;
    std::string getName() const override { return "Орбита Земли"; }
    double getEnergyModifier() const override { return 1.0; }
    double getDamageModifier() const override { return 0.0; }
};

// Глубокий космос: энергопотребление ×1.5, периодические радиационные импульсы.
class DeepSpace : public Environment {
public:
    void applyEffect(Module& module) override;
    std::string getName() const override { return "Глубокий космос"; }
    double getEnergyModifier() const override { return 1.5; }
    double getDamageModifier() const override { return 1.5; }
    static constexpr int kRadiationPeriod = 3;  // импульс, когда возраст модуля кратен 3
};

// Астероидное поле: постоянный физический урон, исследовательские модули блокируются.
class AsteroidField : public Environment {
public:
    void applyEffect(Module& module) override;
    std::string getName() const override { return "Астероидное поле"; }
    double getEnergyModifier() const override { return 1.2; }
    double getDamageModifier() const override { return 2.0; }
};