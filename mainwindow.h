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
//-------------------------------------------------------------------
#include "qt_vk.h"
#include "qt_bigreed.h"
#include "qt_time_line.h"
#include "qt_vak_32_fkd.h"
#include "qt_deptcol.h"
#include "qt_ml.h"
#include "velocity.h"
#include "qt_akp_file.h"
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
    void cmdSetDepth(const qint32 dept_cm);
    void cmdSetDeptStep(const qint32 dept_step_cm);
    void showDept(const qint32 dept);
    void showNewData(void);

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

    //-------------------------------------------------------------------
    akp_class           akp;
    QThread*            akp_thread;
    akp_check_state     check_state;
    //-------------------------------------------------------------------
    qt_akp_file_save    akp_file;
    QThread*            file_thread;
    //-------------------------------------------------------------------
    CVELOCITY*          velocity;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_vk1;
    QGraphicsView*      view_vk1;
    Qt_VK*              vk1;
    Qt_biGREED*         vk1_greed;
    Qt_TIME_LINE*       time_line_vk1;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_vk2;
    QGraphicsView*      view_vk2;
    Qt_VK*              vk2;
    Qt_biGREED*         vk2_greed;
    Qt_TIME_LINE*       time_line_vk2;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_fkd_vk1;
    QGraphicsView*      view_fkd_vk1;
    CVAK32_FKD*         fkd_vk1;
    //-------------------------------------------------------------------
    QGraphicsScene*     scene_fkd_vk2;
    QGraphicsView*      view_fkd_vk2;
    CVAK32_FKD*         fkd_vk2;
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

    QLabel*             label_ML;

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

    int     max_ampl;
    int     fkd_level;

    qint32  blk_count;
    qint32  bad_blk;

    QColor  get_color_on_CRC(const bool crc);
    void    load_settings(void);
    void    save_settings(void);

private slots:

    void on_pushButtonSettings(void);
    void on_pushButtonStart(void);
    void on_pushButtonStop(void);
    void on_pushButtonRecord(void);
    void on_dialogSetup(void);
    void on_dialogToolModeCmdRestoreFolder(void);

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

    void on_showRXtype  (const bool crc, const quint16 value);
    void on_showRXdelay (const bool crc, const quint16 value);
    void on_showRXtd    (const bool crc, const quint16 value);
    void on_showRXku    (const bool crc, const quint16 value);

    void on_showToolNo  (const bool crc, const quint16 value);
    void on_showSoftVer (const bool crc, const quint16 soft_version_major, const quint16 soft_version_minor);

    void on_showTimeMeserment(const bool crc, const quint32 time_meserment);

};
//-------------------------------------------------------------------
#endif // MAINWINDOW_H
//-------------------------------------------------------------------
