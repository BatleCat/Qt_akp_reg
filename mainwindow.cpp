//-----------------------------------------------------------------------------
#include <QString>
#include <QColor>
#include <QDialog>
#include <QVector>
#include <QtNetwork/QUdpSocket>
//#include <QTimer>
//#include <QTimerEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QStatusBar>
#include <QFile>
#include <QDir>
#include <QDesktopWidget>
#include <QSettings>
#include <QRgb>
#include <QButtonGroup>
#include <QEvent>
#include <QResizeEvent>
//-----------------------------------------------------------------------------
#include <memory.h>
#include <winsock.h>
//-----------------------------------------------------------------------------
#include "mainwindow.h"
#include "ui_mainwindow.h"
//-----------------------------------------------------------------------------
#include "dialog_setup.h"
#include "dialog_well_info.h"
//#include "dialog_tool_mode.h"
//#include "vak32_ctl_cmd.h"
#include "vak_8.h"
#include "vak_8_2pc.h"
//#include "qt_vak_8u_vk.h"
//#include "qt_vak_32_file.h"
#include "qt_vak_32_fkd.h"
#include "qt_deptcol.h"
#include "qt_ml.h"
#include "qt_vk.h"
//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app_settings(new QSettings("TNG-Group", "vak32_reg")),
    dialogSetup(NULL),
    dialogWellInfo(NULL),
    akp(parent),
    check_state(parent),
    velocity(new CVELOCITY(this)),
    bflag_CRC1_Ok(false),
    bflag_CRC2_Ok(false),
    bflag_CRC3_Ok(false),
    bflag_CRC4_Ok(false),
    bflag_CRC5_Ok(false),
    bflag_CRC6_Ok(false),
    bflag_CRC7_Ok(false),
    bflag_CRC8_Ok(false),
//    timer_interval(500),
    port(1500),
    p_count(0),
    host(QHostAddress("10.2.22.245")),
    ToolNo(0)
{
    //-------------------------------------------------------------------------
    qRegisterMetaType<TDataPocket>("TDataPocket");
    qRegisterMetaType<TCtrlPocket>("TCtrlPocket");
    //-------------------------------------------------------------------------
    akp_thread = new QThread();
    akp.moveToThread(akp_thread);
    connect(akp_thread, SIGNAL(started(           )), &akp,         SLOT(start(       )));
    connect(&akp,       SIGNAL(connectionClosed(  )), akp_thread,   SLOT(quit(        )));
    connect(akp_thread, SIGNAL(finished(          )), akp_thread,   SLOT(deleteLater( )));
    //-------------------------------------------------------------------------
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/TNG.ico"));
//    setGeometry(0, 0, QApplication::desktop()->screen(-1)->width(), QApplication::desktop()->screen(-1)->height());

    ui->pushButton_Stop->setVisible(false);
    ui->pushButton_Stop->setDisabled(true);

    QCoreApplication::setOrganizationName(QString::fromUtf8("TNG-Group"));
    QCoreApplication::setApplicationName(QString::fromUtf8("qt_akp_reg"));
    //-------------------------------------------------------------------------
    load_settings();
    //-------------------------------------------------------------------------
    label_Depth = new QLabel(QString::fromUtf8("Глубина: 99999.99 м"));
    label_Depth->setAlignment(Qt::AlignLeft);
    label_Depth->setMinimumSize(label_Depth->sizeHint());
    statusBar()->addWidget(label_Depth);
    label_Depth->setText(QString::fromUtf8("Глубина: %1 м").arg( Depth / 100.0, 0, 'f', 2));

    label_Distance = new QLabel(QString::fromUtf8("Пройдено: 9999.99 м"));
    label_Distance->setAlignment(Qt::AlignLeft);
    label_Distance->setMinimumSize(label_Distance->sizeHint());
    statusBar()->addWidget(label_Distance);
    label_Distance->setText(QString::fromUtf8("Пройдено: 0.00 м"));

    label_Velocity = new QLabel(QString::fromUtf8("Скорость: 9999 м/ч"));
    label_Velocity->setAlignment(Qt::AlignLeft);
    label_Velocity->setMinimumSize(label_Velocity->sizeHint());
    statusBar()->addWidget(label_Velocity);
    label_Velocity->setText(QString::fromUtf8("Скорость: 0 м/ч"));

    label_GoodBlk = new QLabel(QString::fromUtf8("Хорошие блоки: 999999999"));
    label_GoodBlk->setAlignment(Qt::AlignLeft);
    label_GoodBlk->setMinimumSize(label_GoodBlk->sizeHint());
    statusBar()->addWidget(label_GoodBlk);
    label_GoodBlk->setText(QString::fromUtf8("Хорошие блоки: 0"));

    label_BadBlk = new QLabel(QString::fromUtf8("Плохие блоки: 999999999"));
    label_BadBlk->setAlignment(Qt::AlignLeft);
    label_BadBlk->setMinimumSize(label_BadBlk->sizeHint());
    statusBar()->addWidget(label_BadBlk);
    label_BadBlk->setText(QString::fromUtf8("Плохие блоки: 0"));

    label_CRC1_check = new QLabel(QString::fromUtf8("CRC1: ОШИБКА!!!"));
    label_CRC1_check->setAlignment(Qt::AlignLeft);
    label_CRC1_check->setMinimumSize(label_CRC1_check->sizeHint());
    statusBar()->addWidget(label_CRC1_check);
    label_CRC1_check->setText(QString::fromUtf8(""));

    label_CRC2_check = new QLabel(QString::fromUtf8("CRC2: ОШИБКА!!!"));
    label_CRC2_check->setAlignment(Qt::AlignLeft);
    label_CRC2_check->setMinimumSize(label_CRC2_check->sizeHint());
    statusBar()->addWidget(label_CRC2_check);
    label_CRC2_check->setText(QString::fromUtf8(""));

    label_CRC3_check = new QLabel(QString::fromUtf8("CRC3: ОШИБКА!!!"));
    label_CRC3_check->setAlignment(Qt::AlignLeft);
    label_CRC3_check->setMinimumSize(label_CRC3_check->sizeHint());
    statusBar()->addWidget(label_CRC3_check);
    label_CRC3_check->setText(QString::fromUtf8(""));

    label_CRC4_check = new QLabel(QString::fromUtf8("CRC4: ОШИБКА!!!"));
    label_CRC4_check->setAlignment(Qt::AlignLeft);
    label_CRC4_check->setMinimumSize(label_CRC4_check->sizeHint());
    statusBar()->addWidget(label_CRC4_check);
    label_CRC4_check->setText(QString::fromUtf8(""));

    label_CRC5_check = new QLabel(QString::fromUtf8("CRC5: ОШИБКА!!!"));
    label_CRC5_check->setAlignment(Qt::AlignLeft);
    label_CRC5_check->setMinimumSize(label_CRC5_check->sizeHint());
    statusBar()->addWidget(label_CRC5_check);
    label_CRC5_check->setText(QString::fromUtf8(""));

    label_CRC6_check = new QLabel(QString::fromUtf8("CRC6: ОШИБКА!!!"));
    label_CRC6_check->setAlignment(Qt::AlignLeft);
    label_CRC6_check->setMinimumSize(label_CRC6_check->sizeHint());
    statusBar()->addWidget(label_CRC6_check);
    label_CRC6_check->setText(QString::fromUtf8(""));

    label_CRC7_check = new QLabel(QString::fromUtf8("CRC7: ОШИБКА!!!"));
    label_CRC7_check->setAlignment(Qt::AlignLeft);
    label_CRC7_check->setMinimumSize(label_CRC7_check->sizeHint());
    statusBar()->addWidget(label_CRC7_check);
    label_CRC7_check->setText(QString::fromUtf8(""));

    label_CRC8_check = new QLabel(QString::fromUtf8("CRC8: ОШИБКА!!!"));
    label_CRC8_check->setAlignment(Qt::AlignLeft);
    label_CRC8_check->setMinimumSize(label_CRC8_check->sizeHint());
    statusBar()->addWidget(label_CRC8_check);
    label_CRC8_check->setText(QString::fromUtf8(""));

    label_ML = new QLabel(QString::fromUtf8("MM"));
    label_ML->setAlignment(Qt::AlignCenter);

    QSize ML_Size = label_ML->sizeHint();
    ML_Size.setWidth(ML_Size.width() * 2);

    label_ML->setMinimumSize(ML_Size);
//    label_ML->setFrameStyle(QFrame::Box);
    QColor color(Qt::red);
    color = palette().background().color();
    label_ML->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    statusBar()->addPermanentWidget(label_ML);
    //-------------------------------------------------------------------------
    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->radioButton_VK1,   1);
    buttonGroup->addButton(ui->radioButton_VK5,   5);
    //-------------------------------------------------------------------------
