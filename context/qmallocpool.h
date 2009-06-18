/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QMALLOCPOOL_H
#define QMALLOCPOOL_H

#include <cstdlib>
#include <QString>

#include "qcontextglobal.h"

class QMallocPoolPrivate;

class Q_CFW_EXPORT QMallocPool
{
public:
    enum PoolType { Owned, NewShared, Shared };
    QMallocPool();
    QMallocPool(void * poolBase, unsigned int poolLength,
                PoolType type = Owned, const QString& name = QString());
    ~QMallocPool();

    size_t size_of(void *);
    void *calloc(size_t nmemb, size_t size);
    void *malloc(size_t size);
    void free(void *ptr);
    void *realloc(void *ptr, size_t size);

    bool isValid() const;

    struct MemoryStats {
        unsigned long poolSize;
        unsigned long maxSystemBytes;
        unsigned long systemBytes;
        unsigned long inuseBytes;
        unsigned long keepCost;
    };
    MemoryStats memoryStatistics() const;
    void dumpStats() const;

private:
    Q_DISABLE_COPY(QMallocPool)
    QMallocPoolPrivate * d;
};

#endif
