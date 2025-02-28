//-----------------------------------------------------------------------------
#ifndef QT_VK_H
#define QT_VK_H
//-----------------------------------------------------------------------------
#include <QGraphicsObject>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QColor>
#include <QFont>
#include <QString>
//-----------------------------------------------------------------------------
#include "vak_8.h"
//-----------------------------------------------------------------------------
//класс построения Волновых Картинок
//-----------------------------------------------------------------------------
class Qt_VK: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Qt_VK(const QRect &rect, QGraphicsScene *scene);

    void            AddData(const TVAK8_WAVE &data);
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

    void    setLevel(int newLevel) {Level = newLevel;}
    int     level() const {return Level;}

    void    setColorLevelLine(QColor newColor) {clLevelLine = newColor;}
    QColor  colorLevelLine() const {return clLevelLine;}

    void    setColorLevelBack(QColor newColor) {clLevelBack = newColor;}
    QColor  colorLevelBack() const {return clLevelBack;}

    void    setColorText(QColor newColor) {clText = newColor;}
    QColor  colorText() const {return clText;}

    void    setXScale(int new_scale) {x_scale = new_scale;}
    int     XScale() const {return x_scale;}

    void    setMaxAmpl(int new_max_ampl) {max_ampl = new_max_ampl;}
    int     maxAmpl() const {return max_ampl;}

    void    setCaption(QString text) {Caption = text;}

private:
    int     Width;       //ширина
    int     Height;      //высота
    QColor  clBack;

    int     line_width;  //толщина основной линии
    QColor  clLine;      //цвет линий сетки

    int     Level;
    QColor  clLevelLine;
    QColor  clLevelBack;

    QColor  clText;      //цвет линий текста

    int     x_scale;
    int     y_scale;
    int     zero;
    int     max_ampl;
    QFont   font;
    QString Caption;

    TVAK8_WAVE vk_data;

signals:
    void update(void);

public slots:
    void on_changeTimeScale(int newScale);
    void on_changeMaxAmpl(int newMaxAmpl);
    void on_changeLevel(int newLevel);
};
//-----------------------------------------------------------------------------
#endif // QT_VK_H
//-----------------------------------------------------------------------------
