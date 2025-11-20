//-----------------------------------------------------------------------------
#include <QString>
#include <QColor>
#include <QDialog>
#include <QVector>
//#include <QtNetwork/QUdpSocket>
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
#if defined(Q_OS_LINUX)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#elif defined(Q_OS_WIN)
    #include <winsock.h>
#endif
/*
#ifdef linux
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#else
    #include <winsock.h>
#endif
*/
//-----------------------------------------------------------------------------
#include "mainwindow.h"
#include "ui_mainwindow.h"
//-----------------------------------------------------------------------------
#include "dialog_setup.h"
#include "dialog_well_info.h"
#include "vak_8.h"
#include "vak_8_2pc.h"
#include "qt_vak_32_fkd.h"
#include "qt_deptcol.h"
#include "qt_ml.h"
#include "qt_vk.h"
//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app_settings(new QSettings("TNG-Group", "qt_akp_reg")),
    dialogSetup(NULL),
    dialogWellInfo(NULL),
    akp(parent),
    check_state(parent),
    akp_file(parent),
    velocity(new CVELOCITY(this))
{
    //-------------------------------------------------------------------------
    qRegisterMetaType<TDataPocket>("TDataPocket");
    qRegisterMetaType<TCtrlPocket>("TCtrlPocket");
    //-------------------------------------------------------------------------
    akp_thread = new QThread();
    akp.moveToThread(akp_thread);
    connect(akp_thread, &QThread::started,            &akp,       &akp_class::start     );
    connect(&akp,       &akp_class::connectionClosed, akp_thread, &QThread::quit        );
    connect(akp_thread, &QThread::finished,           akp_thread, &QThread::deleteLater );
    //-------------------------------------------------------------------------
    file_thread = new QThread();
    akp_file.moveToThread(file_thread);
    connect(file_thread, &QThread::started,         &akp_file,     &qt_akp_file_save::start );
    connect(&akp_file,   &qt_akp_file_save::closed, file_thread,   &QThread::quit           );
    connect(file_thread, &QThread::finished,        file_thread,   &QThread::deleteLater    );
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

    label_Distance = new QLabel(QString::fromUtf8("Пройдено: 99999.99 м"));
    label_Distance->setAlignment(Qt::AlignLeft);
    label_Distance->setMinimumSize(label_Distance->sizeHint());
    statusBar()->addWidget(label_Distance);
    label_Distance->setText(QString::fromUtf8("Пройдено: 0.00 м"));

    label_Velocity = new QLabel(QString::fromUtf8("Скорость: 9999999 м/ч"));
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
    //-------------------------------------------------------------------------
    connect(&akp,           &akp_class::dataUpdate,         &check_state,   &akp_check_state::onDataUpdate  );
//    connect(&akp,           &akp_class::dataUpdate,         &akp_file,      &qt_akp_file_save::onDataUpdate );

    connect(&check_state,  &akp_check_state::deptUpdate,            this, &MainWindow::on_showDept          );
    connect(&check_state,  &akp_check_state::mlUpdate,              this, &MainWindow::on_showML            );

    connect(&check_state,  &akp_check_state::VK_update,             this, &MainWindow::on_showNewData       );
    connect(&check_state,  &akp_check_state::good_blk_cnt_update,   this, &MainWindow::on_showPocketCount   );
    connect(&check_state,  &akp_check_state::bad_blk_cnt_update,    this, &MainWindow::on_showBadPocketCount);

    connect(&check_state,  &akp_check_state::izl_type_update,       this, &MainWindow::on_showIZLtype       );
    connect(&check_state,  &akp_check_state::Fsig_update,           this, &MainWindow::on_showIZLfreq       );
    connect(&check_state,  &akp_check_state::izl_periods_update,    this, &MainWindow::on_showIZLnum        );

    connect(&check_state,  &akp_check_state::rx_type_update,        this, &MainWindow::on_showRXtype        );
    connect(&check_state,  &akp_check_state::rx_delay_update,       this, &MainWindow::on_showRXdelay       );
    connect(&check_state,  &akp_check_state::Td_update,             this, &MainWindow::on_showRXtd          );
    connect(&check_state,  &akp_check_state::Ku_update,             this, &MainWindow::on_showRXku          );

    connect(&check_state,  &akp_check_state::tool_no_update,        this, &MainWindow::on_showToolNo        );
    connect(&check_state,  &akp_check_state::soft_version_update,   this, &MainWindow::on_showSoftVer       );

    connect(&check_state,  &akp_check_state::time_meserment_update, this, &MainWindow::on_showTimeMeserment );
    //-------------------------------------------------------------------------
    connect(ui->pushButton_Start,    &QPushButton::pressed,         this, &MainWindow::on_pushButtonStart   );
    connect(ui->pushButton_Stop,     &QPushButton::pressed,         this, &MainWindow::on_pushButtonStop    );
    connect(ui->pushButton_Record,   &QPushButton::pressed,         this, &MainWindow::on_pushButtonRecord  );
    connect(ui->pushButton_Settings, &QPushButton::pressed,         this, &MainWindow::on_pushButtonSettings);
    connect(ui->pushButton_incAmpl,  &QPushButton::pressed,         this, &MainWindow::on_cmdIncAmpl        );
    connect(ui->pushButton_decAmpl,  &QPushButton::pressed,         this, &MainWindow::on_cmdDecAmpl        );
    connect(ui->pushButton_upFKD,    &QPushButton::pressed,         this, &MainWindow::on_cmdIncLevel       );
    connect(ui->pushButton_downFKD,  &QPushButton::pressed,         this, &MainWindow::on_cmdDecLevel       );
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    connect(this,                   &MainWindow::cmdSetDepth,       &akp,           &akp_class::on_cmdSetDepth          );
    connect(this,                   &MainWindow::cmdSetDepth,       this,           &MainWindow::on_showDept            );

    connect(ui->pushButton_Start,   &QPushButton::pressed,          &akp,           &akp_class::on_cmdStartMeserment    );

    connect(ui->pushButton_Stop,    &QPushButton::pressed,          &akp,           &akp_class::on_cmdStopMeserment     );
    connect(ui->pushButton_Stop,    &QPushButton::pressed,          &check_state,   &akp_check_state::clear_block_count );

    connect(this,                   &MainWindow::cmdSetDeptStep,    &akp,           &akp_class::on_setDeptStep          );
    //-------------------------------------------------------------------------
    QRect  rect;
    QRectF rectf;
    rectf.setRect(0.0, 0.0, 100.0, 100.0);

    scene_vk1 = new QGraphicsScene(rectf);

    view_vk1 = new QGraphicsView(scene_vk1);
    view_vk1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_vk1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_vk1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, VAK_8_NUM_POINTS * FKDstep, 380);

    vk1 = new Qt_VK(rect, scene_vk1);
    vk1->setColorLine(VKColor);
    vk1->setColorBack(FonColor);
    vk1->setColorText(TextColor);
    vk1->setMaxAmpl(max_ampl);
    vk1->setXScale(FKDstep);
    vk1->setColorLevelBack(LevelColor);
    vk1->setLineWidth(VKlineSize);
    vk1->setCaption(QString::fromUtf8("ВК-1 "));

    connect(this, &MainWindow::changeFKDstep,   vk1,        &Qt_VK::on_changeTimeScale  );
    connect(this, &MainWindow::changeFKDlevel,  vk1,        &Qt_VK::on_changeLevel      );
    connect(this, &MainWindow::changeVKmaxAmpl, vk1,        &Qt_VK::on_changeMaxAmpl    );
    connect(vk1,  SIGNAL( update() ),           scene_vk1,  SLOT( update()            ) );

    vk1_greed = new Qt_biGREED(rect, scene_vk1);
    vk1_greed->setColorLine(GreedColor);
    vk1_greed->setColorBack(Qt::transparent);
    vk1_greed->setLineWidth(1);
    vk1_greed->set_step_x_line(128);
    vk1_greed->set_num_y_line(4);

    time_line_vk1 = new Qt_TIME_LINE(rect, scene_vk1);
    time_line_vk1->setColorText(TextColor);
    time_line_vk1->setColorBack(Qt::transparent);
    time_line_vk1->set_step_x_line(128);
    time_line_vk1->set_time_zero(0);
    time_line_vk1->set_time_step(2);       // Td 2mks
    time_line_vk1->set_x_scale(FKDstep);

    connect(this, &MainWindow::changeFKDstep,   time_line_vk1, &Qt_TIME_LINE::on_changeTimeScale );

    ui->horizontalLayout_VK1->addWidget(view_vk1);
    //-------------------------------------------------------------------------
    ui->groupBox_Ampl_Ctrl->setTitle(QString::fromUtf8("Амплитуда (%1 / div)").arg(vk1->maxAmpl() / vk1_greed->get_num_y_line()));
    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 100.0, 100.0);

    scene_vk2 = new QGraphicsScene(rectf);

    view_vk2 = new QGraphicsView(scene_vk2);
    view_vk2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_vk2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_vk2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, VAK_8_NUM_POINTS * FKDstep, 380);

    vk2 = new Qt_VK(rect, scene_vk2);
    vk2->setColorLine(VKColor);
    vk2->setColorBack(FonColor);
    vk2->setColorText(TextColor);
    vk2->setMaxAmpl(max_ampl);
    vk2->setXScale(FKDstep);
    vk2->setColorLevelBack(LevelColor);
    vk2->setLineWidth(VKlineSize);
    vk2->setCaption(QString::fromUtf8("ВК-2 "));

    connect(this, &MainWindow::changeFKDstep,   vk2,        &Qt_VK::on_changeTimeScale  );
    connect(this, &MainWindow::changeFKDlevel,  vk2,        &Qt_VK::on_changeLevel      );
    connect(this, &MainWindow::changeVKmaxAmpl, vk2,        &Qt_VK::on_changeMaxAmpl    );
    connect(vk2,  SIGNAL( update() ),           scene_vk2,  SLOT( update()            ) );

    vk2_greed = new Qt_biGREED(rect, scene_vk2);
    vk2_greed->setColorLine(GreedColor);
    vk2_greed->setColorBack(Qt::transparent);
    vk2_greed->setLineWidth(1);
    vk2_greed->set_step_x_line(128);
    vk2_greed->set_num_y_line(4);

    time_line_vk2 = new Qt_TIME_LINE(rect, scene_vk2);
    time_line_vk2->setColorText(TextColor);
    time_line_vk2->setColorBack(Qt::transparent);
    time_line_vk2->set_step_x_line(128);
    time_line_vk2->set_time_zero(0);
    time_line_vk2->set_time_step(2);       // Td 2mks
    time_line_vk2->set_x_scale(FKDstep);

    connect(this, &MainWindow::changeFKDstep,   time_line_vk2, &Qt_TIME_LINE::on_changeTimeScale );

    ui->horizontalLayout_VK2->addWidget(view_vk2);
    //-------------------------------------------------------------------------

