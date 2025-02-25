//-----------------------------------------------------------------------------
#ifndef AKP_CLASS_H
#define AKP_CLASS_H
//-----------------------------------------------------------------------------
#include <qsystemdetection.h>
//-----------------------------------------------------------------------------
#ifdef Q_OS_WIN
    #include <Windows.h>
    #include <winsock.h>
#endif
//-----------------------------------------------------------------------------
#ifdef Q_OS_LINUX
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif
//-----------------------------------------------------------------------------
//#pragma comment(lib, "wsock32.lib")
//-----------------------------------------------------------------------------
#include <QObject>
#include <QTimer>
#include <QtNetwork/QUdpSocket>
//-----------------------------------------------------------------------------
#include "vak_8_2pc.h"
//-----------------------------------------------------------------------------
#define HOST_PORT 1500
#define HOST_ADR  "10.2.22.245"
//-----------------------------------------------------------------------------
/*
//Определение выводов проекта
//-----------------------------------------------------------------------------
#define DEPT_RESET_PIN				0x0001		//PF0
#define AD25256_CS_PIN				0x0002		//PF1
#define IIM7010_IRQ_PIN				0x0004		//PF2
#define W3100_RESET_PIN				0x0008		//PF3
#define RTL8201L_RESET_PIN			0x0010		//PF4

#define SHDSL_nSS_PIN           	0x0020		//PF5
#define SHDSL_COT_nRTA_PIN      	0x0040		//PF6
#define SHDSL_MASTER_nSLAVE_PIN 	0x0080		//PF7
#define ML_PIN             			0x0200		//PF9
#define INIT_OK_PIN             	0x0400		//PF10
#define ERROR_PIN               	0x0800		//PF11
#define SHDSL_nRDY_PIN        		0x1000		//PF12
#define SHDSL_nLINK_PIN         	0x2000		//PF13
*/
//-----------------------------------------------------------------------------
class akp_class : public QObject
{
    Q_OBJECT
public:
    explicit akp_class(QObject *parent = 0);
    ~akp_class();

private:
    volatile uint   rx_blk_count;
    quint16         tx_blk_count;
//    quint16         cmd_id;
    QUdpSocket*     udp_socket;
    QHostAddress    host;
    quint16         port;
    QHostAddress    senderAdr;
    quint16         senderPort;

    qint32          dept_step_cm;

    bool            continuous_meserment;

    QTimer          timer;
//    int             timer_id;
    int             timer_interval;

    void    send_cmd(TCtrlPocket &ctl_pocket);

public:
    qint32  get_dept_step(void)                 {return dept_step_cm;}

//    void    set_timer_interval(int time_ms)     {timer_interval = time_ms;}
    int     get_timer_interval(void)            {return timer_interval;}

signals:
    void connectionClosed  (void);
    void dataUpdate        (const uint blk_cnt, const TDataPocket &data);
//    void netStatUpdate      (const quint16 rx_blk_cnt, const quint16 tx_blk_cnt, const QString sender_adr, const quint16 sender_port);

private slots:
    void onUdpDataRx        (void);
    void onDoMeserment      (void);

public slots:
    void start              (void);

    void on_cmdSetDepth     (const qint32 dept_cm);
    void on_cmdReadDepth    (void);
    void on_setDeptStep     (const qint32 step_cm);

    void on_cmdDoSingleMeserment (void);
    void on_cmdStartMeserment    (void);
    void on_cmdStopMeserment     (void);

//    void on_tool_start      (void);
};
//-----------------------------------------------------------------------------
#endif // AKP_CLASS_H
//-----------------------------------------------------------------------------
