/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIAPLAYLISTIOPLUGIN_H
#define QMEDIAPLAYLISTIOPLUGIN_H

#include <QtCore/qobject.h>
#include <QtCore/qplugin.h>
#include <QtCore/qfactoryinterface.h>

#include <qmobilityglobal.h>

#include "qmediacontent.h"

QT_BEGIN_NAMESPACE
class QString;
class QUrl;
class QByteArray;
class QIODevice;
class QStringList;

class Q_MEDIA_EXPORT QMediaPlaylistReader
{
public:
    virtual ~QMediaPlaylistReader();

    virtual bool atEnd() const = 0;
    virtual QMediaContent readItem() = 0;
    virtual void close() = 0;
};

class Q_MEDIA_EXPORT QMediaPlaylistWriter
{
public:
    virtual ~QMediaPlaylistWriter();

    virtual bool writeItem(const QMediaContent &content) = 0;
    virtual void close() = 0;
};

struct Q_MEDIA_EXPORT QMediaPlaylistIOInterface : public QFactoryInterface
{
    virtual bool canRead(QIODevice *device, const QByteArray &format = QByteArray() ) const = 0;
    virtual bool canRead(const QUrl& location, const QByteArray &format = QByteArray()) const = 0;

    virtual bool canWrite(QIODevice *device, const QByteArray &format) const = 0;

    virtual QMediaPlaylistReader *createReader(QIODevice *device, const QByteArray &format = QByteArray()) = 0;
    virtual QMediaPlaylistReader *createReader(const QUrl& location, const QByteArray &format = QByteArray()) = 0;

    virtual QMediaPlaylistWriter *createWriter(QIODevice *device, const QByteArray &format) = 0;
};

#define QMediaPlaylistIOInterface_iid "com.nokia.Qt.QMediaPlaylistIOInterface"
Q_DECLARE_INTERFACE(QMediaPlaylistIOInterface, QMediaPlaylistIOInterface_iid);

class Q_MEDIA_EXPORT QMediaPlaylistIOPlugin : public QObject, public QMediaPlaylistIOInterface
{
Q_OBJECT
Q_INTERFACES(QMediaPlaylistIOInterface:QFactoryInterface)
public:
    explicit QMediaPlaylistIOPlugin(QObject *parent = 0);
    virtual ~QMediaPlaylistIOPlugin();

    virtual bool canRead(QIODevice *device, const QByteArray &format = QByteArray() ) const = 0;
    virtual bool canRead(const QUrl& location, const QByteArray &format = QByteArray()) const = 0;

    virtual bool canWrite(QIODevice *device, const QByteArray &format) const = 0;

    virtual QStringList keys() const = 0;

    virtual QMediaPlaylistReader *createReader(QIODevice *device, const QByteArray &format = QByteArray()) = 0;
    virtual QMediaPlaylistReader *createReader(const QUrl& location, const QByteArray &format = QByteArray()) = 0;

    virtual QMediaPlaylistWriter *createWriter(QIODevice *device, const QByteArray &format) = 0;
};

QT_END_NAMESPACE

#endif // QMEDIAPLAYLISTIOPLUGIN_H
