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

#ifndef QGALLERY_H
#define QGALLERY_H

#include <qgalleryrequest.h>

class QGalleryAbstractResponse;
class QGalleryAbstractRequest;

class Q_GALLERY_EXPORT QAbstractGallery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rootDocumentId READ rootDocumentId NOTIFY rootDocumentIdChanged);
    Q_PROPERTY(QStringList supportedDocumentTypes READ supportedDocumentTypes NOTIFY supportedDocumentTypesChanged)
public:
    QAbstractGallery(QObject *parent = 0);
    ~QAbstractGallery();

    virtual bool isRequestSupported(QGalleryAbstractRequest::Type type) const = 0;
    virtual QString rootDocumentId() const = 0;
    virtual QStringList supportedDocumentTypes() const = 0;

Q_SIGNALS:
    void rootDocumentIdChanged();
    void supportedDocumentTypesChanged();

protected:
    virtual QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request) = 0;

    friend class QGalleryAbstractRequest;
};

class Q_GALLERY_EXPORT QDocumentGallery : public QAbstractGallery
{
    QDocumentGallery(QObject *parent = 0);
    ~QDocumentGallery();

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const;
    QString rootDocumentId() const;
    QStringList supportedDocumentTypes() const;

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request);
};

#endif
