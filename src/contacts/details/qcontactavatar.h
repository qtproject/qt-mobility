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
    const char* DefinitionName;
    const char* FieldImageUrl;
    const char* FieldVideoUrl;

    // deprecated keys:
    const char* FieldAvatar;
    const char* FieldAvatarPixmap;
    const char* FieldSubType;
    const char* SubTypeImage;
    const char* SubTypeVideo;
    const char* SubTypeAudioRingtone;
    const char* SubTypeVideoRingtone;
    const char* SubTypeTexturedMesh;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactAvatar, "Avatar")
    Q_DECLARE_LATIN1_LITERAL(FieldImageUrl, "ImageUrl");
    Q_DECLARE_LATIN1_LITERAL(FieldVideoUrl, "VideoUrl");
    // MeshUri, VibetoneUri, Audio(theme)Uri, ...?

    // deprecated keys:
    Q_DECLARE_LATIN1_LITERAL(FieldAvatar, "Avatar");
    Q_DECLARE_LATIN1_LITERAL(FieldAvatarPixmap, "AvatarPixmap");
    Q_DECLARE_LATIN1_LITERAL(FieldSubType, "SubType");
    Q_DECLARE_LATIN1_LITERAL(SubTypeImage, "Image");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVideo, "Video");
    Q_DECLARE_LATIN1_LITERAL(SubTypeAudioRingtone, "AudioRingtone");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVideoRingtone, "VideoRingtone");
    Q_DECLARE_LATIN1_LITERAL(SubTypeTexturedMesh, "TexturedMesh");
#endif

    void setImageUrl(const QUrl& imageUrl) {setValue(FieldImageUrl, imageUrl);}
    QUrl imageUrl() const {return value<QUrl>(FieldImageUrl);}

    void setVideoUrl(const QUrl& videoUrl) {setValue(FieldVideoUrl, videoUrl);}
    QUrl videoUrl() const {return value<QUrl>(FieldVideoUrl);}


    // old, deprecated API: to be removed after the transition period has elapsed.
    QString Q_DECL_DEPRECATED avatar() const;
    void Q_DECL_DEPRECATED setAvatar(const QString& avatar);
    QPixmap Q_DECL_DEPRECATED pixmap() const;
    void Q_DECL_DEPRECATED setPixmap(const QPixmap& pixmap);
    QString Q_DECL_DEPRECATED subType() const;
    void Q_DECL_DEPRECATED setSubType(const QString& subtype);
};

QTM_END_NAMESPACE

#endif
