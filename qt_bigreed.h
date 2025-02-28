//-----------------------------------------------------------------------------
#ifndef QT_BIGREED_H
#define QT_BIGREED_H
//-----------------------------------------------------------------------------
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QWidget>
//-----------------------------------------------------------------------------
//класс построения сетки для Волновых Картинок
//-----------------------------------------------------------------------------
class Qt_biGREED : public QGraphicsItem
{
public:
    Qt_biGREED(const QRect &rect, QGraphicsScene *scene);

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

    void    setLineWidth(int new_width) {line_width = new_width;}
    int     lineWidth() const {return line_width;}

    void    set_num_x_line(int num_line);
    void    set_num_y_line(int num_line);
    void    set_step_x_line(int step);

    int     get_num_x_line()  const {return num_x_line; }
    int     get_num_y_line()  const {return num_y_line; }
    int     get_step_x_line() const {return step_x_line;}
    int     get_step_y_line() const {return step_y_line;}


private:
    int     Width;          //ширина
    int     Height;         //высота
    QColor  clBack;         //цвет фона
    QColor  clLine;         //цвет линий сетки
    int     line_width;     //толщина основной линии

    int     zero;
    int     num_x_line;
    int     num_y_line;
    int     step_x_line;
    int     step_y_line;

signals:
//    void update(void);

public slots:
//    void on_changeTimeZero(int newTimeZero);
//    void on_changeTimeStep(int newTimeStep);
//    void on_changeTimeScale(int newScale);
};
//-----------------------------------------------------------------------------
#endif // QT_BIGREED_H
//-----------------------------------------------------------------------------
