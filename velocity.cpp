//---------------------------------------------------------------------------
#include <QTime>
#include <QList>
//---------------------------------------------------------------------------
#include "velocity.h"
//---------------------------------------------------------------------------
CVELOCITY::CVELOCITY(QObject *parent):
    QObject(parent)
{
}
//---------------------------------------------------------------------------
CVELOCITY::~CVELOCITY(void)
{
  TVELOCITY* list_item;

  while (!list.isEmpty())
  {
    list_item = list.first();
    delete list_item;
    list_item = NULL;
    list.removeFirst();
  }
}
//---------------------------------------------------------------------------
void CVELOCITY::add_dept_point(qint32 new_dept)
{
  TVELOCITY* list_item = new TVELOCITY;

  list_item->dept = new_dept;
  list_item->time = QTime::currentTime();

  list.append(list_item);

  while (list.size() > 10)
  {
    list_item = list.first();
    delete list_item;
    list_item = NULL;
    list.removeFirst();
  }
}  
//---------------------------------------------------------------------------
qint16 CVELOCITY::get_velocity(void)
{
  if (list.isEmpty()) return 0;

  TVELOCITY* li_start = list.first();
  TVELOCITY* li_end   = list.last();

  int time = li_start->time.msecsTo(li_end->time);

  if (time == 0) return 0;

  qint16 vel = ((36000 * abs(li_start->dept - li_end->dept)) / time);

  return vel;
}
//---------------------------------------------------------------------------

