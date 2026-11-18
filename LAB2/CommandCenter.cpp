#include "CommandCenter.h"

#include <algorithm>
#include <ostream>
#include <stdexcept>

#include "Environments.h"
#include "ExternalObject.h"
#include "LoggingProxy.h"
#include "ModuleFactory.h"
#include "Operators.h"

using LS = LogSource;
using LP = LoggingProxy;

namespace {
    std::shared_ptr<EnvironmentProxy> makeProxy(std::shared_ptr<Environment> env) {
        return std::make_shared<EnvironmentProxy>(std::move(env));
    }
}  

CommandCenter::CommandCenter(double energy, double oxygen, int safetyLevel, std::size_t maxModules)
    : energy_(energy), oxygen_(oxygen), safetyLevel_(safetyLevel), maxModules_(maxModules),
    currentEnvironment_(makeProxy(std::make_shared<EarthOrbit>())) {}

// правило пяти 
void CommandCenter::cloneFrom(const CommandCenter& o) {
    energy_ = o.energy_; oxygen_ = o.oxygen_; safetyLevel_ = o.safetyLevel_;
    maxModules_ = o.maxModules_; totalDamage_ = o.totalDamage_;
    nextModuleId_ = o.nextModuleId_; turn_ = o.turn_;
    pendingRemoval_ = o.pendingRemoval_;
    currentEnvironment_ = makeProxy(o.currentEnvironment_->getReal());
    modules_.clear();
    for (const auto& m : o.modules_) {
        auto copy = m->clone();       
        copy->subscribe(this);        
        modules_.push_back(std::move(copy));
    }
}

void CommandCenter::detachAll() {
    for (auto& m : modules_) m->unsubscribe(this);
}

CommandCenter::CommandCenter(const CommandCenter& o)
    : energy_(0), oxygen_(0), safetyLevel_(0), maxModules_(0) {
    cloneFrom(o);
}

CommandCenter::CommandCenter(CommandCenter&& o)
    : energy_(o.energy_), oxygen_(o.oxygen_), safetyLevel_(o.safetyLevel_), maxModules_(o.maxModules_),
    modules_(std::move(o.modules_)), currentEnvironment_(std::move(o.currentEnvironment_)),
    pendingRemoval_(std::move(o.pendingRemoval_)), totalDamage_(o.totalDamage_),
    nextModuleId_(o.nextModuleId_), turn_(o.turn_) {
    for (auto& m : modules_) {
        m->unsubscribe(&o);
        m->subscribe(this);
    }
    o.modules_.clear();
    o.currentEnvironment_ = makeProxy(std::make_shared<EarthOrbit>());  
}

CommandCenter& CommandCenter::operator=(const CommandCenter& o) {
    if (this != &o) {
        detachAll();
        cloneFrom(o);
    }
    return *this;
}

CommandCenter& CommandCenter::operator=(CommandCenter&& o) {
    if (this != &o) {
        detachAll();
        energy_ = o.energy_; oxygen_ = o.oxygen_; safetyLevel_ = o.safetyLevel_;
        maxModules_ = o.maxModules_; totalDamage_ = o.totalDamage_;
        nextModuleId_ = o.nextModuleId_; turn_ = o.turn_;
        pendingRemoval_ = std::move(o.pendingRemoval_);
        modules_ = std::move(o.modules_);
        currentEnvironment_ = std::move(o.currentEnvironment_);
        for (auto& m : modules_) {
            m->unsubscribe(&o);
            m->subscribe(this);
        }
        o.modules_.clear();
        o.currentEnvironment_ = makeProxy(std::make_shared<EarthOrbit>());
    }
    return *this;
}

CommandCenter::~CommandCenter() { detachAll(); }

//  модули 
bool CommandCenter::addModule(std::unique_ptr<Module> module) {
    if (!module) return false;
    if (modules_.size() >= maxModules_) {
        LP::log(LS::CENTER, "Лимит модулей достигнут (" + std::to_string(maxModules_) + ")");
        return false;
    }
    module->subscribe(this);          
    Module* raw = module.get();
    modules_.push_back(std::move(module));
    raw->activate();                  
    return true;
}

Module* CommandCenter::createModule(const ModuleFactory& factory) {
    if (modules_.size() >= maxModules_) {
        LP::log(LS::CENTER, "Нельзя создать " + factory.typeName() + ": достигнут лимит модулей");
        return nullptr;
    }
    const double cost = factory.cost();
    if (energy_ < cost) {
        LP::log(LS::CENTER, "Нельзя создать " + factory.typeName() + ": нужно " + LP::fmt(cost) +
            " энергии, есть " + LP::fmt(energy_));
        return nullptr;
    }
    auto module = factory.create(nextModuleId_++);
    Module* raw = module.get();
    energy_ -= cost;
    LP::log(LS::CENTER, "Создан " + raw->label() + " (стоимость " + LP::fmt(cost) + ")");
    return addModule(std::move(module)) ? raw : nullptr;
}

Module* CommandCenter::findModule(int id) {
    for (auto& m : modules_)
        if (m->getId() == id) return m.get();
    return nullptr;
}

bool CommandCenter::transferEnergy(int fromId, int toId, double amount) {
    Module* from = findModule(fromId);
    Module* to = findModule(toId);
    if (!from || !to || from == to) return false;
    return from->transferEnergyTo(*to, amount);
}