//    p_count = 0;
//    port = 1500;
//    host = QHostAddress("10.2.22.245");

//    udp_socket.bind(port);
//    connect(&udp_socket,    SIGNAL( readyRead(void)         ), this, SLOT( on_udpDataRx(void)           ) );

//    connect(this,           SIGNAL( cmdSetDepth(void)       ), this, SLOT( on_cmdSetDepth(void)         ) );
//    connect(this,           SIGNAL( vak32CmdSend(void)      ), this, SLOT( on_vak32CmdSend(void)        ) );

//    connect(this,           SIGNAL( setDept(qint32)          ), this, SLOT( on_setDept(qint32)          ) );
//    connect(this,           SIGNAL( setML(bool)              ), this, SLOT( on_showML(bool)              ) ); // сигнал перенесен в check_state
    connect(&check_state,   SIGNAL( VK_update(const quint16, const TVAK8_VK&) ), this, SLOT( on_showNewData(const quint16, const TVAK8_VK&) ) );
//    connect(this,           SIGNAL( setPocketCount(qint32)   ), this, SLOT( on_showPocketCount(qint32)   ) );
//    connect(this,           SIGNAL( setBadPocketCount(qint32)), this, SLOT( on_showBadPocketCount(qint32)) );
    connect(&check_state,   SIGNAL( good_blk_cnt_update(const int) ),  this, SLOT( on_showPocketCount   (const int)  ) );
    connect(&check_state,   SIGNAL( bad_blk_cnt_update (const int) ),  this, SLOT( on_showBadPocketCount(const int)  ) );

//    connect(this,           SIGNAL( showIZLtype(qint16)     ), this, SLOT( on_showIZLtype(qint16)       ) );
    connect(&check_state,   SIGNAL( Fsig_update(const bool, const quint16) ), this, SLOT( on_showIZLtype(const bool, const quint16) ) );
//    connect(this,           SIGNAL( showIZLfreq(qint16)     ), this, SLOT( on_showIZLfreq(qint16)       ) );
    connect(&check_state,   SIGNAL( Fsig_update(const bool, const quint16) ), this, SLOT( on_showIZLfreq(const bool, const quint16) ) );
//    connect(this,           SIGNAL( showIZLnum(qint16)      ), this, SLOT( on_showIZLnum(qint16)        ) );
    connect(&check_state,   SIGNAL( Fsig_update(const bool, const quint16) ), this, SLOT( on_showIZLnum(const bool, const quint16) ) );

//    connect(this,           SIGNAL( showRXdelay(qint16)     ), this, SLOT( on_showRXdelay(qint16)       ) );
    connect(&check_state,   SIGNAL( rx_delay_update(const bool, const quint16) ), SLOT( on_showRXdelay(const bool, const qint16) ));
//    connect(this,           SIGNAL( showRXtd(qint16)        ), this, SLOT( on_showRXtd(qint16)          ) );
    connect(&check_state,   SIGNAL( rx_delay_update(const bool, const quint16) ), SLOT( on_showRXtd(const bool, const qint16) ));
//    connect(this,           SIGNAL( showRXku(qint16)        ), this, SLOT( on_showRXku(qint16)          ) );
    connect(&check_state,   SIGNAL( Ku_update(const bool, const quint16) ), this, SLOT( on_showRXku(const bool, const quint16)  ) );

//    connect(this,           SIGNAL( showSDstatus(qint16)    ), this, SLOT( on_showSDstatus(qint16)      ) );

//    connect(this,           SIGNAL( showGK(qint16)          ), this, SLOT( on_showGK(qint16)            ) );
//    connect(this,           SIGNAL( showGx(qint16)          ), this, SLOT( on_showGx(qint16)            ) );
//    connect(this,           SIGNAL( showGy(qint16)          ), this, SLOT( on_showGy(qint16)            ) );
//    connect(this,           SIGNAL( showGz(qint16)          ), this, SLOT( on_showGz(qint16)            ) );

    connect(ui->pushButton_Start,    SIGNAL( pressed() ), this, SLOT( on_pushButtonStart()      ) );
    connect(ui->pushButton_Stop,     SIGNAL( pressed() ), this, SLOT( on_pushButtonStop()       ) );
    connect(ui->pushButton_Record,   SIGNAL( pressed() ), this, SLOT( on_pushButtonRecord()     ) );
    connect(ui->pushButton_Settings, SIGNAL( pressed() ), this, SLOT( on_pushButtonSettings()   ) );
    connect(ui->pushButton_incAmpl,  SIGNAL( pressed() ), this, SLOT( on_cmdIncAmpl()           ) );
    connect(ui->pushButton_decAmpl,  SIGNAL( pressed() ), this, SLOT( on_cmdDecAmpl()           ) );
    connect(ui->pushButton_upFKD,    SIGNAL( pressed() ), this, SLOT( on_cmdIncLevel()          ) );
    connect(ui->pushButton_downFKD,  SIGNAL( pressed() ), this, SLOT( on_cmdDecLevel()          ) );

    connect(buttonGroup,    SIGNAL( buttonClicked(int) ), this, SLOT( on_VKxClicked(int) )  );
    //-------------------------------------------------------------------------

    connect(this,                       SIGNAL( cmdSetDepth(const qint32) ),     &akp, SLOT( on_cmdSetDepth(const qint32) ));
//    connect(this,                       SIGNAL( cmd_do_meserment(int) ), &akp, SLOT( on_cmdDoMeserment(int) ));

//    connect(ui->pushButton_set_dept,    SIGNAL( pressed() ),             this, SLOT( on_pushButton_set_dept_clicked() ));
//    connect(ui->pushButton_read_dept,   SIGNAL( pressed() ),             &akp, SLOT( on_cmdReadDepth() ));

//    connect(this,                   SIGNAL( cmdStartMeserment() ),  &akp, SLOT( on_cmdStartMeserment() ));
//    connect(this,                   SIGNAL( cmdStopMeserment () ),  &akp, SLOT( on_cmdStopMeserment () ));
    connect(ui->pushButton_Start,   SIGNAL( pressed() ),                    &akp, SLOT( on_cmdStartMeserment() ));
    connect(ui->pushButton_Stop,    SIGNAL( pressed() ),                    &akp, SLOT( on_cmdStopMeserment () ));
    connect(this,                   SIGNAL( cmdSetDeptStep(const qint32) ), &akp, SLOT( on_setDeptStep(const qint32) ));

