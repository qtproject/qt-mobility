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
    Q_PROPERTY(QObject* audioOutput READ audioOutput WRITE setAudioOutput)
    Q_PROPERTY(QObject* videoOutput READ videoOutput WRITE setVideoOutput)
    Q_PROPERTY(QObject* dataOutput READ dataOutput WRITE setDataOutput)
    Q_PROPERTY(QObject* audioInput READ audioInput WRITE setAudioInput)
    Q_PROPERTY(QObject* videoInput READ videoInput WRITE setVideoInput)
    Q_PROPERTY(QObject* dataInput READ dataInput WRITE setDataInput)
public:
    QObject *audioOutput() const;
    virtual void setAudioOutput(QObject *output);

    QObject *videoOutput() const;
    virtual void setVideoOutput(QObject *output);

    QObject *dataOutput() const;
    virtual void setDataOutput(QObject *output);

    QObject *audioInput() const;
    virtual void setAudioInput(QObject *input);

    QObject *videoInput() const;
    virtual void setVideoInput(QObject *input);

    QObject *dataInput() const;
    virtual void setDataInput(QObject *input);

    virtual QList<QByteArray> supportedEndpointInterfaces(
            QMediaEndpointInterface::Direction direction) const;

    virtual QObject *createEndpoint(const char *interface);

    virtual QAbstractMediaControl* control(const char *name) const = 0;

#ifndef QT_NO_MEMBER_TEMPLATES
    template <typename T> inline T control() {
        if (QObject *object = control(qmediacontrol_iid<T>())) {
            if (T control = qobject_cast<T>(object))
                return control;
            delete object;
        }
        return 0;
    }

    template <typename T> inline T createEndpoint() {
        if (QObject *object = createEndpoint(qmediaendpoint_iid<T>())) {
            if (T endpoint = qobject_cast<T>(object))
                return endpoint;
            delete object;
        }
        return 0;
    }
#endif

protected:
    QAbstractMediaService(QObject* parent);

private:
    Q_DECLARE_PRIVATE(QAbstractMediaService)
};

#endif  // QABSTRACTMEDIASERVICE_H