//    ui->widgetAllVK->installEventFilter(this);

    rectf.setRect(0.0, 0.0, 100.0, 100.0);
    scene_fkd_vk1 = new QGraphicsScene(rectf);

    view_fkd_vk1 = new QGraphicsView(scene_fkd_vk1);
    view_fkd_vk1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_fkd_vk1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_fkd_vk1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_fkd_vk1->scale(1.0, 1.0);

    rect.setRect(0, 0, VAK_8_NUM_POINTS, 200);
    fkd_vk1 = new CVAK32_FKD(rect, scene_fkd_vk1);
    fkd_vk1->setColorBack(FonColor);
    fkd_vk1->setColorGreed(GreedColor);
    fkd_vk1->setColorLine(FKDColor);
    fkd_vk1->on_changeGreedStepX(128);
    fkd_vk1->on_changeDpsX(dpsX);
    fkd_vk1->on_changeDpsY(dpsY);
    fkd_vk1->on_changeDeptScale(DepthScale);
    fkd_vk1->on_setDirectionUp();
    fkd_vk1->on_changeTimeScale(FKDstep);

    ui->verticalLayout_VK1_FKD->addWidget(view_fkd_vk1);

    connect(&check_state,   &akp_check_state::deptUpdate,   fkd_vk1,         &CVAK32_FKD::on_changeDept         );
    connect(this,           &MainWindow::changeDpsX,        fkd_vk1,         &CVAK32_FKD::on_changeDpsX         );
    connect(this,           &MainWindow::changeDpsY,        fkd_vk1,         &CVAK32_FKD::on_changeDpsY         );
    connect(this,           &MainWindow::changeDeptScale,   fkd_vk1,         &CVAK32_FKD::on_changeDeptScale    );
    connect(this,           &MainWindow::changeFKDstep,     fkd_vk1,         &CVAK32_FKD::on_changeTimeScale    );
    connect(this,           &MainWindow::changeFKDlevel,    fkd_vk1,         &CVAK32_FKD::on_changeLevel        );
    connect(fkd_vk1,        SIGNAL( update() ),             scene_fkd_vk1,   SLOT( update() )                   );

    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 100.0, 100.0);
    scene_fkd_vk2 = new QGraphicsScene(rectf);

    view_fkd_vk2 = new QGraphicsView(scene_fkd_vk2);
    view_fkd_vk2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_fkd_vk2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_fkd_vk2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_fkd_vk2->scale(1.0, 1.0);

    rect.setRect(0, 0, VAK_8_NUM_POINTS, 200);
    fkd_vk2 = new CVAK32_FKD(rect, scene_fkd_vk2);
    fkd_vk2->setColorBack(FonColor);
    fkd_vk2->setColorGreed(GreedColor);
    fkd_vk2->setColorLine(FKDColor);
    fkd_vk2->on_changeGreedStepX(128);
    fkd_vk2->on_changeDpsX(dpsX);
    fkd_vk2->on_changeDpsY(dpsY);
    fkd_vk2->on_changeDeptScale(DepthScale);
    fkd_vk2->on_setDirectionUp();
    fkd_vk2->on_changeTimeScale(FKDstep);

    ui->verticalLayout_VK2_FKD->addWidget(view_fkd_vk2);

    connect(&check_state,   &akp_check_state::deptUpdate,   fkd_vk2,         &CVAK32_FKD::on_changeDept         );
    connect(this,           &MainWindow::changeDpsX,        fkd_vk2,         &CVAK32_FKD::on_changeDpsX         );
    connect(this,           &MainWindow::changeDpsY,        fkd_vk2,         &CVAK32_FKD::on_changeDpsY         );
    connect(this,           &MainWindow::changeDeptScale,   fkd_vk2,         &CVAK32_FKD::on_changeDeptScale    );
    connect(this,           &MainWindow::changeFKDstep,     fkd_vk2,         &CVAK32_FKD::on_changeTimeScale    );
    connect(this,           &MainWindow::changeFKDlevel,    fkd_vk2,         &CVAK32_FKD::on_changeLevel        );
    connect(fkd_vk2,        SIGNAL( update() ),             scene_fkd_vk2,   SLOT( update() )                   );

    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 80.0, 100.0);
    scene_dept_col = new QGraphicsScene(rectf);

    view_dept_col = new QGraphicsView(scene_dept_col);
    view_dept_col->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_dept_col->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_dept_col->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, 80, 100);
    deptCol = new Qt_DEPTCOL(rect, scene_dept_col);
    deptCol->setColorBack(FonColor);
    deptCol->setColorLine(GreedColor);
    deptCol->setColorText(TextColor);
    deptCol->on_changeDpsY(dpsY);
    deptCol->on_changeDeptScale(DepthScale);
    deptCol->on_setDirectionUp();

    ui->verticalLayout_dept_col->addWidget(view_dept_col);

    connect(&check_state,   &akp_check_state::deptUpdate,       deptCol,         &Qt_DEPTCOL::on_changeDept             );
    connect(this,           &MainWindow::changeDpsY,            deptCol,         &Qt_DEPTCOL::on_changeDpsY             );
    connect(this,           &MainWindow::changeDeptScale,       deptCol,         &Qt_DEPTCOL::on_changeDeptScale        );
    connect(this,           &MainWindow::cmdSetDepth,           deptCol,         &Qt_DEPTCOL::on_changeDept             );
    connect(deptCol,        SIGNAL( update() ),                 scene_dept_col,  SLOT( update()                       ) );
    connect(deptCol,        &Qt_DEPTCOL::changeBaseLinesShift,  fkd_vk1,         &CVAK32_FKD::on_changeBaseLinesShift   );
    connect(deptCol,        &Qt_DEPTCOL::changeBaseLinesShift,  fkd_vk2,         &CVAK32_FKD::on_changeBaseLinesShift   );

    //-------------------------------------------------------------------------
    rectf.setRect(0.0, 0.0, 10.0, 100.0);
    scene_ml_col = new QGraphicsScene(rectf);

    view_ml_col = new QGraphicsView(scene_ml_col);
    view_ml_col->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view_ml_col->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_ml_col->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    rect.setRect(0, 0, 5, 100);
    mlCol = new Qt_ML(rect, scene_ml_col);
    mlCol->setColorBack(FonColor);
    mlCol->setColorML(MMColor);
    mlCol->on_changeDpsY(dpsY);
    mlCol->on_changeDeptScale(DepthScale);
    mlCol->on_setDirectionUp();
    mlCol->setLineWidth(2);

    ui->verticalLayout_ML->addWidget(view_ml_col);

    connect(&check_state,   &akp_check_state::deptUpdate,       mlCol,        &Qt_ML::on_changeDept         );
    connect(this,           &MainWindow::changeDpsY,            mlCol,        &Qt_ML::on_changeDpsY         );
    connect(this,           &MainWindow::changeDeptScale,       mlCol,        &Qt_ML::on_changeDeptScale    );
    connect(mlCol,          SIGNAL( update() ),                 scene_ml_col, SLOT( update()              ) );
    //-------------------------------------------------------------------------
    akp_thread->start();
    file_thread->start();
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

    delete fkd_vk2;
    delete view_fkd_vk2;
    delete scene_fkd_vk2;

    delete time_line_vk2;
    delete vk2_greed;
    delete vk2;
    delete view_vk2;
    delete scene_vk2;

    delete fkd_vk1;
    delete view_fkd_vk1;
    delete scene_fkd_vk1;

    delete time_line_vk1;
    delete vk1_greed;
    delete vk1;
    delete view_vk1;
    delete scene_vk1;
    //-------------------------------------------------------------------------
    akp_thread->exit();
    while ( akp_thread->isRunning() );
    delete akp_thread;
    //-------------------------------------------------------------------------
    file_thread->exit();
    while ( file_thread->isRunning() );
    delete file_thread;
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
        connect(dialogWellInfo, &Dialog_well_info::cmd_Setup, this, &MainWindow::on_dialogSetup);
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

    disconnect(dialogWellInfo, &Dialog_well_info::cmd_Setup, this, &MainWindow::on_dialogSetup);
    delete dialogWellInfo;
    dialogWellInfo = NULL;

    check_state.clear_block_count();    //???

    curentDepthStep = 0;

    fkd_vk1->clearData();
    fkd_vk2->clearData();
    mlCol->delPoints();

    ui->pushButton_Start->setDisabled(false);
}
//-------------------------------------------------------------------
void MainWindow::on_dialogSetup(void)
{
    if (!dialogSetup)
    {
        dialogSetup = new Dialog_setup(this);
        connect(dialogSetup, &Dialog_setup::cmd_RestoreFolder, this, &MainWindow::on_dialogToolModeCmdRestoreFolder );
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
        //---------------------------------------------------------------------
        vk1->setColorBack(FonColor);
        vk1->setColorLevelBack(LevelColor);
        vk1->setColorLevelLine(LevelColor);
        vk1->setColorLine(VKColor);
        vk1->setColorText(TextColor);
        vk1->setLineWidth(VKlineSize);

        time_line_vk1->setColorText(TextColor);

        vk1_greed->setColorLine(GreedColor);
        //---------------------------------------------------------------------
        vk2->setColorBack(FonColor);
        vk2->setColorLevelBack(LevelColor);
        vk2->setColorLevelLine(LevelColor);
        vk2->setColorLine(VKColor);
        vk2->setColorText(TextColor);
        vk2->setLineWidth(VKlineSize);

        time_line_vk2->setColorText(TextColor);

        vk2_greed->setColorLine(GreedColor);
        //---------------------------------------------------------------------
        deptCol->setColorBack(FonColor);
        deptCol->setColorLine(GreedColor);
        deptCol->setColorText(TextColor);
        //---------------------------------------------------------------------
        mlCol->setColorBack(FonColor);
        mlCol->setColorML(MMColor);
        //---------------------------------------------------------------------
        fkd_vk1->setColorBack(FonColor);
        fkd_vk1->setColorGreed(GreedColor);
        fkd_vk1->setColorLine(FKDColor);
        //---------------------------------------------------------------------
        fkd_vk2->setColorBack(FonColor);
        fkd_vk2->setColorGreed(GreedColor);
        fkd_vk2->setColorLine(FKDColor);
        //---------------------------------------------------------------------
        FolderName  = dialogSetup->getFolder();
        dialogWellInfo->setFolderName(FolderName);
        //---------------------------------------------------------------------
        bExtFolderCtl = dialogSetup->get_ExtCtl();
    }

    disconnect(dialogSetup, &Dialog_setup::cmd_RestoreFolder, this, &MainWindow::on_dialogToolModeCmdRestoreFolder );

    delete dialogSetup;
    dialogSetup = NULL;
}
//-------------------------------------------------------------------
void MainWindow::on_dialogToolModeCmdRestoreFolder(void)
{
    dialogSetup->setFolder(FolderName);
}
//-------------------------------------------------------------------
void MainWindow::on_cmdIncAmpl(void)
{
    int new_max;
    int new_step;

    new_max = vk1->maxAmpl();
    new_step = new_max / vk1_greed->get_num_y_line();

    switch(new_step)
    {
//        case 10000:
//        {
//            new_step =  5000;
//            break;
//        }
        case 5000:
        {
            new_step =  2000;
            break;
        }
        case 2000:
        {
            new_step =  1000;
            break;
        }
        case 1000:
        {
            new_step =  500;
            break;
        }
        case 500:
        {
            new_step =  200;
            break;
        }
        case 200:
        {
            new_step =  100;
            break;
        }
        case 100:
        {
            new_step =  50;
            break;
        }
        case 50:
        {
//            new_step =  20;
            break;
        }
//        case 20:
//        {
//            new_step =  10;
//            break;
//        }
        default:
        {
            new_step =  50;
            break;
        }
    }

    new_max = new_step * vk1_greed->get_num_y_line();
    max_ampl = new_max;
    emit changeVKmaxAmpl(max_ampl);

    ui->groupBox_Ampl_Ctrl->setTitle(QString::fromUtf8("Амплитуда (%1 / div)").arg(new_step));
}
//-------------------------------------------------------------------
void MainWindow::on_cmdDecAmpl(void)
{
    int new_max;
    int new_step;

    new_max = vk1->maxAmpl();
    new_step = new_max / vk1_greed->get_num_y_line();
    switch(new_step)
    {
        case 5000:
        {
//            new_step =  10000;
            break;
        }
        case 2000:
        {
            new_step =  5000;
            break;
        }
        case 1000:
        {
            new_step =  2000;
            break;
        }
        case 500:
        {
            new_step =  1000;
            break;
        }
        case 200:
        {
            new_step =  500;
            break;
        }
        case 100:
        {
            new_step =  200;
            break;
        }
        case 50:
        {
            new_step =  100;
            break;
        }
//        case 20:
//        {
//            new_step =  50;
//            break;
//        }
//        case 10:
//        {
//            new_step =  20;
//            break;
//        }
        default:
        {
            new_step =  5000;
            break;
        }
    }

    new_max = new_step * vk1_greed->get_num_y_line();
    max_ampl = new_max;
    emit changeVKmaxAmpl(max_ampl);

    ui->groupBox_Ampl_Ctrl->setTitle(QString::fromUtf8("Амплитуда (%1 / div)").arg(new_step));
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

    bCvantumDept = false;
    curentDepthStep = 0;
    emit cmdSetDeptStep(curentDepthStep);

    startDepth = lastDepth;

    fkd_vk1->clearData();
    fkd_vk2->clearData();

}
//-------------------------------------------------------------------
void MainWindow::on_pushButtonStop(void)
{
    ui->pushButton_Stop->setDisabled(true);
    ui->pushButton_Stop->setVisible(false);

    ui->pushButton_Start->setVisible(true);
    ui->pushButton_Start->setDisabled(true);

    ui->pushButton_Record->setDisabled(true);

    ui->pushButton_Settings->setDisabled(false);

    disconnect(&akp,    &akp_class::dataUpdate,     &akp_file,      &qt_akp_file_save::onDataUpdate );

    bCvantumDept = false;
    curentDepthStep = 0;
    emit cmdSetDeptStep(curentDepthStep);

    startDepth = check_state.get_dept();

    akp_file.close_file();
}
//-----------------------------------------------------------------------------
void MainWindow::on_pushButtonRecord(void)
{
    ui->pushButton_Record->setDisabled(true);

    check_state.clear_block_count();    //???

    bCvantumDept = true;
    curentDepthStep = -DepthStep;
    emit cmdSetDeptStep(curentDepthStep);

    startDepth = check_state.get_dept();
    //-------------------------------------------------------------------------
    akp_file.setBufLen(20);
    akp_file.setFolderName(FolderName);
    akp_file.setFildName(FildName);
    akp_file.setWellNo          (WellNo);
    akp_file.setOperatorName    (OperatorName);
    akp_file.setCurrentDate     ();
    akp_file.setCurrentTime     ();
    akp_file.setStartDepth      ( startDepth );

    akp_file.setShiftPointIZL   (0);
    akp_file.setShiftPointVK1   (100);
    akp_file.setShiftPointVK2   (150);

    if (bExtFolderCtl)
        akp_file.setExtendedFolderCtl();
    else
        akp_file.clearExtendedFolderCtl();

    akp_file.find_validFileName ();
    akp_file.write_head();

    connect(&akp,   &akp_class::dataUpdate,     &akp_file,  &qt_akp_file_save::onDataUpdate );
    //-------------------------------------------------------------------------

    fkd_vk1->clearData();
    fkd_vk2->clearData();
}
//-----------------------------------------------------------------------------
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
    if ( ((lastDepth + curentDepthStep) != dept) && bCvantumDept )
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
        mlCol->setML(lastDepth);    //==========>>??????
    }
    else    color = palette().background().color();

    label_ML->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void MainWindow::on_showNewData(const quint16 vk_no, const TVAK8_WAVE &vk)
{
    if (vk_no == 0)
    {
//        qDebug() << QString::fromUtf8("ВК-1 AddData");
        vk1->AddData(vk);
        fkd_vk1->delete_no_use_points();
//        qDebug() << QString::fromUtf8("ВК-1 FKD addData");
//        fkd_vk1->addData(lastDepth, vk);
        fkd_vk1->addData(check_state.get_dept(), vk);
//        qDebug() << QString::fromUtf8("ВК-1 complited");
    }
    else if(vk_no == 4)
    {
//        qDebug() << QString::fromUtf8("ВК-2 AddData");
        vk2->AddData(vk);
        fkd_vk2->delete_no_use_points();
//        qDebug() << QString::fromUtf8("ВК-2 FKD addData");
//        fkd_vk2->addData(lastDepth, vk);
        fkd_vk2->addData(check_state.get_dept(), vk);
//        qDebug() << QString::fromUtf8("ВК-2 complited");
    }
}
//-------------------------------------------------------------------
void MainWindow::on_showIZLtype(const bool crc, const quint16 value)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_IZLtype->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

    switch (value)
    {
        case 0: //IZL_MONOPOL:
            ui->label_IZLtype->setText("Монополь");
            break;
        default:
            ui->label_IZLtype->setText("НЕИЗВЕСТНО");
            break;
    }
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
    QColor  color = get_color_on_CRC(crc);
    ui->label_IZLnum->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

    switch (value)
    {
        case 1:
            ui->label_IZLnum->setText(QString::fromUtf8("1 период"));
            break;
        case 2:
            ui->label_IZLnum->setText(QString::fromUtf8("2 периода"));
            break;
        case 3:
            ui->label_IZLnum->setText(QString::fromUtf8("3 периода"));
            break;
        case 4:
            ui->label_IZLnum->setText(QString::fromUtf8("4 периода"));
            break;
        case 5:
            ui->label_IZLnum->setText(QString::fromUtf8("5 периодов"));
            break;
        case 6:
            ui->label_IZLnum->setText(QString::fromUtf8("6 периодов"));
            break;
        case 7:
            ui->label_IZLnum->setText(QString::fromUtf8("7 периодов"));
            break;
        case 8:
            ui->label_IZLnum->setText(QString::fromUtf8("8 периодов"));
            break;
        default:
            ui->label_IZLnum->setText(QString::fromUtf8("НЕИЗВЕСТНО"));
            break;
    }
}
//-------------------------------------------------------------------
void MainWindow::on_showRXtype  (const bool crc, const quint16 value)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_RXtype->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    if (0 == value)
    {
        ui->label_RXtype->setText(QString::fromUtf8("Монополь"));
    }
    else
    {
        ui->label_RXtype->setText(QString::fromUtf8("Тип неизвестен"));
    }
}
//-------------------------------------------------------------------
void MainWindow::on_showRXdelay(const bool crc, const quint16 value)
{
    qint16 delay = 2 * value;
    time_line_vk1->set_time_zero(delay / time_line_vk1->get_time_step());

    QColor  color = get_color_on_CRC(crc);
    ui->label_RXdelay->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->label_RXdelay->setText(QString::fromUtf8("Задержка %1 мкс").arg(delay));
}
//-------------------------------------------------------------------
void MainWindow::on_showRXtd(const bool crc, const quint16 value)
{
    Q_UNUSED(crc);
    Q_UNUSED(value);

    Td = 2;
    time_line_vk1->set_time_step(Td);
    ui->label_RXtd->setText(QString::fromUtf8("Тд = %1 мкс").arg(Td));
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
void MainWindow::on_showToolNo  (const bool crc, const quint16 value)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_ToolNo->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

    ui->label_ToolNo->setText(QString::fromUtf8("Прибор № %1").arg(value));
}
//-------------------------------------------------------------------
void MainWindow::on_showSoftVer (const bool crc, const quint16 soft_version_major, const quint16 soft_version_minor)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_SoftVer->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

    ui->label_SoftVer->setText(QString::fromUtf8("Версия прошивки: %1.%2").arg(soft_version_major).arg(soft_version_minor));
}
//-------------------------------------------------------------------
void MainWindow::on_showTimeMeserment(const bool crc, const quint32 time_meserment)
{
    QColor  color = get_color_on_CRC(crc);
    ui->label_TimeOfMeserment->setStyleSheet(QString::fromUtf8("color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

    ui->label_TimeOfMeserment->setText(QString::fromUtf8("Интервал = %1 мкс").arg(time_meserment));
}
//-------------------------------------------------------------------
QColor MainWindow::get_color_on_CRC(const bool crc)
{
    if (crc)
    {
        return TextColor;
//        return QColor(Qt::black);
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
    DepthStep       = app_settings->value(QString::fromUtf8("/DepthStep"),     10                           ).toInt();
    curentDepthStep = 0;
#if defined(Q_OS_LINUX)
    FolderName      = QDir::toNativeSeparators(app_settings->value(QString::fromUtf8("/FolderName"),    QString::fromUtf8("./Скважины")     ).toString());
#elif defined(Q_OS_WIN)
    FolderName      = app_settings->value(QString::fromUtf8("/FolderName"),    QString::fromUtf8("D:\\Скважины")     ).toString();
#endif
    bExtFolderCtl   = app_settings->value(QString::fromUtf8("/ExtFolderCtl"),  true                        ).toBool();
    FileName        = QString::fromUtf8("%1/%2a.4sd").arg(FolderName).arg(WellNo);
    bCvantumDept    = false;

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

    FKDstep         = app_settings->value(QString::fromUtf8("/FKDstep"),       2                           ).toInt();

    color = QColor(Qt::blue);
    VKColor.setRgba(app_settings->value(QString::fromUtf8("/VKColor"),         color.rgba()                ).toInt());

    VKlineSize      = app_settings->value(QString::fromUtf8("/VKlineSize"),    1                           ).toInt();
    DepthScale      = app_settings->value(QString::fromUtf8("/DepthScale"),    100                         ).toInt();
    dpsX            = app_settings->value(QString::fromUtf8("/dpsX"),          26                          ).toInt();
    dpsY            = app_settings->value(QString::fromUtf8("/dpsY"),          26                          ).toInt();

    max_ampl        = app_settings->value(QString::fromUtf8("/MaxAmpl"),       8000                        ).toInt();
    fkd_level       = app_settings->value(QString::fromUtf8("/FkdLevel"),      0                           ).toInt();

    app_settings->endGroup();
}
//-----------------------------------------------------------------------------
void MainWindow::save_settings(void)
{
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

    app_settings->endGroup();

    app_settings->sync();
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
