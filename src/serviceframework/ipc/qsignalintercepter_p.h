/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSIGNALINTERCEPTER_H
#define QSIGNALINTERCEPTER_H

#include "qmobilityglobal.h"
#include <qobject.h>
#include <qvariant.h>
#include <qlist.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QSignalIntercepterPrivate;
class QSignalIntercepter : public QObject
{
    // Note: Do not put Q_OBJECT here.
    friend class QSlotInvoker;
    friend class QCopProxy;
public:
    QSignalIntercepter( QObject *sender, const QByteArray& signal,
                        QObject *parent=0 );
    ~QSignalIntercepter();

    QObject *sender() const;
    QByteArray signal() const;

    bool isValid() const;

    static const int QVariantId = -243;

    static int *connectionTypes( const QByteArray& member, int& nargs );

protected:
    int qt_metacall( QMetaObject::Call c, int id, void **a );
    virtual void activated( const QList<QVariant>& args ) = 0;

private:
    QSignalIntercepterPrivate *d;

    static int typeFromName( const QByteArray& name );
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
