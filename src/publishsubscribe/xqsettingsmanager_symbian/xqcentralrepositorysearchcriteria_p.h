/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef XQCENTRALREPOSITORYSEARCHCRITERIA_H
#define XQCENTRALREPOSITORYSEARCHCRITERIA_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QVariant>
#include "settingsmanager_global_p.h"

class XQSETTINGSMANAGER_EXPORT XQCentralRepositorySearchCriteria
{
public:
    explicit XQCentralRepositorySearchCriteria(long int repositoryUid,
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
