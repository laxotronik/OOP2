#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "CommandCenter.h"
#include "Environments.h"
#include "ExternalObjects.h"
#include "LoggingProxy.h"
#include "ModuleFactory.h"
#include "Operators.h"

namespace {
    void section(const std::string& title) { std::cout << "\n########## " << title << " ##########\n"; }
}  

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); 
#endif

    EnergyModuleFactory energyF;
    CrewModuleFactory crewF;
    ResearchModuleFactory researchF;
    ReactorModuleFactory reactorF;

    // ===== Сценарий 1: создание центра, модули, EarthOrbit =====
    section("Сценарий 1: создание центра, модули, окружение EarthOrbit");
    CommandCenter center(1000.0, 500.0, 10, 6);
    const int reactorId = center.createModule(reactorF)->getId();
    const int energyId = center.createModule(energyF)->getId();
    const int crewId = center.createModule(crewF)->getId();
    const int researchId = center.createModule(researchF)->getId();
    center.setEnvironment(std::make_shared<EarthOrbit>());
    center.tick();
    center.tick();
    center.printStatistics(std::cout);

    // ===== Сценарий 2: DeepSpace, эффекты через Proxy, Observer =====
    section("Сценарий 2: DeepSpace, эффекты через EnvironmentProxy");
    center.setEnvironment(std::make_shared<DeepSpace>());
    for (int i = 0; i < 3; ++i) center.tick();   // радиоимпульс, когда возраст модуля кратен 3
    std::cout << "\n-- Повторный вызов эффекта в том же ходу (должен сработать кеш) --\n";
    center.getEnvironment().applyEffect(*center.findModule(crewId));
    center.printStatistics(std::cout);

    // ===== Сценарий 3: SolarStorm -> operator- -> уведомление центра =====
    section("Сценарий 3: SolarStorm (Стратегия + operator-/operator*)");
    SolarStorm storm(6.0);
    center.handleExternalObject(storm);
    center.printStatistics(std::cout);

    // ===== Сценарий 4: CargoContainer -> operator+ =====
    section("Сценарий 4: CargoContainer (operator+)");
    std::cout << "Энергия до: " << center.getEnergy() << ", кислород до: " << center.getOxygen() << "\n";
    CargoContainer cargo(ResourcePackage{ 300.0, 100.0, 2 });
    center.handleExternalObject(cargo, energyId);
    std::cout << "Энергия после: " << center.getEnergy() << ", кислород после: " << center.getOxygen() << "\n";
    (void)(center + ResourcePackage{ 50.0, 0.0, 0 });   // прямое применение оператора

    // ===== Сценарий 5: SpaceAnomaly -> operator* =====
    section("Сценарий 5: SpaceAnomaly (operator*)");
    SpaceAnomaly anomaly(2.0, 42u);
    center.handleExternalObject(anomaly, researchId);
    center.printStatistics(std::cout);

    // ===== Сценарий 6: AsteroidField + метеориты, блокировка, Destroyed =====
    section("Сценарий 6: AsteroidField, блокировка исследований, MeteorShower, Destroyed");
    center.setEnvironment(std::make_shared<AsteroidField>());
    center.tick();
    center.tick();
    MeteorShower meteors(60.0, 7u);
    for (int i = 0; i < 3; ++i) center.handleExternalObject(meteors);
    center.printStatistics(std::cout);

    // ===== Сценарий 7: взаимодействие модулей (передача энергии) =====
    section("Сценарий 7: передача энергии между модулями");
    if (center.findModule(reactorId) && center.findModule(crewId)) {
        bool ok = center.transferEnergy(reactorId, crewId, 10.0);
        std::cout << "Передача реактор → экипаж: " << (ok ? "успешно" : "не удалась") << "\n";
    }

    // ===== Сценарий 8: копирование / перенос центра =====
    section("Сценарий 8: конструкторы копирования и перемещения CommandCenter");
    CommandCenter copy(center);
    std::cout << "Копия: модулей " << copy.getModuleCount() << ", урон " << copy.getTotalDamage() << "\n";
    if (Module* m = copy.findModule(crewId)) {
        (void)(*m - Damage{ 20.0, DamageType::Physical });
        std::cout << "Урон копии: " << copy.getTotalDamage()
            << ", урон оригинала: " << center.getTotalDamage() << " (не изменился)\n";
    }
    CommandCenter moved(std::move(copy));
    std::cout << "После move: модулей " << moved.getModuleCount() << "\n";

    section("Итоги логирования");
    std::cout << "ENV=" << LoggingProxy::count(LogSource::ENV)
        << " OBJECT=" << LoggingProxy::count(LogSource::OBJECT)
        << " MODULE=" << LoggingProxy::count(LogSource::MODULE)
        << " CENTER=" << LoggingProxy::count(LogSource::CENTER)
        << " всего=" << LoggingProxy::total() << "\n";
    return 0;
}