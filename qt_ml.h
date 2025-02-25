//-----------------------------------------------------------------------------
//объект построения колонки магнитных меток
//-----------------------------------------------------------------------------
#ifndef QT_ML_H
#define QT_ML_H
//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QRect>
#include <QRectF>
#include <QGraphicsScene>
#include <QPainter>
#include <QWidget>
#include <QList>
//-----------------------------------------------------------------------------
class Qt_ML: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Qt_ML(const QRect &rect, QGraphicsScene *scene);
    ~Qt_ML(void);

    virtual QRectF  boundingRect() const {return QRectF(QRect(0, 0, Width, Height));}
    virtual void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void    setWidth(int new_width) {Width = new_width;}
    int     width() const {return Width;}

    void    setHeight(int new_height) {Height = new_height;}
    int     height() const {return Height;}

    void    setColorBack(QColor color) {clBack = color;}
    QColor  colorBack() const {return clBack;}

    void    setLineWidth(int new_width) {line_width = new_width;}
    int     lineWidth() const {return line_width;}

    void    setColorML(QColor color) {clML = color;}
    QColor  colorML() const {return clML;}

    void setML(qint32 dept); //установить магнитную метку
    void delPoints(void);    //удалить все точки

private:
    int     Width;      //Ширина колонки
    int     Height;     //Высота колонки
    QColor  clBack;     //Цвет фона

    QColor  clML;       //Цвет магнитных меток
    int     line_width; //толщина основной линии

    bool     bDelNoUsePoint;//флаг ограничения списка: true - удалять точки, выходящие за вертикальные границы, false - не удалять

    qint32  curentDept; //текущее значение глубины, см
    int     dpsY;       //число точек на см, по вертикали
    int     scale;      //массштаб, метры в сантиметре
    bool    direction;  //направление записи: true - вверх, false - вниз

    QList<qint32*>*  ml_list;

signals:
    void update(void);

public slots:
    void on_changeDept     (const qint32 newDept);
    void on_changeDpsY     (const int    newDpsY);
    void on_changeDeptScale(const int    newScale);
    void on_setDirectionUp     (void);
    void on_setDirectionDown   (void);
    void on_setDeleteUnusePoint(void)   {bDelNoUsePoint = true;  }
    void on_setNoDeletePoint   (void)   {bDelNoUsePoint = false; }
};
//-------------const qint32 newDept--------------------------------------------------
#endif // QT_ML_H
//-----------------------------------------------------------------------------
