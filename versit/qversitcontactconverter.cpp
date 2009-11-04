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

/*!
 * \class QVersitContactConverter
 *
 * \brief The QVersitContactConverter class converts the contact into versit document.
 *
 * A QVersitContactConverter process contact detail and generates the corresponding
 * versit document.
 *
 * \sa QContact, QVersitDocument, QVersitProperty
 */

#include "qversitcontactconverter.h"
#include "qversitcontactconverter_p.h"
#include <qcontact.h>
#include <qcontactdetail.h>

/*!
 * Constructs a new contact converter
 */
QVersitContactConverter::QVersitContactConverter()
    : d(new QVersitContactConverterPrivate())
{
    connect(d, SIGNAL(scale(const QString&,QByteArray&)),
            this, SIGNAL(scale(const QString&,QByteArray&)));
}

/*!
 * Frees any memory in use by this contact converter
 */
QVersitContactConverter::~QVersitContactConverter()
{
}

/*!
 * Returns the versit document corresponding to the \a contact 
 */
QVersitDocument QVersitContactConverter::convertContact(
    const QContact& contact,
    QVersitDocument::VersitType versitType)
{
    QVersitDocument versitDocument;
    versitDocument.setVersitType(versitType);
    QList<QContactDetail> allDetails = contact.details();
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);
        d->encodeFieldInfo(versitDocument,detail);
    }
    return versitDocument;
}
