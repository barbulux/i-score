#include "ScenarioViewInterface.hpp"

#include "TemporalScenarioPresenter.hpp"
#include "source/Process/ScenarioModel.hpp"
#include "source/Process/Temporal/TemporalScenarioViewModel.hpp"
#include "source/Process/Temporal/TemporalScenarioView.hpp"

#include "Document/Constraint/ViewModels/Temporal/TemporalConstraintView.hpp"
#include "Document/Constraint/ViewModels/Temporal/TemporalConstraintPresenter.hpp"
#include "Document/Constraint/ViewModels/Temporal/TemporalConstraintViewModel.hpp"
#include "Document/Constraint/ConstraintModel.hpp"

#include "Document/Event/EventModel.hpp"
#include "Document/Event/EventPresenter.hpp"
#include "Document/Event/EventView.hpp"

#include "Document/TimeNode/TimeNodeModel.hpp"
#include "Document/TimeNode/TimeNodeView.hpp"
#include "Document/TimeNode/TimeNodePresenter.hpp"

#include <QGraphicsScene>

ScenarioViewInterface::ScenarioViewInterface(TemporalScenarioPresenter* presenter) :
    QObject{presenter},
    m_presenter(presenter)
{
    connect(&m_presenter->m_viewModel, &TemporalScenarioViewModel::eventMoved,
            this, &ScenarioViewInterface::on_eventMoved);

    connect(&m_presenter->m_viewModel, &TemporalScenarioViewModel::constraintMoved,
            this, &ScenarioViewInterface::on_constraintMoved);
}

void ScenarioViewInterface::on_eventMoved(const id_type<EventModel>& eventId)
{
    auto rect = m_presenter->m_view->boundingRect();
    auto ev = m_presenter->m_events.at(eventId);

    ev->view()->setPos({(ev->model().date().msec() / m_presenter->m_zoomRatio),
                        rect.height() * ev->model().heightPercentage()
                       });

    auto timeNode = m_presenter->m_timeNodes.at(ev->model().timeNode());
    timeNode->view()->setPos({(timeNode->model().date().msec() / m_presenter->m_zoomRatio),
                              rect.height() * timeNode->model().y()});

    m_presenter->m_view->update();
}

void ScenarioViewInterface::on_constraintMoved(const id_type<ConstraintModel>& constraintId)
{
    auto rect = m_presenter->m_view->boundingRect();
    auto msPerPixel = m_presenter->m_zoomRatio;

    TemporalConstraintPresenter* pres = m_presenter->m_constraints.at(constraintId);
    const auto& cstr_model = pres->model();

    auto startPos = cstr_model.startDate().toPixels(msPerPixel);
    auto delta = view(pres)->x() - startPos;
    bool dateChanged = (delta * delta > 1); // Magnetism

    if(dateChanged)
    {
        view(pres)->setPos({startPos,
                            rect.height() * cstr_model.heightPercentage()});
    }
    else
    {
        view(pres)->setY(qreal(rect.height() * cstr_model.heightPercentage()));
    }

    view(pres)->setDefaultWidth(cstr_model.defaultDuration().toPixels(msPerPixel));
    view(pres)->setMinWidth(cstr_model.minDuration().toPixels(msPerPixel));
    view(pres)->setMaxWidth(cstr_model.maxDuration().isInfinite(),
                            cstr_model.maxDuration().isInfinite()? -1 :
                                                                   cstr_model.maxDuration().toPixels(msPerPixel));

    auto endEvent = cstr_model.endEvent();
    updatePointInEvent(endEvent, constraintId, cstr_model.heightPercentage());

    auto startEvent= cstr_model.startEvent();
    updatePointInEvent(startEvent, constraintId, cstr_model.heightPercentage());

    m_presenter->m_view->update();
}

template<typename T>
void update_min_max(const T& val, T& min, T& max)
{
    min = val < min ? val : min;
    max = val > max ? val : max;
}

