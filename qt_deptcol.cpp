//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QRect>
#include <QGraphicsScene>
#include <QColor>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QWidget>
//-----------------------------------------------------------------------------
#include "qt_deptcol.h"
//-----------------------------------------------------------------------------
Qt_DEPTCOL::Qt_DEPTCOL(const QRect &rect, QGraphicsScene *scene):
    QGraphicsObject(),
    Width(rect.width()),
    Height(rect.height()),
    clBack(QColor(Qt::transparent)),
    line_width(1),
    clLine(QColor(Qt::black)),
    clText(QColor(Qt::black)),
    font(QFont()),
    curentDept(0),
    dpsY(36),
    scale(200),
    direction(true)
{
    setPos(rect.topLeft());
    scene->addItem(this);
}
//-----------------------------------------------------------------------------
Qt_DEPTCOL::~Qt_DEPTCOL(void)
{
}
//-----------------------------------------------------------------------------
void Qt_DEPTCOL::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
//    Q_UNUSED(widget);

    int     x;
    int     y;
    int     i;
    int     kYsol;
    int     Dep;
    int     kDep;
    int     TopDep;
    QString qsDep;
    QRect   r;

    QFont           font;
    QFontMetrics    fm(font);
    QBrush          brush_back;
    QPen            pen_line;
    QPen            pen_text;

    painter->save();

    Width = widget->width();
    Height = widget->height();

    if (direction) TopDep = curentDept;
    else TopDep = curentDept - (scale * Height / dpsY);

    brush_back.setColor(clBack);
    brush_back.setStyle(Qt::SolidPattern);

    pen_line.setColor(clLine);
    pen_line.setStyle(Qt::SolidLine);
    pen_line.setWidth(line_width);

    pen_text.setColor(clText);
    pen_text.setStyle(Qt::SolidLine);
    pen_text.setWidth(1);

    r.setRect(0, 0, Width, Height);
    painter->fillRect(r, brush_back);

    kDep = StepY1 * (scale);
    kYsol = (dpsY) * StepY1;
    x = (scale) * StepY1;
    Dep = x * (TopDep / x);
    shift = (dpsY * (Dep - TopDep)) / (scale);

    for(i = 0; i < ((Height / kYsol) + 2); i++)
    {
        painter->setPen(pen_line);

        y = kYsol * i + shift;
        painter->drawLine(0, y, Width, y);
        y++;

        qsDep = QString::fromUtf8("%1").arg( Dep / 100);//.arg( Dep / 100.0, 0, 'f', 2);
        painter->setPen(pen_text);
        painter->setFont(font);
        r.setTop(y);
        r.setHeight(1.5 * fm.height());
        r.setWidth(1.5 *  fm.width(qsDep));

        x = (Width - r.width()) / 2;
        r.setLeft(x);
        painter->drawText(r, qsDep);

        Dep += kDep;
    }

    painter->restore();
    emit changeBaseLinesShift(shift);
}
//-----------------------------------------------------------------------------
void Qt_DEPTCOL::on_changeDept(const qint32 newDept)
{
    curentDept = newDept;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_DEPTCOL::on_changeDpsY(const int newDpsY)
{
    dpsY = newDpsY;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_DEPTCOL::on_changeDeptScale(const int newScale)
{
    scale = newScale;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_DEPTCOL::on_setDirectionUp(void)
{
    direction = true;
    emit update();
}
//-----------------------------------------------------------------------------
void Qt_DEPTCOL::on_setDirectionDown(void)
{
    direction = false;
    emit update();
}
//-----------------------------------------------------------------------------
