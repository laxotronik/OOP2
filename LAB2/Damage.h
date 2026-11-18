#pragma once

enum class DamageType { Radiation, Physical, Electronic, Suffocation };

struct Damage {
    double amount;
    DamageType type = DamageType::Physical;
};

inline const char* toString(DamageType t) {
    switch (t) {
    case DamageType::Radiation:   return "radiation";
    case DamageType::Physical:    return "physical";
    case DamageType::Electronic:  return "electronic";
    case DamageType::Suffocation: return "suffocation";
    }
    return "?";
}