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

#ifndef QCAMERAIMAGECAPTURE_H
#define QCAMERAIMAGECAPTURE_H

#include <qmediaobject.h>
#include <qmediaencodersettings.h>
#include <qmediabindableinterface.h>

QT_BEGIN_NAMESPACE
class QSize;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QImageEncoderSettings;

class QCameraImageCapturePrivate;
class Q_MULTIMEDIA_EXPORT QCameraImageCapture : public QObject, public QMediaBindableInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaBindableInterface)
    Q_ENUMS(Error)
    Q_PROPERTY(bool readyForCapture READ isReadyForCapture NOTIFY readyForCaptureChanged)
public:
    enum Error
    {
        NoError,
        NotReadyError,
        ResourceError,
        OutOfSpaceError,
        NotSupportedFeatureError,
        FormatError
    };

#ifndef Q_DOC
    enum DriveMode
    {
        SingleImageCapture
    };
#endif

    QCameraImageCapture(QMediaObject *mediaObject, QObject *parent = 0);
    ~QCameraImageCapture();

    bool isAvailable() const;
    QtMultimediaKit::AvailabilityError availabilityError() const;

    QMediaObject *mediaObject() const;

    Error error() const;
    QString errorString() const;

    bool isReadyForCapture() const;

    QStringList supportedImageCodecs() const;
    QString imageCodecDescription(const QString &codecName) const;

    QList<QSize> supportedResolutions(const QImageEncoderSettings &settings = QImageEncoderSettings(),
                                      bool *continuous = 0) const;

    QImageEncoderSettings encodingSettings() const;
    void setEncodingSettings(const QImageEncoderSettings& settings);

public Q_SLOTS:
    int capture(const QString &location = QString());
    void cancelCapture();

Q_SIGNALS:
    void error(QCameraImageCapture::Error error);

    void readyForCaptureChanged(bool);

    void imageExposed(int id);
    void imageCaptured(int id, const QImage &preview);
    void imageSaved(int id, const QString &fileName);

protected:
    bool setMediaObject(QMediaObject *);

    QCameraImageCapturePrivate *d_ptr;
private:
    Q_DISABLE_COPY(QCameraImageCapture)
    Q_DECLARE_PRIVATE(QCameraImageCapture)
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &))
    Q_PRIVATE_SLOT(d_func(), void _q_readyChanged(bool))
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QCameraImageCapture::Error)

#endif

