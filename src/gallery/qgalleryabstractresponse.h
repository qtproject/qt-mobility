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

#ifndef QGALLERYABSTRACTRESPONSE_H
#define QGALLERYABSTRACTRESPONSE_H

#include "qgalleryabstractrequest.h"
#include "qgalleryitemlist.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

class QGalleryAbstractResponsePrivate;

class Q_GALLERY_EXPORT QGalleryAbstractResponse : public QGalleryItemList
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryAbstractResponse)
public:
    QGalleryAbstractResponse(QObject *parent = 0);
    ~QGalleryAbstractResponse();

    int result() const;
    bool isIdle() const;

    virtual void cancel();

    virtual bool waitForFinished(int msecs) = 0;

Q_SIGNALS:
    void finished();
    void progressChanged(int current, int maximum);

protected:
    QGalleryAbstractResponse(QGalleryAbstractResponsePrivate &dd, QObject *parent);

    void finish(int result, bool idle = false);
};

QTM_END_NAMESPACE

#endif
