
#include <QtCore/private/qobject_p.h>

#include "qabstractmediaobject.h"

class QAbstractMediaObjectPrivate : public QObjectPrivate
{
public:
};

QAbstractMediaObject::QAbstractMediaObject(QObject *parent):
    QObject(*new QAbstractMediaObjectPrivate, parent)
{
}