//    connect(ui->pushButton_stop,        SIGNAL( pressed() ),             &check_state, SLOT( clear_block_count() ));

    connect(&akp,    SIGNAL(dataUpdate(const uint , const TDataPocket)), &check_state, SLOT(onDataUpdate(const uint, const TDataPocket)));

    connect(&check_state,   SIGNAL( deptUpdate (const qint32) ), this,      SLOT( on_showDept     (const qint32) ) );
    connect(&check_state,   SIGNAL( mlUpdate   (const bool)   ), this,      SLOT( on_showML       (const bool)   ) );

    connect(&check_state,   SIGNAL( timer_clk_update           (const bool, const quint32) ), this, SLOT(on_timerClkUpdate(const bool, const quint32)          ) );
    connect(&check_state,   SIGNAL( time_start_meserment_update(const bool, const quint32) ), this, SLOT(on_timeStartMesermentUpdate(const bool, const quint32)) );
    connect(&check_state,   SIGNAL( time_stop_meserment_update (const bool, const quint32) ), this, SLOT(on_timeStopMesermentUpdate (const bool, const quint32)) );

    connect(&check_state,   SIGNAL( CRC1_update(const bool) ), this, SLOT(on_CRC1update(const bool)) );
    connect(&check_state,   SIGNAL( CRC2_update(const bool) ), this, SLOT(on_CRC2update(const bool)) );
    connect(&check_state,   SIGNAL( CRC3_update(const bool) ), this, SLOT(on_CRC3update(const bool)) );
    connect(&check_state,   SIGNAL( CRC4_update(const bool) ), this, SLOT(on_CRC4update(const bool)) );
    connect(&check_state,   SIGNAL( CRC5_update(const bool) ), this, SLOT(on_CRC5update(const bool)) );
    connect(&check_state,   SIGNAL( CRC6_update(const bool) ), this, SLOT(on_CRC6update(const bool)) );
    connect(&check_state,   SIGNAL( CRC7_update(const bool) ), this, SLOT(on_CRC7update(const bool)) );
    connect(&check_state,   SIGNAL( CRC8_update(const bool) ), this, SLOT(on_CRC8update(const bool)) );

    connect(&check_state,   SIGNAL( good_blk_cnt_update(const int) ), this, SLOT(on_goodBlkCntUpdate(const int)) );
    connect(&check_state,   SIGNAL( bad_blk_cnt_update (const int) ), this, SLOT(on_badBlkCntUpdate (const int)) );

    //-------------------------------------------------------------------------
    QRect  rect;
    QRectF rectf;
    rectf.setRect(0.0, 0.0, 100.0, 100.0);

    scene_vk = new QGraphicsScene(rectf);

    view_vk = new QGraphicsView(scene_vk);
    view_vk->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_vk->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_vk->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, VAK_8_NUM_POINTS * FKDstep, 380);

    vk1 = new Qt_VK(rect, scene_vk);
    vk1->setColorLine(VKColor);
    vk1->setColorBack(FonColor);
    vk1->setColorText(TextColor);
    vk1->setMaxAmpl(max_ampl);
    vk1->setXScale(FKDstep);
    vk1->setColorLevelBack(LevelColor);
    vk1->setCaption(QString::fromUtf8("Волновая картинка "));

    connect(this, SIGNAL( changeFKDstep(int)   ), vk1,      SLOT( on_changeTimeScale(int) ) );
    connect(this, SIGNAL( changeFKDlevel(int)  ), vk1,      SLOT( on_changeLevel(int)     ) );
    connect(this, SIGNAL( changeVKmaxAmpl(int) ), vk1,      SLOT( on_changeMaxAmpl(int)   ) );
    connect(vk1,  SIGNAL( update()             ), scene_vk, SLOT( update()                ) );

    vk1_greed = new Qt_biGREED(rect, scene_vk);
    vk1_greed->setColorLine(GreedColor);
    vk1_greed->setColorBack(Qt::transparent);
    vk1_greed->setLineWidth(1);
    vk1_greed->set_step_x_line(128);
    vk1_greed->set_num_y_line(4);

    time_line1 = new Qt_TIME_LINE(rect, scene_vk);
    time_line1->setColorText(TextColor);
    time_line1->setColorBack(Qt::transparent);
    time_line1->set_step_x_line(128);
    time_line1->set_time_zero(0);
    time_line1->set_time_step(4);
    time_line1->set_x_scale(FKDstep);

    connect(this, SIGNAL( changeFKDstep(int) ), time_line1, SLOT( on_changeTimeScale(int) ) );

    ui->horizontalLayout_VK->addWidget(view_vk);
    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 100.0, 100.0);
    rect.setRect(0, 0, VAK_8_NUM_POINTS, 100);

    scene_ctl_vk1 = new QGraphicsScene(rectf);

    view_ctl_vk1 = new QGraphicsView(scene_ctl_vk1);
    view_ctl_vk1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_ctl_vk1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_ctl_vk1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_ctl_vk1->scale(1.0, 1.0);

    ctl_vk1 = new Qt_VK(rect, scene_ctl_vk1);
    ctl_vk1->setColorLine(VKColor);
    ctl_vk1->setColorBack(FonColor);
    ctl_vk1->setColorText(TextColor);
    ctl_vk1->setMaxAmpl(max_ampl);
    ctl_vk1->setXScale(1);
    ctl_vk1->setColorLevelBack(FonColor);
    ctl_vk1->setCaption(QString::fromUtf8("ВК-1"));

    connect(this,     SIGNAL( changeVKmaxAmpl(int) ), ctl_vk1,       SLOT( on_changeMaxAmpl(int)   ) );
    connect(ctl_vk1,  SIGNAL( update()             ), scene_ctl_vk1, SLOT( update()                ) );

    ctl_vk1_greed = new Qt_biGREED(rect, scene_ctl_vk1);
    ctl_vk1_greed->setColorLine(GreedColor);
    ctl_vk1_greed->setColorBack(Qt::transparent);
    ctl_vk1_greed->setLineWidth(1);
    ctl_vk1_greed->set_step_x_line(128);
    ctl_vk1_greed->set_num_y_line(4);

    ui->verticalLayout_Ctl1VK->addWidget(view_ctl_vk1);
    //-------------------------------------------------------------------------
    scene_ctl_vk2 = new QGraphicsScene(rectf);

    view_ctl_vk2 = new QGraphicsView(scene_ctl_vk2);
    view_ctl_vk2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_ctl_vk2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_ctl_vk2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_ctl_vk2->scale(1.0, 1.0);

    ctl_vk2 = new Qt_VK(rect, scene_ctl_vk2);
    ctl_vk2->setColorLine(VKColor);
    ctl_vk2->setColorBack(FonColor);
    ctl_vk2->setColorText(TextColor);
    ctl_vk2->setMaxAmpl(max_ampl);
    ctl_vk2->setXScale(1);
    ctl_vk2->setColorLevelBack(FonColor);
    ctl_vk2->setCaption(QString::fromUtf8("ВК-2"));

    connect(this,     SIGNAL( changeVKmaxAmpl(int) ), ctl_vk2,       SLOT( on_changeMaxAmpl(int)   ) );
    connect(ctl_vk2,  SIGNAL( update()             ), scene_ctl_vk2, SLOT( update()                ) );

    ctl_vk2_greed = new Qt_biGREED(rect, scene_ctl_vk2);
    ctl_vk2_greed->setColorLine(GreedColor);
    ctl_vk2_greed->setColorBack(Qt::transparent);
    ctl_vk2_greed->setLineWidth(1);
    ctl_vk2_greed->set_step_x_line(128);
    ctl_vk2_greed->set_num_y_line(4);

    ui->verticalLayout_Ctl2VK->addWidget(view_ctl_vk2);
    //-------------------------------------------------------------------------

