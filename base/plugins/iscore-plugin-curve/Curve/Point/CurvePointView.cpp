#include <Curve/CurveStyle.hpp>
#include <iscore/selection/Selectable.hpp>
#include <QColor>
#include <QtGlobal>
#include <QGraphicsSceneEvent>
#include <qnamespace.h>
#include <QPainter>
#include <QPen>
#include <QCursor>

#include "CurvePointModel.hpp"
#include "CurvePointView.hpp"
#include <iscore/tools/Todo.hpp>

class QStyleOptionGraphicsItem;
class QWidget;
#include <iscore/tools/SettableIdentifier.hpp>
namespace Curve
{
const qreal radius = 2.5;
const QRectF ellipse{-radius, -radius, 2*radius, 2*radius};

PointView::PointView(
        const PointModel* model,
        const Curve::Style& style,
        QGraphicsItem* parent):
    QGraphicsItem{parent},
    m_style{style}
{
    this->setZValue(2);
    this->setCursor(Qt::CrossCursor);

    setModel(model);
}

void PointView::setModel(const PointModel* model)
{
    m_model = model;
    if(m_model)
    {
        con(m_model->selection, &Selectable::changed,
            this, &PointView::setSelected);
    }
}

const PointModel& PointView::model() const
{
    return *m_model;
}

const Id<PointModel>& PointView::id() const
{
    return m_model->id();
}

QRectF PointView::boundingRect() const
{
    const qreal gripSize = radius * 2;
    return {-gripSize, -gripSize, 2 * gripSize, 2 * gripSize};
}

void PointView::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    if(!m_enabled)
        return;

    QPen pen;
    QColor c = m_selected
               ? m_style.PointSelected
               : m_style.Point;

    pen.setColor(c);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(c);

    pen.setCosmetic(true);
    pen.setWidth(1);

    painter->setPen(pen);
    painter->drawEllipse(ellipse);
}

void PointView::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

void PointView::enable()
{
    m_enabled = true;
    update();
}

void PointView::disable()
{
    m_enabled = false;
    update();
}

void PointView::contextMenuEvent(QGraphicsSceneContextMenuEvent* ev)
{
    emit contextMenuRequested(ev->screenPos(), ev->scenePos());
}
}
