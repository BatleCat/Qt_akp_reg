//-------------------------------------------------------------------
#include "dialog_well_info.h"
#include "ui_dialog_well_info.h"
//-------------------------------------------------------------------
Dialog_well_info::Dialog_well_info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_well_info)
{
    ui->setupUi(this);

    connect(ui->pushButtonOK,    SIGNAL(pressed()), this, SLOT(accept()));
    connect(ui->pushButtonSetup, SIGNAL(pressed()), this, SIGNAL(cmd_Setup()));
}
//-------------------------------------------------------------------
Dialog_well_info::~Dialog_well_info()
{
    delete ui;
}
//-------------------------------------------------------------------
void Dialog_well_info::setWellNo(QString qs_WellNo)
{
    ui->lineEdit_well_No->setText(qs_WellNo);
}
//-------------------------------------------------------------------
void Dialog_well_info::setFildName(QString qs_FildName)
{
    ui->lineEdit_fild_name->setText(qs_FildName);
}
//-------------------------------------------------------------------
void Dialog_well_info::setOperatorName(QString qs_OperatorName)
{
    ui->lineEdit_operator_name->setText(qs_OperatorName);
}
//-------------------------------------------------------------------
void Dialog_well_info::setDept(int dept)
{
    ui->lineEdit_depth->setText(QString::fromUtf8("%1.%2%3 м").arg(dept / 100, 5).arg((dept / 10) % 10).arg(dept % 10));
//    ui->lineEdit_depth->setText(QString::fromUtf8("%1.%2%3 м").arg(dept / 100, 4).arg((dept / 10) % 10).arg(dept % 10));
}
//-------------------------------------------------------------------
void Dialog_well_info::setDeptStep(int dept_step)
{
    switch (dept_step)
    {
    case 2:
        ui->comboBox->setCurrentIndex(0);
        break;
    case 5:
        ui->comboBox->setCurrentIndex(1);
        break;
    case 10:
        ui->comboBox->setCurrentIndex(2);
        break;
    default:
        ui->comboBox->setCurrentIndex(0);
        break;
    }
}
//-------------------------------------------------------------------
void Dialog_well_info::setFolderName(QString qs_FolderName)
{
    ui->textBrowser->clear();
    ui->textBrowser->setPlainText(qs_FolderName);
}
//-------------------------------------------------------------------
QString Dialog_well_info::getWellNo(void)
{
    return ui->lineEdit_well_No->text();
}
//-------------------------------------------------------------------
QString Dialog_well_info::getFildName(void)
{
    return ui->lineEdit_fild_name->text();
}
//-------------------------------------------------------------------
QString Dialog_well_info::getOperatorName(void)
{
    return ui->lineEdit_operator_name->text();
}
//-------------------------------------------------------------------
int Dialog_well_info::getDept(void)
{
    QString qsDept = ui->lineEdit_depth->text();

    // убираю пробелы из строки
    int i = qsDept.indexOf(" ");
    while (!(i < 0))
    {
        qsDept.remove(i, 1);
        i = qsDept.indexOf(" ");
    }
    i = qsDept.indexOf("м");
    qsDept.remove(i, 1);

    int dept = (int)(100 * qsDept.toDouble());

    return dept;
}
//-------------------------------------------------------------------
int Dialog_well_info::getDeptStep(void)
{
    if ( ui->comboBox->currentIndex() == 0 ) return 2;
    if ( ui->comboBox->currentIndex() == 1 ) return 5;
    if ( ui->comboBox->currentIndex() == 2 ) return 10;

    return 2;
}
//-------------------------------------------------------------------
QString Dialog_well_info::getFolderName(void)
{
    return ui->textBrowser->toPlainText();
}
//-------------------------------------------------------------------
