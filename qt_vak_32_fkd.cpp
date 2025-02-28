//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QList>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QBrush>
#include <QPen>
//-----------------------------------------------------------------------------
#include "qt_vak_32_fkd.h"
#include "qt_deptcol.h"
//-----------------------------------------------------------------------------
/*
const unsigned int StepY1 = 2; //шаг основной сетки, см
const unsigned int StepY2 = 4; //количество линий вспомогательной сетки на основную
const unsigned int wDash  = 2;
const unsigned int DashToDash  = 2;
*/
//-----------------------------------------------------------------------------
CVAK32_FKD::CVAK32_FKD(const QRect &rect, QGraphicsScene *scene) :
    QGraphicsObject(),
    Width(rect.width()),
    Height(rect.height()),
    clBack(QColor(Qt::transparent)),
    clLine(QColor(Qt::black)),
    clGreed(QColor(Qt::green)),
    dpsX(32),
    dpsY(32),
    x_scale(128),
    scale(200),
    level(0),
    step_x_line(1),
    direction(true),
    line_shift(0),
    curentDept(0),
    top_dept(0),
    bot_dept(0),
    list(new QList<TVK_POINT*>),
    pvk_point(NULL)
{
    setPos(rect.topLeft());
    scene->addItem(this);
}
//-----------------------------------------------------------------------------
CVAK32_FKD::~CVAK32_FKD()
{
    clearData();
    delete list;
    list = NULL;
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
//    Q_UNUSED(widget);

    int     i;
    int     k;
    qint32  x1;
    qint32  y1;
    qint32  x2;
    qint32  y2;
    qint32  m_height = ((Height * scale) / dpsY);
    QRect   r;
    QBrush  brush_back;
    QBrush  brush_line;
    QPen    pen_line;
    QPen    pen_greed;

    Height = widget->height();
    Width  = widget->width();

    painter->save();

    brush_back.setColor(clBack);
    brush_back.setStyle(Qt::SolidPattern);

    brush_line.setColor(clLine);
    brush_line.setStyle(Qt::SolidPattern);

    pen_line.setColor(clLine);
    pen_line.setStyle(Qt::SolidLine);
    pen_line.setWidth(1);

    pen_greed.setColor(clGreed);
    pen_line.setStyle(Qt::SolidLine);
    pen_line.setWidth(1);

    if (level < 0) level = - level;

    r.setRect(0, 0, Width, Height);
    painter->fillRect(r, brush_back);

    if (!list->isEmpty())
    {
        painter->setPen(pen_line);
        //Построение ФКД
        if (direction)
        {
            top_dept = curentDept;
            bot_dept = top_dept + m_height;
        }
        else
        {
            top_dept = curentDept - m_height;
            bot_dept = curentDept;
        }

        pvk_point = (TVK_POINT*)list->last();
        y1 = ((pvk_point->dept - top_dept) * dpsY) / scale;

        for(i = 0; i < list->count(); i++)
        {
            pvk_point = (TVK_POINT*)list->at(list->count() - i - 1);
            x1 = 0;
            y2 = ((pvk_point->dept - top_dept) * dpsY) / scale;
            for(k = 0; k < VAK_8_NUM_POINTS; k++)
            {
                x2 = x1 + x_scale;
                if(pvk_point->vk_data[k] > level)
                {
                    r.setCoords(x1, y1, x2, y2);
                    painter->fillRect(r, brush_line);
                }
                x1 = x2;
                if (x1 > Width) break;
            }
            y1 = y2;
            if (y1 > Height) break;
        }
    }

    //---построение сетки ФКД---
    painter->setPen(pen_greed);
    x1 = 0;
    for(i = 0; i <= (Width / step_x_line); i++)
    {
        painter->drawLine(x1, 0, x1, Height);
        x1 += step_x_line;
    }

  //-------------------------------------
    int  y;
    int  shift;
    int  dash_shift;

    int kYsol = dpsY * StepY1;
    int kYdot = kYsol / StepY2;
    int kXwdash = dpsX * wDash / 10;
    int kYwdash = dpsY * wDash / 10;
    int kXdash_2_dash = dpsX * DashToDash / 10;
    int kYdash_2_dash = dpsY * DashToDash / 10;

    while (line_shift < 0) line_shift = kYsol + line_shift;
    shift = line_shift;
    while (shift >= kYdot) shift -= kYdot;

    dash_shift = line_shift;
    while (dash_shift >= kYwdash + kYdash_2_dash)
        dash_shift -= (kYwdash + kYdash_2_dash);

    for (i=0; (i < (Height / kYdot) + 1); i++)
    {
        y = kYdot * i + shift;
        int x1 = 0;
        int x2 = x1 + kXwdash;
        while (x1 < Width)
        {
            painter->drawLine(x1, y, x2, y);
            x1 = x2 + kXdash_2_dash;
            x2 = x1 + kXwdash;
        }
    }

    shift = line_shift;
    while (shift > kYsol) shift -= kYsol;
    for (i=0; (i < (Height / kYsol) + 1); i++)
    {
        y = kYsol * i + shift;
        painter->drawLine(0, y, Width, y);
    }
    painter->restore();
}
//-----------------------------------------------------------------------------
#pragma pack(1)
void CVAK32_FKD::addData(const qint32 dept, const TVAK8_WAVE &data)
{
    pvk_point = new(TVK_POINT);
    pvk_point->dept = dept;
/*
    int i;
    for (i = 0; i < VAK32_WAVE_NUM_POINTS; i++)
    {
      if ( ((*data)[i] & 0x2000 ) != 0) pvk_point->vk_data[i] = (*data)[i] | 0xC000;
      else pvk_point->vk_data[i] = (*data)[i] & 0x1FFF;
    }
*/
    memcpy(&(pvk_point->vk_data), data, sizeof(TVAK8_WAVE));
    list->append(pvk_point);
    emit update();
}
#pragma pack()
//-----------------------------------------------------------------------------
void CVAK32_FKD::clearData(void)
{
    while (!list->isEmpty())
    {
        pvk_point = list->first();
        delete pvk_point;
        list->removeFirst();
    }
}
//-----------------------------------------------------------------------------
//---Удаление не используемых данных---
void CVAK32_FKD::delete_no_use_points(void)
{
    if (!(list->isEmpty()))
    {
        qint32 m_height = ((Height * scale) / dpsY);
        qint32 bot = bot_dept + m_height;
        qint32 top = top_dept - m_height;

        pvk_point = (TVK_POINT*)list->first();

        while ((pvk_point->dept > bot) ||
               (pvk_point->dept < top))
        {
          list->removeFirst();
          delete pvk_point;
          if (list->isEmpty()) break;
          pvk_point = (TVK_POINT*)list->first();
        }
    }
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeBaseLinesShift(const qint16 newShift)
{
    line_shift = newShift;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeDept(const qint32 newDept)
{
    curentDept = newDept;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeDpsX(const int newDpsX)
{
    dpsX = newDpsX;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeDpsY(const int newDpsY)
{
    dpsY = newDpsY;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeDeptScale(const int newScale)
{
    scale = newScale;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeTimeScale(const int newScale)
{
    x_scale = newScale;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeLevel(const int newLevel)
{
    level = newLevel;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_changeGreedStepX(const int newStep)
{
    step_x_line = newStep;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_setDirectionUp(void)
{
    direction = true;
    emit update();
}
//-----------------------------------------------------------------------------
void CVAK32_FKD::on_setDirectionDown(void)
{
    direction = false;
    emit update();
}
//-----------------------------------------------------------------------------
