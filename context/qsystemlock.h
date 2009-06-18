/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSYSTEMLOCK_H
#define QSYSTEMLOCK_H

#include "qtopiailglobal.h"

class QSystemReadWriteLockPrivate;
class QTOPIAIL_EXPORT QSystemReadWriteLock
{
public:
    QSystemReadWriteLock(unsigned int id, bool own);
    ~QSystemReadWriteLock();

    bool isNull() const;
    unsigned int id() const;

    bool lockForRead(int milliSec);
    bool lockForWrite(int milliSec);
    void unlock();

private:
    QSystemReadWriteLockPrivate * d;
};

class QSystemMutex_Private;
class QTOPIAIL_EXPORT QSystemMutex
{
public:
    QSystemMutex(unsigned int id, bool own);
    ~QSystemMutex();

    bool isNull() const;
    unsigned int id() const;

    bool lock(int milliSec);
    void unlock();

private:
    QSystemMutex_Private *m_data;
};

#endif
