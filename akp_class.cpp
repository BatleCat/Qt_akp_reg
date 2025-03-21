//-----------------------------------------------------------------------------
#include <memory>
//-----------------------------------------------------------------------------
#include "akp_class.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
akp_class::akp_class(QObject *parent) :
    QObject(parent)
{
    rx_blk_count = -1;
    tx_blk_count = 0;

    dept_step_cm = 0;
    timer_interval = 500;

    continuous_meserment = false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
akp_class::~akp_class()
{
    delete udp_socket;
    emit connectionClosed();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void akp_class::start()
{
    udp_socket = new QUdpSocket();

    port = HOST_PORT;//1500;
    host = QHostAddress(HOST_ADR);//"10.2.22.245");

    udp_socket->bind(port);

//    connect(udp_socket, SIGNAL(readyRead() ), this, SLOT(onUdpDataRx() ) );
    connect(udp_socket, &QUdpSocket::readyRead, this, &akp_class::onUdpDataRx );

}
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::send_cmd(TCtrlPocket &ctl_pocket)
{
    tx_blk_count++;
    ctl_pocket.n_pocket = htons(tx_blk_count);

    udp_socket->writeDatagram((char*)&ctl_pocket, sizeof(TCtrlPocket), host, port);
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::on_setDeptStep(const qint32 step_cm)
 {
    dept_step_cm = step_cm;

    if (dept_step_cm == 0) timer_interval = 450;
    else timer_interval = 75;   // 75 ms + 50 ms = 125 ms this time must be greater than 104 ms
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::on_cmdSetDepth(const qint32 dept_cm)
{
    TCtrlPocket ctl_pocket;
    memset((char*)&ctl_pocket, 0, sizeof(TCtrlPocket));

    {
        quint16 cmd = (quint16)COMAND_SET_DEPT;
        ctl_pocket.id = htons(cmd);
    }
    ctl_pocket.dept = htonl(dept_cm);

    send_cmd(ctl_pocket);
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::on_cmdReadDepth(void)
{
    TCtrlPocket ctl_pocket;
    memset((char*)&ctl_pocket, 0, sizeof(TCtrlPocket));

    {
        quint16 cmd = (quint16)COMAND_READ_DEPT;
        ctl_pocket.id = htons(cmd);
    }

    send_cmd(ctl_pocket);
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::onDoMeserment(void)
{
    TCtrlPocket ctl_pocket;
    memset((char*)&ctl_pocket, 0, sizeof(TCtrlPocket));

    {
        quint16 cmd = (quint16)COMAND_AKP_DO_MESERMENT;
        ctl_pocket.id = htons(cmd);
    }
    ctl_pocket.dept = htonl(dept_step_cm);

    send_cmd(ctl_pocket);
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::on_cmdStartMeserment(void)
{
    continuous_meserment = true;
    onDoMeserment();
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::on_cmdDoSingleMeserment(void)
{
    continuous_meserment = false;
    onDoMeserment();
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::on_cmdStopMeserment(void)
{
    continuous_meserment = false;
}
//#pragma pack()
//-----------------------------------------------------------------------------
//#pragma pack(1)
void akp_class::onUdpDataRx(void)
{
    while (udp_socket->hasPendingDatagrams())
    {
/*
        QByteArray* datagram = new QByteArray;
        datagram->resize(udp_socket.pendingDatagramSize());
        udp_socket.readDatagram(datagram->data(), datagram->size(), &senderAdr, &senderPort);
        data_list << datagram;
*/
        TDataPocket* pdata = new(TDataPocket);

        udp_socket->readDatagram((char*) pdata, sizeof(TDataPocket), &senderAdr, &senderPort);

        pdata->id         = ntohs(pdata->id);
        pdata->dept       = ntohl(pdata->dept);
        pdata->ml         = ntohs(pdata->ml);
        pdata->block_mask = ntohs(pdata->block_mask);

        if (    ( 1 == pdata->block_mask)
             && ( continuous_meserment  )
             && ( (quint16)COMAND_AKP_DO_MESERMENT == pdata->id )
           )
        {
            timer.singleShot(timer_interval, this, &akp_class::onDoMeserment);
        }

        for (uint i = 0; i < 4; i++)
            pdata->work_mode[i] = ntohs(pdata->work_mode[i]);

        for (uint i = 0; i < VAK_8_NUM_POINTS; i++)
            pdata->data[i] = ntohs(pdata->data[i]);

        rx_blk_count++;

        emit dataUpdate(rx_blk_count, (*pdata));

        delete pdata;
    }
}
//#pragma pack()
//-----------------------------------------------------------------------------
