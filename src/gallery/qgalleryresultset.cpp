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

#include "qgalleryresultset_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGalleryResultSet

*/

/*!
*/

QGalleryResultSet::QGalleryResultSet(QObject *parent)
    : QGalleryAbstractResponse(*new QGalleryResultSetPrivate, parent)
{
}


/*!
*/

QGalleryResultSet::QGalleryResultSet(QGalleryResultSetPrivate &dd, QObject *parent)
    : QGalleryAbstractResponse(dd, parent)
{
}

/*!
*/

QGalleryResultSet::~QGalleryResultSet()
{

}

/*!
    \fn QGalleryResultSet::propertyKey(const QString &property) const

*/

/*!
    \fn QGalleryResultSet::propertyAttributes(int key) const

*/

/*!
    \fn QGalleryResultSet::propertyType(int key) const


*/

/*!
    \fn QGalleryResultSet::itemCount() const


*/

/*!
    \fn QGalleryResultSet::itemId() const

*/

/*!
    \fn QGalleryResultSet::itemUrl() const

*/

/*!
    \fn QGalleryResultSet::itemType() const

*/

/*!
    \fn QGalleryResultSet::resources() const


*/

/*!
    \fn QGalleryResultSet::metaData(int key) const

*/

/*!
    \fn QGalleryResultSet::setMetaData(int key, const QVariant &value)


*/

/*!
    \fn QGalleryResultSet::currentIndex() const

*/

/*!
    \fn QGalleryResultSet::seek(int index, bool relative)


*/

/*!
    \fn QGalleryResultSet::currentIndexChanged(int index)

*/

/*!
    \fn QGalleryResultSet::itemsInserted(int index, int count)

*/

/*!
    \fn QGalleryResultSet::itemsRemoved(int index, int count)

*/

/*!
    \fn QGalleryResultSet::itemsMoved(int from, int to, int count)


*/

/*!
    \fn QGalleryResultSet::metaDataChanged(int index, int count, const QList<int> &keys)
*/

#include "moc_qgalleryresultset.cpp"

QTM_END_NAMESPACE
