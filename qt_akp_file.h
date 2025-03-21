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
#include "akp_check_state.h"
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
//typedef struct
//{
//    qint32          dept;
//    qint8           ml;
//    TVAK_8_DATA     ch1;
//    TVAK_8_DATA     ch2;
//} TAKP_FRAME;
//#pragma pack(pop)
//-----------------------------------------------------------------------------
typedef struct
{
    qint32          dept;
    qint8           ml;
    TVAK_8_DATA     ch1;
    TVAK_8_DATA     ch2;
} __attribute__ ((__packed__ )) TAKP_FRAME;
//-----------------------------------------------------------------------------
//typedef struct
//{
//    qint32          dept;
//    qint8           ml;

//    quint16 frame_label;
//    quint16 vk_number;
//    quint16 rx_type;
//    quint16 Td;
//    quint16 Ku;
//    bool    CRC1_OK;
//    quint16 rx_delay;
//    quint16 Fsig;
//    quint16 izl_type;
//    quint16 izl_ampl;
//    quint16 izl_periods;
//    quint16 tool_type;
//    quint16 mode_number;
//    bool    CRC2_OK;
//    quint16	mode_count;
//    bool    CRC3_OK;
//    bool    CRC4_OK;
//    quint16 vk_calibration_amplitude;
//    bool    CRC5_OK;
//    quint16 vk_calibration_offset;
//    quint16 tool_no;
//    quint16 soft_version_major;
//    quint16 soft_version_minor;
//    bool    CRC6_OK;
//    quint32 time_meserment;
//    bool    CRC7_OK;
//    bool    CRC8_OK;

