//-----------------------------------------------------------------------------
#include <QDebug>
#include "akp_check_state.h"
#include "akp_class.h"
#include "tool_type.h"
//-----------------------------------------------------------------------------
#define frame_pos_CRC1  48
#define frame_pos_CRC2  (frame_pos_CRC1 + 64)
#define frame_pos_CRC3  (frame_pos_CRC2 + 64)
#define frame_pos_CRC4  (frame_pos_CRC3 + 64)
#define frame_pos_CRC5  (frame_pos_CRC4 + 64)
#define frame_pos_CRC6  (frame_pos_CRC5 + 64)
#define frame_pos_CRC7  (frame_pos_CRC6 + 64)
#define frame_pos_CRC8  (frame_pos_CRC7 + 64)
//-----------------------------------------------------------------------------
#define CRC1_win_left   0
#define CRC2_win_left   (CRC1_win_left + 64)
#define CRC3_win_left   (CRC2_win_left + 64)
#define CRC4_win_left   (CRC3_win_left + 64)
#define CRC5_win_left   (CRC4_win_left + 64)
#define CRC6_win_left   (CRC5_win_left + 64)
#define CRC7_win_left   (CRC6_win_left + 64)
#define CRC8_win_left   (CRC7_win_left + 64)
//-----------------------------------------------------------------------------
// CRC1 check
#define frame_pos_frame_label  				0
#define frame_pos_num      					16
#define frame_pos_Rx_type  					32
#define frame_pos_Ku       					48
//-----------------------------------------------------------------------------
// CRC2 check
#define frame_pos_delay     				64
#define frame_pos_freq     					80
#define frame_pos_tool_type 				96
#define frame_pos_mode_num  				112
//-----------------------------------------------------------------------------
// CRC3 check
#define frame_pos_mode_count  				128
//-----------------------------------------------------------------------------
#define frame_pos_angle_x					144
#define frame_pos_accel_x					160
#define frame_pos_angle_y					176
//-----------------------------------------------------------------------------
// CRC4 check
#define frame_pos_accel_y					192
#define frame_pos_angle_z1					208
#define frame_pos_accel_z1					224
#define frame_pos_angle_z2					240
//-----------------------------------------------------------------------------
// CRC5 check
#define frame_pos_accel_z2					256
#define frame_pos_temp_int					272
#define frame_pos_temp_ppt					288
//-----------------------------------------------------------------------------
#define frame_pos_calibr_ampl				304
//-----------------------------------------------------------------------------
// CRC6 check
#define frame_pos_calibr_ofset				320
#define frame_pos_tool_no					336
#define frame_pos_ver_major					352
#define frame_pos_ver_minor					368
//-----------------------------------------------------------------------------
// CRC7 check
#define frame_pos_timer_clk_lo				384
#define frame_pos_timer_clk_hi				400
#define frame_pos_time_start_meserment_lo	416
#define frame_pos_time_start_meserment_hi	432
//-----------------------------------------------------------------------------
// CRC8 check
#define frame_pos_time_stop_meserment_lo	448
#define frame_pos_time_stop_meserment_hi	464
//-----------------------------------------------------------------------------
typedef union
{
    quint32 word32;
    struct
    {
        quint16 lo;
        quint16 hi;
    } word16;
} __attribute__ ((__packed__)) TWORD32_BYTES;
//-----------------------------------------------------------------------------
typedef union
{
    quint16 word16;
    struct
    {
        quint8 lo;
        quint8 hi;
    } word8;
} __attribute__ ((__packed__)) TWORD16_BYTES;
//-----------------------------------------------------------------------------
akp_check_state::akp_check_state(QObject *parent):
    QObject(parent)
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
akp_check_state::~akp_check_state()
{
    emit finished();
}
//-----------------------------------------------------------------------------
quint16 akp_check_state::take_from_14th_bit(const quint16 frame_pos, const TVAK_8_DATA &frame)
{
  int     i;
  quint16 x;
  quint16 rez;

  rez = 0;
  for(i = 0; i < 16; i++)
  {
    x = frame[i + frame_pos] & 0x4000;
    if (x == 0x4000) rez = (rez | (1 << (15 - i)));
  }
  return rez;
}
//-----------------------------------------------------------------------------
quint16 akp_check_state::take_from_15th_bit(const quint16 frame_pos, const TVAK_8_DATA &frame)
{
  int     i;
  quint16 x;
  quint16 rez;

  rez = 0;
  for(i = 0; i < 16; i++)
  {
    x = frame[i + frame_pos] & 0x8000;
    if (x == 0x8000) rez = (rez | (1 << (15 - i)));
  }
  return rez;
}
//-----------------------------------------------------------------------------
quint16 akp_check_state::calc_CRC(const quint16 CRC_win_left, const TVAK_8_DATA &frame)
{
  const int start = CRC_win_left;
  const int stop  = start + 64;

  int            i;
  quint16 crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (frame[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_frame_label(const TVAK_8_DATA &data)
{
    frame_label = take_from_14th_bit(frame_pos_frame_label, data);
//    qDebug() << QString("%1").arg(frame_label, 4, 16, QChar('0')).toUpper();
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_vk_number(const TVAK_8_DATA &data)
{
    old_vk_number = vk_number;
    vk_number     = take_from_14th_bit(frame_pos_num, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_Rx_type(const TVAK_8_DATA &data)
{
    TWORD16_BYTES x;

    old_rx_type = rx_type;
    old_Td      = Td;

    x.word16    = take_from_14th_bit(frame_pos_Rx_type, data);

    rx_type     = x.word8.lo;
    Td          = x.word8.hi;

}
//-----------------------------------------------------------------------------
void akp_check_state::encode_freq_value(const TVAK_8_DATA &data)
{
    union
    {
        uint16_t ui16;
        struct
        {
            uint16_t freq   : 8;
            uint16_t type   : 3;
            uint16_t ampl   : 2;
            uint16_t period : 3;
        } filds;
    } izl;

    old_Fsig        = Fsig;
    old_izl_type    = izl_type;
    old_izl_ampl    = izl_ampl;
    old_izl_periods = izl_periods;

    izl.ui16        = take_from_14th_bit(frame_pos_freq, data);

    Fsig        = izl.filds.freq;
    izl_type    = izl.filds.type;
    izl_ampl    = izl.filds.ampl;
    izl_periods = izl.filds.period;
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_delay_value(const TVAK_8_DATA &data)
{
    old_rx_delay = rx_delay;
    rx_delay     = take_from_14th_bit(frame_pos_delay, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_Ku_value(const TVAK_8_DATA &data)
{
    old_Ku = Ku;
    Ku     = take_from_14th_bit(frame_pos_Ku, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_tool_type(const TVAK_8_DATA &data)
{
    old_tool_type = tool_type;
    tool_type     = take_from_14th_bit(frame_pos_tool_type, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_mode_number(const TVAK_8_DATA &data)
{
    old_mode_number = mode_number;
    old_mode_count  = mode_count;

    mode_number     = take_from_14th_bit(frame_pos_mode_num,   data);
    mode_count      = take_from_14th_bit(frame_pos_mode_count, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_calibration_vk(const TVAK_8_DATA &data)
{
    old_vk_calibration_amplitude = vk_calibration_amplitude;
    old_vk_calibration_offset    = vk_calibration_offset;

    vk_calibration_amplitude     = take_from_14th_bit(frame_pos_calibr_ampl,  data);
    vk_calibration_offset        = take_from_14th_bit(frame_pos_calibr_ofset, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_tool_no(const TVAK_8_DATA &data)
{
    old_tool_no = tool_no;
    tool_no     = take_from_14th_bit(frame_pos_tool_no, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_soft_version(const TVAK_8_DATA &data)
{
    old_soft_version_major = soft_version_major;
    old_soft_version_minor = soft_version_minor;

    soft_version_major     = take_from_14th_bit(frame_pos_ver_major, data);
    soft_version_minor     = take_from_14th_bit(frame_pos_ver_minor, data);
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_timer_clk(const TVAK_8_DATA &data)
{
    TWORD32_BYTES x;

    x.word16.lo = take_from_14th_bit(frame_pos_timer_clk_lo, data);
    x.word16.hi = take_from_14th_bit(frame_pos_timer_clk_hi, data);

    old_timer_clk = timer_clk;
    timer_clk     = x.word32;
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_time_start_meserment(const TVAK_8_DATA &data)
{
    TWORD32_BYTES x;

    x.word16.lo = take_from_14th_bit(frame_pos_time_start_meserment_lo, data);
    x.word16.hi = take_from_14th_bit(frame_pos_time_start_meserment_hi, data);

    old_time_start_meserment = time_start_meserment;
    time_start_meserment = x.word32;
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_time_stop_meserment(const TVAK_8_DATA &data)
{
    TWORD32_BYTES x;

    x.word16.lo = take_from_14th_bit(frame_pos_time_stop_meserment_lo, data);
    x.word16.hi = take_from_14th_bit(frame_pos_time_stop_meserment_hi, data);

    old_time_stop_meserment = time_stop_meserment;
    time_stop_meserment = x.word32;
}
//-----------------------------------------------------------------------------
void akp_check_state::encode_wave(const TVAK_8_DATA &data)
{
    int     i;
    for (i = 0; i < VAK_8_NUM_POINTS; i++)
    {
        qint16 smpl = 0x3FFF & data[i];
        vk[i] = smpl | ((smpl & 0x2000) * 7);

//        quint16 x;
//        x = 0x3FFF & data.data[i];
//        if ( 0 == (x & 0x2000) ) vk[i] = x;
//        else vk[i] = -x;

//        if ( (data.data[i] & 0x2000 ) != 0) vk[i] = data.data[i] | 0xC000;
//        else vk[i] = data.data[i] & 0x1FFF;
    }
}
//-----------------------------------------------------------------------------
void akp_check_state::check_CRC(const TVAK_8_DATA &data)
{
    quint16 CRC_read;
    quint16 CRC_calc;
    //-------------------------------------------------------
    // CRC1
    //-------------------------------------------------------
    old_CRC1_OK = CRC1_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC1, data);
    CRC_calc = calc_CRC(CRC1_win_left, data);
    if (CRC_read == CRC_calc) CRC1_OK = true;
    else CRC1_OK = false;
    //-------------------------------------------------------
    // CRC2
    //-------------------------------------------------------
    old_CRC2_OK = CRC2_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC2, data);
    CRC_calc = calc_CRC(CRC2_win_left, data);
    if (CRC_read == CRC_calc) CRC2_OK = true;
    else CRC2_OK = false;
    //-------------------------------------------------------
    // CRC3
    //-------------------------------------------------------
    old_CRC3_OK = CRC3_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC3, data);
    CRC_calc = calc_CRC(CRC3_win_left, data);
    if (CRC_read == CRC_calc) CRC3_OK = true;
    else CRC3_OK = false;
    //-------------------------------------------------------
    // CRC4
    //-------------------------------------------------------
    old_CRC4_OK = CRC4_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC4, data);
    CRC_calc = calc_CRC(CRC4_win_left, data);
    if (CRC_read == CRC_calc) CRC4_OK = true;
    else CRC4_OK = false;
    //-------------------------------------------------------
    // CRC5
    //-------------------------------------------------------
    old_CRC5_OK = CRC5_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC5, data);
    CRC_calc = calc_CRC(CRC5_win_left, data);
    if (CRC_read == CRC_calc) CRC5_OK = true;
    else CRC5_OK = false;
    //-------------------------------------------------------
    // CRC6
    //-------------------------------------------------------
    old_CRC6_OK = CRC6_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC6, data);
    CRC_calc = calc_CRC(CRC6_win_left, data);
    if (CRC_read == CRC_calc) CRC6_OK = true;
    else CRC6_OK = false;
    //-------------------------------------------------------
    // CRC7
    //-------------------------------------------------------
    old_CRC7_OK = CRC7_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC7, data);
    CRC_calc = calc_CRC(CRC7_win_left, data);
    if (CRC_read == CRC_calc) CRC7_OK = true;
    else CRC7_OK = false;
    //-------------------------------------------------------
    // CRC8
    //-------------------------------------------------------
    old_CRC8_OK = CRC8_OK;
    CRC_read = take_from_15th_bit(frame_pos_CRC8, data);
    CRC_calc = calc_CRC(CRC8_win_left, data);
    if (CRC_read == CRC_calc) CRC8_OK = true;
    else CRC8_OK = false;
    //-------------------------------------------------------
    CRC_OK = CRC1_OK && CRC2_OK && CRC3_OK && CRC4_OK && CRC5_OK && CRC6_OK && CRC7_OK && CRC8_OK;
    //-------------------------------------------------------
}
//-----------------------------------------------------------------------------
void akp_check_state::check_ml(const TDataPocket &data)
{
    old_ml      = ml;
    if (data.ml == 0) ml = false;
    else ml = true;
}
//-----------------------------------------------------------------------------
void akp_check_state::check_dept(const TDataPocket &data)
{
    old_dept_cm = dept_cm;
    dept_cm = (data.dept);
}
//-----------------------------------------------------------------------------
void akp_check_state::calc_time_meserment(void)
{
    quint32 delta;
    quint32 freq;
    quint32 time;

    old_time_meserment = time_meserment;

    delta = (time_start_meserment - time_stop_meserment);
    freq = timer_clk / delta;
    time = (quint32)1000000  / freq;
    time_meserment = time - 2;
}
//-----------------------------------------------------------------------------
void akp_check_state::set_state(const TDataPocket &data)
{
    check_ml  (data);
    check_dept(data);

    check_CRC (data.data);

    encode_frame_label         (data.data);
    encode_vk_number           (data.data);
    encode_Rx_type             (data.data);
    encode_freq_value          (data.data);
    encode_delay_value         (data.data);
    encode_Ku_value            (data.data);
    encode_tool_type           (data.data);
    encode_mode_number         (data.data);
    encode_calibration_vk      (data.data);
    encode_tool_no             (data.data);
    encode_soft_version        (data.data);
    encode_timer_clk           (data.data);
    encode_time_start_meserment(data.data);
    encode_time_stop_meserment (data.data);

    calc_time_meserment();
    //-------------------------------------------------------------------------
    if ( (true == CRC_OK) && (0x9999 == frame_label) )
    {
        good_block_cnt++;
    }
    else
    {
        old_bad_block_cnt = bad_block_cnt;
        bad_block_cnt++;
    }
    //-------------------------------------------------------------------------
    encode_wave(data.data);
    //-------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
void akp_check_state::clear_block_count(void)
{
    bad_block_cnt  = 0;
    good_block_cnt = 0;

    //old_bad_block_cnt = 0;
}
//-----------------------------------------------------------------------------
void akp_check_state::get_wave(TVAK8_WAVE &wave)
{
    memcpy(&wave, vk, sizeof(TVAK8_WAVE));
}
//-----------------------------------------------------------------------------
void akp_check_state::start(void)
{
    ml          = false;
    dept_cm     = (qint32)0;

    old_ml      = false;
    old_dept_cm = (qint32)0;

    bad_block_cnt  = 0;
    good_block_cnt = 0;

    old_bad_block_cnt = 0;

    frame_label = 0;
    vk_number = 0;
    rx_type = 0;
    Td = 0;
    Fsig = 0;
    izl_type = 0;
    izl_ampl = 0;
    izl_periods = 0;
    rx_delay = 0;
    Ku = 0;
    tool_type = TOOL_UNKNOW;
    mode_number = 0;
    mode_count = 0;
    vk_calibration_amplitude = 0;
    vk_calibration_offset = 0;
    tool_no = 0;
    soft_version_major = 0;
    soft_version_minor = 0;
    timer_clk = 0;
    time_start_meserment = 0;
    time_stop_meserment = 0;
    time_meserment = 0;
    CRC1_OK = false;
    CRC2_OK = false;
    CRC3_OK = false;
    CRC4_OK = false;
    CRC5_OK = false;
    CRC6_OK = false;
    CRC7_OK = false;
    CRC8_OK = false;
    CRC_OK  = false;

    emit mlUpdate(ml);
    emit deptUpdate(dept_cm);

    //---------------------------------------------------------------------
    // CRC1 check
    //---------------------------------------------------------------------
    emit vk_number_update(false, vk_number);
    emit rx_type_update  (false, rx_type);
    emit Td_update       (false, Td);
    emit Ku_update       (false, Ku);
    //---------------------------------------------------------------------
    // CRC2 check
    //---------------------------------------------------------------------
    emit rx_delay_update   (false, rx_delay);
    emit Fsig_update       (false, Fsig);
    emit izl_type_update   (false, izl_type);
    emit izl_ampl_update   (false, izl_ampl);
    emit izl_periods_update(false, izl_periods);
    emit tool_type_update  (false, tool_type);
    emit mode_number_update(false, mode_number);
    //---------------------------------------------------------------------
    // CRC3 check
    //---------------------------------------------------------------------
    emit mode_count_update(false, mode_count);
    //---------------------------------------------------------------------
    // CRC4 check
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    // CRC5 check
    //---------------------------------------------------------------------
    emit vk_calibration_amp_update(false, vk_calibration_amplitude);
    //---------------------------------------------------------------------
    // CRC6 check
    //---------------------------------------------------------------------
    emit vk_calibration_ofs_update  (false, vk_calibration_offset);
    emit tool_no_update             (false, tool_no);
    emit soft_version_update        (false, soft_version_major, soft_version_minor);
    //---------------------------------------------------------------------
    // CRC7 check
    //---------------------------------------------------------------------
    emit timer_clk_update           (false, timer_clk);
    emit time_start_meserment_update(false, time_start_meserment);
    //---------------------------------------------------------------------
    // CRC8 check
    //---------------------------------------------------------------------
    emit time_stop_meserment_update(false, time_stop_meserment);
    //---------------------------------------------------------------------
    emit time_meserment_update      (false, time_meserment);
    //---------------------------------------------------------------------
    emit CRC1_update(CRC1_OK);
    emit CRC2_update(CRC2_OK);
    emit CRC3_update(CRC3_OK);
    emit CRC4_update(CRC4_OK);
    emit CRC5_update(CRC5_OK);
    emit CRC6_update(CRC6_OK);
    emit CRC7_update(CRC7_OK);
    emit CRC8_update(CRC8_OK);
    //---------------------------------------------------------------------
    emit good_blk_cnt_update(good_block_cnt);
    emit bad_blk_cnt_update(bad_block_cnt);
    //---------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
void akp_check_state::onDataUpdate(const uint blk_cnt, const TDataPocket &data)
{
    Q_UNUSED(blk_cnt);

//    qDebug() << QString::fromUtf8("AKP check state : ") << &data;

    //-------------------------------------------------------------------------
    // Проверка идентефикатора ответа data.id
    //-------------------------------------------------------------------------
    if (COMAND_SET_DEPT == data.id)
    {
        check_ml  (data);
        check_dept(data);
        if (ml   != old_ml  )       emit mlUpdate(ml);
        if (dept_cm != old_dept_cm) emit deptUpdate(dept_cm);

        return;
    }
    //-------------------------------------------------------------------------
    if (COMAND_READ_DEPT == data.id)
    {
        check_ml  (data);
        check_dept(data);
        if (ml   != old_ml  )       emit mlUpdate(ml);
        if (dept_cm != old_dept_cm) emit deptUpdate(dept_cm);

        return;
    }
    //-------------------------------------------------------------------------
    if (COMAND_AKP_DO_MESERMENT == data.id)
    {
        set_state(data);

        if (ml   != old_ml  )       emit mlUpdate(ml);
        if (dept_cm != old_dept_cm) emit deptUpdate(dept_cm);

        if (bad_block_cnt == old_bad_block_cnt) emit good_blk_cnt_update(good_block_cnt);
        else emit bad_blk_cnt_update(bad_block_cnt);

        //---------------------------------------------------------------------
        // CRC1 check
        //---------------------------------------------------------------------
        if (CRC1_OK)
        {
            if (old_CRC1_OK != CRC1_OK)
            {
                emit vk_number_update(true, vk_number);
                emit rx_type_update  (true, rx_type);
                emit Td_update       (true, Td);
                emit Ku_update       (true, Ku);
            }
            else
            {
                if (frame_label != 0x9999)
                {
                    emit frame_sync_error();
                }

                if (old_vk_number != vk_number)
                {
                    emit vk_number_update(true, vk_number);
                }

                if (old_rx_type != rx_type)
                {
                    emit rx_type_update(true, rx_type);
                }

                if (old_Td != Td)
                {
                    emit Td_update(true, Td);
                }

                if (old_Ku != Ku)
                {
                    emit Ku_update(true, Ku);
                }
            }
        }
        else
        {
            emit vk_number_update(false, vk_number);
            emit rx_type_update  (false, rx_type);
            emit Td_update       (false, Td);
            emit Ku_update       (false, Ku);
        }
        //---------------------------------------------------------------------
        // CRC2 check
        //---------------------------------------------------------------------
        if (CRC2_OK)
        {
            if (old_CRC2_OK != CRC2_OK)
            {
                emit rx_delay_update   (true, rx_delay);
                emit Fsig_update       (true, Fsig);
                emit izl_type_update   (true, izl_type);
                emit izl_ampl_update   (true, izl_ampl);
                emit izl_periods_update(true, izl_periods);
                emit tool_type_update  (true, tool_type);
                emit mode_number_update(true, mode_number);
            }
            else
            {
                if (old_rx_delay != rx_delay)
                {
                    emit rx_delay_update(true, rx_delay);
                }

                if (old_Fsig != Fsig)
                {
                    emit Fsig_update(true, Fsig);
                }

                if (old_izl_type != izl_type)
                {
                    emit izl_type_update(true, izl_type);
                }

                if (old_izl_ampl != izl_ampl)
                {
                    emit izl_ampl_update(true, izl_ampl);
                }

                if (old_izl_periods != izl_periods)
                {
                    emit izl_periods_update(true, izl_periods);
                }

                if (old_tool_type != tool_type)
                {
                    emit tool_type_update(true, tool_type);
                }

                if (old_mode_number != mode_number)
                {
                    emit mode_number_update(true, mode_number);
                }
            }
        }
        else
        {
            emit rx_delay_update   (false, rx_delay);
            emit Fsig_update       (false, Fsig);
            emit izl_type_update   (false, izl_type);
            emit izl_ampl_update   (false, izl_ampl);
            emit izl_periods_update(false, izl_periods);
            emit tool_type_update  (false, tool_type);
            emit mode_number_update(false, mode_number);
        }
        //---------------------------------------------------------------------
        // CRC3 check
        //---------------------------------------------------------------------
        if (CRC3_OK)
        {
            if ( (old_mode_count != mode_count) || (old_CRC3_OK != CRC3_OK) )
            {
                emit mode_count_update(true, mode_count);
            }
        }
        else
        {
            emit mode_count_update(false, mode_count);
        }
        //---------------------------------------------------------------------
        // CRC4 check
        //---------------------------------------------------------------------

        //---------------------------------------------------------------------
        // CRC5 check
        //---------------------------------------------------------------------
        if (CRC5_OK)
        {
            if ( (old_vk_calibration_amplitude != vk_calibration_amplitude) || (old_CRC5_OK != CRC5_OK) )
            {
                emit vk_calibration_amp_update(true, vk_calibration_amplitude);
            }
        }
        else
        {
            emit vk_calibration_amp_update(false, vk_calibration_amplitude);
        }
        //---------------------------------------------------------------------
        // CRC6 check
        //---------------------------------------------------------------------
        if (CRC6_OK)
        {
            if (old_CRC6_OK != CRC6_OK)
            {
                emit vk_calibration_ofs_update  (true, vk_calibration_offset);
                emit tool_no_update             (true, tool_no);
                emit soft_version_update        (true, soft_version_major, soft_version_minor);
            }
            else
            {
                if (old_vk_calibration_offset != vk_calibration_offset)
                {
                    emit vk_calibration_ofs_update(true, vk_calibration_offset);
                }

                if (old_tool_no != tool_no)
                {
                    emit tool_no_update(true, tool_no);
                }

                if ( (old_soft_version_major != soft_version_major) || (old_soft_version_minor != soft_version_minor) )
                {
                    emit soft_version_update(true, soft_version_major, soft_version_minor);
                }
            }
        }
        else
        {
            emit vk_calibration_ofs_update  (false, vk_calibration_offset);
            emit tool_no_update             (false, tool_no);
            emit soft_version_update        (false, soft_version_major, soft_version_minor);
        }
        //---------------------------------------------------------------------
        // CRC7 check
        //---------------------------------------------------------------------
        if (CRC7_OK)
        {
            if (old_CRC7_OK != CRC7_OK)
            {
                emit timer_clk_update           (true, timer_clk);
                emit time_start_meserment_update(true, time_start_meserment);
            }
            else
            {
                if (old_timer_clk != timer_clk)
                {
                    emit timer_clk_update(true, timer_clk);
                }

                if (old_time_start_meserment != time_start_meserment)
                {
                    emit time_start_meserment_update(true, time_start_meserment);
                }
            }
        }
        else
        {
            emit timer_clk_update           (false, timer_clk);
            emit time_start_meserment_update(false, time_start_meserment);
        }
        //---------------------------------------------------------------------
        // CRC8 check
        //---------------------------------------------------------------------
        if (CRC8_OK)
        {
            if ( (old_time_stop_meserment != time_stop_meserment) || (old_CRC8_OK != CRC8_OK) )
            {
                emit time_stop_meserment_update(true, time_stop_meserment);
            }
        }
        else
        {
            emit time_stop_meserment_update(false, time_stop_meserment);
        }
        //---------------------------------------------------------------------
        if ( (CRC7_OK) && (CRC8_OK) )
        {
            if ( (old_time_meserment != time_meserment) || (old_CRC7_OK != CRC7_OK) || (old_CRC8_OK != CRC8_OK) )
            {
                emit time_meserment_update(true, time_meserment);
            }
        }
        else
        {
            emit time_meserment_update(false, time_meserment);
        }
        //---------------------------------------------------------------------
        if (old_CRC1_OK != CRC1_OK)
        {
            emit CRC1_update(CRC1_OK);
        }

        if (old_CRC2_OK != CRC2_OK)
        {
            emit CRC2_update(CRC2_OK);
        }

        if (old_CRC3_OK != CRC3_OK)
        {
            emit CRC3_update(CRC3_OK);
        }

        if (old_CRC4_OK != CRC4_OK)
        {
            emit CRC4_update(CRC4_OK);
        }

        if (old_CRC5_OK != CRC5_OK)
        {
            emit CRC5_update(CRC5_OK);
        }

        if (old_CRC6_OK != CRC6_OK)
        {
            emit CRC6_update(CRC6_OK);
        }

        if (old_CRC7_OK != CRC7_OK)
        {
            emit CRC7_update(CRC7_OK);
        }

        if (old_CRC8_OK != CRC8_OK)
        {
            emit CRC8_update(CRC8_OK);
        }
        //---------------------------------------------------------------------
        emit VK_update(get_vk_number(), vk);
        //---------------------------------------------------------------------
    }
    //-------------------------------------------------------------------------
//    emit logMessage(QString::fromUtf8("Пакет %1 : Счетчик переданных пакетов не изменился.").arg(blk_cnt));
}
//-----------------------------------------------------------------------------
