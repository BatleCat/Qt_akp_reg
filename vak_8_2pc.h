/*-------------------------------------------------------------------
                            vak_8_2pc.h
    Описаны типы данных для обмена м/у ПК и Ethernet-контроллером

                                                     Петрухин А.С.
                                                   4 октября 2007г.
-------------------------------------------------------------------*/
#ifndef	VAK_8_2_PC_H
#define	VAK_8_2_PC_H
//-------------------------------------------------------------------
#include <qglobal.h>
#include <QMetaType>
#include <QDataStream>
//-------------------------------------------------------------------
#include "vak_8.h"
//-------------------------------------------------------------------
#define	PC_PORT	1500
//-------------------------------------------------------------------
#define	COMAND_SET_DEPT                 0x0001
#define	COMAND_READ_DEPT                0x0002
#define	COMAND_REPEAT_LAST_DATA	        0x0003
#define	COMAND_DO_MESERMENT             0x0004
#define	COMAND_VAK8N_DO_MESERMENT       0x0005
#define	COMAND_VAK8N_SD_DO_MESERMENT	0x0006
#define	COMAND_VAK8M09_DO_MESERMENT     0x0007
#define	COMAND_VAK8_SLOW_DO_MESERMENT	0x0008
#define	COMAND_SAS_BP_SLOW_DO_MESERMENT	0x0009
#define	COMAND_VAK32_DO_MESERMENT       0x000A
#define	COMAND_AKP_DO_MESERMENT			0x000B
//-------------------------------------------------------------------
#define	CTRL_POCKET_LEN                 20
//-------------------------------------------------------------------
typedef struct
{
    quint16     id;
    qint32      dept;
    quint16     ml;
    quint16     block_mask;
    quint16     work_mode[4];
    quint16     n_pocket;
    quint16     data[40];
} __attribute__ ((__packed__)) TCtrlPocket;
//-------------------------------------------------------------------
typedef struct
{
    quint16     id;
    qint32      dept;
    quint16     ml;
    quint16     block_mask;
    quint16     work_mode[4];
    TVAK_8_DATA	data;
} __attribute__ ((__packed__)) TDataPocket;
//-------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TDataPocket &data_pocket);
QDataStream& operator >>(QDataStream &in, TDataPocket &data_pocket);
Q_DECLARE_METATYPE(TDataPocket)
//-------------------------------------------------------------------
#endif	//	VAK_8_2_PC_H
//-------------------------------------------------------------------

