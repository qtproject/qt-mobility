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

#ifndef QGALLERYABSTRACTREQUEST_H
#define QGALLERYABSTRACTREQUEST_H

#include <qmobilityglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

class QAbstractGallery;
class QGalleryAbstractResponse;

class QGalleryAbstractRequestPrivate;

class Q_GALLERY_EXPORT QGalleryAbstractRequest : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryAbstractRequest)
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery)
    Q_PROPERTY(bool supported READ isSupported NOTIFY supportedChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int result READ result NOTIFY resultChanged)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)
    Q_PROPERTY(int maximumProgress READ maximumProgress NOTIFY progressChanged)
    Q_ENUMS(State)
    Q_ENUMS(Result)
    Q_ENUMS(Type)
public:
    enum State
    {
        Inactive,
        Active,
        Cancelling,
        Idle
    };

    enum Result
    {
        NoResult,
        Succeeded,
        Cancelled,
        NoGallery,
        NotSupported,
        ConnectionError,
        InvalidItemError,
        ItemTypeError,
        InvalidPropertyError,
        PropertyTypeError,
        UnsupportedFilterTypeError,
        UnsupportedFilterOptionError,
        PermissionsError,
        InvalidDestinationError,
        InvalidUrlError,
        RequestError = 100
    };

    enum Type
    {
        Item,
        Url,
        Container,
        Filter,
        Count,
        Remove
    };

    explicit QGalleryAbstractRequest(Type type, QObject *parent = 0);
    explicit QGalleryAbstractRequest(QAbstractGallery *gallery, Type type, QObject *parent = 0);
    ~QGalleryAbstractRequest();

    QAbstractGallery *gallery() const;
    void setGallery(QAbstractGallery *gallery);

    bool isSupported() const;

    Type type() const;
    State state() const;
    int result() const;

    int currentProgress() const;
    int maximumProgress() const;

    bool waitForFinished(int msecs);

public Q_SLOTS:
    void execute();
    void cancel();
    void clear();

Q_SIGNALS:
    void supportedChanged();
    void succeeded();
    void cancelled();
    void failed(int result);
    void finished(int result);
    void stateChanged(QGalleryAbstractRequest::State state);
    void resultChanged();
    void progressChanged(int current, int maximum);

protected:
    virtual void setResponse(QGalleryAbstractResponse *response) = 0;

    explicit QGalleryAbstractRequest(QGalleryAbstractRequestPrivate &dd, QObject *parent);

    QGalleryAbstractRequestPrivate *d_ptr;

private:
    Q_PRIVATE_SLOT(d_ptr, void _q_finished())
    Q_PRIVATE_SLOT(d_ptr, void _q_galleryDestroyed())
    Q_PRIVATE_SLOT(d_ptr, void _q_progressChanged(int, int))
};

QTM_END_NAMESPACE

#endif
