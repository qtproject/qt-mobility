/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWMPMETADATA_H
#define QWMPMETADATA_H

#include "qmetadataprovider.h"

#include <wmp.h>

class QWmpEvents;

class QWmpMetaData : public QMetadataProvider
{
    Q_OBJECT
public:
    QWmpMetaData(IWMPCore3 *player, QWmpEvents *events, QObject *parent = 0);
    ~QWmpMetaData();

    bool metadataAvailable() const;
    bool isReadOnly() const;
    void setReadOnly(bool readonly);

    QList<QString> availableMetadata() const;
    QVariant metadata(QString const &name) const;
    void setMetadata(QString const &name, QVariant const &value);

    static QStringList keys(IWMPMedia *media);

    static int valueCount(IWMPMedia *media, const QString &key);
    
    static QVariant value(IWMPMedia *media, const QString &key, int value);
    static QVariantList values(IWMPMedia *media, const QString &key);

private Q_SLOTS:
    void currentItemChangeEvent(IDispatch *dispatch);
    void mediaChangeEvent(IDispatch *dispatch);

private:
    IWMPMedia *m_media;
};

#endif
