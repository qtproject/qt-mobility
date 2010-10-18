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

#ifndef QDECLARATIVECONTACTRINGTONE_H
#define QDECLARATIVECONTACTRINGTONE_H

#include <QUrl>

#include "qdeclarativecontactdetail_p.h"
#include "qcontactringtone.h"

class QDeclarativeContactRingtone : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QUrl audioRingtoneUrl READ audioRingtoneUrl WRITE setAudioRingtoneUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl videoRingtoneUrl READ videoRingtoneUrl WRITE setVideoRingtoneUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl vibrationRingtoneUrl READ vibrationRingtoneUrl WRITE setVibrationRingtoneUrl NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_CLASSINFO("DefaultProperty", "audioRingtoneUrl")
public:
    enum FieldType {
        AudioRingtoneUrl = 0,
        VideoRingtoneUrl,
        VibrationRingtoneUrl
    };

    QDeclarativeContactRingtone(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactRingtone());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactRingtone;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case AudioRingtoneUrl:
            return QContactRingtone::FieldAudioRingtoneUrl;
        case VideoRingtoneUrl:
            return QContactRingtone::FieldVideoRingtoneUrl;
        case VibrationRingtoneUrl:
            return QContactRingtone::FieldVibrationRingtoneUrl;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setAudioRingtoneUrl(const QUrl& v)
    {
        if (!readOnly() && v != audioRingtoneUrl()) {
            detail().setValue(QContactRingtone::FieldAudioRingtoneUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl audioRingtoneUrl() const {return detail().value(QContactRingtone::FieldAudioRingtoneUrl);}

    void setVideoRingtoneUrl(const QUrl& v)
    {
        if (!readOnly() && v != videoRingtoneUrl()) {
            detail().setValue(QContactRingtone::FieldVideoRingtoneUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl videoRingtoneUrl() const {return detail().value(QContactRingtone::FieldVideoRingtoneUrl);}

    void setVibrationRingtoneUrl(const QUrl& v)
    {
        if (!readOnly() && v != vibrationRingtoneUrl()) {
            detail().setValue(QContactRingtone::FieldVibrationRingtoneUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl vibrationRingtoneUrl() const {return detail().value(QContactRingtone::FieldVibrationRingtoneUrl);}
signals:
    void fieldsChanged();

};
QML_DECLARE_TYPE(QDeclarativeContactRingtone)
#endif
