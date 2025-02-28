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
#include <QThread>
//-------------------------------------------------------------------
#include "akp_class.h"
#include "akp_check_state.h"
//-------------------------------------------------------------------
#include "dialog_setup.h"
#include "dialog_well_info.h"
//-------------------------------------------------------------------
#include "vak_8.h"
#include "vak_8_2pc.h"
//#include "qt_vak_8u_vk.h"
//-------------------------------------------------------------------
#include "qt_vk.h"
#include "qt_bigreed.h"
#include "qt_time_line.h"
#include "qt_vak_32_fkd.h"
#include "qt_deptcol.h"
#include "qt_ml.h"
#include "velocity.h"
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
//    void cmdStartMeserment(void);
//    void cmdStopMeserment(void);

    void cmdSetDepth(const qint32 dept_cm);
    void cmdSetDeptStep(const qint32 dept_step_cm);
//    void vak32CmdSend(void);
//    void setPocketCount(qint32 count);
//    void setBadPocketCount(qint32 count);
    void showDept(const qint32 dept);
//    void setML(bool ml);
    void showNewData(void);

//    void showIZLtype(qint16 value);
//    void showIZLfreq(qint16 value);
//    void showIZLnum(qint16 value);

//    void showRXdelay(qint16 value);
//    void showRXtd(qint16 value);
//    void showRXku(qint16 value);

    void changeFKDstep(int newStep);
    void changeDpsX(int newDpsX);
    void changeDpsY(int newDpsY);
    void changeDeptScale(int newScale);
    void changeFKDlevel(int newLevel);
    void changeVKmaxAmpl(int newMaxAmpl);

public slots:

private:
    Ui::MainWindow*     ui;

    QSettings*          app_settings;
    Dialog_setup*       dialogSetup;
    Dialog_well_info*   dialogWellInfo;

    QButtonGroup*       buttonGroup;

    //-------------------------------------------------------------------
    akp_class           akp;
    QThread*            akp_thread;
    akp_check_state     check_state;
    //-------------------------------------------------------------------
    CVELOCITY*          velocity;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_vk;
    QGraphicsView*      view_vk;
    Qt_VK*              vk1;
    Qt_biGREED*         vk1_greed;
    Qt_TIME_LINE*       time_line1;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_ctl_vk1;
    QGraphicsView*      view_ctl_vk1;
    Qt_VK*              ctl_vk1;
    Qt_biGREED*         ctl_vk1_greed;

    QGraphicsScene*     scene_ctl_vk2;
    QGraphicsView*      view_ctl_vk2;
    Qt_VK*              ctl_vk2;
    Qt_biGREED*         ctl_vk2_greed;
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

    QLabel*             label_CRC1_check;
//    bool                bflag_CRC1_Ok;
    QLabel*             label_CRC2_check;
//    bool                bflag_CRC2_Ok;
    QLabel*             label_CRC3_check;
//    bool                bflag_CRC3_Ok;
    QLabel*             label_CRC4_check;
//    bool                bflag_CRC4_Ok;
    QLabel*             label_CRC5_check;
//    bool                bflag_CRC5_Ok;
    QLabel*             label_CRC6_check;
//    bool                bflag_CRC6_Ok;
    QLabel*             label_CRC7_check;
//    bool                bflag_CRC7_Ok;
    QLabel*             label_CRC8_check;
//    bool                bflag_CRC8_Ok;

    QLabel*             label_ML;

//    QTimer              timer;
//    int                 timer_id;
//    int                 timer_interval;

    int                 port;
//    short int           p_count;
//    short int           cmd_id;
//    QUdpSocket          udp_socket;
    QHostAddress        host;
//    TVAK_8_DATA         v8_data;

//    QVector<vak32_ctrl_command_class*>              mode_list;
//    QVector<vak32_ctrl_command_class*>::iterator    mode;

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

//    int     vkNum;
//    int     vkNumRx;
    int     vkNum4fkd;
//    int     modeNum;
//    int     modeNum4fkd;

    int     max_ampl;
    int     fkd_level;
//    int     num_y_line_on_grid;
//    int     step_x_line_on_grid;

//    int     ToolNo;

    qint32  blk_count;
    qint32  bad_blk;

    QColor  get_color_on_CRC(const bool crc);
    void    load_settings(void);
    void    save_settings(void);

private slots:
//    void on_cmdSetDepth(void);
//    void on_vak32CmdSend(void);
//    void on_udpDataRx(void);

    void on_pushButtonSettings(void);
    void on_pushButtonStart(void);
    void on_pushButtonStop(void);
    void on_pushButtonRecord(void);
    void on_dialogSetup(void);
//    void on_dialogToolModeCmdRestoreFolder(void);
//    void on_dialogToolModeCmdChangeToolModes(void);
//    void on_dialogToolModeCmdRestoreToolModes(void);

    void on_cmdIncAmpl(void);
    void on_cmdDecAmpl(void);

    void on_cmdIncLevel(void);
    void on_cmdDecLevel(void);

    void on_showPocketCount     (const int count);
    void on_showBadPocketCount  (const int count);
    void on_showDept    (const qint32 dept);
    void on_showML      (const bool ml);
    void on_showNewData (const quint16 vk_no, const TVAK8_WAVE &vk);

    void on_showIZLtype (const bool crc, const quint16 value);
    void on_showIZLfreq (const bool crc, const quint16 value);
    void on_showIZLnum  (const bool crc, const quint16 value);

    void on_showRXdelay (const bool crc, const quint16 value);
    void on_showRXtd    (const bool crc, const quint16 value);
    void on_showRXku    (const bool crc, const quint16 value);

    void on_VKxClicked(int id);

    void on_showCRC1 (const bool crc);
    void on_showCRC2 (const bool crc);
    void on_showCRC3 (const bool crc);
    void on_showCRC4 (const bool crc);
    void on_showCRC5 (const bool crc);
    void on_showCRC6 (const bool crc);
    void on_showCRC7 (const bool crc);
    void on_showCRC8 (const bool crc);

};
//-------------------------------------------------------------------
#endif // MAINWINDOW_H
//-------------------------------------------------------------------
