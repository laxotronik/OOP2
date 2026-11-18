#include <iostream>

#include "CommandCenter.h"
#include "Environments.h"
#include "ExternalObjects.h"
#include "LoggingProxy.h"
#include "ModuleFactory.h"
#include "Modules.h"
#include "Operators.h"
#include "Strategies.h"

static int g_failures = 0;
#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << "FAIL line " << __LINE__ << ": " #cond "\n";            \
            ++g_failures;                                                        \
        }                                                                        \
    } while (0)

void testObserver() {
    CommandCenter c(1000, 100, 10, 5);
    EnergyModuleFactory f;
    Module* m = c.createModule(f);
    CHECK(m != nullptr);
    (void)(*m - Damage{ 15.0, DamageType::Physical });
    CHECK(c.getTotalDamage() == 15.0);   // центр получил уведомление Damaged
    CHECK(c.getSafetyLevel() == 8);      // 15 >= 10 → −2
    (void)(*m - Damage{ 1000.0, DamageType::Physical });
    c.purgeDestroyed();                  // Destroyed → удаление из списка
    CHECK(c.getModuleCount() == 0);
}

void testProxyAndEnvironments() {
    {   // AsteroidField блокирует исследовательские модули
        CommandCenter c(1000, 100, 10, 5);
        ResearchModuleFactory f;
        Module* m = c.createModule(f);
        c.setEnvironment(std::make_shared<AsteroidField>());
        c.tick();
        CHECK(m->isBlocked());
        c.setEnvironment(std::make_shared<EarthOrbit>());
        CHECK(!m->isBlocked());
    }
    {   // DeepSpace: периодические повреждения
        CommandCenter c(1000, 100, 10, 5);
        CrewModuleFactory f;
        Module* m = c.createModule(f);
        c.setEnvironment(std::make_shared<DeepSpace>());
        c.tick(); c.tick();
        CHECK(m->getHealth() == m->getMaxHealth());
        c.tick();
        CHECK(m->getHealth() < m->getMaxHealth());
    }
    {   // EarthOrbit: без повреждений
        CommandCenter c(1000, 100, 10, 5);
        CrewModuleFactory f;
        Module* m = c.createModule(f);
        for (int i = 0; i < 5; ++i) c.tick();
        CHECK(m->getHealth() == m->getMaxHealth());
    }
    {   // Кеш прокси
        EnvironmentProxy proxy(std::make_shared<DeepSpace>());
        CrewModule m(1);
        proxy.applyEffect(m);
        proxy.applyEffect(m);
        CHECK(proxy.cacheMisses() == 1 && proxy.cacheHits() == 1);
        proxy.nextTurn();
        proxy.applyEffect(m);
        CHECK(proxy.cacheMisses() == 2);
    }
}

void testStrategies() {
    CrewModule crew(1);
    ReactorModule reactor(2);
    auto s1 = selectStrategy(crew, EffectKind::Radiation, 2.0);
    auto s2 = selectStrategy(reactor, EffectKind::Radiation, 2.0);
    CHECK(s1->name() == "CrewModuleStrategy");
    CHECK(s2->name() == "ReactorModuleStrategy");
    s1->apply(crew);
    s2->apply(reactor);
    CHECK(crew.getMaxHealth() - crew.getHealth() > reactor.getMaxHealth() - reactor.getHealth());
}

void testOperators() {
    CommandCenter c(100, 50, 1, 3);
    (void)(c + ResourcePackage{ 100.0, 50.0, 1 });
    CHECK(c.getEnergy() == 200.0 && c.getOxygen() == 100.0 && c.getSafetyLevel() == 2);

    EnergyModule m(1);
    (void)(m - Damage{ 10.0, DamageType::Physical });
    CHECK(m.getHealth() == 90.0);
    (void)(m * 2.0);
    CHECK(m.getEfficiency() == 2.0);
}

void testExternalObjects() {
    CommandCenter c(1000, 100, 10, 5);
    EnergyModuleFactory f;
    c.createModule(f);
    const double before = c.getEnergy();
    CargoContainer cargo(ResourcePackage{ 300.0, 0.0, 0 });
    c.handleExternalObject(cargo);
    CHECK(c.getEnergy() == before + 300.0);

    SpaceAnomaly a(1.0, 1u, AnomalyEffect::DoubleEnergy);
    const double e = c.getEnergy();
    c.handleExternalObject(a);
    CHECK(c.getEnergy() == 2.0 * e);
}

void testCopyMove() {
    CommandCenter c(1000, 100, 10, 5);
    CrewModuleFactory f;
    c.createModule(f);
    c.createModule(f);

    CommandCenter copy(c);
    CHECK(copy.getModuleCount() == 2);
    (void)(*copy.findModule(1) - Damage{ 20.0, DamageType::Physical });
    CHECK(copy.getTotalDamage() == 20.0);
    CHECK(c.getTotalDamage() == 0.0);              // копия наблюдается копией

    CommandCenter moved(std::move(copy));
    CHECK(moved.getModuleCount() == 2);
    (void)(*moved.findModule(2) - Damage{ 20.0, DamageType::Physical });
    CHECK(moved.getTotalDamage() == 40.0);         // подписка перепривязана
}

int main() {
    LoggingProxy::setEnabled(false);
    testObserver();
    testProxyAndEnvironments();
    testStrategies();
    testOperators();
    testExternalObjects();
    testCopyMove();
    if (g_failures == 0) std::cout << "ALL TESTS PASSED\n";
    return g_failures;
}