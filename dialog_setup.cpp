//-------------------------------------------------------------------
#include <QPalette>
#include <QString>
#include <QFileDialog>
#include <QColorDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
//-------------------------------------------------------------------
#include "dialog_setup.h"
#include "ui_dialog_setup.h"
//-------------------------------------------------------------------
#include "vak32_ctl_cmd.h"
//-------------------------------------------------------------------
Dialog_setup::Dialog_setup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_setup)
{
    ui->setupUi(this);

    ui->tableWidget->setRowCount(5);
    ui->tableWidget->setColumnCount(7);

    connect(ui->pushButton_ok,              SIGNAL(pressed()), this, SLOT(accept()                              ));
    connect(ui->pushButtonMMColor,          SIGNAL(pressed()), this, SLOT(on_pushButtonMMColor_clicked()        ));
    connect(ui->pushButtonFonColor,         SIGNAL(pressed()), this, SLOT(on_pushButtonFonColor_clicked()       ));
    connect(ui->pushButtonGreedColor,       SIGNAL(pressed()), this, SLOT(on_pushButtonGreedColor_clicked()     ));
    connect(ui->pushButtonTextColor,        SIGNAL(pressed()), this, SLOT(on_pushButtonTextColor_clicked()      ));
    connect(ui->pushButtonLevelColor,       SIGNAL(pressed()), this, SLOT(on_pushButtonLevelColor_clicked()     ));
    connect(ui->pushButtonFKDColor,         SIGNAL(pressed()), this, SLOT(on_pushButtonFKDColor_clicked()       ));
    connect(ui->pushButtonVKColor,          SIGNAL(pressed()), this, SLOT(on_pushButtonVKColor_clicked()        ));
    connect(ui->pushButtonFolderChange,     SIGNAL(pressed()), this, SLOT(on_pushButtonFolderChange_clicked()   ));
    connect(ui->pushButtonFolderRestore,    SIGNAL(pressed()), this, SIGNAL(cmd_RestoreFolder()                 ));
    connect(ui->pushButtonToolModesChange,  SIGNAL(pressed()), this, SIGNAL(cmd_ChangeToolModes()               ));
    connect(ui->pushButtonToolModesRestore, SIGNAL(pressed()), this, SIGNAL(cmd_RestoreToolModes()              ));
}
//-------------------------------------------------------------------
Dialog_setup::~Dialog_setup()
{
    delete ui;
}
//-------------------------------------------------------------------
void Dialog_setup::setToolMode(vak32_ctrl_command_class *mode)
{
    QTableWidgetItem *qsIzlType;
    QTableWidgetItem *qsFreq;
    QTableWidgetItem *qsPeriodNo;
    QTableWidgetItem *qsTd;
    QTableWidgetItem *qsDelay;
    QTableWidgetItem *qsKuWinStart;
    QTableWidgetItem *qsKuWinWidth;

    int row = mode->get_mode_number();
    switch (mode->get_izl_type())
    {
    case IZL_MONOPOL:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Монополь"));
        break;
    case IZL_DIPOL_1:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Диполь-1"));
        break;
    case IZL_DIPOL_2:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Диполь-2"));
        break;
    case IZL_QUADROPOL:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Квадруполь"));
        break;
    case IZL_SFERA_1:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Сфера-1"));
        break;
    case IZL_SFERA_2:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Сфера-2"));
        break;
    case IZL_SFERA_3:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Сфера-3"));
        break;
    case IZL_SFERA_4:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8("Сфера-4"));
        break;
    default:
        qsIzlType = new QTableWidgetItem(QString::fromUtf8(""));
        break;
    }
    qsFreq = new QTableWidgetItem(QString::fromUtf8("%1 кГц").arg(mode->get_Fsig()));
    qsPeriodNo = new QTableWidgetItem(QString::fromUtf8("%1").arg(mode->get_period_number()));
    switch (mode->get_Td())
    {
    case TD_4_MKS:
        qsTd = new QTableWidgetItem(QString::fromUtf8("4 мкс"));
        break;
    case TD_8_MKS:
        qsTd = new QTableWidgetItem(QString::fromUtf8("8 мкс"));
        break;
    case TD_16_MKS:
        qsTd = new QTableWidgetItem(QString::fromUtf8("16 мкс"));
        break;
    case TD_32_MKS:
        qsTd = new QTableWidgetItem(QString::fromUtf8("32 мкс"));
        break;
    default:
        qsTd = new QTableWidgetItem(QString::fromUtf8("неизвестно"));
        break;
    }
    qsDelay = new QTableWidgetItem(QString::fromUtf8("%1 мкс").arg(mode->get_rx_delay() * 2));
    qsKuWinStart = new QTableWidgetItem(QString::fromUtf8("%1").arg(mode->get_KU_window_start()));
    qsKuWinWidth = new QTableWidgetItem(QString::fromUtf8("%1").arg(mode->get_KU_window_width()));

    ui->tableWidget->setItem(row, 0, qsIzlType);
    ui->tableWidget->setItem(row, 1, qsFreq);
    ui->tableWidget->setItem(row, 2, qsPeriodNo);
    ui->tableWidget->setItem(row, 3, qsTd);
    ui->tableWidget->setItem(row, 4, qsDelay);
    ui->tableWidget->setItem(row, 5, qsKuWinStart);
    ui->tableWidget->setItem(row, 6, qsKuWinWidth);
}
//-------------------------------------------------------------------
void Dialog_setup::setMMColor(QColor color)
{
    ui->pushButtonMMColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
//    ui->pushButtonMMColor->setAutoFillBackground(true);
}
//-------------------------------------------------------------------
void Dialog_setup::setFonColor(QColor color)
{
    ui->pushButtonFonColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void Dialog_setup::setGreedColor(QColor color)
{
    ui->pushButtonGreedColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void Dialog_setup::setTextColor(QColor color)
{
    ui->pushButtonTextColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void Dialog_setup::setLevelColor(QColor color)
{
    ui->pushButtonLevelColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void Dialog_setup::setFKDColor(QColor color)
{
    ui->pushButtonFKDColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void Dialog_setup::setVKColor(QColor color)
{
    ui->pushButtonVKColor->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}
//-------------------------------------------------------------------
void Dialog_setup::setFKDstep(int step)
{
    switch(step)
    {
    case 1:
        ui->comboBox_FKD_step->setCurrentIndex(0);
        break;
    case 2:
        ui->comboBox_FKD_step->setCurrentIndex(1);
        break;
    case 3:
        ui->comboBox_FKD_step->setCurrentIndex(2);
        break;
    default:
        ui->comboBox_FKD_step->setCurrentIndex(1);
        break;
    }
}
//-------------------------------------------------------------------
void Dialog_setup::setVKlineSize(int size)
{
    switch(size)
    {
    case 1:
        ui->comboBox_VK_line_size->setCurrentIndex(0);
        break;
    case 2:
        ui->comboBox_VK_line_size->setCurrentIndex(1);
        break;
    case 3:
        ui->comboBox_VK_line_size->setCurrentIndex(2);
        break;
    default:
        ui->comboBox_VK_line_size->setCurrentIndex(0);
        break;
    }
}
//-------------------------------------------------------------------
void Dialog_setup::setDepthScale(int scale)
{
    switch(scale)
    {
    case 50:
        ui->comboBox_dept_scale->setCurrentIndex(0);
        break;
    case 100:
        ui->comboBox_dept_scale->setCurrentIndex(1);
        break;
    case 200:
        ui->comboBox_dept_scale->setCurrentIndex(2);
        break;
    default:
        ui->comboBox_dept_scale->setCurrentIndex(1);
        break;
    }
}
//-------------------------------------------------------------------
void Dialog_setup::set_dpsY(int dpsY)
{
    ui->lineEdit_screen_height->setText(QString::fromUtf8("%1").arg((height() * 10) / dpsY));
}
//-------------------------------------------------------------------
void Dialog_setup::set_dpsX(int dpsX)
{
    ui->lineEdit_screen_width->setText(QString::fromUtf8("%1").arg((width() * 10) / dpsX));
}
//-------------------------------------------------------------------
void Dialog_setup::setExtCtl(void)
{
    ui->checkBox_ext_ctl->setChecked(true);
}
//-------------------------------------------------------------------
void Dialog_setup::clrExtCtl(void)
{
    ui->checkBox_ext_ctl->setChecked(false);
}
//-------------------------------------------------------------------
void Dialog_setup::setFolder(QString qsFolderName)
{
    ui->textBrowser->clear();
    ui->textBrowser->setPlainText(qsFolderName);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonFolderChange_clicked(void)
{
    QString folder_name = QFileDialog::getExistingDirectory(this, QString::fromUtf8("Выберите папку для записи..."), "");
    setFolder(folder_name);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonMMColor_clicked(void)
{
    QColor old_color = getMMColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setMMColor(new_color);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonFonColor_clicked(void)
{
    QColor old_color = getFonColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setFonColor(new_color);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonGreedColor_clicked(void)
{
    QColor old_color = getGreedColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setGreedColor(new_color);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonTextColor_clicked(void)
{
    QColor old_color = getTextColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setTextColor(new_color);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonLevelColor_clicked(void)
{
    QColor old_color = getLevelColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setLevelColor(new_color);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonFKDColor_clicked(void)
{
    QColor old_color = getFKDColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setFKDColor(new_color);
}
//-------------------------------------------------------------------
void Dialog_setup::on_pushButtonVKColor_clicked(void)
{
    QColor old_color = getVKColor();
    QColor new_color = QColorDialog::getColor(old_color, this);

    if (new_color.isValid()) setVKColor(new_color);
}
//-------------------------------------------------------------------
QColor Dialog_setup::getMMColor(void)
{
    QPalette palette = ui->pushButtonMMColor->palette();
    QColor   color = palette.color(ui->pushButtonMMColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
QColor Dialog_setup::getFonColor(void)
{
    QPalette palette = ui->pushButtonFonColor->palette();
    QColor   color = palette.color(ui->pushButtonFonColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
QColor Dialog_setup::getGreedColor(void)
{
    QPalette palette = ui->pushButtonGreedColor->palette();
    QColor   color = palette.color(ui->pushButtonGreedColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
QColor Dialog_setup::getTextColor(void)
{
    QPalette palette = ui->pushButtonTextColor->palette();
    QColor   color = palette.color(ui->pushButtonTextColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
QColor Dialog_setup::getLevelColor(void)
{
    QPalette palette = ui->pushButtonLevelColor->palette();
    QColor   color = palette.color(ui->pushButtonLevelColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
QColor Dialog_setup::getFKDColor(void)
{
    QPalette palette = ui->pushButtonFKDColor->palette();
    QColor   color = palette.color(ui->pushButtonFKDColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
QColor Dialog_setup::getVKColor(void)
{
    QPalette palette = ui->pushButtonVKColor->palette();
    QColor   color = palette.color(ui->pushButtonVKColor->backgroundRole());

    return color;
}
//-------------------------------------------------------------------
int Dialog_setup::getFKDstep(void)
{
    int step;
    switch(ui->comboBox_FKD_step->currentIndex())
    {
    case 0:
        step = 1;
        break;
    case 1:
        step = 2;
        break;
    case 2:
        step = 3;
        break;
    default:
        step = 2;
        break;
    }
    return step;
}
//-------------------------------------------------------------------
int Dialog_setup::getVKlineSize(void)
{
    int size;
    switch(ui->comboBox_VK_line_size->currentIndex())
    {
    case 0:
        size = 1;
        break;
    case 1:
        size = 2;
        break;
    case 2:
        size = 3;
        break;
    default:
        size = 1;
        break;
    }
    return size;
}
//-------------------------------------------------------------------
int Dialog_setup::getDepthScale(void)
{
    int scale;
    switch(ui->comboBox_dept_scale->currentIndex())
    {
    case 0:
        scale = 50;
        break;
    case 1:
        scale = 100;
        break;
    case 2:
        scale = 200;
        break;
    default:
        scale = 100;
        break;
    }
    return scale;
}
//-------------------------------------------------------------------
int Dialog_setup::get_dpsY(void)
{
    int height = ui->lineEdit_screen_height->text().toInt();
    int dpsY = (this->height() * 10) / height;

    return dpsY;
}
//-------------------------------------------------------------------
int Dialog_setup::get_dpsX(void)
{
    int width = ui->lineEdit_screen_width->text().toInt();
    int dpsX = (this->width() * 10) / width;

    return dpsX;
}
//-------------------------------------------------------------------
bool Dialog_setup::get_ExtCtl(void)
{
    return ui->checkBox_ext_ctl->isChecked();
}
//-------------------------------------------------------------------
QString Dialog_setup::getFolder(void)
{
    return ui->textBrowser->toPlainText();
}
//-------------------------------------------------------------------
