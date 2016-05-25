#pragma once
#include <QMetaType>

namespace Scenario{
enum class Tool { Disabled, Create, Select, Play, MoveSlot, Playing };
}
W_REGISTER_ARGTYPE(Scenario::Tool)
