
#include <QtCore/private/qobject_p.h>
#include <QtCore/qtimer.h>

#include "qabstractmediaservice.h"


class QAbstractMediaServicePrivate : public QObjectPrivate
{
public:
};

QAbstractMediaService::QAbstractMediaService(QObject *parent):
    QObject(*new QAbstractMediaServicePrivate, parent)
{
    Q_D(QAbstractMediaService
}
