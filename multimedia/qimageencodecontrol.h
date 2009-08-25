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

#ifndef QIMAGEENCODECONTROL_H
#define QIMAGEENCODECONTROL_H

#include "qabstractmediacontrol.h"

#include <QtCore/qsize.h>

class QByteArray;
class QStringList;

class Q_MEDIA_EXPORT QImageEncodeControl : public QAbstractMediaControl
{
    Q_OBJECT
public:
    virtual ~QImageEncodeControl();

    virtual QSize resolution() const = 0;
    virtual QSize minimumResolution() const = 0;
    virtual QSize maximumResolution() const = 0;
    virtual QList<QSize> supportedResolutions() const;
    virtual void setResolution(const QSize &) = 0;

    virtual QStringList supportedImageCodecs() const = 0;
    virtual QString imageCodec() const = 0;
    virtual bool setImageCodec(const QString &codecName) = 0;

    virtual QString imageCodecDescription(const QString &codecName) const = 0;

    virtual qreal quality() const = 0;
    virtual void setQuality(qreal) = 0;

protected:
    QImageEncodeControl(QObject *parent);
};

#define QImageEncodeControl_iid "com.nokia.Qt.QImageEncodeControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QImageEncodeControl, QImageEncodeControl_iid)

#endif // QVIDEOCAPTUREPROPERTIESCONTROL_H
