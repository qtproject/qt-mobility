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

#include "qmediasink.h"
class QMediaSinkPrivate : public QSharedData
{
public:
    QMediaSinkPrivate()
        :QSharedData() { ref.ref(); }

    QMediaSinkPrivate(const QString &mimeType, const QVariant &url)
        :QSharedData(),
         mimeType(mimeType),
         url(url)
    {}

    QMediaSinkPrivate(const QMediaSinkPrivate &other)
        :QSharedData(other),
         mimeType(other.mimeType),
         url(other.url)
    {}

    ~QMediaSinkPrivate() {}

    QString mimeType;
    QVariant url;
};

Q_GLOBAL_STATIC(QMediaSinkPrivate, qt_sharedMediaSink)

/*!
    \class QMediaSink
    \ingroup multimedia

    \preliminary
    \brief The QMediaSink class provides an media sink location.

    \sa
*/

/*!
  Construct an empty media sink object.
*/
QMediaSink::QMediaSink()
    :d(qt_sharedMediaSink())
{
}

/*!
  Construct a media sink by passing \a mimeType and \a url.
*/
QMediaSink::QMediaSink(const QVariant &url)
    :d(new QMediaSinkPrivate(QString(), url))
{
}

/*!
  Constructs a copy of \a other.
*/
QMediaSink::QMediaSink(const QMediaSink &other)
    :d(other.d)
{
}

/*!
  Assigns the specified media sink to this object.
*/
QMediaSink &QMediaSink::operator =(const QMediaSink &other)
{
    d = other.d;
    return *this;
}

/*!
  Destroys the media sink.
*/
QMediaSink::~QMediaSink()
{
}

/*!
  Returns true if this media sink is null; otherwise returns false.
*/
bool QMediaSink::isNull() const
{
    return d == qt_sharedMediaSink();
}

/*!
  Returns the mime type of this media sink if available; otherwise returns an empty string.
*/
QString QMediaSink::mimeType() const
{
    return d->mimeType;
}

/*!
  Set the mime type of this media sink to \a mimeType.
*/
void QMediaSink::setMimeType(QString const& mimeType)
{
    d->mimeType = mimeType;
}

/*!
  Returns the data location of this media sink.
*/
QVariant QMediaSink::dataLocation() const
{
    return d->url;
}

/*!
  Set the data location of this media sink to \a url.
  \sa dataLocation()
*/
void QMediaSink::setDataLocation(QVariant const& url)
{
    d->url = url;
}

/*!
  Returns true if this media sink and the given sink are equal; otherwise returns false.
*/
bool QMediaSink::operator ==(const QMediaSink& other) const
{
    return d->url == other.d->url;
}

/*!
  Returns true if this media sink and the given sink are not equal; otherwise returns false.
*/
bool QMediaSink::operator !=(const QMediaSink& other) const
{
    return d->url != other.d->url;
}
