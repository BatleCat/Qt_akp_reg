//-----------------------------------------------------------------------------
//          Класс записи/чтения файлов с данными приборов типа АКП
//
//                                                              Петрухин А.С.
//                                                                 10/03/2025
//-----------------------------------------------------------------------------
#ifndef QT_AKP_FILE_H
#define QT_AKP_FILE_H
//-----------------------------------------------------------------------------
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QString>
#include <QList>
#include <QDate>
#include <QTime>
//-----------------------------------------------------------------------------
#include "vak_8.h"
#include "vak_8_2pc.h"
#include "tool_type.h"
//-----------------------------------------------------------------------------
enum TAKP_FILE_ERROR
{
    AKP_FILE_success,
    AKP_FILE_error,
    AKP_FILE_unknow_file,
    AKP_FILE_CRC_error,
    AKP_FILE_index_out_of_band,
    AKP_FILE_bad_pointer
};
//-----------------------------------------------------------------------------
//#pragma pack(push, 1)
typedef struct
{
    qint32          dept;
    qint8           ml;
    TVAK_8_DATA     ch1;
    TVAK_8_DATA     ch2;
} __attribute__ ((__packed__, __mode__(__byte__) )) TAKP_FRAME;
//} TAKP_FRAME;
//#pragma pack(pop)
//-----------------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TAKP_FRAME &akp_frame);
QDataStream& operator >>(QDataStream &in,        TAKP_FRAME &akp_frame);
Q_DECLARE_METATYPE(TAKP_FRAME)
//-----------------------------------------------------------------------------
class qt_akp_file : public QObject
{
    Q_OBJECT
public:
    explicit qt_akp_file(QObject *parent = nullptr);
    ~qt_akp_file();

private:
    int          Count;
    QString      File_Type;
    QString      Ver;
    QString      Well_Number;
    QString      Fild_Name;
    QString      Operator_Name;
    QString      Log_Date;
    QString      Start_Dept;
    TTOOL_TYPE   Tool_Type;
    int          Model;
    int          Number_of_Zondes;
    int          Shift_Point_IZL;
    int          Shift_Point_VK1;
    int          Shift_Point_VK2;

    TDataPocket  data_pocket;
    TAKP_FRAME*  akp_curent_frame;
    int          curent_index;

    QList<TAKP_FRAME*>  data_list;

    void   load_head    (const QString      &file_name);
    void   load_well_sec(      QTextStream  &head);
    void   load_tool_sec(      QTextStream  &head);
    void   load_data    (const QString      &file_name);

public:
    //-------------------------------------------------------------------------
    void   load         (const QString &file_name);
    void   clear        (void);
    //-------------------------------------------------------------------------
    void   read_frame   (const int index, TAKP_FRAME &item);
    void   read_ch1     (const int index, TVAK8_WAVE &wave); //TVAK_8_DATA
    void   read_ch2     (const int index, TVAK8_WAVE &wave); //TVAK_8_DATA
    //-------------------------------------------------------------------------
    qint32 read_dept(const int index);
    bool   read_ml      (const int index);
    //-------------------------------------------------------------------------
    quint16 read_frame_label                     (const int index);
    quint16 read_vk_number                       (const int index);

    quint16 read_izl_type                        (const int index);
    quint16 read_izl_freq                        (const int index);
    quint16 read_izl_periods                     (const int index);
    quint16 read_izl_ampl                        (const int index);

    quint16 read_rx_type                         (const int index);
    quint16 read_rx_Td                           (const int index);
    quint16 read_rx_Ku                           (const int index);
    quint16 read_rx_delay                        (const int index);

    quint16 read_tool_type                       (const int index);
    quint16 read_tool_no                         (const int index);
    quint16 read_soft_version_major              (const int index);
    quint16 read_soft_version_minor              (const int index);

    quint16 read_mode_number                     (const int index);
    quint16	read_mode_count                      (const int index);

    quint16 read_vk_calibration_amplitude        (const int index);
    quint16 read_vk_calibration_offset           (const int index);

    quint32 read_timer_clk                       (const int index);
    quint32 read_time_start_meserment            (const int index);
    quint32 read_time_stop_meserment             (const int index);
    quint32 read_time_meserment                  (const int index);

    bool is_frame_CRC_OK                         (const int index);
    bool is_frame_CRC_OK_for_ch1                 (const int index);
    bool is_frame_CRC_OK_for_ch2                 (const int index);

