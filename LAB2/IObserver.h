#pragma once
#include "ModuleEvent.h"

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onModuleEvent(const ModuleEvent& event) = 0;
};