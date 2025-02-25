//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QRect>
#include <QRectF>
#include <QGraphicsScene>
#include <QPainter>
#include <QWidget>
#include <QList>
//-----------------------------------------------------------------------------
#include "qt_ml.h"
//-----------------------------------------------------------------------------
Qt_ML::Qt_ML(const QRect &rect, QGraphicsScene *scene):
    QGraphicsObject(),
    Width(rect.width()),
    Height(rect.height()),
    clBack(QColor(Qt::transparent)),
    clML(QColor(Qt::red)),
    line_width(1),
    bDelNoUsePoint(false),
    curentDept(0),
    dpsY(36),
    scale(200),
    direction(true),
    ml_list(new QList<qint32*>)
{
    setPos(rect.topLeft());
    scene->addItem(this);
}
//-----------------------------------------------------------------------------
Qt_ML::~Qt_ML(void)
{
    delPoints();
}
//-----------------------------------------------------------------------------
void Qt_ML::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
//    Q_UNUSED(widget);

    int     y;
    int     i;
    qint32  TopDep;
    qint32* pDep;
    QRect   r;

    QBrush          brush_back;
    QPen            pen_line;

    painter->save();

    Width = widget->width();
    Height = widget->height();

    if (direction) TopDep = curentDept;
    else TopDep = curentDept - ((scale * Height) / (dpsY));

    brush_back.setColor(clBack);
    brush_back.setStyle(Qt::SolidPattern);

    pen_line.setColor(clML);
    pen_line.setStyle(Qt::SolidLine);
    pen_line.setWidth(line_width);

    r.setRect(0, 0, Width, Height);
    painter->fillRect(r, brush_back);

    painter->setPen(pen_line);

    for (i = 0; i < ml_list->count(); i++)
    {
        pDep = (qint32*)ml_list->at(i);
        y = dpsY * (*pDep - TopDep) / (scale);
        if ((0 <= y) && (y  <= Height))
        {
            painter->drawLine(0, y, Width, y);
        }
    }
    painter->restore();
}
//-----------------------------------------------------------------------------
void Qt_ML::setML(qint32 dept)
{
    qint32* pDep;
    qint32* p0;
    qint32  h;

    pDep = new(qint32);
    *pDep = dept;
    ml_list->append(pDep);

    if (bDelNoUsePoint)
    {
        p0 = (qint32*)ml_list->first();
        h = abs(*p0 - *pDep) * (dpsY) / (scale);

        if (h > Height)
        {
            ml_list->removeFirst();
            delete(p0);
        }
    }
//    emit update();
}
//-----------------------------------------------------------------------------
void Qt_ML::delPoints(void)
{
    qint32*  p;

    while (!ml_list->isEmpty())
    {
        p = (qint32*)ml_list->first();
        delete(p);
        ml_list->removeFirst();
    }
}
//-----------------------------------------------------------------------------
void Qt_ML::on_changeDept(const qint32 newDept)
{
    curentDept = newDept;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_ML::on_changeDpsY(const int newDpsY)
{
    dpsY = newDpsY;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_ML::on_changeDeptScale(const int newScale)
{
    scale = newScale;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_ML::on_setDirectionUp(void)
{
    direction = true;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_ML::on_setDirectionDown(void)
{
    direction = false;
    emit update();
}
//-----------------------------------------------------------------------------