//    ui->widgetAllVK->installEventFilter(this);

    rectf.setRect(0.0, 0.0, 100.0, 100.0);
    scene_fkd = new QGraphicsScene(rectf);

    view_fkd = new QGraphicsView(scene_fkd);
    view_fkd->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_fkd->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_fkd->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_fkd->scale(1.0, 1.0);

    rect.setRect(0, 0, VAK_8_NUM_POINTS, 200);
    fkd = new CVAK32_FKD(rect, scene_fkd);
    fkd->setColorBack(FonColor);
    fkd->setColorGreed(GreedColor);
    fkd->setColorLine(FKDColor);
    fkd->on_changeGreedStepX(128);
    fkd->on_changeDpsX(dpsX);
    fkd->on_changeDpsY(dpsY);
    fkd->on_changeDeptScale(DepthScale);
    fkd->on_setDirectionUp();
    fkd->on_changeTimeScale(FKDstep);

    ui->verticalLayout_FKD->addWidget(view_fkd);

    connect(&check_state,   SIGNAL( deptUpdate(const qint32)), fkd,         SLOT( on_changeDept(const qint32) ) );
    connect(this,           SIGNAL( changeDpsX(int)         ), fkd,         SLOT( on_changeDpsX(int)          ) );
    connect(this,           SIGNAL( changeDpsY(int)         ), fkd,         SLOT( on_changeDpsY(int)          ) );
    connect(this,           SIGNAL( changeDeptScale(int)    ), fkd,         SLOT( on_changeDeptScale(int)     ) );
    connect(this,           SIGNAL( changeFKDstep(int)      ), fkd,         SLOT( on_changeTimeScale(int)     ) );
    connect(this,           SIGNAL( changeFKDlevel(int)     ), fkd,         SLOT( on_changeLevel(int)         ) );
    connect(fkd,            SIGNAL( update()                ), scene_fkd,   SLOT( update()                    ) );

    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 80.0, 150.0);
    scene_dept_col = new QGraphicsScene(rectf);

    view_dept_col = new QGraphicsView(scene_dept_col);
    view_dept_col->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_dept_col->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_dept_col->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, 80, 150);
    deptCol = new Qt_DEPTCOL(rect, scene_dept_col);
    deptCol->setColorBack(FonColor);
    deptCol->setColorLine(GreedColor);
    deptCol->setColorText(TextColor);
    deptCol->on_changeDpsY(dpsY);
    deptCol->on_changeDeptScale(DepthScale);
    deptCol->on_setDirectionUp();

    ui->verticalLayout_dept_col->addWidget(view_dept_col);

    connect(&check_state,   SIGNAL( deptUpdate(const qint32)     ), deptCol,         SLOT( on_changeDept(const qint32)     ) );
    connect(this,           SIGNAL( changeDpsY(int)              ), deptCol,         SLOT( on_changeDpsY(int)              ) );
    connect(this,           SIGNAL( changeDeptScale(int)         ), deptCol,         SLOT( on_changeDeptScale(int)         ) );
    connect(deptCol,        SIGNAL( update()                     ), scene_dept_col,  SLOT( update()                        ) );
    connect(deptCol,        SIGNAL( changeBaseLinesShift(qint16) ), fkd,             SLOT( on_changeBaseLinesShift(qint16) ) );

    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 10.0, 150.0);
    scene_ml_col = new QGraphicsScene(rectf);

    view_ml_col = new QGraphicsView(scene_ml_col);
    view_ml_col->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_ml_col->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_ml_col->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, 5, 150);
    mlCol = new Qt_ML(rect, scene_ml_col);
    mlCol->setColorBack(FonColor);
    mlCol->setColorML(MMColor);
    mlCol->on_changeDpsY(dpsY);
    mlCol->on_changeDeptScale(DepthScale);
    mlCol->on_setDirectionUp();
    mlCol->setLineWidth(2);

    ui->verticalLayout_ML->addWidget(view_ml_col);

    connect(&check_state,   SIGNAL( deptUpdate(const qint32)), mlCol,        SLOT( on_changeDept(const qint32) ) );
    connect(this,           SIGNAL( changeDpsY(int)         ), mlCol,        SLOT( on_changeDpsY(int)          ) );
    connect(this,           SIGNAL( changeDeptScale(int)    ), mlCol,        SLOT( on_changeDeptScale(int)     ) );
    connect(mlCol,          SIGNAL( update()                ), scene_ml_col, SLOT( update()                    ) );
    //-------------------------------------------------------------------------
    akp_thread->start();
    check_state.start();
    //-------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    //-------------------------------------------------------------------------
    save_settings();
    //-------------------------------------------------------------------------
    delete mlCol;
    delete view_ml_col;
    delete scene_ml_col;

    delete deptCol;
    delete view_dept_col;
    delete scene_dept_col;

    delete fkd;
    delete view_fkd;
    delete scene_fkd;

    delete ctl_vk2;
    delete ctl_vk2_greed;
    delete view_ctl_vk2;
    delete scene_ctl_vk2;

    delete ctl_vk1;
    delete ctl_vk1_greed;
    delete view_ctl_vk1;
    delete scene_ctl_vk1;

    delete time_line1;
    delete vk1_greed;
    delete vk1;
    delete view_vk;
    delete scene_vk;
    //-------------------------------------------------------------------------
    akp_thread->exit();
    while ( akp_thread->isRunning() );
    delete akp_thread;
    //-------------------------------------------------------------------------
    delete ui;
    //-------------------------------------------------------------------------
}
//-------------------------------------------------------------------
void MainWindow::on_pushButtonSettings(void)
{
    if (!dialogWellInfo)
    {
        dialogWellInfo = new Dialog_well_info(this);
        connect(dialogWellInfo, SIGNAL(cmd_Setup()), this, SLOT(on_dialogSetup()));
    }

    dialogWellInfo->setOperatorName(OperatorName);
    dialogWellInfo->setWellNo(WellNo);
    dialogWellInfo->setFildName(FildName);
    dialogWellInfo->setDept(Depth);
    dialogWellInfo->setDeptStep(DepthStep);
    dialogWellInfo->setFolderName(FolderName);

    if (dialogWellInfo->exec() == QDialog::Accepted)
    {
        OperatorName = dialogWellInfo->getOperatorName();
        WellNo       = dialogWellInfo->getWellNo();
        FildName     = dialogWellInfo->getFildName();
        Depth        = dialogWellInfo->getDept();
        startDepth   = Depth;
        lastDepth    = Depth;
        velocity->add_dept_point(Depth);
        DepthStep    = dialogWellInfo->getDeptStep();
        emit showDept(Depth);
        emit cmdSetDepth(Depth);
    }

    disconnect(dialogWellInfo, SIGNAL(cmd_Setup()), this, SLOT(on_dialogSetup()));
    delete dialogWellInfo;
    dialogWellInfo = NULL;

//    blk_count = 0;
//    bad_blk = 0;
//    emit setPocketCount(blk_count);
//    emit setBadPocketCount(bad_blk);

    check_state.clear_block_count();    //???

    curentDepthStep = 0;

    fkd->clearData();
    mlCol->delPoints();

    ui->pushButton_Start->setDisabled(false);

    delete dialogWellInfo;
    dialogWellInfo = NULL;
}
//-------------------------------------------------------------------
void MainWindow::on_dialogSetup(void)
{
    if (!dialogSetup)
    {
        dialogSetup = new Dialog_setup(this);
        connect(dialogSetup, SIGNAL(cmd_RestoreFolder()    ), this, SLOT(on_dialogToolModeCmdRestoreFolder()    ) );
    }

    dialogSetup->setMMColor(MMColor);
    dialogSetup->setFonColor(FonColor);
    dialogSetup->setGreedColor(GreedColor);
    dialogSetup->setTextColor(TextColor);
    dialogSetup->setLevelColor(LevelColor);
    dialogSetup->setFKDColor(FKDColor);
    dialogSetup->setFKDstep(FKDstep);
    dialogSetup->setVKColor(VKColor);
    dialogSetup->setVKlineSize(VKlineSize);
    dialogSetup->setDepthScale(DepthScale);
    dialogSetup->set_dpsX(dpsX);
    dialogSetup->set_dpsY(dpsY);
    dialogSetup->setFolder(FolderName);

    if (bExtFolderCtl) dialogSetup->setExtCtl();
    else dialogSetup->clrExtCtl();

    if (dialogSetup->exec() == QDialog::Accepted)
    {
        MMColor     = dialogSetup->getMMColor();
        FonColor    = dialogSetup->getFonColor();
        GreedColor  = dialogSetup->getGreedColor();
        TextColor   = dialogSetup->getTextColor();
        LevelColor  = dialogSetup->getLevelColor();
        FKDColor    = dialogSetup->getFKDColor();
        FKDstep     = dialogSetup->getFKDstep();
        VKColor     = dialogSetup->getVKColor();
        VKlineSize  = dialogSetup->getVKlineSize();
        DepthScale  = dialogSetup->getDepthScale();
        dpsX        = dialogSetup->get_dpsX();
        dpsY        = dialogSetup->get_dpsY();

        emit changeDpsX(dpsX);
        emit changeDpsY(dpsY);
        emit changeDeptScale(DepthScale);
        emit changeFKDstep(FKDstep);

        vk1->setColorBack(FonColor);
        vk1->setColorLevelBack(LevelColor);
        vk1->setColorLevelLine(LevelColor);
        vk1->setColorLine(VKColor);
        vk1->setColorText(TextColor);

        time_line1->setColorText(TextColor);

        vk1_greed->setColorLine(GreedColor);

        deptCol->setColorBack(FonColor);
        deptCol->setColorLine(GreedColor);
        deptCol->setColorText(TextColor);

        mlCol->setColorBack(FonColor);
        mlCol->setColorML(MMColor);

        fkd->setColorBack(FonColor);
        fkd->setColorGreed(GreedColor);
        fkd->setColorLine(FKDColor);

        ctl_vk1->setColorBack(FonColor);
        ctl_vk1->setColorLevelBack(FonColor);
        ctl_vk1->setColorLevelLine(FonColor);
        ctl_vk1->setColorLine(VKColor);
        ctl_vk1->setColorText(TextColor);

        ctl_vk1_greed->setColorLine(GreedColor);

        ctl_vk2->setColorBack(FonColor);
        ctl_vk2->setColorLevelBack(FonColor);
        ctl_vk2->setColorLevelLine(FonColor);
        ctl_vk2->setColorLine(VKColor);
        ctl_vk2->setColorText(TextColor);

        ctl_vk2_greed->setColorLine(GreedColor);

        FolderName  = dialogSetup->getFolder();
        dialogWellInfo->setFolderName(FolderName);

        bExtFolderCtl = dialogSetup->get_ExtCtl();
    }

    disconnect(dialogSetup, SIGNAL(cmd_RestoreFolder()    ), this, SLOT(on_dialogToolModeCmdRestoreFolder()    ) );
//    disconnect(dialogSetup, SIGNAL(cmd_ChangeToolModes()  ), this, SLOT(on_dialogToolModeCmdChangeToolModes()  ) );
//    disconnect(dialogSetup, SIGNAL(cmd_RestoreToolModes() ), this, SLOT(on_dialogToolModeCmdRestoreToolModes() ) );

    delete dialogSetup;
    dialogSetup = NULL;
}
//-------------------------------------------------------------------
//void MainWindow::on_dialogToolModeCmdRestoreFolder(void)
//{
//    dialogSetup->setFolder(FolderName);
//}
//-------------------------------------------------------------------
//void MainWindow::on_dialogToolModeCmdChangeToolModes(void)
//{
//    if (!dialogToolMode)
//    {
//        dialogToolMode = new Dialog_Tool_Mode(this);
//    }

