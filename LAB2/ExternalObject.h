#pragma once
#include <string>
#include <utility>

class CommandCenter;
class Module;

class ExternalObject {
public:
    ExternalObject(std::string type, double strength) : type_(std::move(type)), strength_(strength) {}
    virtual ~ExternalObject() = default;

    virtual void interact(CommandCenter& center, Module& module) = 0;
    // true — воздействует на все модули станции, false — на один целевой модуль
    virtual bool isAreaEffect() const { return false; }

    const std::string& getType() const { return type_; }
    double getStrength() const { return strength_; }

protected:
    std::string type_;
    double strength_;
};