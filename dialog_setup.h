//-------------------------------------------------------------------
#ifndef DIALOG_SETUP_H
#define DIALOG_SETUP_H
//-------------------------------------------------------------------
#include <QDialog>
#include <QColor>
#include <QString>
//-------------------------------------------------------------------
#include "vak32_ctl_cmd.h"
//-------------------------------------------------------------------
namespace Ui {
class Dialog_setup;
}
//-------------------------------------------------------------------
class Dialog_setup : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_setup(QWidget *parent = 0);
    ~Dialog_setup();

    void    setToolMode(vak32_ctrl_command_class* mode);

    void    setMMColor(QColor color);
    void    setFonColor(QColor color);
    void    setGreedColor(QColor color);
    void    setTextColor(QColor color);
    void    setLevelColor(QColor color);
    void    setFKDColor(QColor color);
    void    setVKColor(QColor color);

    void    setFKDstep(int step);
    void    setVKlineSize(int size);
    void    setDepthScale(int scale);

    void    set_dpsY(int dpsY);
    void    set_dpsX(int dpsX);

    void    setExtCtl(void);
    void    clrExtCtl(void);

    void    setFolder(QString qsFolderName);

    QColor  getMMColor(void);
    QColor  getFonColor(void);
    QColor  getGreedColor(void);
    QColor  getTextColor(void);
    QColor  getLevelColor(void);
    QColor  getFKDColor(void);
    QColor  getVKColor(void);

    int     getFKDstep(void);
    int     getVKlineSize(void);
    int     getDepthScale(void);

    int     get_dpsY(void);
    int     get_dpsX(void);

    bool    get_ExtCtl(void);

    QString getFolder(void);

signals:
    void cmd_RestoreFolder(void);
    void cmd_ChangeToolModes(void);
    void cmd_RestoreToolModes(void);

private slots:
    void on_pushButtonFolderChange_clicked(void);

    void on_pushButtonMMColor_clicked(void);
    void on_pushButtonFonColor_clicked(void);
    void on_pushButtonGreedColor_clicked(void);
    void on_pushButtonTextColor_clicked(void);
    void on_pushButtonLevelColor_clicked(void);
    void on_pushButtonFKDColor_clicked(void);
    void on_pushButtonVKColor_clicked(void);
private:
    Ui::Dialog_setup *ui;
};
//-------------------------------------------------------------------
#endif // DIALOG_SETUP_H
//-------------------------------------------------------------------
