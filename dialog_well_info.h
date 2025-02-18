//-------------------------------------------------------------------
#ifndef DIALOG_WELL_INFO_H
#define DIALOG_WELL_INFO_H
//-------------------------------------------------------------------
#include <QDialog>
#include <QString>
//-------------------------------------------------------------------
namespace Ui {
class Dialog_well_info;
}
//-------------------------------------------------------------------
class Dialog_well_info : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_well_info(QWidget *parent = 0);
    ~Dialog_well_info();

    void setWellNo(QString qs_WellNo);
    void setFildName(QString qs_FildName);
    void setOperatorName(QString qs_OperatorName);
    void setDept(int dept);
    void setDeptStep(int dept_step);
    void setFolderName(QString qs_FolderName);

    QString getWellNo(void);
    QString getFildName(void);
    QString getOperatorName(void);
    int getDept(void);
    int getDeptStep(void);
    QString getFolderName(void);

signals:
    void cmd_Setup(void);

private:
    Ui::Dialog_well_info *ui;
};
//-------------------------------------------------------------------
#endif // DIALOG_WELL_INFO_H
//-------------------------------------------------------------------
