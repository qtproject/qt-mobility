/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSLOTINVOKER_H
#define QSLOTINVOKER_H

#include "qmobilityglobal.h"
#include <qobject.h>
#include <qvariant.h>
#include <qlist.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE
class QSlotInvokerPrivate;
class QSlotInvoker : public QObject
{
    Q_OBJECT
public:
    QSlotInvoker( QObject *receiver, const QByteArray& member,
                  QObject *parent=0 );
    ~QSlotInvoker();

    bool memberPresent() const;
    bool canInvoke( int numArgs ) const;
    QObject *receiver() const;
    QByteArray member() const;
    int *parameterTypes() const;
    int parameterTypesCount() const;

public slots:
    QVariant invoke( const QList<QVariant>& args );

private slots:
    void receiverDestroyed();

private:
    QSlotInvokerPrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
