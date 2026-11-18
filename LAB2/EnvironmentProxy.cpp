#include "EnvironmentProxy.h"

#include <stdexcept>
#include <utility>

#include "LoggingProxy.h"
#include "Module.h"

using LS = LogSource;

EnvironmentProxy::EnvironmentProxy(std::shared_ptr<Environment> real) : realEnvironment_(std::move(real)) {
    if (!realEnvironment_) throw std::invalid_argument("EnvironmentProxy: null environment");
}

std::string EnvironmentProxy::getName() const { return realEnvironment_->getName(); }
double EnvironmentProxy::getEnergyModifier() const { return realEnvironment_->getEnergyModifier(); }
double EnvironmentProxy::getDamageModifier() const { return realEnvironment_->getDamageModifier(); }

const EnvironmentProxy::EffectRecord* EnvironmentProxy::lastEffect(int id) const {
    auto it = cache_.find(id);
    return it == cache_.end() ? nullptr : &it->second;
}

void EnvironmentProxy::applyEffect(Module& module) {
    // 1. Проверка состояния модуля
    if (module.isDestroyed()) {
        LoggingProxy::log(LS::ENV, "Proxy: отказ — " + module.label() + " уничтожен");
        return;
    }
    // 2. Проверка типа модуля (исследовательские — под особым контролем)
    if (module.isResearch()) {
        LoggingProxy::log(LS::ENV, "Proxy: проверка доступа исследовательского модуля " + module.label() +
            " в среде «" + getName() + "»");
    }
    // 3. Кеш: повторное применение эффекта к модулю в том же ходу не выполняется
    auto it = cache_.find(module.getId());
    if (it != cache_.end() && it->second.turn == turn_) {
        ++hits_;
        LoggingProxy::log(LS::ENV, "Proxy: кеш — эффект на " + module.label() + " уже применён в этом ходу (ΔHP " +
            LoggingProxy::fmt(it->second.healthDelta) + ")");
        return;
    }
    // 4. Делегирование реальному окружению
    const double before = module.getHealth();
    realEnvironment_->applyEffect(module);
    cache_[module.getId()] = EffectRecord{ turn_, before - module.getHealth() };
    ++misses_;
}