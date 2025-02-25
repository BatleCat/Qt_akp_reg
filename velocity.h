//---------------------------------------------------------------------------
#ifndef velocityH
#define velocityH
//---------------------------------------------------------------------------
#include <QObject>
#include <QList>
#include <QTime>
//---------------------------------------------------------------------------
typedef struct
{
  qint32 dept;
  QTime  time;
} TVELOCITY;
//---------------------------------------------------------------------------
class CVELOCITY : public QObject
{
    Q_OBJECT
private:
  QList<TVELOCITY*> list;

public:
  explicit CVELOCITY(QObject *parent = 0);
  ~CVELOCITY(void);

  void   add_dept_point(const qint32 new_dept);
  qint16 get_velocity  (void);
};
//---------------------------------------------------------------------------
#endif //velocityH
//---------------------------------------------------------------------------
