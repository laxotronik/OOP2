#include "Module.h"

#include <algorithm>
#include <sstream>

#include "LoggingProxy.h"

using LS = LogSource;

Module::Module(int id, std::shared_ptr<const ModuleTypeInfo> info, ModuleKind kind)
    : id_(id), kind_(kind), info_(std::move(info)),
    health_(info_->maxHealth), efficiency_(info_->baseEfficiency), sector_(id) {}

std::string Module::label() const { return info_->name + "#" + std::to_string(id_); }

void Module::emit(ModuleEventType type, double damage, double energy) {
    ModuleEvent e{ type, id_, damage, std::chrono::system_clock::now(), energy };
    notify(e);
}

void Module::activate() {
    if (isDestroyed() || active_) return;
    active_ = true;
    LoggingProxy::log(LS::MODULE, label() + " активирован");
    emit(ModuleEventType::Activated);
}

void Module::deactivate() {
    if (!active_) return;
    active_ = false;
    LoggingProxy::log(LS::MODULE, label() + " отключён");
}

void Module::block() {
    if (blocked_ || isDestroyed()) return;
    blocked_ = true;
    LoggingProxy::log(LS::MODULE, label() + " ЗАБЛОКИРОВАН");
}

void Module::unblock() {
    if (!blocked_) return;
    blocked_ = false;
    LoggingProxy::log(LS::MODULE, label() + " разблокирован");
}

void Module::takeDamage(double amount) {
    if (isDestroyed() || amount <= 0.0) return;
    health_ = std::max(0.0, health_ - amount);
    LoggingProxy::log(LS::MODULE, label() + " получил урон " + LoggingProxy::fmt(amount) +
        " (HP " + LoggingProxy::fmt(health_) + ")");
    emit(ModuleEventType::Damaged, amount);
    if (health_ <= 0.0) {
        active_ = false;
        LoggingProxy::log(LS::MODULE, label() + " УНИЧТОЖЕН");
        emit(ModuleEventType::Destroyed, amount);
    }
}

void Module::repair(double amount) {
    if (isDestroyed() || amount <= 0.0) return;
    health_ = std::min(info_->maxHealth, health_ + amount);
}

void Module::scaleEfficiency(double factor) {
    efficiency_ = std::clamp(efficiency_ * factor, 0.0, kMaxEfficiency);
}

void Module::consumeEnergy(double envModifier) {
    const double need = info_->baseEnergyConsumption * envModifier;
    const double fromReserve = std::min(reserve_, need);
    reserve_ -= fromReserve;
    const double fromCenter = need - fromReserve;
    if (fromCenter > 0.0) emit(ModuleEventType::EnergyConsumed, 0.0, fromCenter);
}

void Module::addReserve(double amount) {
    reserve_ = std::min(kMaxReserve, reserve_ + amount);
}

bool Module::transferEnergyTo(Module& other, double amount) {
    const double a = std::min(amount, reserve_);
    if (a <= 0.0 || other.isDestroyed()) return false;
    reserve_ -= a;
    other.addReserve(a);
    LoggingProxy::log(LS::MODULE, label() + " передал " + LoggingProxy::fmt(a) +
        " энергии модулю " + other.label());
    return true;
}

std::string Module::describe() const {
    std::ostringstream os;
    const char* state = isDestroyed() ? "DESTROYED" : blocked_ ? "BLOCKED" : active_ ? "ACTIVE" : "IDLE";
    os << label() << " [HP " << LoggingProxy::fmt(health_) << "/" << LoggingProxy::fmt(info_->maxHealth)
        << ", eff " << LoggingProxy::fmt(efficiency_, 2) << ", " << state
        << ", sector " << sector_ << ", reserve " << LoggingProxy::fmt(reserve_) << "]";
    return os.str();
}