#pragma once
#include <QMetaType>

namespace Scenario
{
enum class ConstraintExecutionState {
    Enabled, Disabled
};
}
W_REGISTER_ARGTYPE(Scenario::ConstraintExecutionState)
