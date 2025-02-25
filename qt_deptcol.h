//-----------------------------------------------------------------------------
//объект построения колонки глубин
//-----------------------------------------------------------------------------
#ifndef QT_DEPTCOL_H
#define QT_DEPTCOL_H
//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QGraphicsScene>
#include <QFont>
//-----------------------------------------------------------------------------
const unsigned int StepX1 = 2; //шаг основной сетки, см
const unsigned int StepY1 = 2; //шаг основной сетки, см
const unsigned int StepX2 = 4; //количество линий вспомогательной сетки на основную
const unsigned int StepY2 = 4; //количество линий вспомогательной сетки на основную
const unsigned int wDash  = 2;
const unsigned int DashToDash  = 2;
//-----------------------------------------------------------------------------
class Qt_DEPTCOL: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Qt_DEPTCOL(const QRect &rect, QGraphicsScene *scene);
    ~Qt_DEPTCOL(void);

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

    void    setColorLine(QColor color) {clLine = color;}
    QColor  colorLine() const {return clLine;}

    void    setColorText(QColor color) {clText = color;}
    QColor  colorText() const {return clText;}

private:
    int     Width;      //Ширина колонки
    int     Height;     //Высота колонки
    QColor  clBack;     //Цвет фона

    int     line_width; //толщина основной линии
    QColor  clLine;     //цвет линий сетки

    QColor  clText;     //цвет линий текста
    QFont   font;

    qint32  curentDept; //текущее значение глубины, см
    qint16  shift;
    int     dpsY;       //число точек на см, по вертикали
    int     scale;      //массштаб, метры в сантиметре
    bool    direction;  //направление записи: true - вверх, false - вниз

signals:
    void update(void);
    void changeBaseLinesShift(qint16 shift);

public slots:
    void on_changeDept(const qint32 newDept);
    void on_changeDpsY(const int newDpsY);
    void on_changeDeptScale (const int newScale);
    void on_setDirectionUp  (void);
    void on_setDirectionDown(void);
};
//-----------------------------------------------------------------------------
#endif // QT_DEPTCOL_H
//-----------------------------------------------------------------------------
