//-----------------------------------------------------------------------------
#include <QGraphicsItem>
#include <QRect>
#include <QGraphicsScene>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QWidget>
//-----------------------------------------------------------------------------
#include "qt_bigreed.h"
//-----------------------------------------------------------------------------
//класс построения сетки для Волновых Картинок
//-----------------------------------------------------------------------------
Qt_biGREED::Qt_biGREED(const QRect &rect, QGraphicsScene *scene):
    QGraphicsItem(),
    Width(rect.width()),
    Height(rect.height()),
    clBack(QColor(Qt::transparent)),
    clLine(QColor(Qt::black)),
    line_width(1),
    num_x_line(1),
    num_y_line(1),
    step_x_line(Width),
    step_y_line(Height)
{
    setPos(rect.topLeft());
    scene->addItem(this);
}
//-----------------------------------------------------------------------------
void Qt_biGREED::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
//    Q_UNUSED(widget);

    int     i;
    int     x1;
    int     y1;
    int     x2;
    int     y2;
    QRect   r;
    QBrush  brush_back = QBrush(clBack, Qt::SolidPattern);
    QPen    pen_line = QPen(clLine);
    QPen    pen_zero_line = QPen(clLine);

    pen_line.setWidth(line_width);
    pen_line.setStyle(Qt::SolidLine);

    pen_zero_line.setWidth(2 * line_width);
    pen_zero_line.setStyle(Qt::SolidLine);

    Width = widget->width();
    Height = widget->height();

    num_x_line = Width / step_x_line;

    zero = Height / 2;
    step_y_line = zero / num_y_line;

    painter->save();

    r.setRect(0, 0, Width, Height);
    painter->fillRect(r, brush_back);

    //Линия нулевого уровня
    x1 = 0;
    y1 = zero;
    x2 = Width;
    y2 = zero;
    painter->setPen(pen_zero_line);
    painter->drawLine(x1, y1, x2, y2);

    painter->setPen(pen_line);

    x1 = 0;
    y1 = zero;
    x2 = Width;
    y2 = zero;
    for(i = 1; i <= num_y_line; i++)
    {
        y1 += step_y_line;
        painter->drawLine(x1, y1, x2, y1);
        y2 -= step_y_line;
        painter->drawLine(x1, y2, x2, y2);
    }

    x1 = 0;
    y1 = 0;
    y2 = Height;
    for(i = 0; i <= num_x_line; i++)
    {
        painter->drawLine(x1, y1, x1, y2);
        x1 += step_x_line;
    }

    painter->restore();
}
//-----------------------------------------------------------------------------
void Qt_biGREED::set_num_x_line(int num_line)
{
    num_x_line = num_line;
    step_x_line = Width / num_x_line;
}
//-----------------------------------------------------------------------------
void Qt_biGREED::set_step_x_line(int step)
{
    step_x_line = step;
    num_x_line = Width / step_x_line;
}
//-----------------------------------------------------------------------------
void Qt_biGREED::set_num_y_line(int num_line)
{
    num_y_line = num_line;
    zero = Height / 2;
    step_y_line = zero / num_y_line;
}
//-----------------------------------------------------------------------------
