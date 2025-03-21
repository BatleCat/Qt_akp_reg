//-----------------------------------------------------------------------------
// Разработал:  Петрухин А.С.
// Дата:        23.01.2025
//-----------------------------------------------------------------------------
#ifndef AKP_CHECK_STATE_H
#define AKP_CHECK_STATE_H
//-----------------------------------------------------------------------------
#include <QObject>
#include <QString>
//-----------------------------------------------------------------------------
#include "vak_8.h"
#include "vak_8_2pc.h"
//-----------------------------------------------------------------------------
class akp_check_state : public QObject
{
    Q_OBJECT
public:
    explicit akp_check_state(QObject *parent = 0);
    ~akp_check_state();

private:
    bool    ml;
    qint32  dept_cm;

    bool    old_ml;
    qint32  old_dept_cm;

    int     bad_block_cnt;
    int     good_block_cnt;

    int     old_bad_block_cnt;

    TVAK8_WAVE vk;

    quint16 frame_label;
    quint16 vk_number;
    quint16 rx_type;
    quint16 Td;
    quint16 Fsig;
    quint16 izl_type;
    quint16 izl_ampl;
    quint16 izl_periods;
    quint16 rx_delay;
    quint16 Ku;
    quint16 tool_type;
    quint16 mode_number;
    quint16	mode_count;
    quint16 vk_calibration_amplitude;
    quint16 vk_calibration_offset;
    quint16 tool_no;
    quint16 soft_version_major;
    quint16 soft_version_minor;
    quint32 timer_clk;
    quint32 time_start_meserment;
    quint32 time_stop_meserment;
    quint32 time_meserment;
    bool    CRC1_OK;
    bool    CRC2_OK;
    bool    CRC3_OK;
    bool    CRC4_OK;
    bool    CRC5_OK;
    bool    CRC6_OK;
    bool    CRC7_OK;
    bool    CRC8_OK;
    bool    CRC_OK;

    quint16 old_vk_number;
    quint16 old_rx_type;
    quint16 old_Td;
    quint16 old_Fsig;
    quint16 old_izl_type;
    quint16 old_izl_ampl;
    quint16 old_izl_periods;
    quint16 old_rx_delay;
    quint16 old_Ku;
    quint16 old_tool_type;
    quint16 old_mode_number;
    quint16	old_mode_count;
    quint16 old_vk_calibration_amplitude;
    quint16 old_vk_calibration_offset;
    quint16 old_tool_no;
    quint16 old_soft_version_major;
    quint16 old_soft_version_minor;
    quint32 old_timer_clk;
    quint32 old_time_start_meserment;
    quint32 old_time_stop_meserment;
    quint32 old_time_meserment;
    bool    old_CRC1_OK;
    bool    old_CRC2_OK;
    bool    old_CRC3_OK;
    bool    old_CRC4_OK;
    bool    old_CRC5_OK;
    bool    old_CRC6_OK;
    bool    old_CRC7_OK;
    bool    old_CRC8_OK;

    quint16 take_from_14th_bit(const quint16 frame_pos, const TVAK_8_DATA &frame);
    quint16 take_from_15th_bit(const quint16 frame_pos, const TVAK_8_DATA &frame);

    quint16 calc_CRC(const quint16 CRC_win_left, const TVAK_8_DATA &frame);

protected:
    void    encode_frame_label         (const TVAK_8_DATA &data);
    void    encode_vk_number           (const TVAK_8_DATA &data);
    void    encode_Rx_type             (const TVAK_8_DATA &data);
    void    encode_freq_value          (const TVAK_8_DATA &data);
    void    encode_delay_value         (const TVAK_8_DATA &data);
    void    encode_Ku_value            (const TVAK_8_DATA &data);
    void    encode_tool_type           (const TVAK_8_DATA &data);
    void    encode_mode_number         (const TVAK_8_DATA &data);
    void    encode_calibration_vk      (const TVAK_8_DATA &data);
    void    encode_tool_no             (const TVAK_8_DATA &data);
    void    encode_soft_version        (const TVAK_8_DATA &data);
    void    encode_timer_clk           (const TVAK_8_DATA &data);
    void    encode_time_start_meserment(const TVAK_8_DATA &data);
    void    encode_time_stop_meserment (const TVAK_8_DATA &data);

    void    encode_wave                (const TVAK_8_DATA &data);

    void    check_CRC                  (const TVAK_8_DATA &data);
    void    check_ml                   (const TDataPocket &data);
    void    check_dept                 (const TDataPocket &data);

    void    calc_time_meserment        (void);

public:
    void    set_state(const TDataPocket &data);

    bool    get_ml_state(void)                   {return ml;     }
    qint32  get_dept(void)                       {return dept_cm;}

    int     get_bad_block_count(void)            {return bad_block_cnt;  }
    int     get_good_block_count(void)           {return good_block_cnt; }
    void    clear_block_count(void);

    quint16 get_frame_label(void)                {return frame_label;}
    quint16 get_vk_number(void)                  {return vk_number;  }
    quint16 get_rx_type(void)                    {return rx_type;    }
    quint16 get_Td(void)                         {return Td;         }
    quint16 get_Fsig(void)                       {return Fsig;       }
    quint16 get_izl_type(void)                   {return izl_type;   }
    quint16 get_izl_ampl(void)                   {return izl_ampl;   }
    quint16 get_izl_periods(void)                {return izl_periods;}
    quint16 get_rx_delay(void)                   {return rx_delay;   }
    quint16 get_Ku(void)                         {return Ku;         }
    quint16 get_tool_type(void)                  {return tool_type;  }
    quint16 get_mode_number(void)                {return mode_number;}
    quint16	get_mode_count(void)                 {return mode_count; }
    quint16 get_vk_calibration_amplitude(void)   {return vk_calibration_amplitude;}
    quint16 get_vk_calibration_offset(void)      {return vk_calibration_offset;   }
    quint16 get_tool_no(void)                    {return tool_no;            }
    quint16 get_soft_version_major(void)         {return soft_version_major; }
    quint16 get_soft_version_minor(void)         {return soft_version_minor; }
    quint32 get_timer_clk(void)                  {return timer_clk;          }
    quint32 get_time_start_meserment(void)       {return time_start_meserment; }
    quint32 get_time_stop_meserment(void)        {return time_stop_meserment;  }
    quint32 get_time_meserment(void)             {return time_meserment;       }