void ScenarioViewInterface::updateTimeNode(const id_type<TimeNodeModel>& timeNodeId)
{
    auto timeNode = m_presenter->m_timeNodes.at(timeNodeId);
    auto h =  m_presenter->m_view->boundingRect().height();

    double min = 1.0;
    double max = 0.0;

    double tnMin = 1.0;
    double tnMax = 0.0;

    for(const auto& eventId : timeNode->model().events())
    {
        EventPresenter* event = m_presenter->m_events.at(eventId);

        for(const auto& constraint_id : event->model().previousConstraints())
        {
            auto cstr_pres = m_presenter->m_constraints.at(constraint_id);
            update_min_max(cstr_pres->model().heightPercentage(), min, max);
        }

        for(const auto& constraint_id : event->model().nextConstraints())
        {
            auto cstr_pres = m_presenter->m_constraints.at(constraint_id);
            update_min_max(cstr_pres->model().heightPercentage(), min, max);
        }

        update_min_max(min, tnMin, tnMax);
        update_min_max(max, tnMin, tnMax);

        min -= event->model().heightPercentage();
        max -= event->model().heightPercentage();
        event->view()->setExtremities(int(h*min), int(h*max));

        min = 1.;
        max = 0.;
    }

    tnMin -= timeNode->model().y();
    tnMax -= timeNode->model().y();

    timeNode->view()->setExtremities(int (h * tnMin), int (h * tnMax));
}

void ScenarioViewInterface::addPointInEvent(const id_type<EventModel> &eventId, double y)
{
    auto event = m_presenter->m_events.at(eventId);
    auto h = m_presenter->m_view->boundingRect().height();
    event->view()->addPoint(int (h * (y - event->model().heightPercentage())));

    auto tn = m_presenter->m_timeNodes.at(event->model().timeNode());
    tn->view()->addPoint(int(h * (y - tn->model().y()) ));
}

void ScenarioViewInterface::updatePointInEvent(const id_type<EventModel> &eventId, const id_type<ConstraintModel> &cstrId, double newValue)
{
    auto event = m_presenter->m_events.at(eventId);
    auto h = m_presenter->m_view->boundingRect().height();
    auto timeNode = m_presenter->m_timeNodes.at(event->model().timeNode());

    if(cstrId != event->extremityMin().first && cstrId != event->extremityMax().first)
    {
        if(newValue < event->extremityMin().second)
        {
            event->updateMinExtremities(cstrId, newValue);
            event->view()->setExtremities(int (h * newValue), int (h * event->extremityMax().second));
        }
        else if(newValue > event->extremityMax().second)
        {
            event->updateMaxExtremities(cstrId, newValue);
            event->view()->setExtremities(int (h * event->extremityMin().second), int (h *newValue ));
        }
    }
    if(cstrId != timeNode->extremityMin().first && cstrId != timeNode->extremityMax().first)
    {
        if(newValue < timeNode->extremityMin().second)
        {
            timeNode->updateMinExtremities(cstrId, newValue);
            timeNode->view()->setExtremities(int (h * newValue), int (h * timeNode->extremityMax().second));
        }
        else if(newValue > timeNode->extremityMax().second)
        {
            timeNode->updateMaxExtremities(cstrId, newValue);
            timeNode->view()->setExtremities(int (h * timeNode->extremityMin().second), int (h *newValue ));
        }
    }
    else
        updateTimeNode(timeNode->id());
}

void ScenarioViewInterface::on_hoverOnConstraint(const id_type<ConstraintModel>& constraintId, bool enter)
{
    const auto& constraint = m_presenter->m_constraints.at(constraintId)->model();
    EventPresenter* start = m_presenter->m_events.at(constraint.startEvent());
    start->view()->setShadow(enter);
    EventPresenter* end = m_presenter->m_events.at(constraint.endEvent());
    end->view()->setShadow(enter);
}

void ScenarioViewInterface::on_hoverOnEvent(const id_type<EventModel>& eventId, bool enter)
{
    const auto& event = m_presenter->m_events.at(eventId)->model();
    for (const auto& cstr : event.constraints())
    {
        auto cstrView = view(m_presenter->m_constraints.at(cstr));
        cstrView->setShadow(enter);
    }
}
