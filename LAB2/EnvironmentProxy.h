#pragma once
#include <cstddef>
#include <memory>
#include <unordered_map>

#include "Environment.h"

// Прокси: проверки + логирование + кеш результатов (один эффект на модуль за ход).
class EnvironmentProxy : public Environment {
public:
    struct EffectRecord {
        long turn = -1;
        double healthDelta = 0.0;
    };

    explicit EnvironmentProxy(std::shared_ptr<Environment> real);

    void applyEffect(Module& module) override;
    std::string getName() const override;
    double getEnergyModifier() const override;
    double getDamageModifier() const override;

    void nextTurn() { ++turn_; }
    const std::shared_ptr<Environment>& getReal() const { return realEnvironment_; }
    const EffectRecord* lastEffect(int moduleId) const;
    std::size_t cacheHits() const { return hits_; }
    std::size_t cacheMisses() const { return misses_; }

private:
    std::shared_ptr<Environment> realEnvironment_;
    std::unordered_map<int, EffectRecord> cache_;
    long turn_ = 0;
    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
};