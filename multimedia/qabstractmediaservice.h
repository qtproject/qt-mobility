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

#ifndef QABSTRACTMEDIASERVICE_H
#define QABSTRACTMEDIASERVICE_H

#include "qmediaendpointinterface.h"
#include "qabstractmediacontrol.h"

#include <QtCore/qobject.h>
#include <QtCore/qlist.h>

class QAbstractMediaServicePrivate;

class Q_MEDIA_EXPORT QAbstractMediaService : public QObject
{
    Q_OBJECT
public:
    enum MediaEndpoint
    {
        VideoInput,
        VideoOutput,
        AudioInput,
        AudioOutput,
        StreamInput,
        StreamOutput
    };

    virtual bool isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType);

    virtual void setInputStream(QIODevice* stream);
    virtual QIODevice* inputStream() const;

    virtual void setOutputStream(QIODevice* stream);
    virtual QIODevice* outputStream() const;

    virtual QString activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType);
    virtual void setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint);
    virtual QList<QString> supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const;

    virtual QAbstractMediaControl* control(const char *name) const = 0;

#ifndef QT_NO_MEMBER_TEMPLATES
    template <typename T> inline T control() const {
        if (QObject *object = control(qmediacontrol_iid<T>())) {
            return qobject_cast<T>(object);
        }
        return 0;
    }
#endif

protected:
    QAbstractMediaService(QObject* parent);
    QAbstractMediaService(QAbstractMediaServicePrivate &dd, QObject *parent);
    ~QAbstractMediaService();

    QAbstractMediaServicePrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QAbstractMediaService)
};

#endif  // QABSTRACTMEDIASERVICE_H

