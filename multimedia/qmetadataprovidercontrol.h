/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMETADATAPROVIDERCONTROL_H
#define QMETADATAPROVIDERCONTROL_H

#include <multimedia/qmediacontrol.h>
#include <multimedia/qmediaobject.h>

#include <multimedia/qmediaresource.h>

class QMetaDataProviderControlPrivate;
class Q_MEDIA_EXPORT QMetaDataProviderControl : public QMediaControl
{
    Q_OBJECT

public:
    ~QMetaDataProviderControl();

    virtual bool isWritable() const = 0;
    virtual bool isMetaDataAvailable() const = 0;

    virtual QVariant metaData(QMediaObject::MetaData key) const = 0;
    virtual void setMetaData(QMediaObject::MetaData key, const QVariant &value) = 0;

    virtual QVariant extendedMetaData(const QString &key) const = 0;
    virtual void setExtendedMetaData(const QString &key, const QVariant &value) = 0;


Q_SIGNALS:
    void metaDataChanged();

    void writableChanged(bool writable);
    void metaDataAvailableChanged(bool available);

protected:
    QMetaDataProviderControl(QObject *parent = 0);
};

#define QMetaDataProviderControl_iid "com.nokia.Qt.QMetaDataProviderControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QMetaDataProviderControl, QMetaDataProviderControl_iid)

#endif  // QMETADATAPROVIDER_H
