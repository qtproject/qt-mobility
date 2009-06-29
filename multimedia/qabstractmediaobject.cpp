
#include "qabstractmediaobject_p.h"

QAbstractMediaObject::QAbstractMediaObject(QObject *parent):
    QObject(*new QAbstractMediaObjectPrivate, parent)
{
}

/*!
    \internal
*/
QAbstractMediaObject::QAbstractMediaObject(QAbstractMediaObjectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}