//    mode = mode_list.begin();
//    for (; mode != mode_list.end(); ++mode)
//    {
//        vak32_ctrl_command_class* cmd = *mode;

//        dialogToolMode->set_izl_type(cmd->get_izl_type());
//        dialogToolMode->set_Fsig(cmd->get_Fsig());
//        dialogToolMode->set_ampl_sig(cmd->get_ampl_sig());
//        dialogToolMode->set_period_number(cmd->get_period_number());

//        dialogToolMode->set_rx_type(cmd->get_rx_type());
//        dialogToolMode->set_Td(cmd->get_Td());
//        dialogToolMode->set_rx_delay(cmd->get_rx_delay());
//        dialogToolMode->set_KU_window_start(cmd->get_KU_window_start());
//        dialogToolMode->set_KU_window_width(cmd->get_KU_window_width());

//        if (dialogToolMode->exec() == QDialog::Accepted)
//        {
//            cmd->on_set_izl_type(dialogToolMode->get_izl_type());
//            cmd->on_set_Fsig(dialogToolMode->get_Fsig());
//            cmd->on_set_ampl_sig(dialogToolMode->get_ampl_sig());
//            cmd->on_set_period_number(dialogToolMode->get_period_number());

//            cmd->on_set_rx_type(dialogToolMode->get_rx_type());
//            cmd->on_set_Td(dialogToolMode->get_Td());
//            cmd->on_set_rx_delay(dialogToolMode->get_rx_delay());
//            cmd->on_set_KU_window_start(dialogToolMode->get_KU_window_start());
//            cmd->on_set_KU_window_width(dialogToolMode->get_KU_window_width());

//            dialogSetup->setToolMode(cmd);
//        }
//    }

//    delete dialogToolMode;
//    dialogToolMode = NULL;
//}
//-------------------------------------------------------------------
//void MainWindow::on_dialogToolModeCmdRestoreToolModes(void)
//{
//}
//-------------------------------------------------------------------
void MainWindow::on_cmdIncAmpl(void)
{
    int new_max = vk1->maxAmpl();
    if (new_max < 16000)
    {
        new_max = new_max * 2;
        max_ampl = new_max;
        emit changeVKmaxAmpl(max_ampl);
    }
    ui->groupBox_3->setTitle(QString::fromUtf8("Амплитуда (%1)").arg(new_max));
}
//-------------------------------------------------------------------
void MainWindow::on_cmdDecAmpl(void)
{
    int new_max = vk1->maxAmpl();
    if (new_max > 100)
    {
        new_max = new_max / 2;
        max_ampl = new_max;
        emit changeVKmaxAmpl(max_ampl);
    }
    ui->groupBox_3->setTitle(QString::fromUtf8("Амплитуда (%1)").arg(new_max));
}
//-------------------------------------------------------------------
void MainWindow::on_cmdIncLevel(void)
{
    int max_ampl = vk1->maxAmpl();
    fkd_level += max_ampl / 16;
    if (fkd_level > max_ampl) fkd_level = max_ampl;
    emit changeFKDlevel(fkd_level);
}
//-------------------------------------------------------------------
void MainWindow::on_cmdDecLevel(void)
{
    int max_ampl = vk1->maxAmpl();
    fkd_level -= max_ampl / 16;
    if (fkd_level < 0) fkd_level = 0;
    emit changeFKDlevel(fkd_level);
}
//-------------------------------------------------------------------
void MainWindow::on_pushButtonStart(void)
{
    ui->pushButton_Start->setDisabled(true);
    ui->pushButton_Start->setVisible(false);

    ui->pushButton_Stop->setVisible(true);
    ui->pushButton_Stop->setDisabled(false);

    ui->pushButton_Record->setDisabled(false);

    ui->pushButton_Settings->setDisabled(true);

    emit cmdSetDeptStep(0);       //???
//    emit cmdStartMeserment();
}
//-------------------------------------------------------------------
void MainWindow::on_pushButtonStop(void)
{
    ui->pushButton_Start->setDisabled(true);
    ui->pushButton_Start->setVisible(true);

    ui->pushButton_Stop->setVisible(false);
    ui->pushButton_Stop->setDisabled(false);

    ui->pushButton_Record->setDisabled(true);

    ui->pushButton_Settings->setDisabled(false);

    bWriteEnable = false;

//    emit cmdStopMeserment();
}
//-------------------------------------------------------------------
void MainWindow::on_pushButtonRecord(void)
{
    QString FileExt = QString::fromUtf8(".4sd");
    QDate   date = QDate::currentDate();

    ui->pushButton_Record->setDisabled(true);

//    blk_count = 0;
//    bad_blk = 0;
//    emit setPocketCount(blk_count);
//    emit setBadPocketCount(bad_blk);

    check_state.clear_block_count();    //???

    curentDepthStep = -DepthStep;
    akp.on_setDeptStep(DepthStep);       //???

    FileName = FolderName;
    if (bExtFolderCtl)
    {
        FileName += QString::fromUtf8("/");
        FileName += FildName;
        FileName += QString::fromUtf8("/");
        FileName += WellNo;
        FileName += QString::fromUtf8("/");
        if (date.day() < 10) FileName += QString::fromUtf8("0");
        FileName += QString::fromUtf8("%1_").arg(date.day());
        if (date.month() < 10) FileName += QString::fromUtf8("0");
        FileName += QString::fromUtf8("%1_").arg(date.month());
        FileName += QString::fromUtf8("%1").arg(date.year());
    }
    QDir dir;
    if (!dir.mkpath(FileName))
    {
        //throw
        qDebug() << QString::fromUtf8("Ошибка создания директории: %1").arg(FileName);
        return;
    }

    FileName += QString::fromUtf8("/");
    FileName += WellNo;

    qDebug() << QString::fromUtf8("%1").arg(FileName);

    char ch;
    for (ch = 'a'; ch <= 'z'; ch++)
    {
        if (!QFile::exists(QString::fromUtf8("%1_%2%3").arg(FileName).arg(ch).arg(FileExt))) break;
    }
    FileName += QString::fromUtf8("_%1").arg(ch);
    FileName += FileExt;

    qDebug() << QString::fromUtf8("%1").arg(FileName);
/*
    write_vak32_head(FileName, QDate::currentDate(), FildName, WellNo,
                          OperatorName, Depth, QString::fromUtf8("ВАК-32"), ToolNo,
                            393, 393, 393, 393,
                            403, 403, 403, 403,
                            413, 413, 413, 413,
                            423, 423, 423, 423,
                            433, 433, 433, 433,
                            443, 443, 443, 443,
                            453, 453, 453, 453,
                            463, 463, 463, 463
                          );
*/
    bWriteEnable = true;

    startDepth = Depth;
}
//-------------------------------------------------------------------
//#pragma pack(1)
//void MainWindow::on_cmdSetDepth(void)
//{
//    akp.on_cmdSetDepth(Depth);
//}
//#pragma pack()
//-------------------------------------------------------------------
//#pragma pack(1)
//void MainWindow::on_vak32CmdSend(void)
//{
//    TCTRLPOCKET                 ctl_pocket;
//    unsigned char               ctl_cmd[21];
//    vak32_ctrl_command_class*   vak32_ctl_cmd;
//    u_short                     cmd = (u_short)COMAND_VAK32_DO_MESERMENT;