    bool is_frame_CRC_OK_for_frame_label         (const int index);
    bool is_frame_CRC_OK_for_vk_number           (const int index);
    bool is_frame_CRC_OK_for_rx_type             (const int index);
    bool is_frame_CRC_OK_for_Ku                  (const int index);

    bool is_frame_CRC_OK_for_rx_delay            (const int index);
    bool is_frame_CRC_OK_for_Fsig                (const int index);
    bool is_frame_CRC_OK_for_tool_type           (const int index);
    bool is_frame_CRC_OK_for_mode_number         (const int index);

    bool is_frame_CRC_OK_for_mode_count          (const int index);

    bool is_frame_CRC_OK_for_vk_calibration_amp  (const int index);

    bool is_frame_CRC_OK_for_vk_calibration_ofs  (const int index);
    bool is_frame_CRC_OK_for_tool_no             (const int index);
    bool is_frame_CRC_OK_for_soft_version        (const int index);

    bool is_frame_CRC_OK_for_timer_clk           (const int index);
    bool is_frame_CRC_OK_for_time_start_meserment(const int index);

    bool is_frame_CRC_OK_for_time_stop_meserment (const int index);

    bool is_frame_CRC_OK_for_time_meserment      (const int index);
    //-------------------------------------------------------------------------
    int          count()            const {return Count;}
    QString      file_type()        const {return File_Type;}
    QString      ver()              const {return Ver;}
    QString      well_number()      const {return Well_Number;}
    QString      fild_name()        const {return Fild_Name;}
    QString      operator_name()    const {return Operator_Name;}
    QString      log_date()         const {return Log_Date;}
    QString      start_dept()       const {return Start_Dept;}
    TTOOL_TYPE   tool_type()        const {return Tool_Type;}
    int          model()            const {return Model;}
    int          number_of_zondes() const {return Number_of_Zondes;}
    int          shift_point_izl()  const {return Shift_Point_IZL;}
    int          shift_point_vk1()  const {return Shift_Point_VK1;}
    int          shift_point_vk2()  const {return Shift_Point_VK2;}
    //-------------------------------------------------------------------------

signals:
    //-------------------------------------------------------------------------
    void finished(void);
    void logMessage(const QString message);

public slots:
    //-------------------------------------------------------------------------
    void start(void);
};
//-----------------------------------------------------------------------------
class qt_akp_file_save : public QObject
{
    Q_OBJECT
public:
    explicit qt_akp_file_save(QObject *parent = 0);
    ~qt_akp_file_save();

//    void create_file(QString fName);
    void write_head     (void);
    void write_data     (const TAKP_FRAME &data);    //(const TDataPocket &data);
    void close_file     (void);

    void setBufLen      (int len)           {if (len > 1) buf_len = len;}
    void setFileName    (QString Name)      {fileName = Name;}
    void setFildName    (QString Name)      {fild     = Name;}
    void setWellNo      (QString WellNo)    {well     = WellNo;}
    void setOperatorName(QString Name)      {name = Name;}
    void setDate        (QDate newDate)     {date = newDate;}
    void setCurrentDate (void)              {date = QDate::currentDate();}
    void setTime        (QTime newTime)     {time = newTime;}
    void setCurrentTime (void)              {time.currentTime();}
    void setDept        (int newDept)       {dept = newDept;}

    void setShiftPointIZL(int newValue)     {Shift_Point_IZL = newValue;}
    void setShiftPointVK1(int newValue)     {Shift_Point_VK1 = newValue;}
    void setShiftPointVK2(int newValue)     {Shift_Point_VK2 = newValue;}

private:
    int         buf_len;
    QFile       file;
    QDataStream stream;

    QString     fileName;
    QString     fild;
    QString     well;
    QString     name;
    QString     tool_type;

    QDate       date;
    QTime       time;

    int         dept;

    int         Shift_Point_IZL;
    int         Shift_Point_VK1;
    int         Shift_Point_VK2;

    TAKP_FRAME* akp_curent_frame;
    int         curent_index;

    QList<TAKP_FRAME*>  data_list;

signals:
    void closed(void);

public slots:
    void start(void);
    void on_data_update (const int blk_cnt, const TDataPocket &data);
};
//-----------------------------------------------------------------------------
#endif // QT_AKP_FILE_H
//-----------------------------------------------------------------------------
