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

#ifndef QIMAGEMEDIAOUTPUT_H
#define QIMAGEMEDIAOUTPUT_H

#include <QtCore/qplugin.h>
#include <QtCore/qsize.h>
#include <QtGui/qimage.h>

class QImageMediaOutputInterface
{
public:
    virtual ~QImageMediaOutputInterface();
};

#define QImageMediaOutputInterface_iid "com.nokia.Qt.QImageMediaOutputInterface/1.0"

Q_DECLARE_INTERFACE(QImageMediaOutputInterface, QImageMediaOutputInterface_iid)

class QImage;

class QImageMediaOutputPrivate;

class QImageMediaOutput : public QObject, public QImageMediaOutputInterface
{
    Q_OBJECT
    Q_INTERFACES(QImageMediaOutputInterface)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool hasImage READ hasImage)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_DECLARE_PRIVATE(QImageMediaOutput)
public:
    enum State
    {
        NoImage,
        PartialImage,
        CompleteImage
    };

    QImageMediaOutput(QObject *parent = 0);
    ~QImageMediaOutput();

    QImage image() const;
    State state() const;

    bool hasImage() const;

    QSize maximumSize() const;
    virtual void setMaximumSize(const QSize &size);

Q_SIGNALS:
    void imageChanged(const QImage &image);
    void stateChanged(QImageMediaOutput::State state);

protected:
    void setImage(const QImage &image, State state);
};

#endif