    void    get_wave(TVAK8_WAVE &wave);

    bool is_CRC_OK(void)                         {return CRC_OK; }

    bool is_CRC1_OK(void)                        {return CRC1_OK; }
    bool is_CRC2_OK(void)                        {return CRC2_OK; }
    bool is_CRC3_OK(void)                        {return CRC3_OK; }
    bool is_CRC4_OK(void)                        {return CRC4_OK; }
    bool is_CRC5_OK(void)                        {return CRC5_OK; }
    bool is_CRC6_OK(void)                        {return CRC6_OK; }
    bool is_CRC7_OK(void)                        {return CRC7_OK; }
    bool is_CRC8_OK(void)                        {return CRC8_OK; }

    bool is_CRC_OK_for_frame_label(void)         {return CRC1_OK;}
    bool is_CRC_OK_for_vk_number(void)           {return CRC1_OK;}
    bool is_CRC_OK_for_rx_type(void)             {return CRC1_OK;}
    bool is_CRC_OK_for_Td(void)                  {return CRC1_OK;}
    bool is_CRC_OK_for_Ku(void)                  {return CRC1_OK;}

    bool is_CRC_OK_for_rx_delay(void)            {return CRC2_OK;}
    bool is_CRC_OK_for_Fsig(void)                {return CRC2_OK;}
    bool is_CRC_OK_for_izl_type(void)            {return CRC2_OK;}
    bool is_CRC_OK_for_izl_ampl(void)            {return CRC2_OK;}
    bool is_CRC_OK_for_izl_periods(void)         {return CRC2_OK;}
    bool is_CRC_OK_for_tool_type(void)           {return CRC2_OK;}
    bool is_CRC_OK_for_mode_number(void)         {return CRC2_OK;}

    bool is_CRC_OK_for_mode_count(void)          {return CRC3_OK;}

    bool is_CRC_OK_for_vk_calibration_amp(void)  {return CRC5_OK;}

    bool is_CRC_OK_for_vk_calibration_ofs(void)  {return CRC6_OK;}
    bool is_CRC_OK_for_tool_no(void)             {return CRC6_OK;}
    bool is_CRC_OK_for_soft_version(void)        {return CRC6_OK;}

    bool is_CRC_OK_for_timer_clk(void)           {return CRC7_OK;}
    bool is_CRC_OK_for_time_start_meserment(void){return CRC7_OK;}

    bool is_CRC_OK_for_time_stop_meserment(void) {return CRC8_OK;}

    bool is_CRC_OK_for_time_meserment(void)      {return (CRC7_OK && CRC8_OK);}

private:

signals:
    void finished(void);
    void logMessage(const QString message);

    void mlUpdate  (const bool status);
    void deptUpdate(const qint32 new_dept_cm);

    void frame_sync_error           (void);
    void vk_number_update           (const bool crc, const quint16 vk_number);
    void rx_type_update             (const bool crc, const quint16 rx_type  );
    void Td_update                  (const bool crc, const quint16 Td       );
    void Ku_update                  (const bool crc, const quint16 Ku       );

    void rx_delay_update            (const bool crc, const quint16 rx_delay   );
    void Fsig_update                (const bool crc, const quint16 Fsig       );
    void izl_type_update            (const bool crc, const quint16 izl_type   );
    void izl_ampl_update            (const bool crc, const quint16 izl_ampl   );
    void izl_periods_update         (const bool crc, const quint16 izl_periods);
    void tool_type_update           (const bool crc, const quint16 tool_type  );
    void mode_number_update         (const bool crc, const quint16 mode_number);

    void mode_count_update          (const bool crc, const quint16 mode_count);

    void vk_calibration_amp_update  (const bool crc, const quint16 vk_calibration_amplitude);

    void vk_calibration_ofs_update  (const bool crc, const quint16 vk_calibration_offset);
    void tool_no_update             (const bool crc, const quint16 tool_no);
    void soft_version_update        (const bool crc, const quint16 soft_version_major, const quint16 soft_version_minor);

    void timer_clk_update           (const bool crc, const quint32 timer_clk);
    void time_start_meserment_update(const bool crc, const quint32 time_start_meserment);

    void time_stop_meserment_update (const bool crc, const quint32 time_stop_meserment);

    void time_meserment_update      (const bool crc, const quint32 time_meserment);

    void CRC1_update             (const bool crc);
    void CRC2_update             (const bool crc);
    void CRC3_update             (const bool crc);
    void CRC4_update             (const bool crc);
    void CRC5_update             (const bool crc);
    void CRC6_update             (const bool crc);
    void CRC7_update             (const bool crc);
    void CRC8_update             (const bool crc);

    void good_blk_cnt_update        (const int cnt);
    void bad_blk_cnt_update         (const int cnt);
    void VK_update(const quint16 vk_no, const TVAK8_WAVE &vk);

public slots:
    void start(void);
    void onDataUpdate(const uint blk_cnt, const TDataPocket &data);
};
//-----------------------------------------------------------------------------
#endif // AKP_CHECK_STATE_H
//-----------------------------------------------------------------------------
