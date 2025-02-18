/*-------------------------------------------------------------------
                              vak_8_2pc.cpp

                                                     Петрухин А.С.
                                                      10/09/2019г.
-------------------------------------------------------------------*/
#include "vak_8_2pc.h"
//-------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TDataPocket &data_pocket)
{
    out.writeRawData((char*)(&data_pocket), sizeof(TDataPocket));
    return out;
}
//-------------------------------------------------------------------
QDataStream& operator >>(QDataStream &in, TDataPocket &data_pocket)
{
    in.readRawData((char*)(&data_pocket), sizeof(TDataPocket));
    return in;
}
//-------------------------------------------------------------------

