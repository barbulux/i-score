#include "TimeRuler.hpp"

#include <QPainter>

#include <QDebug>
#include <QGraphicsScene>

TimeRuler::TimeRuler()
{
    m_duration.addMSecs(30000);
    m_width = m_duration.msec() * m_pixelPerMillis;
}

QRectF TimeRuler::boundingRect() const
{
    return QRectF{0, -m_height, m_width + 20, m_height};
}

void TimeRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(QBrush(QColor(100, 0, 255)), 2, Qt::SolidLine));
    painter->drawLine(0, 0, m_width, 0);

    double pixPerSec = m_pixelPerMillis * 1000;
    double t = 0;
    int i = 0;

    while (t < m_width + 10)
    {
        painter->drawLine(t, 0, t, -10);
        painter->drawText(t, -20, QString::number(10 * i));
        t += 10 * pixPerSec;
        i++;
        qDebug () << i ;
    }
    qDebug() << m_width << pixPerSec;
}

void TimeRuler::setHeight(qreal newHeight)
{
    prepareGeometryChange();
    m_height = newHeight;
}

void TimeRuler::setWidth(qreal newWidth)
{
    prepareGeometryChange();
    m_width = newWidth;
}

void TimeRuler::setPixelPerMillis(double newFactor)
{
    m_pixelPerMillis = newFactor;
    updateGraduationsSize();
}

void TimeRuler::setDuration(TimeValue dur)
{
    m_duration = dur;
    updateGraduationsSize();
}

void TimeRuler::updateGraduationsSize()
{
    prepareGeometryChange();
    m_width = m_duration.msec() * m_pixelPerMillis;
    return;
}


