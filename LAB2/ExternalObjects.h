#pragma once
#include <random>

#include "ExternalObject.h"
#include "ResourcePackage.h"

// Солнечная буря: радиация, эффект зависит от типа модуля (Стратегия).
class SolarStorm : public ExternalObject {
public:
    explicit SolarStorm(double strength) : ExternalObject("SolarStorm", strength) {}
    void interact(CommandCenter& center, Module& module) override;
    bool isAreaEffect() const override { return true; }
};

// Метеоритный поток: физический урон через operator-.
class MeteorShower : public ExternalObject {
public:
    explicit MeteorShower(double strength, unsigned seed = 1)
        : ExternalObject("MeteorShower", strength), rng_(seed) {}
    void interact(CommandCenter& center, Module& module) override;
    bool isAreaEffect() const override { return true; }
private:
    std::mt19937 rng_;
};

// Грузовой контейнер: ресурсы через operator+.
class CargoContainer : public ExternalObject {
public:
    explicit CargoContainer(ResourcePackage cargo) : ExternalObject("CargoContainer", 1.0), cargo_(cargo) {}
    void interact(CommandCenter& center, Module& module) override;
private:
    ResourcePackage cargo_;
};

enum class AnomalyEffect { Random = 0, Teleport = 1, DoubleEnergy = 2, Shutdown = 3 };

// Космическая аномалия: непредсказуемый эффект, модификация через operator*.
class SpaceAnomaly : public ExternalObject {
public:
    SpaceAnomaly(double strength, unsigned seed, AnomalyEffect forced = AnomalyEffect::Random)
        : ExternalObject("SpaceAnomaly", strength), rng_(seed), forced_(forced) {}
    void interact(CommandCenter& center, Module& module) override;
private:
    std::mt19937 rng_;
    AnomalyEffect forced_;
};