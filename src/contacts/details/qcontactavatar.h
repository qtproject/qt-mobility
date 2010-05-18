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

#ifndef QCONTACTAVATAR_H
#define QCONTACTAVATAR_H

#include <QString>
#include <QUrl>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactAvatar : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldImageUrl;
    static const QLatin1Constant FieldVideoUrl;

    // deprecated keys:
    static const QLatin1Constant FieldAvatar;
    static const QLatin1Constant FieldAvatarPixmap;
    static const QLatin1Constant FieldSubType;
    static const QLatin1Constant SubTypeImage;
    static const QLatin1Constant SubTypeVideo;
    static const QLatin1Constant SubTypeAudioRingtone;
    static const QLatin1Constant SubTypeVideoRingtone;
    static const QLatin1Constant SubTypeTexturedMesh;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactAvatar, "Avatar")
    Q_DECLARE_LATIN1_CONSTANT(FieldImageUrl, "ImageUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldVideoUrl, "VideoUrl");
    // MeshUri, VibetoneUri, Audio(theme)Uri, ...?

    // deprecated keys:
    Q_DECLARE_LATIN1_CONSTANT(FieldAvatar, "Avatar");
    Q_DECLARE_LATIN1_CONSTANT(FieldAvatarPixmap, "AvatarPixmap");
    Q_DECLARE_LATIN1_CONSTANT(FieldSubType, "SubType");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeImage, "Image");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeVideo, "Video");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeAudioRingtone, "AudioRingtone");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeVideoRingtone, "VideoRingtone");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeTexturedMesh, "TexturedMesh");
#endif

    void setImageUrl(const QUrl& imageUrl) {setValue(FieldImageUrl, imageUrl);}
    QUrl imageUrl() const {return value<QUrl>(FieldImageUrl);}

    void setVideoUrl(const QUrl& videoUrl) {setValue(FieldVideoUrl, videoUrl);}
    QUrl videoUrl() const {return value<QUrl>(FieldVideoUrl);}


    // old, deprecated API: to be removed after the transition period has elapsed.
    QString Q_DECL_DEPRECATED avatar() const;
    bool Q_DECL_DEPRECATED setAvatar(const QString& avatar);
    QPixmap Q_DECL_DEPRECATED pixmap() const;
    bool Q_DECL_DEPRECATED setPixmap(const QPixmap& pixmap);
    QString Q_DECL_DEPRECATED subType() const;
    void Q_DECL_DEPRECATED setSubType(const QString& subtype);
};

QTM_END_NAMESPACE

#endif
