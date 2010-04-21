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

#include "qmaptilereply.h"
#include "qmaptilereply_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMapTileReply
    \brief The QMapTileReply class represents the response from a mapping service.
    \ingroup location

    This class represents the response from a mapping service.
    It also takes responsibility for any errors that happen while the request is
    in submitted to and processed by the service.
*/

/*!
    \enum ErrorCode

    \value NoError
        No error has occurred.
    \value NetworkError
        A networking error occurred.
    \value NoContentError
        The reply contained no content.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    Constructs a QMapTileReply from a QMapTileRequest \a request, and with parent \a parent.
*/
QMapTileReply::QMapTileReply(QObject *parent)
        : QObject(parent), d_ptr(new QMapTileReplyPrivate())
{
}

/*!
    Destructor.
*/
QMapTileReply::~QMapTileReply()
{
    Q_D(QMapTileReply);
    delete d;
}

/*!
    Returns the requested map data.
*/
QByteArray QMapTileReply::data() const
{
    Q_D(const QMapTileReply);
    return d->data;
}

/*!
    Sets the requested map data to \a data.
*/
void QMapTileReply::setData(const QByteArray &data)
{
    Q_D(QMapTileReply);
    d->data = data;
}

/*!
    Causes this QMapTileReply to emit finished() if it has been configured properly
    or error() if there is a problem with the configuration.
*/
void QMapTileReply::done()
{
    Q_D(QMapTileReply);
    if (d->data.size() > 0) {
        emit finished();
    } else {
        emit error(QMapTileReply::NoContentError, "The reply to the routing request was empty.");
    }
}

quint32 QMapTileReply::level() const
{
    Q_D(const QMapTileReply);
    return d->level;
}

void QMapTileReply::setLevel(quint32 level)
{
    Q_D(QMapTileReply);
    d->level = level;
}

quint32 QMapTileReply::row() const
{
    Q_D(const QMapTileReply);
    return d->row;
}

void QMapTileReply::setRow(quint32 row)
{
    Q_D(QMapTileReply);
    d->row = row;
}

quint32 QMapTileReply::col() const
{
    Q_D(const QMapTileReply);
    return d->col;
}

void QMapTileReply::setCol(quint32 col)
{
    Q_D(QMapTileReply);
    d->col = col;
}

/*!
    \fn void QMapTileReply::cancel()

    Cancels the receiving of this reply if the reply hasn't been received already.
*/

/*!
    \fn void QMapTileReply::finished()

    Indicates that the reply has been received and processed without error, and is ready to be used.
*/
/*!
    \fn void QMapTileReply::error(QMapTileReply::ErrorCode errorCode, const QString &errorString = QString())

    Indicates that an error occurred during the receiving or processing of the reply.
*/

/*******************************************************************************
*******************************************************************************/

QMapTileReplyPrivate::QMapTileReplyPrivate() {}

#include "moc_qmaptilereply.cpp"

QTM_END_NAMESPACE
