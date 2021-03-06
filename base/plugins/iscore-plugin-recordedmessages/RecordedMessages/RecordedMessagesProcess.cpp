#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <OSSIA/OSSIA2iscore.hpp>
#include <OSSIA/iscore2OSSIA.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
#include <vector>

#include "Editor/Message.h"
#include "Editor/State.h"

#include "RecordedMessagesProcess.hpp"
#include <Editor/Clock.h>
#include <Editor/TimeConstraint.h>
#include <RecordedMessages/RecordedMessagesProcessModel.hpp>
namespace OSSIA {
class StateElement;
}  // namespace OSSIA


namespace RecordedMessages
{
namespace Executor
{
ProcessExecutor::ProcessExecutor(
        const Explorer::DeviceDocumentPlugin& devices,
        const RecordedMessagesList& lst):
    m_devices{devices.list()},
    m_list{lst}
{
    m_list.detach();
}

std::shared_ptr<OSSIA::StateElement> ProcessExecutor::state()
{
    return state(parent->getPosition());
}

std::shared_ptr<OSSIA::StateElement> ProcessExecutor::state(double t)
{
    auto st = OSSIA::State::create();
    OSSIA::TimeConstraint& par_cst = *parent;

    auto cur_pos = t;
    auto span = par_cst.getGranularity();

    auto max_pos = cur_pos + span;

    // Look for all the messages

    int n = m_list.size();
    for(int i = 0; i < n; i++)
    {
        auto& mess = m_list[i];
        auto time = mess.percentage * par_cst.getDurationNominal();
        if(time >= cur_pos && time < max_pos)
        {
            qDebug() << mess.message.toString();
            auto ossia_mess = iscore::convert::message(mess.message, m_devices);
            if(ossia_mess)
                st->stateElements().push_back(ossia_mess);
        }
    }

    return st;
}

std::shared_ptr<OSSIA::StateElement> ProcessExecutor::offset(
        const OSSIA::TimeValue& off)
{
    return state(off / parent->getDurationNominal());
}

Component::Component(
        RecreateOnPlay::ConstraintElement& parentConstraint,
        RecordedMessages::ProcessModel& element,
        const RecreateOnPlay::Context& ctx,
        const Id<iscore::Component>& id,
        QObject* parent):
    ::RecreateOnPlay::ProcessComponent_T<RecordedMessages::ProcessModel>{parentConstraint, element, ctx, id, "RecordedMessagesComponent", parent}
{
    auto proc = std::make_shared<ProcessExecutor>(ctx.devices, element.messages());
    m_ossia_process = proc;
}
}
}
