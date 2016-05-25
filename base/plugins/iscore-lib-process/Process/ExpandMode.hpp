#pragma once
#include <QMetaType>

enum ExpandMode { Scale, GrowShrink, ForceGrow, Fixed };
W_REGISTER_ARGTYPE(ExpandMode)
