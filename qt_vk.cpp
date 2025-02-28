//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QColor>
#include <QFont>
#include <QString>
//-----------------------------------------------------------------------------
#include "qt_vk.h"
//-----------------------------------------------------------------------------
//класс построения Волновых Картинок
//-----------------------------------------------------------------------------
Qt_VK::Qt_VK(const QRect &rect, QGraphicsScene *scene):
    QGraphicsObject(),
    Width(rect.width()),
    Height(rect.height()),
    clBack(QColor(Qt::transparent)),
    line_width(1),
    clLine(QColor(Qt::black)),
    Level(0),
    clLevelLine(QColor(Qt::gray)),
    clLevelBack(QColor(Qt::gray)),
    clText(QColor(Qt::black)),
    x_scale(1),
    y_scale(6553),      // y_scale = max_ampl / zero;
    zero(5),            // zero = height / 2;
    max_ampl(0x7FFF),
    font(QFont()),
    Caption(QString::fromUtf8("Волновая картинка"))
{
    memset(&vk_data, 0, sizeof(TVAK8_WAVE));
    setPos(rect.topLeft());
    scene->addItem(this);
}
//-----------------------------------------------------------------------------
#pragma pack(1)
void Qt_VK::AddData(const TVAK8_WAVE &data)
{
    int i;
    for (i = 0; i < VAK_8_NUM_POINTS; i++)
    {
        vk_data[i] = data[i];
/*
        if ( (data[i] & 0x2000 ) != 0) vk_data[i] = data[i] | 0xC000;
        else vk_data[i] = data[i] & 0x1FFF;
*/
    }
    emit update();
}
#pragma pack()
//-----------------------------------------------------------------------------
void Qt_VK::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
//    Q_UNUSED(widget);

    int             x1;
    int             y1;
    int             x2;
    int             y2;
    QRect           r;
    QFont           font;
    QFontMetrics    fm(font);
    QBrush          brush_back;
    QBrush          brush_level;
    QPen            pen_line;
    QPen            pen_level;
    QPen            pen_text;

    painter->save();

    Width = widget->width();
    Height = widget->height();

    if (Level < 0) Level = - Level;

    zero = Height / 2;

    y_scale = max_ampl / zero;
    if (y_scale == 0) y_scale = 1;

    x1 = 0;
    y1 = zero - Level / y_scale;
    if (y1 < 0) y1 = 0;

    brush_back.setColor(clBack);
    brush_back.setStyle(Qt::SolidPattern);

    brush_level.setColor(clLevelBack);
    brush_level.setStyle(Qt::SolidPattern);

    pen_level.setColor(clLevelLine);
    pen_level.setStyle(Qt::SolidLine);
    pen_level.setWidth(1);

    pen_line.setColor(clLine);
    pen_line.setStyle(Qt::SolidLine);
    pen_line.setWidth(line_width);

    pen_text.setColor(clText);
    pen_text.setStyle(Qt::SolidLine);
    pen_text.setWidth(1);

    r.setCoords(0, 0, Width, y1);
    painter->fillRect(r, brush_back);

    r.setCoords(0, y1, Width, Height);
    painter->fillRect(r, brush_level);

    //Линия уровня построения ФКД
    painter->setPen(pen_level);
    painter->drawLine(0, y1, Width, y1);

    //Построение волновой картинки
    painter->setPen(pen_line);

    x1 = 0;
    y1 = zero - vk_data[0] / y_scale;
    if (y1 > Height) y1 = Height;
    if (y1 < 0) y1 = 0;

    for(int i = 1; i < VAK_8_NUM_POINTS; i++)
    {
        x2 = x1 + x_scale;
        y2 = zero - vk_data[i] / y_scale;

        if ( x2 > Width)
        {
            qreal a = (qreal)(y1 - y2) / (qreal)(x1 - x2);
            qreal b = (qreal)(y2 * x1 - y1 * x2) / (qreal)(x1 - x2);

            x2 = Width;
            y2 = (int) (a * (qreal)(x2) + b);

            if (y2 > Height) y2 = Height;
            if (y2 < 0) y2 = 0;

            painter->drawLine(x1, y1, x2, y2);
            break;
        }

        if (y2 > Height) y2 = Height;
        if (y2 < 0) y2 = 0;

        painter->drawLine(x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    }

    painter->setPen(pen_text);
    painter->setFont(font);
    r.setLeft(10);
    r.setTop(2);
    r.setHeight(1.5 * fm.height());
    r.setWidth(1.5 * fm.width(Caption));
    painter->drawText(r, Caption);

    painter->restore();
}
//-----------------------------------------------------------------------------
void Qt_VK::on_changeTimeScale(int newScale)
{
    x_scale = newScale;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_VK::on_changeMaxAmpl(int newMaxAmpl)
{
    max_ampl = newMaxAmpl;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_VK::on_changeLevel(int newLevel)
{
    Level = newLevel;
    emit update();
}
//-----------------------------------------------------------------------------
