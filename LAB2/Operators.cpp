#include "Operators.h"

#include "CommandCenter.h"
#include "LoggingProxy.h"
#include "Module.h"

CommandCenter& operator+(CommandCenter& center, const ResourcePackage& pkg) {
    center.receiveResources(pkg);
    return center;
}

Module& operator-(Module& module, const Damage& damage) {
    LoggingProxy::log(LogSource::MODULE, "operator-: " + module.label() + " −" +
        LoggingProxy::fmt(damage.amount) + " (" + toString(damage.type) + ")");
    module.takeDamage(damage.amount);
    return module;
}

Module& operator*(Module& module, double modifier) {
    module.scaleEfficiency(modifier);
    LoggingProxy::log(LogSource::MODULE, "operator*: " + module.label() + " эффективность ×" +
        LoggingProxy::fmt(modifier, 2) + " → " +
        LoggingProxy::fmt(module.getEfficiency(), 2));
    return module;
}