//    TVAK8_WAVE vk;
//} __attribute__ ((__packed__)) TAKP_FRAME;
//-----------------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TAKP_FRAME &akp_frame);
QDataStream& operator >>(QDataStream &in,        TAKP_FRAME &akp_frame);
Q_DECLARE_METATYPE(TAKP_FRAME)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class qt_akp_file_read : public QObject
{
    Q_OBJECT
public:
    explicit qt_akp_file_read(QObject *parent = nullptr);
    ~qt_akp_file_read();

private:
//    int          Count;
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

//    TDataPocket  data_pocket;
    akp_check_state ch1;
    akp_check_state ch2;
    TAKP_FRAME*     akp_curent_frame;
    int             curent_index;
    bool            bItemLoaded;

    QList<TAKP_FRAME*>  data_list;

    void   load_head    (const QString      &file_name);
    void   load_well_sec(      QTextStream  &head);
    void   load_tool_sec(      QTextStream  &head);
    void   load_data    (const QString      &file_name);

protected:
    bool is_index_valid (const int index);
    void select_item    (const int index);
    void load_item      (const int index);

public:
    //-------------------------------------------------------------------------
    void   load         (const QString &file_name);
    void   clear        (void);
    //-------------------------------------------------------------------------
    void   read_frame   (const int index, TAKP_FRAME &item);
    void   read_ch1     (const int index, TVAK8_WAVE &wave); //TVAK_8_DATA
    void   read_ch2     (const int index, TVAK8_WAVE &wave); //TVAK_8_DATA
    //-------------------------------------------------------------------------
    qint32 read_dept    (const int index);
    bool   read_ml      (const int index);
    //-------------------------------------------------------------------------
    quint16 read_ch1_frame_label                        (const int index);
    quint16 read_ch2_frame_label                        (const int index);
    quint16 read_ch1_vk_number                          (const int index);
    quint16 read_ch2_vk_number                          (const int index);

    quint16 read_izl_type                               (const int index);
    quint16 read_izl_freq                               (const int index);
    quint16 read_izl_periods                            (const int index);
    quint16 read_izl_ampl                               (const int index);

    quint16 read_rx_type                                (const int index);
    quint16 read_rx_Td                                  (const int index);
    quint16 read_rx_Ku                                  (const int index);
    quint16 read_rx_delay                               (const int index);

    quint16 read_tool_type                              (const int index);
    quint16 read_tool_no                                (const int index);
    quint16 read_soft_version_major                     (const int index);
    quint16 read_soft_version_minor                     (const int index);

    quint16 read_mode_number                            (const int index);
    quint16	read_mode_count                             (const int index);

    quint16 read_vk_calibration_amplitude               (const int index);
    quint16 read_vk_calibration_offset                  (const int index);

    quint32 read_timer_clk                              (const int index);
    quint32 read_time_start_meserment                   (const int index);
    quint32 read_time_stop_meserment                    (const int index);
    quint32 read_time_meserment                         (const int index);

    bool is_frame_CRC_OK                                (const int index);
    bool is_frame_CRC_OK_for_ch1                        (const int index);
    bool is_frame_CRC_OK_for_ch2                        (const int index);

    bool is_frame_CRC_OK_for_ch1_frame_label            (const int index);
    bool is_frame_CRC_OK_for_ch2_frame_label            (const int index);
    bool is_frame_CRC_OK_for_ch1_vk_number              (const int index);
    bool is_frame_CRC_OK_for_ch2_vk_number              (const int index);

    bool is_frame_CRC_OK_for_izl_type                   (const int index);
    bool is_frame_CRC_OK_for_izl_freq                   (const int index);
    bool is_frame_CRC_OK_for_izl_periods                (const int index);
    bool is_frame_CRC_OK_for_izl_ampl                   (const int index);

    bool is_frame_CRC_OK_for_rx_type                    (const int index);
    bool is_frame_CRC_OK_for_rx_Td                      (const int index);
    bool is_frame_CRC_OK_for_rx_Ku                      (const int index);
    bool is_frame_CRC_OK_for_rx_delay                   (const int index);

    bool is_frame_CRC_OK_for_tool_type                  (const int index);
    bool is_frame_CRC_OK_for_tool_no                    (const int index);
    bool is_frame_CRC_OK_for_soft_version               (const int index);

    bool is_frame_CRC_OK_for_mode_number                (const int index);
    bool is_frame_CRC_OK_for_mode_count                 (const int index);

    bool is_frame_CRC_OK_for_vk_calibration_amplitude   (const int index);
    bool is_frame_CRC_OK_for_vk_calibration_offset      (const int index);

    bool is_frame_CRC_OK_for_timer_clk                  (const int index);
    bool is_frame_CRC_OK_for_time_start_meserment       (const int index);

    bool is_frame_CRC_OK_for_time_stop_meserment        (const int index);

    bool is_frame_CRC_OK_for_time_meserment             (const int index);

    //-------------------------------------------------------------------------
    int          count()            const {return data_list.count();}
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
//-----------------------------------------------------------------------------
class qt_akp_file_save : public QObject //akp_check_state
{
    Q_OBJECT
public:
    explicit qt_akp_file_save(QObject *parent = 0);
    ~qt_akp_file_save();

    void find_validFileName (void);
    void write_head         (void);
    void write_data         (void);
    void close_file         (void);

    bool isFileNameValid    (void)              {return bFileNameValid;}

    void setBufLen          (int len)           {if (len > 1) buf_len = len;}
    void setFolderName      (QString Name)      {folderName = Name;}
//    void setFileName        (QString Name)      {fileName = Name;}
    void setFildName        (QString Name)      {fildName     = Name;}
    void setWellNo          (QString WellNo)    {wellNo     = WellNo;}
    void setOperatorName    (QString Name)      {name = Name;}
    void setDate            (QDate newDate)     {date = newDate;}
    void setCurrentDate     (void)              {date = QDate::currentDate();}
    void setTime            (QTime newTime)     {time = newTime;}
    void setCurrentTime     (void)              {time.currentTime();}
    void setStartDepth      (int newDepth)      {startDepth = newDepth;}

    void setShiftPointIZL   (int newValue)      {Shift_Point_IZL = newValue;}
    void setShiftPointVK1   (int newValue)      {Shift_Point_VK1 = newValue;}
    void setShiftPointVK2   (int newValue)      {Shift_Point_VK2 = newValue;}

    void setExtendedFolderCtl   (void)          {bExtFolderCtl = true;}
    void clearExtendedFolderCtl (void)          {bExtFolderCtl = false;}

private:
    int         buf_len;
    QFile       file;
    QDataStream stream;

    QString     fileName;
    QString     folderName;

    QString     fildName;
    QString     wellNo;
    QString     name;
    QString     tool_type;

    QDate       date;
    QTime       time;

    int         startDepth;

    int         Shift_Point_IZL;
    int         Shift_Point_VK1;
    int         Shift_Point_VK2;

    bool        bWriteEnable;
    bool        bExtFolderCtl;
    bool        bFileNameValid;

    TAKP_FRAME* akp_curent_frame;

    QList<TAKP_FRAME*>  data_list;

signals:
    void closed(void);

public slots:
    void start(void);
    void onDataUpdate (const int blk_cnt, const TDataPocket &data);
};
//-----------------------------------------------------------------------------
#endif // QT_AKP_FILE_H
//-----------------------------------------------------------------------------
