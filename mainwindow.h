//-------------------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//-------------------------------------------------------------------
#include <QMainWindow>
#include <QColor>
#include <QString>
#include <QVector>
#include <QtNetwork/QUdpSocket>
#include <QTimer>
#include <QTimerEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QLabel>
#include <QSettings>
#include <QButtonGroup>
//-------------------------------------------------------------------
#include "dialog_setup.h"
#include "dialog_well_info.h"
#include "dialog_tool_mode.h"
#include "vak32_ctl_cmd.h"
#include "vak_8.h"
#include "vak_8_2pc.h"
#include "velocity.h"
//#include "qt_vak_8u_vk.h"
#include "qt_vak_32_fkd.h"
#include "qt_deptcol.h"
#include "qt_ml.h"
#include "qt_time_line.h"
#include "qt_bigreed.h"
#include "qt_vk.h"
//-------------------------------------------------------------------
namespace Ui {
class MainWindow;
}
//-------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
//    bool eventFilter(QObject *target, QEvent *event);

signals:
    void cmdSetDepth(void);
    void vak32CmdSend(void);
    void setPocketCount(qint32 count);
    void setBadPocketCount(qint32 count);
    void setDept(qint32 dept);
    void setML(bool ml);
    void showNewData(void);

    void showIZLtype(qint16 value);
    void showIZLfreq(qint16 value);
    void showIZLnum(qint16 value);

    void showRXdelay(qint16 value);
    void showRXtd(qint16 value);
    void showRXku(qint16 value);

    void showGK(qint16 value);
    void showGx(qint16 value);
    void showGy(qint16 value);
    void showGz(qint16 value);

    void showSDstatus(qint16 value);

    void changeFKDstep(int newStep);
    void changeDpsX(int newDpsX);
    void changeDpsY(int newDpsY);
    void changeDeptScale(int newScale);
    void changeFKDlevel(int newLevel);
    void changeVKmaxAmpl(int newMaxAmpl);

public slots:

private:
    Ui::MainWindow *ui;

    QSettings*          app_settings;
    Dialog_setup*       dialogSetup;
    Dialog_well_info*   dialogWellInfo;
    Dialog_Tool_Mode*   dialogToolMode;

    QButtonGroup*       buttonGroup;

    CVELOCITY*          velocity;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_vk;
    QGraphicsView*      view_vk;
    Qt_VK*              vk1;
    Qt_biGREED*         vk1_greed;
    Qt_TIME_LINE*       time_line1;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_ctl1_vk;
    QGraphicsView*      view_ctl1_vk;
    Qt_VK*              ctl1_vk;
    Qt_biGREED*         ctl1_vk_greed;

    QGraphicsScene*     scene_ctl2_vk;
    QGraphicsView*      view_ctl2_vk;
    Qt_VK*              ctl2_vk;
    Qt_biGREED*         ctl2_vk_greed;

    QGraphicsScene*     scene_ctl3_vk;
    QGraphicsView*      view_ctl3_vk;
    Qt_VK*              ctl3_vk;
    Qt_biGREED*         ctl3_vk_greed;

    QGraphicsScene*     scene_ctl4_vk;
    QGraphicsView*      view_ctl4_vk;
    Qt_VK*              ctl4_vk;
    Qt_biGREED*         ctl4_vk_greed;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_fkd;
    QGraphicsView*      view_fkd;
    CVAK32_FKD*         fkd;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_dept_col;
    QGraphicsView*      view_dept_col;
    Qt_DEPTCOL*         deptCol;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_ml_col;
    QGraphicsView*      view_ml_col;
    Qt_ML*              mlCol;

    QLabel*             label_GoodBlk;
    QLabel*             label_BadBlk;

    QLabel*             label_Depth;
    QLabel*             label_Distance;
    QLabel*             label_Velocity;
    QLabel*             label_CMD_CRC_check;
    bool                bflag_CMD_CRC;
    QLabel*             label_data_CRC_check;
    bool                bflag_data_CRC;

    QLabel*             label_ML;

    QTimer              timer;
    int                 timer_id;
    int                 timer_interval;

    int                 port;
    short int           p_count;
    short int           cmd_id;
    QUdpSocket          udp_socket;
    QHostAddress        host;
    TVAK_8_DATA         v8_data;

    QVector<vak32_ctrl_command_class*>              mode_list;
    QVector<vak32_ctrl_command_class*>::iterator    mode;

    QString OperatorName;
    QString WellNo;
    QString FildName;
    qint32  Depth;
    qint32  startDepth;
    qint32  lastDepth;
    qint32  DepthStep;
    qint32  curentDepthStep;
    QString FolderName;
    bool    bExtFolderCtl;
    QString FileName;
    bool    bWriteEnable;

    QColor  MMColor;
    QColor  FonColor;
    QColor  GreedColor;
    QColor  TextColor;
    QColor  LevelColor;
    QColor  FKDColor;
    int     FKDstep;
    int     Td;
    QColor  VKColor;
    int     VKlineSize;
    int     DepthScale;
    int     dpsX;
    int     dpsY;

    int     vkNum;
    int     vkNumRx;
    int     vkNum4fkd;
    int     modeNum;
    int     modeNum4fkd;

    int     max_ampl;
    int     fkd_level;

    int     ToolNo;

    qint32  blk_count;
    qint32  bad_blk;

    quint16 calc_rx_data_CRC16(void);

    void    load_settings(void);
    void    save_settings(void);

private slots:
    void on_cmdSetDepth(void);
    void on_vak32CmdSend(void);
    void on_udpDataRx(void);

    void on_pushButtonSettings(void);
    void on_pushButtonStart(void);
    void on_pushButtonStop(void);
    void on_pushButtonRecord(void);
    void on_dialogSetup(void);
    void on_dialogToolModeCmdRestoreFolder(void);
    void on_dialogToolModeCmdChangeToolModes(void);
    void on_dialogToolModeCmdRestoreToolModes(void);

    void on_cmdIncAmpl(void);
    void on_cmdDecAmpl(void);

    void on_cmdIncLevel(void);
    void on_cmdDecLevel(void);

    void on_setPocketCount(qint32 count);
    void on_setBadPocketCount(qint32 count);
    void on_setDept(qint32 dept);
    void on_setML(bool ml);
    void on_showNewData(void);

    void on_showIZLtype(qint16 value);
    void on_showIZLfreq(qint16 value);
    void on_showIZLnum(qint16 value);

    void on_showRXdelay(qint16 value);
    void on_showRXtd(qint16 value);
    void on_showRXku(qint16 value);

    void on_showGK(qint16 value);
    void on_showGx(qint16 value);
    void on_showGy(qint16 value);
    void on_showGz(qint16 value);

    void on_showSDstatus(qint16 value);

    void on_VKxClicked(int id);
};
//-------------------------------------------------------------------
#endif // MAINWINDOW_H
//-------------------------------------------------------------------
