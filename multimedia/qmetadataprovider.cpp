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

#include "qabstractmediacontrol_p.h"
#include "qmetadataprovider.h"


/*!
    \class QMetadataProvider
    \ingroup multimedia

    \preliminary
    \brief Implement this class to provide metadata information about your Multimedia object.
*/

/*!
    Destroy the metadata object.
*/

QMetadataProvider::~QMetadataProvider()
{
}

/*!
    \fn bool QMetadataProvider::metadataAvailable() const

    Returns true if metadata can be be provided by the service.
*/

/*!
    \fn bool QMetadataProvider::isReadOnly() const

    Returns true if metadata elements can not be added or changed for the service.
*/

/*!
    \fn QList<QString> QMetadataProvider::availableMetadata() const

    Returns a list of the names of all the metadata elements available.
*/

/*!
    \fn QVariant QMetadataProvider::metadata(QString const &name) const

    Returns the metadata for the element named \a name.
*/

/*!
    \fn void QMetadataProvider::setMetadata(QString const &name, QVariant const &value)

    Change the value of the metadata element named \a name, to \a value.
*/

/*!
    \fn void QMetadataProvider::metadataAvailabilityChanged(bool metadataAvailable)

    Signal the availability of metadata has changed, \a metadataAvailable will
    be true if the multimedia object has metadata.
*/

/*!
    \fn void QMetadataProvider::readOnlyChanged(bool readOnly)

    Signal a change in the read only status of meta data, \a readOnly will be
    true if metadata elements can not be added or adjusted.
*/

/*!
    Construct a QMetadataProvider with \a parent. This class is meant as s base class
    for service specific metadata providers so this contructor is protected.
*/

QMetadataProvider::QMetadataProvider(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}

