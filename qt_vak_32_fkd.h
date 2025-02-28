//-----------------------------------------------------------------------------
#ifndef QT_CVAK32_FKD_H
#define QT_CVAK32_FKD_H
//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QGraphicsScene>
#include <QWidget>
#include <QList>
//-----------------------------------------------------------------------------
#include "vak_8.h"
//-----------------------------------------------------------------------------
typedef struct
{
  qint32       dept;
  TVAK8_WAVE   vk_data;
} TVK_POINT;
//-----------------------------------------------------------------------------
//класс построения ФКД
class CVAK32_FKD : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CVAK32_FKD(const QRect &rect, QGraphicsScene *scene);
    ~CVAK32_FKD(void);

    virtual QRectF  boundingRect() const {return QRectF(QRect(0, 0, Width, Height));}
    virtual void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void    setWidth(int new_width) {Width = new_width;}
    int     width() const {return Width;}

    void    setHeight(int new_height) {Height = new_height;}
    int     height() const {return Height;}

    void    setColorBack(QColor color) {clBack = color;}
    QColor  colorBack() const {return clBack;}

    void    setColorLine(QColor color) {clLine = color;}
    QColor  colorLine() const {return clLine;}

    void    setColorGreed(QColor color) {clGreed = color;}
    QColor  colorGreed() const {return clGreed;}

    void addData(const qint32 dept, const TVAK8_WAVE &data);
    void clearData(void);
    void delete_no_use_points(void);

private:
    int                 Width;
    int                 Height;

    QColor              clBack;     //цвет фона
    QColor              clLine;     //цвет линий
    QColor              clGreed;    //цвет линий сетки

    int                 dpsX;       //число точек на см, по горизонтали
    int                 dpsY;       //число точек на см, по вертикали

    int                 x_scale;    //массштаб по оси времени
    int                 scale;      //массштаб по оси глубин, метры в сантиметре
    int                 level;      //порог построения ФКД
    int                 step_x_line;//шаг линий сетки по оси времени

    bool                direction;  //направление записи: true - вверх;  false - вниз

    qint16              line_shift; //смещение горизонтальных линий сетки
    qint32              curentDept; //текущее значение глубины, см
    qint32              top_dept;
    qint32              bot_dept;

    QList<TVK_POINT*>   *list;
    TVK_POINT*          pvk_point;

signals:
    void update(void);

public slots:
    void on_changeBaseLinesShift(const qint16 newShift);
    void on_changeDept(const qint32 newDept);
    void on_changeDpsX(const int    newDpsX);
    void on_changeDpsY(const int    newDpsY);
    void on_changeDeptScale (const int newScale);
    void on_changeTimeScale (const int newScale);
    void on_changeLevel     (const int newLevel);
    void on_changeGreedStepX(const int newStep);
    void on_setDirectionUp  (void);
    void on_setDirectionDown(void);

};
//-----------------------------------------------------------------------------
#endif // QT_CVAK32_FKD_H
//-----------------------------------------------------------------------------
