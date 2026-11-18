#pragma once
#include <string>

class Module;

// Стратегия: как именно событие влияет на конкретный тип модуля.
class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() = default;
    virtual void apply(Module& module) = 0;
    virtual std::string name() const = 0;
};