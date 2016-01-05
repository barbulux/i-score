#include <API/Headers/Editor/Scenario.h>
#include <API/Headers/Editor/TimeConstraint.h>
#include <API/Headers/Editor/TimeEvent.h>
#include <API/Headers/Editor/TimeNode.h>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include <core/document/Document.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <OSSIA/iscore2OSSIA.hpp>
#include <QDebug>
#include <algorithm>
#include <vector>

#include "Editor/State.h"
#include "Editor/TimeValue.h"
#include <OSSIA/Executor/ConstraintElement.hpp>
#include <OSSIA/Executor/EventElement.hpp>
#include <OSSIA/Executor/ProcessElement.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
#include <OSSIA/Executor/StateElement.hpp>
#include <OSSIA/Executor/TimeNodeElement.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Event/ExecutionStatus.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include "ScenarioElement.hpp"
#include <iscore/tools/IdentifiedObjectMap.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <OSSIA/Executor/ExecutorContext.hpp>

namespace Process { class ProcessModel; }
class QObject;
namespace OSSIA {
class TimeProcess;
}  // namespace OSSIA


namespace RecreateOnPlay
{
ScenarioElement::ScenarioElement(
        ConstraintElement& parentConstraint,
        Scenario::ScenarioModel& element,
        const Context& ctx,
        const Id<iscore::Component>& id,
        QObject* parent):
    ProcessComponent{parentConstraint, element, id, "ScenarioComponent", parent},
    m_ctx{ctx}
{
    this->setObjectName("OSSIAScenarioElement");

    // Setup of the OSSIA API Part
    m_ossia_process = OSSIA::Scenario::create();

    // Create elements for the existing stuff. (e.g. start/ end timenode / event)
    for(const auto& timenode : element.timeNodes)
    {
        on_timeNodeCreated(timenode);
    }
    for(const auto& event : element.events)
    {
        on_eventCreated(event);
    }
    for(const auto& state : element.states)
    {
        on_stateCreated(state);
    }
    for(const auto& constraint : element.constraints)
    {
        on_constraintCreated(constraint);
    }
}

void ScenarioElement::stop()
{
    m_executingConstraints.clear();
    ProcessComponent::stop();
}

static void ScenarioConstraintCallback(const OSSIA::TimeValue&,
                               const OSSIA::TimeValue&,
                               std::shared_ptr<OSSIA::StateElement> element)
{

}


void ScenarioElement::on_constraintCreated(const ConstraintModel& const_constraint)
{
    auto& iscore_scenario = static_cast<Scenario::ScenarioModel&>(m_iscore_process);
    auto& ossia_scenario = dynamic_cast<OSSIA::Scenario&>(*m_ossia_process.get());
    auto& cst = const_cast<ConstraintModel&>(const_constraint);
    // TODO have a ConstraintPlayAspect to prevent this const_cast.
    ISCORE_ASSERT(m_ossia_timeevents.find(iscore_scenario.state(cst.startState()).eventId()) != m_ossia_timeevents.end());
    auto& ossia_sev = m_ossia_timeevents.at(iscore_scenario.state(cst.startState()).eventId());
    ISCORE_ASSERT(m_ossia_timeevents.find(iscore_scenario.state(cst.endState()).eventId()) != m_ossia_timeevents.end());
    auto& ossia_eev = m_ossia_timeevents.at(iscore_scenario.state(cst.endState()).eventId());

    auto ossia_cst = OSSIA::TimeConstraint::create(
                ScenarioConstraintCallback,
                ossia_sev->OSSIAEvent(),
                ossia_eev->OSSIAEvent(),
                iscore::convert::time(cst.duration.defaultDuration()),
                iscore::convert::time(cst.duration.minDuration()),
                iscore::convert::time(cst.duration.maxDuration()));


    ossia_scenario.addTimeConstraint(ossia_cst);

    // Create the mapping object
    auto elt = new ConstraintElement{ossia_cst, cst, m_ctx, this};
    m_ossia_constraints.insert({cst.id(), elt});
}

void ScenarioElement::on_stateCreated(const StateModel &iscore_state)
{
    ISCORE_ASSERT(m_ossia_timeevents.find(iscore_state.eventId()) != m_ossia_timeevents.end());
    auto ossia_ev = m_ossia_timeevents.at(iscore_state.eventId());

    // Create the mapping object
    auto root_state = OSSIA::State::create();
    ossia_ev->OSSIAEvent()->addState(root_state);

    auto state_elt = new StateElement{
            iscore_state,
            root_state,
            m_ctx.devices.list(),
            this};

    m_ossia_states.insert({iscore_state.id(), state_elt});
}

void ScenarioElement::on_eventCreated(const EventModel& const_ev)
{
    // TODO have a EventPlayAspect too
    auto& ev = const_cast<EventModel&>(const_ev);
    ISCORE_ASSERT(m_ossia_timenodes.find(ev.timeNode()) != m_ossia_timenodes.end());
    auto ossia_tn = m_ossia_timenodes.at(ev.timeNode());

    auto ossia_ev = *ossia_tn->OSSIATimeNode()->emplace(
                ossia_tn->OSSIATimeNode()->timeEvents().begin(),
                OSSIA::TimeEvent::ExecutionCallback{});

    // Create the mapping object
    auto elt = new EventElement{ossia_ev, ev, m_ctx.devices.list(), this};
    m_ossia_timeevents.insert({ev.id(), elt});

    elt->OSSIAEvent()->setCallback([=] (OSSIA::TimeEvent::Status st) {
        return eventCallback(*elt, st);
    });
}

void ScenarioElement::on_timeNodeCreated(const TimeNodeModel& tn)
{
    auto& iscore_scenario = static_cast<Scenario::ScenarioModel&>(m_iscore_process);
    auto& ossia_scenario = dynamic_cast<OSSIA::Scenario&>(*m_ossia_process.get());
    std::shared_ptr<OSSIA::TimeNode> ossia_tn;
    if(&tn == &iscore_scenario.startTimeNode())
    {
        ossia_tn = ossia_scenario.getStartTimeNode();
    }
    else if(&tn == &iscore_scenario.endTimeNode())
    {
        ossia_tn = ossia_scenario.getEndTimeNode();
    }
    else
    {
        ossia_tn = OSSIA::TimeNode::create();
        ossia_scenario.addTimeNode(ossia_tn);
    }

    // Create the mapping object
    auto elt = new TimeNodeElement{ossia_tn, tn, m_ctx.devices.list(), this};
    m_ossia_timenodes.insert({tn.id(), elt});
}

void ScenarioElement::startConstraintExecution(const Id<ConstraintModel>& id)
{
    auto& iscore_scenario = static_cast<Scenario::ScenarioModel&>(m_iscore_process);
    auto& cst = iscore_scenario.constraints.at(id);
    if(m_executingConstraints.find(id) == m_executingConstraints.end())
        m_executingConstraints.insert(&cst);

    m_ossia_constraints.at(id)->executionStarted();
}

void ScenarioElement::stopConstraintExecution(const Id<ConstraintModel>& id)
{
    m_executingConstraints.remove(id);
    m_ossia_constraints.at(id)->executionStopped();
}

void ScenarioElement::eventCallback(
        EventElement& ev,
        OSSIA::TimeEvent::Status newStatus)
{
    auto& iscore_scenario = static_cast<Scenario::ScenarioModel&>(m_iscore_process);
    auto the_event = const_cast<EventModel*>(&ev.iscoreEvent());
    the_event->setStatus(static_cast<ExecutionStatus>(newStatus));

    for(auto& state : the_event->states())
    {
        auto& iscore_state = iscore_scenario.states.at(state);

        switch(newStatus)
        {
            case OSSIA::TimeEvent::Status::NONE:
                break;
            case OSSIA::TimeEvent::Status::PENDING:
                break;
            case OSSIA::TimeEvent::Status::HAPPENED:
            {
                // Stop the previous constraints clocks,
                // start the next constraints clocks
                if(iscore_state.previousConstraint())
                {
                    stopConstraintExecution(iscore_state.previousConstraint());
                }

                if(iscore_state.nextConstraint())
                {
                    startConstraintExecution(iscore_state.nextConstraint());
                }
                break;
            }

            case OSSIA::TimeEvent::Status::DISPOSED:
            {
                // TODO disable the constraints graphically
                break;
            }
            default:
                ISCORE_TODO;
                break;
        }
    }
}

const iscore::Component::Key &ScenarioElement::key() const
{
    static iscore::Component::Key k("OSSIAScenarioElement");
    return k;
}

ScenarioComponentFactory::~ScenarioComponentFactory()
{

}

ProcessComponent *ScenarioComponentFactory::make(
        ConstraintElement &cst,
        Process::ProcessModel &proc,
        const Context& ctx,
        const Id<iscore::Component> &id,
        QObject *parent) const
{
    return new ScenarioElement{cst, static_cast<Scenario::ScenarioModel&>(proc), ctx, id, parent};
}

const ScenarioComponentFactory::factory_key_type& ScenarioComponentFactory::key_impl() const
{
    static ScenarioComponentFactory::factory_key_type k("OSSIAScenarioElement");
    return k;
}

bool ScenarioComponentFactory::matches(
        Process::ProcessModel& proc,
        const DocumentPlugin &,
        const iscore::DocumentContext &) const
{
    return dynamic_cast<Scenario::ScenarioModel*>(&proc);
}

}