void CommandCenter::purgeDestroyed() {
    if (pendingRemoval_.empty()) return;
    for (int id : pendingRemoval_) {
        auto it = std::find_if(modules_.begin(), modules_.end(),
            [id](const std::unique_ptr<Module>& m) { return m->getId() == id; });
        if (it != modules_.end()) {
            (*it)->unsubscribe(this);
            LP::log(LS::CENTER, "Модуль " + (*it)->label() + " удалён из списка");
            modules_.erase(it);
        }
    }
    pendingRemoval_.clear();
}

// окружение 
void CommandCenter::setEnvironment(std::shared_ptr<Environment> env) {
    if (!env) throw std::invalid_argument("CommandCenter::setEnvironment: null");
    currentEnvironment_ = makeProxy(std::move(env));  
    for (auto& m : modules_) m->unblock();            
    LP::log(LS::CENTER, "Окружение изменено: " + currentEnvironment_->getName());
}

void CommandCenter::handleExternalObject(ExternalObject& object, int targetModuleId) {
    LP::log(LS::CENTER, "Обнаружен внешний объект: " + object.getType() +
        " (сила " + LP::fmt(object.getStrength()) + ")");
    if (object.isAreaEffect()) {
        for (std::size_t i = 0; i < modules_.size(); ++i) {  
            Module& m = *modules_[i];
            if (!m.isDestroyed()) object.interact(*this, m);
        }
    }
    else {
        Module* target = nullptr;
        if (targetModuleId >= 0) {
            target = findModule(targetModuleId);
        }
        else {
            for (auto& m : modules_)
                if (!m->isDestroyed()) { target = m.get(); break; }
        }
        if (!target) {
            LP::log(LS::CENTER, "Нет подходящего модуля для взаимодействия с " + object.getType());
            return;
        }
        object.interact(*this, *target);
    }
    purgeDestroyed();
}

//  симуляция 
void CommandCenter::receiveResources(const ResourcePackage& pkg) {
    energy_ += pkg.energy;
    oxygen_ += pkg.oxygen;
    safetyLevel_ += pkg.safetyLevel;
    LP::log(LS::CENTER, "Получены ресурсы: энергия +" + LP::fmt(pkg.energy) + ", кислород +" +
        LP::fmt(pkg.oxygen) + ", безопасность +" + std::to_string(pkg.safetyLevel));
}

void CommandCenter::tick() {
    ++turn_;
    currentEnvironment_->nextTurn();
    const double envEnergy = currentEnvironment_->getEnergyModifier();
    LP::log(LS::CENTER, "--- Ход " + std::to_string(turn_) + " [" + currentEnvironment_->getName() + "] ---");

    for (auto& up : modules_) {
        Module& m = *up;
        if (m.isDestroyed()) continue;
        m.advanceTime();
        currentEnvironment_->applyEffect(m);        
        if (m.isDestroyed()) continue;
        if (m.isLiving()) oxygen_ -= m.oxygenConsumption();
        if (!m.isOperational()) continue;
        m.consumeEnergy(envEnergy);                 
        const double produced = m.energyProduction();
        if (produced > 0.0) {
            m.addReserve(produced * 0.2);
            energy_ += produced * 0.8;
        }
        m.work();
    }

    if (energy_ < 0.0) {
        energy_ = 0.0;
        LP::log(LS::CENTER, "ВНИМАНИЕ: энергия исчерпана (blackout)");
    }
    if (oxygen_ <= 0.0) {
        oxygen_ = 0.0;
        LP::log(LS::CENTER, "ВНИМАНИЕ: кислород закончился — экипаж в опасности");
        for (auto& up : modules_)
            if (up->isLiving() && !up->isDestroyed())
                (void)(*up - Damage{ 5.0, DamageType::Suffocation });
    }
    purgeDestroyed();
}

//  Observer 
void CommandCenter::onModuleEvent(const ModuleEvent& e) {
    switch (e.type) {
    case ModuleEventType::Activated:
        LP::log(LS::CENTER, "Событие: модуль #" + std::to_string(e.moduleId) + " активирован");
        break;
    case ModuleEventType::Damaged: {
        totalDamage_ += e.damage;
        const int drop = e.damage >= 10.0 ? 2 : (e.damage >= 3.0 ? 1 : 0);
        safetyLevel_ = std::max(0, safetyLevel_ - drop);
        LP::log(LS::CENTER, "Событие: модуль #" + std::to_string(e.moduleId) + " повреждён (" +
            LP::fmt(e.damage) + "), безопасность → " + std::to_string(safetyLevel_));
        break;
    }
    case ModuleEventType::Destroyed:
        pendingRemoval_.push_back(e.moduleId);
        LP::log(LS::CENTER, "Событие: модуль #" + std::to_string(e.moduleId) + " уничтожен");
        break;
    case ModuleEventType::EnergyConsumed:
        energy_ -= e.energy;
        break;
    }
}

void CommandCenter::printStatistics(std::ostream& os) const {
    os << "===== Командный центр, ход " << turn_ << " =====\n"
        << "Окружение : " << currentEnvironment_->getName()
        << " (энергия ×" << LP::fmt(currentEnvironment_->getEnergyModifier(), 2) << ")\n"
        << "Энергия   : " << LP::fmt(energy_) << "\n"
        << "Кислород  : " << LP::fmt(oxygen_) << "\n"
        << "Безопасн. : " << safetyLevel_ << "\n"
        << "Модули    : " << modules_.size() << "/" << maxModules_ << "\n"
        << "Урон всего: " << LP::fmt(totalDamage_) << "\n"
        << "Кеш Proxy : hits=" << currentEnvironment_->cacheHits()
        << ", misses=" << currentEnvironment_->cacheMisses() << "\n";
    for (const auto& m : modules_) os << "  - " << m->describe() << "\n";
    os << "======================================\n";
}