//    timer.stop();
//    timer.setInterval(timer_interval);

//    memset((char*)&ctl_pocket, 0, sizeof(TCTRLPOCKET));
//    ctl_pocket.id = htons(cmd);
//    p_count++;
//    ctl_pocket.n_pocket = htons(p_count);
//    ctl_pocket.dept = htonl(curentDepthStep);

//    if (mode == mode_list.end()) mode = mode_list.begin();
//    vak32_ctl_cmd = *mode;
//    mode++;

//    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    if (vak32_ctl_cmd->get_mode_number() == (unsigned int)modeNum4fkd)
//    {
//        vak32_ctl_cmd->on_set_vk_number(0x1F & vkNum4fkd);
//    }
//    else
//    {
//        vkNum += 8;
//        if (vkNum == 39) vkNum = 0;
//        if (vkNum >  31) vkNum -= 31;
//        vak32_ctl_cmd->on_set_vk_number(0x1F & vkNum);
//    }
//    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//    vak32_ctl_cmd->write_command(ctl_cmd);
//    int i;
//    for (i = 0; i < 21; i++)
//        ctl_pocket.rezerv[i] = htons(ctl_cmd[i]);

//    udp_socket.writeDatagram((char*)&ctl_pocket, sizeof(TCTRLPOCKET), host, port);
//}
//#pragma pack()
//-------------------------------------------------------------------
//#pragma pack(1)
//void MainWindow::on_udpDataRx(void)
//{
//    int                         i;
//    unsigned char               x_data[sizeof(TDATAPOCKET)];
//    TCTRLPOCKET*                pctrl_pocket = (TCTRLPOCKET*)&x_data;
//    TDATAPOCKET*                pdata_pocket = (TDATAPOCKET*)&x_data;
//    QHostAddress                sender;
//    quint16                     senderPort;
//    vak32_ctrl_command_class    rx_cmd;
////    unsigned char               rx_cmd_data[21];
//    TVAK32_4SD                  data_4sd;
//    quint16                     rx_data_CRC16;

//    do
//    {
//        memset(x_data, 0, sizeof(TDATAPOCKET));
//        udp_socket.readDatagram((char*)x_data, sizeof(TDATAPOCKET), &sender, &senderPort);
//    } while (udp_socket.hasPendingDatagrams());

//    for(i = 0; i < VAK_8_NUM_POINTS; i++) v8_data[i] = ntohs(pdata_pocket->data[i]);

//    rx_data_CRC16 = calc_rx_data_CRC16();
//    if (rx_data_CRC16 == 0)
//    {
//        bflag_data_CRC = false;
//        label_data_CRC_check->setText(QString::fromUtf8("CRC данных: Ок"));
//        ToolNo = v8_data[474];
//    }
//    else
//    {
//        bflag_data_CRC = true;
//        label_data_CRC_check->setText(QString::fromUtf8("CRC данных: Ошибка!"));
//    }

////    Depth = ntohl(pctrl_pocket->dept);
////    velocity->add_dept_point(Depth);
////    emit showDept(Depth);

//    for(i = 0; i < 21; i++)
////        rx_cmd_data[i] = (unsigned char)v8_data[490 + i];
//        data_4sd.cmd_data[i] = (unsigned char)v8_data[490 + i];
////    rx_cmd.read_command(rx_cmd_data);
//    rx_cmd.read_command(data_4sd.cmd_data);

//    if (rx_cmd.calc_CRC16() == 0)
//    {
//        bflag_CMD_CRC = false;
//        label_CMD_CRC_check->setText(QString::fromUtf8("CRC команды: Ок"));
//    }
//    else
//    {
//        bflag_CMD_CRC = true;
//        label_CMD_CRC_check->setText(QString::fromUtf8("CRC команды: Ошибка!"));
//    }

//    if (bWriteEnable)
//    {
//        data_4sd.vak32_rx_data_CRC16 = v8_data[511];
//        data_4sd.vak32_calc_data_CRC16 = rx_data_CRC16;
//        data_4sd.SD_error_flag = v8_data[485];
//        data_4sd.SD_file_char = v8_data[472];
//        data_4sd.blk_count = blk_count;
//        data_4sd.bad_blk = bad_blk;
//        data_4sd.log_velocity = velocity->get_velocity();
////        rx_cmd.write_command(data_4sd.cmd_data);
//        write_vak32_4SD_data(FileName, data_4sd);
//    }

//    try
//    {
////        emit showIZLtype(rx_cmd.get_izl_type());
////        emit showIZLfreq(rx_cmd.get_Fsig());
////        emit showIZLnum(rx_cmd.get_period_number());

////        emit showRXtd(rx_cmd.get_Td());
////        emit showRXdelay(rx_cmd.get_rx_delay());

//        modeNum = rx_cmd.get_mode_number();
//        vkNumRx = rx_cmd.get_vk_number();
////        switch(vkNumRx)
////        {
////            case 0:
////                emit showRXku(v8_data[486]);
////                break;
////            case 1:
////                emit showRXku(v8_data[487]);
////                break;
////            case 2:
////                emit showRXku(v8_data[488]);
////                break;
////            case 3:
////                emit showRXku(v8_data[489]);
////                break;
////            case 4:
////                emit showRXku(v8_data[486]);
////                break;
////            case 5:
////                emit showRXku(v8_data[487]);
////                break;
////            case 6:
////                emit showRXku(v8_data[488]);
////                break;
////            case 7:
////                emit showRXku(v8_data[489]);
////                break;
////            default:
////                emit showRXku(-1);
////                break;
////        }

//        emit showNewData();

//    }
//    catch (vak32_ctrl_cmd_error_class err)
//    {
//        qDebug() << err.get_error_msg();
//    }

//    if (bflag_CMD_CRC | bflag_data_CRC) bad_blk++;
//    else blk_count++;

////    emit setPocketCount(blk_count);
////    emit setBadPocketCount(bad_blk);

//    cmd_id = 0;

