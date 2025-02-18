//-----------------------------------------------------------------------------
//класс построения временной линии
//-----------------------------------------------------------------------------
#ifndef QT_TIME_LINE_H
#define QT_TIME_LINE_H
//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QWidget>
//-----------------------------------------------------------------------------
class Qt_TIME_LINE : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Qt_TIME_LINE(const QRect &rect, QGraphicsScene *scene);

    virtual QRectF  boundingRect() const {return QRectF(QRect(0, 0, Width, Height));}
    virtual void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void    setWidth(int new_width) {Width = new_width;}
    int     width() const {return Width;}

    void    setHeight(int new_height) {Height = new_height;}
    int     height() const {return Height;}

    void    setColorBack(QColor color) {clBack = color;}
    QColor  colorBack() const {return clBack;}

    void    setColorText(QColor color) {clText = color;}
    QColor  colorText() const {return clText;}

    void    set_caption(QString text) {Caption = text;}

    void    set_time_zero(int new_time) {time_zero = new_time;}
    int     get_time_zero() const {return time_zero;}

    void    set_time_step(int new_time) {time_step = new_time;}
    int     get_time_step() const {return time_step;}

    void    set_step_x_line(int new_step) {step_x_line = new_step;} // может быть через ссылку ?
    int     get_step_x_line() const {return step_x_line;}

    void    set_x_scale(int new_scale) {x_scale = new_scale;}       // может быть через ссылку ?
    int     get_x_scale() const {return x_scale;}

private:
    int             Width;      //ширина
    int             Height;     //высота

    QColor          clBack;     //цвет фона
    QColor          clText;     //цвет текста

    QString         Caption;

    int             time_zero;
    int             time_step;
    int             step_x_line;    //шаг сетки по оси времени

    int             x_scale;

    QFont           font;

signals:
    void update(void);

public slots:
    void on_changeTimeZero(int newTimeZero);
    void on_changeTimeStep(int newTimeStep);
    void on_changeTimeScale(int newScale);
};
//-----------------------------------------------------------------------------
#endif // QT_TIME_LINE_H
//-----------------------------------------------------------------------------
