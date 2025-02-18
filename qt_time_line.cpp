//-----------------------------------------------------------------------------
#include <QBrush>
#include <QRect>
#include <QGraphicsScene>
#include <QPainter>
//-----------------------------------------------------------------------------
#include "qt_time_line.h"
//-----------------------------------------------------------------------------
Qt_TIME_LINE::Qt_TIME_LINE(const QRect &rect, QGraphicsScene *scene):
    QGraphicsObject(),
    Width(rect.width()),
    Height(rect.height()),
    clBack(QColor(Qt::transparent)),
    clText(QColor(Qt::black)),
    Caption(QString("")),
    time_zero(0),
    time_step(4),
    step_x_line(128),
    x_scale(1),
    font(QFont())
{
    setPos(rect.topLeft());
    scene->addItem(this);
}
//-----------------------------------------------------------------------------
void Qt_TIME_LINE::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
//    Q_UNUSED(widget);

    int             x;
    int             y;
    int             t;
    int             kt = step_x_line / x_scale;
    QRect           r;
    QString         time;
    QBrush          brush_back(clBack, Qt::SolidPattern);
    QPen            pen(clText);
    QFontMetrics    fm(font);

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);

    Width = widget->width();
    Height = widget->height();

    painter->save();

    r.setRect(0, 0, Width, Height);
    painter->fillRect(r, brush_back);

    painter->setPen(pen);
    r.setRect(4, 2, fm.width(Caption), fm.height());
    painter->drawText(r, Caption);

    t = time_zero;
    time = QString::fromUtf8("%1, мкс").arg(t * time_step);
    x = 4;
//    y = height - fm.height() - 4;
    y = Height - 4;
    painter->drawText(x, y, time);

    while (x < Width)
    {
        t += kt;
        x += step_x_line;
        time = QString::fromUtf8("%1").arg(t * time_step);
        painter->drawText(x, y, time);
    }

    painter->restore();
}
//-----------------------------------------------------------------------------
void Qt_TIME_LINE::on_changeTimeZero(int newTimeZero)
{
    time_zero = newTimeZero;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_TIME_LINE::on_changeTimeStep(int newTimeStep)
{
    time_step = newTimeStep;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_TIME_LINE::on_changeTimeScale(int newScale)
{
    x_scale = newScale;
    emit update();
}
//-----------------------------------------------------------------------------
