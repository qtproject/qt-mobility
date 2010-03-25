/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTRINGTONE_H
#define QCONTACTRINGTONE_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactRingtone : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldAudioRingtoneUrl;
    const char* FieldVideoRingtoneUrl;
    const char* FieldVibrationRingtoneUrl;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactRingtone, "Ringtone")
    Q_DECLARE_LATIN1_CONSTANT(FieldAudioRingtoneUrl, "AudioRingtoneUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldVideoRingtoneUrl, "VideoRingtoneUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldVibrationRingtoneUrl, "VibrationRingtoneUrl");
#endif

    void setAudioRingtoneUrl(const QUrl& audioRingtoneUrl) {setValue(FieldAudioRingtoneUrl, audioRingtoneUrl);}
    QUrl audioRingtoneUrl() const {return value(FieldAudioRingtoneUrl);}

    void setVideoRingtoneUrl(const QUrl& videoRingtoneUrl) {setValue(FieldVideoRingtoneUrl, videoRingtoneUrl);}
    QUrl videoRingtoneUrl() const {return value(FieldVideoRingtoneUrl);}

    void setVibrationRingtoneUrl(const QUrl& vibrationRingtoneUrl) {setValue(FieldVibrationRingtoneUrl, vibrationRingtoneUrl);}
    QUrl vibrationRingtoneUrl() const {return value(FieldVibrationRingtoneUrl);}
};

QTM_END_NAMESPACE

#endif
