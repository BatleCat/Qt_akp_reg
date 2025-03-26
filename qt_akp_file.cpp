//-----------------------------------------------------------------------------
#include <QString>
#include <QStringList>
#include <QDate>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QtDebug>
//-----------------------------------------------------------------------------
#include "qt_akp_file.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TAKP_FRAME &akp_frame)
{
    out.writeRawData((char*)(&akp_frame), sizeof(TAKP_FRAME));
    return out;
}
//-----------------------------------------------------------------------------
QDataStream& operator >>(QDataStream &in,        TAKP_FRAME &akp_frame)
{
    in.readRawData((char*)(&akp_frame), sizeof(TAKP_FRAME));
    return in;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
qt_akp_file_read::qt_akp_file_read(QObject *parent) : QObject(parent)
{
    akp_curent_frame = NULL;
    curent_index     = -1;
    bItemLoaded      = false;
}
//-----------------------------------------------------------------------------
qt_akp_file_read::~qt_akp_file_read(void)
{
    clear();
    emit finished();
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::load_head(const QString &file_name)
{
    QFile       f_data(file_name);
    QString     str;
    QTextStream head;

    File_Type = "";
    Ver = "";

    if (!f_data.open(QIODevice::ReadOnly))
    {
        qDebug() << f_data.errorString();
        throw AKP_FILE_error;
    }

    head.setDevice(&f_data);
    head.setCodec("windows-1251");

    str = head.readLine();
    if (str.isEmpty()) throw AKP_FILE_error;

    if (str != QString::fromUtf8("~head")) throw AKP_FILE_unknow_file;

    str = head.readLine();
    QStringList list = str.split(" ", QString::SkipEmptyParts);
    if (list.at(0) != QString::fromUtf8("Формат")) throw AKP_FILE_unknow_file;
    if (list.at(1) != QString::fromUtf8("GIS")   ) throw AKP_FILE_unknow_file;
    File_Type = list.at(1);
    list.clear();

    str = head.readLine();
    list = str.split(" ", QString::SkipEmptyParts);
    if (list.at(0) != QString::fromUtf8("Версия")) throw AKP_FILE_unknow_file;
    if (list.at(1) != QString::fromUtf8("1.0")   ) throw AKP_FILE_unknow_file;
    else Ver = "1.0";
    list.clear();

    do
    {
        str = head.readLine();
        if (str == QString::fromUtf8("~well"))
        {
            qDebug() << f_data.pos();
            load_well_sec(head);
            continue;
        }
        if (str == QString::fromUtf8("~tool"))
        {
            load_tool_sec(head);
            continue;
        }
    } while (str != QString::fromUtf8("~data"));

    f_data.close();
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::load_well_sec(QTextStream &head)
{
    qint64      pos;
    QString     str = "";

    Fild_Name       = "";
    Well_Number     = "";
    Log_Date        = "";
    Operator_Name   = "";
    Start_Dept      = "";


    do
    {
        pos = head.pos();

        str = head.readLine();
        if (str.isEmpty()) throw AKP_FILE_error;      // Error

        QStringList list = str.split(" ", QString::SkipEmptyParts);

        if (list.count() > 1)
        {
            if (list.at(0) == QString::fromUtf8("Площадь"))
            {
                list.removeFirst();
                Fild_Name = list.takeFirst();
                while (!list.isEmpty())
                {
                    Fild_Name += QString::fromUtf8(" ");
                    Fild_Name += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Скважина"))
            {
                list.removeFirst();
                Well_Number = list.takeFirst();
                while (!list.isEmpty())
                {
                    Well_Number += QString::fromUtf8(" ");
                    Well_Number += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Дата"))
            {
                list.removeFirst();
                Log_Date = list.takeFirst();
                while (!list.isEmpty())
                {
                    Log_Date += QString::fromUtf8(" ");
                    Log_Date += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Оператор"))
            {
                list.removeFirst();
                Operator_Name = list.takeFirst();
                while (!list.isEmpty())
                {
                    Operator_Name += QString::fromUtf8(" ");
                    Operator_Name += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Глубина"))
            {
                list.removeFirst();
                Start_Dept = list.takeFirst();
                while (!list.isEmpty())
                {
                    Start_Dept += QString::fromUtf8(" ");
                    Start_Dept += list.takeFirst();
                }
                continue;
            }
        }
    } while (str.at(0) != '~');

    head.seek(pos);

    return;
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::load_tool_sec(QTextStream &head)
{
    qint64      pos;
    QString     str;

    Tool_Type = TTOOL_TYPE::UNKNOWN;
    Model = 0;
    Number_of_Zondes = 0;
    Shift_Point_VK1 = 0;
    Shift_Point_VK2 = 0;

    do
    {
        pos = head.pos();

        str = head.readLine();
        if (str.isEmpty()) throw AKP_FILE_error;

//        QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QStringList list = str.split(" ", QString::SkipEmptyParts);

        if (list.count() > 1)
        {
            if (list.at(0) == QString::fromUtf8("Прибор"))
            {
                if (list.at(1) == QString::fromUtf8("АКП-52"))
                {
                    Tool_Type = TTOOL_TYPE::AKP_52;
                    continue;
                }
                if (list.at(1) == QString::fromUtf8("АКП-76"))
                    Tool_Type = TTOOL_TYPE::AKP_76;
                continue;
            }

            if (list.at(0) == QString::fromUtf8("Модель"))
            {
                Model = list.at(1).toInt();
                continue;
            }

            if (list.at(0) == QString::fromUtf8("Зонды"))
            {
                Number_of_Zondes = list.at(1).toInt();
                continue;
            }

            if ((list.at(0) == QString::fromUtf8("Точки")) && (list.at(0) == QString::fromUtf8("записи")))
            {
                int i;
                for (i = 0; i < 3; i++)
                {
                    pos = head.pos();
                    str = head.readLine();
                    if (str.at(0) == '~') break;

                    QStringList list1 = str.split(" ", QString::SkipEmptyParts);

                    if (list1.at(0) == QString::fromUtf8("И1")) Shift_Point_IZL = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("П1")) Shift_Point_VK1 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("П2")) Shift_Point_VK2 = list1.at(1).toInt();
                }
            }
        }
    } while (str.at(0) != '~');

    head.seek(pos);

    return;
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::load_data(const QString &file_name)
{
    QFile           f_data(file_name);
    qint64          pos;
    QTextStream     head;
    QDataStream     data;
    QString         s;
    TAKP_FRAME*     pData;

    if (!f_data.open(QIODevice::ReadOnly))
    {
        qDebug() << f_data.errorString();
        return;
    }

    head.setDevice(&f_data);
    head.setCodec("windows-1251");
    do
    {
        s = head.readLine();
    } while (s != QString::fromUtf8("~data"));

//    qDebug() << f_data.pos() << " " << head.pos();
    pos = head.pos();
    head.flush();

    f_data.seek(pos);
    data.setDevice(&f_data);
//    Count = 0;
    while (!f_data.atEnd())
    {
        pData = new TAKP_FRAME;
        data >> (*pData);
        data_list.append(pData);
//        Count++;
    }
    f_data.close();

    curent_index    = -1;
    bItemLoaded     = false;
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::load(const QString &file_name)
{
    load_head(file_name);
    load_data(file_name);
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::clear(void)
{
    curent_index    = -1;
    bItemLoaded     = false;

    while (!data_list.isEmpty())
    {
        akp_curent_frame = data_list.takeFirst();
        delete akp_curent_frame;
//        data_list.removeFirst();
    }
    akp_curent_frame = NULL;
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::start(void)
{
//    Count           = 0;

    File_Type       = QString::fromUtf8("N/A");
    Ver             = QString::fromUtf8("N/A");
    Well_Number     = QString::fromUtf8("N/A");
    Fild_Name       = QString::fromUtf8("N/A");
    Operator_Name   = QString::fromUtf8("N/A");
    Log_Date        = QString::fromUtf8("N/A");
    Start_Dept      = QString::fromUtf8("N/A");

    Tool_Type       = TTOOL_TYPE::UNKNOWN;
    Model           = 0;
    Number_of_Zondes= 0;
    Shift_Point_IZL = 0;
    Shift_Point_VK1 = 0;
    Shift_Point_VK2 = 0;

    akp_curent_frame = NULL;

    curent_index    = -1;

    bItemLoaded     = false;
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_index_valid (const int index)
{
    if ( (index >= 0) && (index < count()) )
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::select_item(const int index)
{
    if ( is_index_valid(index) )
    {
        if (index != curent_index)
        {
            akp_curent_frame = (TAKP_FRAME*) data_list[index];
            curent_index = index;
            bItemLoaded = false;
        }
    }
    else
         throw AKP_FILE_index_out_of_band;
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::load_item(const int index)
{
    select_item(index);
    if (!bItemLoaded)
    {
        TDataPocket data;
        data.dept = akp_curent_frame->dept;
        data.ml   = akp_curent_frame->ml;
        memcpy(data.data, akp_curent_frame->ch1, sizeof(TVAK_8_DATA));
        ch1.set_state(data);
        memcpy(data.data, akp_curent_frame->ch2, sizeof(TVAK_8_DATA));
        ch2.set_state(data);
    }
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::read_frame(const int index, TAKP_FRAME &item)
{
    select_item(index);
    memcpy( &item, akp_curent_frame, sizeof(TAKP_FRAME) );
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::read_ch1(const int index, TVAK8_WAVE &wave)
{
    load_item(index);
    ch1.get_wave(wave);
}
//-----------------------------------------------------------------------------
void qt_akp_file_read::read_ch2(const int index, TVAK8_WAVE &wave)
{
    load_item(index);
    ch2.get_wave(wave);
}
//-------------------------------------------------------------------------
qint32 qt_akp_file_read::read_dept(const int index)
{
    select_item(index);
    return akp_curent_frame->dept;
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::read_ml(const int index)
{
    select_item(index);
    return (bool)akp_curent_frame->ml;
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_ch1_frame_label(const int index)
{
    load_item(index);
    return ch1.get_frame_label();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_ch2_frame_label(const int index)
{
    load_item(index);
    return ch2.get_frame_label();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_ch1_vk_number(const int index)
{
    load_item(index);
    return ch1.get_vk_number();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_ch2_vk_number(const int index)
{
    load_item(index);
    return ch2.get_vk_number();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_izl_type(const int index)
{
    load_item(index);
//    if ( ( ch1.get_izl_type() != ch2.get_izl_type() ) && ch2.is_CRC_OK_for_izl_type() )
    if ( ch2.is_CRC_OK_for_izl_type() )
    {
        return ch2.get_izl_type();
    }
    return ch1.get_izl_type();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_izl_freq(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_Fsig() )
    {
        return ch2.get_Fsig();
    }
    return ch1.get_Fsig();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_izl_periods(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_izl_periods() )
    {
        return ch2.get_izl_periods();
    }
    return ch1.get_izl_periods();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_izl_ampl(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_izl_ampl() )
    {
        return ch2.get_izl_ampl();
    }
    return ch1.get_izl_ampl();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_rx_type(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_rx_type() )
    {
        return ch2.get_rx_type();
    }
    return ch1.get_rx_type();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_rx_Td(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_Td() )
    {
        return ch2.get_Td();
    }
    return ch1.get_Td();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_rx_Ku(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_Ku() )
    {
        return ch2.get_Ku();
    }
    return ch1.get_Ku();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_rx_delay(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_rx_delay() )
    {
        return ch2.get_rx_delay();
    }
    return ch1.get_rx_delay();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_tool_type(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_tool_type() )
    {
        return ch2.get_tool_type();
    }
    return ch1.get_tool_type();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_tool_no(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_tool_no() )
    {
        return ch2.get_tool_no();
    }
    return ch1.get_tool_no();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_soft_version_major(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_soft_version() )
    {
        return ch2.get_soft_version_major();
    }
    return ch1.get_soft_version_major();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_soft_version_minor(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_soft_version() )
    {
        return ch2.get_soft_version_minor();
    }
    return ch1.get_soft_version_minor();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_mode_number(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_mode_number() )
    {
        return ch2.get_mode_number();
    }
    return ch1.get_mode_number();
}
//-----------------------------------------------------------------------------
quint16	qt_akp_file_read::read_mode_count(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_mode_count() )
    {
        return ch2.get_mode_count();
    }
    return ch1.get_mode_count();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_vk_calibration_amplitude(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_vk_calibration_amp() )
    {
        return ch2.get_vk_calibration_amplitude();
    }
    return ch1.get_vk_calibration_amplitude();
}
//-----------------------------------------------------------------------------
quint16 qt_akp_file_read::read_vk_calibration_offset(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_vk_calibration_ofs() )
    {
        return ch2.get_vk_calibration_offset();
    }
    return ch1.get_vk_calibration_offset();
}
//-----------------------------------------------------------------------------
quint32 qt_akp_file_read::read_timer_clk(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_timer_clk() )
    {
        return ch2.get_timer_clk();
    }
    return ch1.get_timer_clk();
}
//-----------------------------------------------------------------------------
quint32 qt_akp_file_read::read_time_start_meserment(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_time_start_meserment() )
    {
        return ch2.get_time_start_meserment();
    }
    return ch1.get_time_start_meserment();
}
//-----------------------------------------------------------------------------
quint32 qt_akp_file_read::read_time_stop_meserment(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_time_stop_meserment() )
    {
        return ch2.get_time_stop_meserment();
    }
    return ch1.get_time_stop_meserment();
}
//-----------------------------------------------------------------------------
quint32 qt_akp_file_read::read_time_meserment(const int index)
{
    load_item(index);
    if ( ch2.is_CRC_OK_for_time_meserment() )
    {
        return ch2.get_time_meserment();
    }
    return ch1.get_time_meserment();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK(const int index)
{
    return ( is_frame_CRC_OK_for_ch1(index) && is_frame_CRC_OK_for_ch2(index) );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_ch1(const int index)
{
    load_item(index);
    return ch1.is_CRC_OK();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_ch2(const int index)
{
    load_item(index);
    return ch2.is_CRC_OK();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_ch1_frame_label(const int index)
{
    load_item(index);
    return ch1.is_CRC_OK_for_frame_label();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_ch2_frame_label(const int index)
{
    load_item(index);
    return ch2.is_CRC_OK_for_frame_label();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_ch1_vk_number(const int index)
{
    load_item(index);
    return ch1.is_CRC_OK_for_vk_number();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_ch2_vk_number(const int index)
{
    load_item(index);
    return ch2.is_CRC_OK_for_vk_number();
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_izl_type(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_izl_type() || ch2.is_CRC_OK_for_izl_type() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_izl_freq(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_Fsig() || ch2.is_CRC_OK_for_Fsig() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_izl_periods(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_izl_periods() || ch2.is_CRC_OK_for_izl_periods() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_izl_ampl(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_izl_ampl() || ch2.is_CRC_OK_for_izl_ampl() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_rx_type(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_rx_type() || ch2.is_CRC_OK_for_rx_type() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_rx_Td(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_Td() || ch2.is_CRC_OK_for_Td() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_rx_Ku(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_Ku() || ch2.is_CRC_OK_for_Ku() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_rx_delay(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_rx_delay() || ch2.is_CRC_OK_for_rx_delay() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_tool_type(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_tool_type() || ch2.is_CRC_OK_for_tool_type() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_tool_no(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_tool_no() || ch2.is_CRC_OK_for_tool_no() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_soft_version(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_soft_version() || ch2.is_CRC_OK_for_soft_version() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_mode_number(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_mode_number() || ch2.is_CRC_OK_for_mode_number() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_mode_count(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_mode_count() || ch2.is_CRC_OK_for_mode_count() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_vk_calibration_amplitude(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_vk_calibration_amp() || ch2.is_CRC_OK_for_vk_calibration_amp() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_vk_calibration_offset(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_vk_calibration_ofs() || ch2.is_CRC_OK_for_vk_calibration_ofs() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_timer_clk(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_timer_clk() || ch2.is_CRC_OK_for_timer_clk() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_time_start_meserment(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_time_start_meserment() || ch2.is_CRC_OK_for_time_start_meserment() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_time_stop_meserment(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_time_stop_meserment() || ch2.is_CRC_OK_for_time_stop_meserment() );
}
//-----------------------------------------------------------------------------
bool qt_akp_file_read::is_frame_CRC_OK_for_time_meserment(const int index)
{
    load_item(index);
    return ( ch1.is_CRC_OK_for_time_meserment() || ch2.is_CRC_OK_for_time_meserment() );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
qt_akp_file_save::qt_akp_file_save(QObject *parent) :
//    akp_check_state (parent),
    QObject(parent),
    buf_len         (1),
    fileName        (QString::fromUtf8("")),
    folderName      ( QString::fromUtf8("''") ),
    fildName        (QString::fromUtf8("Проверочная")),
    wellNo          (QString::fromUtf8("1")),
    name            (QString::fromUtf8("Я")),
    tool_type       (QString::fromUtf8("АКП-76")),
    date            (QDate::currentDate()),
    time            (QTime::currentTime()),
    startDepth      (0),
    bWriteEnable    (false),
    bExtFolderCtl   (true),
    bFileNameValid  (false)
{
}
//-----------------------------------------------------------------------------
qt_akp_file_save::~qt_akp_file_save()
{
    close_file();
    emit closed();
}
//-----------------------------------------------------------------------------
void qt_akp_file_save::find_validFileName(void)
{
    QString FileExt = QString::fromUtf8(".gis");

    fileName = folderName;
    if (bExtFolderCtl)
    {
        fileName += QString::fromUtf8("/");

        fileName += fildName;
        fileName += QString::fromUtf8("/");

        fileName += wellNo;
        fileName += QString::fromUtf8("/");

        if (date.day() < 10) fileName += QString::fromUtf8("0");
        fileName += QString::fromUtf8("%1_").arg(date.day());

        if (date.month() < 10) fileName += QString::fromUtf8("0");
        fileName += QString::fromUtf8("%1_").arg(date.month());

        fileName += QString::fromUtf8("%1").arg(date.year());
    }

    QDir dir;
    if (!dir.mkpath(fileName))
    {
        //throw
        qDebug() << QString::fromUtf8("Ошибка создания директории: %1").arg(fileName);

        return;
    }

    fileName += QString::fromUtf8("/");
    fileName += wellNo;

    //    qDebug() << QString::fromUtf8("%1").arg(FileName);

    char ch;
    for (ch = 'a'; ch <= 'z'; ch++)
    {
        if (!QFile::exists(QString::fromUtf8("%1_%2%3").arg(fileName).arg(ch).arg(FileExt)))
        {
            fileName += QString::fromUtf8("_%1").arg(ch);
            fileName += FileExt;

            bFileNameValid = true;

            return;
        }
    }

    fileName = QString::fromUtf8("");
    return;
}
//-----------------------------------------------------------------------------
void qt_akp_file_save::write_head(void)
{
    if ( isFileNameValid() )
    {
        //---Запись шапки файла -----------------------
        file.setFileName(fileName);

        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << file.errorString();
            return;
        }
        QTextStream head(&file);
        head.setCodec("windows-1251");
        //    head.setCodec("CP866");

        head << QString::fromUtf8("~head\r\n");
        head << QString::fromUtf8("  Формат GIS\r\n");
        head << QString::fromUtf8("  Версия 1.0\r\n");

        head << QString::fromUtf8("~well\r\n");
        head << QString::fromUtf8("  Площадь  %1\r\n").arg(fildName);
        head << QString::fromUtf8("  Скважина №%1\r\n").arg(wellNo);
        head << QString::fromUtf8("  Дата     %1.%2.%3\r\n").arg(date.day(), 2, 10, QChar('0')).arg(date.month(), 2, 10, QChar('0')).arg(date.year(), 4, 10, QChar('0'));
        head << QString::fromUtf8("  Время    %1:%2:%3\r\n").arg(time.hour()).arg(time.minute(), 2, 10, QChar('0')).arg(time.second(), 2, 10, QChar('0'));
//        head << QString::fromUtf8("  Время    %1:%2:%3.%4\r\n").arg(time.hour()).arg(time.minute()).arg(time.second()).arg(time.msec());
        head << QString::fromUtf8("  Оператор %1\r\n").arg(name);
        head << QString::fromUtf8("  Глубина: %1 м.\r\n").arg(((float)startDepth) / 100.0, 0, 'f', 2);

        head << QString::fromUtf8("~tool\r\n");
//        head << QString::fromUtf8("  Прибор %1\r\n").arg(tool_type);
//        head << QString::fromUtf8("  Модель 1.1\r\n");

        head << QString::fromUtf8("  Прибор %1\r\n").arg(tool_type);
//        head << QString::fromUtf8("  Номер %1\r\n").arg(tool_number);
        head << QString::fromUtf8("  Модель 1\r\n");
        head << QString::fromUtf8("  Зонды  2\r\n");
        head << QString::fromUtf8("  Точки записи\r\n");

        head << QString::fromUtf8("    И1   %1\r\n").arg(Shift_Point_IZL);
        head << QString::fromUtf8("    П1   %1\r\n").arg(Shift_Point_VK1);
        head << QString::fromUtf8("    П2   %1\r\n").arg(Shift_Point_VK2);

        head << QString::fromUtf8("~data\r\n");

        file.flush();
        file.close();

        bWriteEnable   = true;
        bFileNameValid = false;
    }
}
//-----------------------------------------------------------------------------
void qt_akp_file_save::write_data(void)
{
    if (!file.open(QIODevice::Append))
    {
        qDebug() << file.errorString();
        return;
    }

    stream.setDevice(&file);
    stream.setVersion(QDataStream::Qt_4_0);

    TAKP_FRAME* frame;
    while(data_list.count() > 0)
    {
        frame = data_list.first();
        stream << *frame;
        delete frame;
        data_list.removeFirst();
    }

    file.flush();
    file.close();
}
//---------------------------------------------------------------------------
void qt_akp_file_save::close_file(void)
{
    bWriteEnable   = false;
    bFileNameValid = false;
    write_data();
}
//---------------------------------------------------------------------------
void qt_akp_file_save::start(void)
{
    bWriteEnable     = false;
    bFileNameValid   = false;
    akp_curent_frame = NULL;
}
//---------------------------------------------------------------------------
void qt_akp_file_save::onDataUpdate (const int blk_cnt, const TDataPocket &data)
{
    Q_UNUSED(blk_cnt);

    //-------------------------------------------------------------------------
    if (bWriteEnable == false)
    {
        return;
    }
    //-------------------------------------------------------------------------
    if (COMAND_AKP_DO_MESERMENT != data.id)
    {
        return;
    }
    //-------------------------------------------------------------------------
//    check_CRC(data.data);
    //-------------------------------------------------------------------------
//    encode_frame_label(data.data);
//    if ( 0x9999 == get_frame_label() )
    {
        //-------------------------------------------------------------------------
//        encode_vk_number(data.data);
        //-------------------------------------------------------------------------
        if (NULL == akp_curent_frame)
        {
            akp_curent_frame = new(TAKP_FRAME);
            memset(akp_curent_frame, 0, sizeof(TAKP_FRAME));

            akp_curent_frame->dept = data.dept ;
            akp_curent_frame->ml   = data.ml;
        }
        //-------------------------------------------------------------------------
        if (akp_curent_frame->dept != data.dept)    // новый кадр
        {
            data_list.append(akp_curent_frame);

            akp_curent_frame = new(TAKP_FRAME);
            memset(akp_curent_frame, 0, sizeof(TAKP_FRAME));

            akp_curent_frame->dept = data.dept ;
            akp_curent_frame->ml   = data.ml;
        }
        //-------------------------------------------------------------------------
//        if ( 0 == get_vk_number() )
        if ( 1 == data.block_mask )
        {
            memcpy( akp_curent_frame->ch1, data.data, sizeof(TVAK_8_DATA) );
        }
//        if ( 4 == get_vk_number() )
        if ( 2 == data.block_mask )
        {
            memcpy( akp_curent_frame->ch2, data.data, sizeof(TVAK_8_DATA) );
        }
        //-------------------------------------------------------------------------
        if (data_list.count() >= buf_len)
        {
            write_data();
        }
    }
}
//---------------------------------------------------------------------------
