#include "AutomationPresenter.hpp"
#include "AutomationModel.hpp"
#include "AutomationViewModel.hpp"
#include "AutomationView.hpp"


#include "../Commands/AddPoint.hpp"
#include "../Commands/MovePoint.hpp"
// TODO #include "../Commands/RemovePoint.hpp"


#include <iscore/document/DocumentInterface.hpp>
#include <iscore/command/OngoingCommandManager.hpp>
#include <QGraphicsSceneMouseEvent>

#include "QCustomPlotProcess/QCustomPlotCurve.hpp"


AutomationPresenter::AutomationPresenter(
        const ProcessViewModel& model,
        ProcessView* view,
        QObject* parent) :
    ProcessPresenter {"AutomationPresenter", parent},
    m_viewModel{static_cast<const AutomationViewModel&>(model)},
    m_view{static_cast<AutomationView*>(view)},
//    m_curve{new QCustomPlotCurve{m_view}},
    m_commandDispatcher{iscore::IDocument::documentFromObject(m_viewModel.sharedProcessModel())->commandStack()},
    m_focusDispatcher{*iscore::IDocument::documentFromObject(m_viewModel.sharedProcessModel())}
{
    connect(&m_viewModel.model(), &AutomationModel::pointsChanged,
            this, &AutomationPresenter::on_modelPointsChanged);

    m_curve = new Curve(m_view);
/*
    connect(m_curve, &QCustomPlotCurve::pointMovingFinished,
            [&](double oldx, double newx, double newy)
    {
        auto cmd = new MovePoint{iscore::IDocument::path(m_viewModel.model()),
                                 oldx,
                                 newx,
                                 1.0 - newy};

        m_commandDispatcher.submitCommand(cmd);
    });

    connect(m_curve, &QCustomPlotCurve::pointCreated,
            [&](QPointF pt)
    {
        auto cmd = new AddPoint{iscore::IDocument::path(m_viewModel.model()),
                   pt.x(),
                   1.0 - pt.y()};

        m_commandDispatcher.submitCommand(cmd);
    });

    // TODO emit a mousePressed signal when clicking on a dot, too.
    connect(m_curve, &QCustomPlotCurve::mousePressed,
            this, [&] ()
    {
        m_focusDispatcher.focus(this);
    });
*/
    parentGeometryChanged();
    on_modelPointsChanged();
}

AutomationPresenter::~AutomationPresenter()
{
    if(m_view)
    {
        auto sc = m_view->scene();

        if(sc)
        {
            sc->removeItem(m_view);
        }

        m_view->deleteLater();
    }
}

void AutomationPresenter::setWidth(int width)
{
    m_view->setWidth(width);
    on_modelPointsChanged();
//    m_curve->setSize({m_view->width(), m_view->height()});
}

void AutomationPresenter::setHeight(int height)
{
    m_view->setHeight(height);
    on_modelPointsChanged();
//    m_curve->setSize({m_view->width(), m_view->height()});
}

void AutomationPresenter::putToFront()
{
//    m_curve->enable();
    m_view->setFlag(QGraphicsItem::ItemStacksBehindParent, false);
}

void AutomationPresenter::putBehind()
{
//    m_curve->disable();
    m_view->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
}

void AutomationPresenter::on_zoomRatioChanged(ZoomRatio val)
{
    m_zoomRatio = val;
    on_modelPointsChanged();

//    m_curve->redraw();
//    m_curve->setSize({m_view->width(), m_view->height()});
}

void AutomationPresenter::parentGeometryChanged()
{
    on_modelPointsChanged();

//    m_curve->setSize({m_view->width(), m_view->height()});
}

const ProcessViewModel& AutomationPresenter::viewModel() const
{
    return m_viewModel;
}

const id_type<ProcessModel>& AutomationPresenter::modelId() const
{
    return m_viewModel.model().id();
}

QVector<QPointF> mapToVector(QMap<double, double> map)
{
    QVector<QPointF> list;
    for(auto key : map.keys())
    {
        list.push_back({key, 1.0 - map[key]});
    }

    return list;
}

void AutomationPresenter::on_modelPointsChanged()
{
    m_curve->setPoints(mapToVector(m_viewModel.model().points()));
    m_curve->setRect(m_view->boundingRect());
}
