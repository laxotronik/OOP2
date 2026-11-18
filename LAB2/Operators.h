#pragma once
#include "Damage.h"
#include "ResourcePackage.h"

class CommandCenter;
class Module;

CommandCenter& operator+(CommandCenter& center, const ResourcePackage& pkg);  // добавить ресурсы
Module& operator-(Module& module, const Damage& damage);                       // нанести урон
Module& operator*(Module& module, double modifier);                            // модифицировать характеристики