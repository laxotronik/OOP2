#pragma once
#include <algorithm>
#include <vector>

#include "IObserver.h"
#include "ModuleEvent.h"

class IObservable {
public:
    IObservable() = default;
    // Подписчики НЕ копируются: копия модуля не должна уведомлять чужой центр.
    IObservable(const IObservable&) {}
    IObservable& operator=(const IObservable&) { return *this; }
    virtual ~IObservable() = default;

    virtual void subscribe(IObserver* observer) {
        if (observer && std::find(observers_.begin(), observers_.end(), observer) == observers_.end())
            observers_.push_back(observer);
    }
    virtual void unsubscribe(IObserver* observer) {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }
    virtual void notify(const ModuleEvent& event) {
        const std::vector<IObserver*> snapshot = observers_;  // копия нарочно: подписчик может отписаться во время обработки
        for (IObserver* o : snapshot) o->onModuleEvent(event);
    }

private:
    std::vector<IObserver*> observers_;  // не владеет
};