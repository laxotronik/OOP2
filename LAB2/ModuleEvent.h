#pragma once
#include <chrono>

enum class ModuleEventType { Activated, Damaged, Destroyed, EnergyConsumed };

struct ModuleEvent {
    ModuleEventType type;
    int moduleId;
    double damage;  // для Damaged / Destroyed
    std::chrono::system_clock::time_point timestamp;
    double energy = 0.0;  // для EnergyConsumed
};