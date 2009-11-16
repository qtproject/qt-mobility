#ifndef XQCENTRALREPOSITORYSEARCHCRITERIA_H
#define XQCENTRALREPOSITORYSEARCHCRITERIA_H

#include <QVariant>
#include "settingsmanager_global.h"

class XQSETTINGSMANAGER_EXPORT XQCentralRepositorySearchCriteria
{
public:
    XQCentralRepositorySearchCriteria(long int repositoryUid,
        unsigned long int partialKey = 0, unsigned long int bitMask = 0);
    ~XQCentralRepositorySearchCriteria();
    void setValueCriteria(const QVariant& value, bool negativeCriteria = false);

    long int repositoryUid() const;
    unsigned long int partialKey() const;
    unsigned long int bitMask() const;
    const QVariant& value() const;
    bool negativeCriteria() const;

private:
    long int m_repositoryUid;
    unsigned long int m_partialKey;
    unsigned long int m_bitMask;
    QVariant m_value;
    bool m_negativeCriteria;
};

#endif //XQCENTRALREPOSITORYSEARCHCRITERIA_H
