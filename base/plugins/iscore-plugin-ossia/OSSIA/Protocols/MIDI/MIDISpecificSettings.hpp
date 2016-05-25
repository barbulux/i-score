#pragma once
#include <QMetaType>
#include <QJsonObject>

namespace Ossia
{
struct MIDISpecificSettings
{
        enum class IO { In, Out } io;
        QString endpoint;
};
}
W_REGISTER_ARGTYPE(Ossia::MIDISpecificSettings)

