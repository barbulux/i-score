#pragma once
#include <QString>
#include <QMetaType>

namespace Ossia
{
struct OSCSpecificSettings
{
    int inputPort{};
    int outputPort{};
    QString host;
};
}

W_REGISTER_ARGTYPE(Ossia::OSCSpecificSettings)
