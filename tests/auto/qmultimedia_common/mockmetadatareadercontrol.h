/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MOCKMETADATAREADERCONTROL_H
#define MOCKMETADATAREADERCONTROL_H

#include "qmetadatareadercontrol.h"

class MockMetaDataReaderControl : public QMetaDataReaderControl
{
    Q_OBJECT
public:
    MockMetaDataReaderControl(QObject *parent = 0)
        : QMetaDataReaderControl(parent)
        , m_available(false)
    {
    }

    bool isMetaDataAvailable() const
    {
        return m_available;
    }
    void setMetaDataAvailable(bool available)
    {
        if (m_available != available)
            emit metaDataAvailableChanged(m_available = available);
    }
    QList<QtMultimediaKit::MetaData> availableMetaData() const
    {
        return m_data.keys();
    }

    QVariant metaData(QtMultimediaKit::MetaData key) const
    {
        return m_data.value(key);
    }

    QVariant extendedMetaData(const QString &key) const
    {
        return m_extendedData.value(key);
    }

    QStringList availableExtendedMetaData() const
    {
        return m_extendedData.keys();
    }

    using QMetaDataReaderControl::metaDataChanged;

    void populateMetaData()
    {
        m_available = true;
    }

    bool m_available;
    QMap<QtMultimediaKit::MetaData, QVariant> m_data;
    QMap<QString, QVariant> m_extendedData;
};

#endif // MOCKMETADATAREADERCONTROL_H