//    timer.singleShot(timer_interval, this, SLOT(on_vak32CmdSend()));
//}
//#pragma pack()
//-------------------------------------------------------------------
void MainWindow::on_showPocketCount(const int count)
{
    label_GoodBlk->setText(QString::fromUtf8("Хорошие блоки: %1").arg(count));
}
//-------------------------------------------------------------------
void MainWindow::on_showBadPocketCount(const int count)
{
    label_BadBlk->setText(QString::fromUtf8("Плохие блоки: %1").arg(count));
}
//-------------------------------------------------------------------
void MainWindow::on_showDept(const qint32 dept)
{
    if ( ((lastDepth + curentDepthStep) != dept) && bWriteEnable )
    {
        QColor color = Qt::red;

        statusBar()->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    }
    else
    {
        QColor color = palette().background().color();

        statusBar()->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    }

    lastDepth = dept;

    label_Depth->setText(QString::fromUtf8("Глубина: %1 м")
                                .arg( dept / 100.0, 0, 'f', 2)
                         );

    label_Distance->setText(QString::fromUtf8("Пройдено: %1 м")
                                   .arg( abs(startDepth - dept) / 100.0, 0, 'f', 2)
                            );

    velocity->add_dept_point(dept);
    label_Velocity->setText(QString::fromUtf8("Скорость: %1 м/ч")
                                   .arg(velocity->get_velocity())
                            );
}
//-------------------------------------------------------------------
void MainWindow::on_showML(const bool ml)
{
    QColor  color;

    if (ml)
    {
        color = MMColor;
        mlCol->setML(Depth);    //==========>>??????
    }
    else    color = palette().background().color();

    label_ML->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void MainWindow::on_showNewData(const quint16 vk_no, const TVAK8_VK &vk)
{
    if (vk_no == vkNum4fkd)
    {
        vk1->setCaption(QString::fromUtf8("ВК-%1  ").arg(vkNum4fkd + 1));
        vk1->AddData(vk);

        fkd->addData(lastDepth, vk);
    }
    if (vk_no == 0)
    {
        ctl_vk1->setCaption(QString::fromUtf8("ВК-1"));
        ctl_vk1->AddData(vk);
    }
    else if(vk_no == 5)
    {
        ctl_vk2->setCaption(QString::fromUtf8("ВК-2"));
        ctl_vk2->AddData(vk);
    }
}
//-------------------------------------------------------------------
void MainWindow::on_showIZLtype(const bool crc, const quint16 value)
{
    Q_UNUSED(crc);
    Q_UNUSED(value);

    ui->label_IZLtype->setText("Монополь");
//    switch (value)
//    {
//    case IZL_MONOPOL:
//        ui->label_IZLtype->setText("Монополь");
//        break;
//    case IZL_DIPOL_1:
//        ui->label_IZLtype->setText("Диполь-1");
//        break;
//    case IZL_DIPOL_2:
//        ui->label_IZLtype->setText("Диполь-2");
//        break;
//    case IZL_QUADROPOL:
//        ui->label_IZLtype->setText("Квадруполь");
//        break;
//    case IZL_SFERA_1:
//        ui->label_IZLtype->setText("Сфера-1");
//        break;
//    case IZL_SFERA_2:
//        ui->label_IZLtype->setText("Сфера-2");
//        break;
//    case IZL_SFERA_3:
//        ui->label_IZLtype->setText("Сфера-3");
//        break;
//    case IZL_SFERA_4:
//        ui->label_IZLtype->setText("Сфера-4");
//        break;
//    default:
//        ui->label_IZLtype->setText("НЕИЗВЕСТНО");
//        break;
//    }
}
//-------------------------------------------------------------------
void MainWindow::on_showIZLfreq(const bool crc, const quint16 value)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_IZLfreq->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->label_IZLfreq->setText(QString::fromUtf8("%1 кГц").arg(value));
}
//-------------------------------------------------------------------
void MainWindow::on_showIZLnum(const bool crc, const quint16 value)
{
    Q_UNUSED(crc);
    Q_UNUSED(value);

    ui->label_IZLnum->setText(QString::fromUtf8("2 периода"));
//    switch (value)
//    {
//        case 1:
//            ui->label_IZLnum->setText(QString::fromUtf8("1 период"));
//            break;
//        case 2:
//            ui->label_IZLnum->setText(QString::fromUtf8("2 периода"));
//            break;
//        case 3:
//            ui->label_IZLnum->setText(QString::fromUtf8("3 периода"));
//            break;
//        case 4:
//            ui->label_IZLnum->setText(QString::fromUtf8("4 периода"));
//            break;
//        case 5:
//            ui->label_IZLnum->setText(QString::fromUtf8("5 периодов"));
//            break;
//        case 6:
//            ui->label_IZLnum->setText(QString::fromUtf8("6 периодов"));
//            break;
//        case 7:
//            ui->label_IZLnum->setText(QString::fromUtf8("7 периодов"));
//            break;
//        case 8:
//            ui->label_IZLnum->setText(QString::fromUtf8("8 периодов"));
//            break;
//        default:
//            ui->label_IZLnum->setText(QString::fromUtf8("НЕИЗВЕСТНО"));
//            break;
//    }
}
//-------------------------------------------------------------------
void MainWindow::on_showRXdelay(const bool crc, const qint16 value)
{
    qint16 delay = 2 * value;
    time_line1->set_time_zero(delay / time_line1->get_time_step());

    QColor  color = get_color_on_CRC(crc);
    ui->label_RXdelay->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->label_RXdelay->setText(QString::fromUtf8("Задержка %1 мкс").arg(delay));
}
//-------------------------------------------------------------------
void MainWindow::on_showRXtd(const bool crc, const qint16 value)
{
    Q_UNUSED(crc);
    Q_UNUSED(value);

    Td = 2;
    time_line1->set_time_step(Td);
    ui->label_RXtd->setText(QString::fromUtf8("Тд = %1 мкс").arg(Td));

//    switch (value)
//    {
//    case TD_4_MKS:
//        Td = 4;
//        time_line1->set_time_step(Td);
//        ui->label_RXtd->setText(QString::fromUtf8("Тд = %1 мкс").arg(Td));
//        break;
//    case TD_8_MKS:
//        Td = 8;
//        time_line1->set_time_step(Td);
//        ui->label_RXtd->setText(QString::fromUtf8("Тд = %1 мкс").arg(Td));
//        break;
//    case TD_16_MKS:
//        Td = 16;
//        time_line1->set_time_step(Td);
//        ui->label_RXtd->setText(QString::fromUtf8("Тд = %1 мкс").arg(Td));
//        break;
//    case TD_32_MKS:
//        Td = 32;
//        time_line1->set_time_step(Td);
//        ui->label_RXtd->setText(QString::fromUtf8("Тд = %1 мкс").arg(Td));
//        break;
//    default:
//        ui->label_RXtd->setText(QString::fromUtf8("Тд - НЕИЗВЕСТНО"));
//        //time_line1->set_time_step(?);
//        break;
//    }
}
//-------------------------------------------------------------------
void MainWindow::on_showRXku(const bool crc, const quint16 value)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_RXku->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    if (value < 0)
        ui->label_RXku->setText(QString::fromUtf8("КУ = нет данных"));
    else
        ui->label_RXku->setText(QString::fromUtf8("КУ = %1").arg(value + 1));
}
//-------------------------------------------------------------------
void MainWindow::on_VKxClicked(int id)
{
    vkNum4fkd = id - 1;
}
//-------------------------------------------------------------------
QColor MainWindow::get_color_on_CRC(const bool crc)
{
    if (crc)
    {
        return QColor(Qt::black);
    }
    else
    {
        return QColor(Qt::red);
    }
}
//-------------------------------------------------------------------
void MainWindow::load_settings(void)
{
    app_settings->beginGroup(QString::fromUtf8("/Settings"));

    OperatorName    = app_settings->value(QString::fromUtf8("/OperatorName"),  QString::fromUtf8("Я")      ).toString();
    WellNo          = app_settings->value(QString::fromUtf8("/WellNo"),        QString::fromUtf8("0")      ).toString();
    FildName        = app_settings->value(QString::fromUtf8("/FildName"),      QString::fromUtf8("None")   ).toString();
    Depth           = app_settings->value(QString::fromUtf8("/Depth"),         0                           ).toInt();
    startDepth      = Depth;
    lastDepth       = Depth;
    DepthStep       = app_settings->value(QString::fromUtf8("/DepthStep"),     2                           ).toInt();
    curentDepthStep = 0;
    FolderName      = app_settings->value(QString::fromUtf8("/FolderName"),    QString::fromUtf8("c:")     ).toString();
    bExtFolderCtl   = app_settings->value(QString::fromUtf8("/ExtFolderCtl"),  true                        ).toBool();
    FileName        = QString::fromUtf8("%1/%2a.4sd").arg(FolderName).arg(WellNo);
    bWriteEnable    = false;

    QColor color = QColor(Qt::red);
    MMColor.setRgba(  app_settings->value(QString::fromUtf8("/MMColor"),       color.rgba()                ).toInt());

    color = palette().background().color();
    FonColor.setRgba( app_settings->value(QString::fromUtf8("/FonColor"),      color.rgba()                ).toInt());

    color = QColor(Qt::green);
    GreedColor.setRgba(app_settings->value(QString::fromUtf8("/GreedColor"),   color.rgba()                ).toInt());

    color = QColor(Qt::black);
    TextColor.setRgba(app_settings->value(QString::fromUtf8("/TextColor"),     color.rgba()                ).toInt());

    color = QColor(Qt::gray);
    LevelColor.setRgba(app_settings->value(QString::fromUtf8("/LevelColor"),   color.rgba()                ).toInt());

    color = QColor(Qt::gray);
    FKDColor.setRgba(app_settings->value(QString::fromUtf8("/FKDColor"),       color.rgba()                ).toInt());

    FKDstep         = app_settings->value(QString::fromUtf8("/FKDstep"),       1                           ).toInt();

    color = QColor(Qt::blue);
    VKColor.setRgba(app_settings->value(QString::fromUtf8("/VKColor"),         color.rgba()                ).toInt());

    VKlineSize      = app_settings->value(QString::fromUtf8("/VKlineSize"),    1                           ).toInt();
    DepthScale      = app_settings->value(QString::fromUtf8("/DepthScale"),    100                         ).toInt();
    dpsX            = app_settings->value(QString::fromUtf8("/dpsX"),          26                          ).toInt();
    dpsY            = app_settings->value(QString::fromUtf8("/dpsY"),          36                          ).toInt();

//    vkNum           = 0;
//    vkNumRx         = 0;
    vkNum4fkd       = 0;
//    modeNum         = 0;
//    modeNum4fkd     = 0;

    max_ampl        = app_settings->value(QString::fromUtf8("/MaxAmpl"),       32000                       ).toInt();
    fkd_level       = app_settings->value(QString::fromUtf8("/FkdLevel"),      0                           ).toInt();

//    int izl_type;
//    int freq;
//    int period_num;
//    int Td;
//    int delay;
//    int KU_win_start;
//    int KU_win_width;
//    int i = 0;
//    app_settings->beginGroup(QString::fromUtf8("Mode_%1").arg(i+1));
//        izl_type        = app_settings->value(QString::fromUtf8("/IzlType"),        IZL_MONOPOL ).toInt();
//        freq            = app_settings->value(QString::fromUtf8("/IzlFreq"),        20          ).toInt();
//        period_num      = app_settings->value(QString::fromUtf8("/PeriodNum"),      2           ).toInt();
//        Td              = app_settings->value(QString::fromUtf8("/Td"),             TD_2_MKS    ).toInt();
//        delay           = app_settings->value(QString::fromUtf8("/Delay"),          0           ).toInt();
//        KU_win_start    = app_settings->value(QString::fromUtf8("/KU_win_start"),   60          ).toInt();
//        KU_win_width    = app_settings->value(QString::fromUtf8("/KU_win_width"),   290         ).toInt();

//    app_settings->endGroup();

    app_settings->endGroup();
}
//-----------------------------------------------------------------------------
void MainWindow::save_settings(void)
{
/*
    app_settings->beginGroup(QString::fromUtf8("/Settings"));

    app_settings->setValue(QString::fromUtf8("/OperatorName"), OperatorName         );
    app_settings->setValue(QString::fromUtf8("/WellNo"),       WellNo               );
    app_settings->setValue(QString::fromUtf8("/FildName"),     FildName             );

    app_settings->setValue(QString::fromUtf8("/Depth"),        Depth                );
    app_settings->setValue(QString::fromUtf8("/DepthStep"),    DepthStep            );

    app_settings->setValue(QString::fromUtf8("/FolderName"),   FolderName           );
    app_settings->setValue(QString::fromUtf8("/ExtFolderCtl"), bExtFolderCtl        );

    app_settings->setValue(QString::fromUtf8("/MMColor"),      MMColor.rgba()       );
    app_settings->setValue(QString::fromUtf8("/FonColor"),     FonColor.rgba()      );
    app_settings->setValue(QString::fromUtf8("/GreedColor"),   GreedColor.rgba()    );
    app_settings->setValue(QString::fromUtf8("/TextColor"),    TextColor.rgba()     );
    app_settings->setValue(QString::fromUtf8("/LevelColor"),   LevelColor.rgba()    );
    app_settings->setValue(QString::fromUtf8("/FKDColor"),     FKDColor.rgba()      );

    app_settings->setValue(QString::fromUtf8("/FKDstep"),      FKDstep              );

    app_settings->setValue(QString::fromUtf8("/VKColor"),      VKColor.rgba()       );

    app_settings->setValue(QString::fromUtf8("/VKlineSize"),   VKlineSize           );
    app_settings->setValue(QString::fromUtf8("/DepthScale"),   DepthScale           );
    app_settings->setValue(QString::fromUtf8("/dpsX"),         dpsX                 );
    app_settings->setValue(QString::fromUtf8("/dpsY"),         dpsY                 );

    app_settings->setValue(QString::fromUtf8("/MaxAmpl"),      max_ampl             );
    app_settings->setValue(QString::fromUtf8("/FkdLevel"),     fkd_level            );

    app_settings->setValue(QString::fromUtf8("/ModeCount"),    mode_list.size()     );

    int i = 0;
    mode = mode_list.begin();
    vak32_ctrl_command_class* modeX;

    app_settings->beginGroup(QString::fromUtf8("Mode_%1").arg(i+1));
        modeX = *mode;
        app_settings->setValue(QString::fromUtf8("/IzlType"),        modeX->get_izl_type()       );
        app_settings->setValue(QString::fromUtf8("/IzlFreq"),        modeX->get_Fsig()           );
        app_settings->setValue(QString::fromUtf8("/PeriodNum"),      modeX->get_period_number()  );
        app_settings->setValue(QString::fromUtf8("/Td"),             modeX->get_Td()             );
        app_settings->setValue(QString::fromUtf8("/Delay"),          modeX->get_rx_delay()       );
        app_settings->setValue(QString::fromUtf8("/KU_win_start"),   modeX->get_KU_window_start());
        app_settings->setValue(QString::fromUtf8("/KU_win_width"),   modeX->get_KU_window_width());
    app_settings->endGroup();


    app_settings->endGroup();
*/
}
//-----------------------------------------------------------------------------
/*
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->widgetAllVK)
    {
        if (event->type() == QEvent::Resize)
        {
            QResizeEvent* rsizeEvent = static_cast<QResizeEvent*>(event);
            QSize new_size = rsizeEvent->size();

            QRect rect;
            int new_top = 0;
            int new_height = new_size.height();
            new_height -=1;
            new_height /= 4;
            new_height /= 4;
            new_height *= 4;

            rect.setRect(0, new_top, VAK32_WAVE_NUM_POINTS, new_height);
            int x = rect.x();
            int y = rect.y();
            dop_vk1->setPos(x, y);
            dop_vk1->set_height(new_height);
            dop_vk1_greed->setPos(x, y);
            dop_vk1_greed->set_num_y_line(4);
            dop_vk1_greed->set_height(new_height);

            new_top += new_height;
            rect.setRect(0, new_top, VAK32_WAVE_NUM_POINTS, new_height);
            x = rect.x();
            y = rect.y();
            dop_vk2->setPos(x, y);
            dop_vk2->set_height(new_height);
            dop_vk2_greed->setPos(x, y);
            dop_vk2_greed->set_num_y_line(4);
            dop_vk2_greed->set_height(new_height);

            new_top += new_height;
            rect.setRect(0, new_top, VAK32_WAVE_NUM_POINTS, new_height);
            x = rect.x();
            y = rect.y();
            dop_vk3->setPos(x, y);
            dop_vk3->set_height(new_height);
            dop_vk3_greed->setPos(x, y);
            dop_vk3_greed->set_num_y_line(4);
            dop_vk3_greed->set_height(new_height);

            new_top += new_height;
            rect.setRect(0, new_top, VAK32_WAVE_NUM_POINTS, new_height);
            x = rect.x();
            y = rect.y();
            dop_vk4->setPos(x, y);
            dop_vk4->set_height(new_height);
            dop_vk4_greed->setPos(x, y);
            dop_vk4_greed->set_num_y_line(4);
            dop_vk4_greed->set_height(new_height);
        }
    }
    return QMainWindow::eventFilter(target, event);

}
//-----------------------------------------------------------------------------
*/
