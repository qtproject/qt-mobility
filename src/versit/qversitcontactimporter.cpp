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

#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

QTM_USE_NAMESPACE

/*!
 * \class QVersitContactImporterPropertyHandler
 *
 * \brief The QVersitContactImporterPropertyHandler class is an interface for clients wishing to implement
 * custom import behaviour for certain versit properties
 *
 * \ingroup versit
 *
 * \sa QVersitContactImporter
 */

/*!
 * \fn virtual bool preProcessProperty(const QVersitProperty& property, QContact* contact) = 0;
 * Process \a property and update \a contact with the corresponding QContactDetail(s).
 *
 * Returns true on success, false on failure.
 *
 * This function is called on every QVersitProperty encountered during an import.  Supply this
 * function and return true to implement custom import behaviour.
 */

/*!
 * \fn virtual bool processUnknownProperty(const QVersitProperty& property, QContact* contact) = 0;
 * Process \a property and update \a contact with the corresponding QContactDetail(s).
 *
 * Returns true on success, false on failure.
 *
 * This function is called on every QVersitProperty encountered during an import which is not
 * handled by either \l preProcessProperty() or by QVersitContactImporter.  Supply this
 * function and return true to implement support for QVersitProperties not supported by
 * QVersitContactImporter.
 */

/*!
 * \class QVersitResourceSaver
 *
 * \brief The QVersitResourceSaver class is an interface for clients wishing to implement file
 * saving to disk when importing.
 *
 * \ingroup versit
 *
 * \sa QVersitContactImporter
 *
 * \fn virtual bool saveFile(const QByteArray& contents, const QVersitProperty& property, QString* filename) = 0;
 * Saves the binary data \a contents to a file on a persistent storage medium.
 *
 * \a property holds the QVersitProperty which is the context in which the binary is coming from.
 * The QVersitResourceSaver can use this, for example, to determine file extension it should choose.
 * \a *filename is filled with the contents of the file.
 * Returns true on success, false on failure.
 */

/*!
 * \class QVersitContactImporter
 *
 * \brief The QVersitContactImporter class creates QContacts from QVersitDocuments.
 *
 * \ingroup versit
 *
 * By associating a QVersitContactImporterPropertyHandler with the importer using setPropertyHandler(), the
 * client can pass in a handler to override the processing of properties and/or handle properties
 * that QVersitContactImporter doesn't support.
 *
 * \code
 *
 * class MyPropertyHandler : public QVersitContactImporterPropertyHandler {
 * public:
 *    bool preProcessProperty(const QVersitProperty& property, QContact* contact) {
 *        return false;
 *    }
 *    bool processUnknownProperty(const QVersitProperty& property, QContact* contact) {
 *        mUnknownProperties.append(property);
 *        return false;
 *    }
 *    QList<QVersitProperty> mUnknownProperties;
 * };
 *
 * class MyResourceSaver : public QVersitResourceSaver {
 * public:
 *    bool saveResource(const QByteArray& contents, const QVersitProperty& property,
 *                      QString* location) {
 *        *location = QString::number(qrand());
 *        QFile file(*location);
 *        file.open(QIODevice::WriteOnly);
 *        file.write(contents);
 *    }
 * }
 *
 * MyPropertyHandler propertyHandler;
 * importer.setPropertyHandler(propertyHandler);
 * MyResourceSaver resourceSaver;
 * importer.setResourceSaver(resourceSaver);
 *
 * QVersitDocument document;
 *
 * QVersitProperty property;
 * property.setName(QString::fromAscii("N"));
 * property.setValue("Citizen;John;Q;;");
 * document.addProperty(property);
 *
 * property.setName(QString::fromAscii("X-UNKNOWN-PROPERTY"));
 * property.setValue("some value");
 * document.addProperty(property);
 *
 * QList<QVersitDocument> list;
 * list.append(document);
 *
 * QList<QContact> contactList = importer.importContacts(list);
 * // contactList.first() now contains the "N" property as a QContactName
 * // propertyHandler.mUnknownProperties contains the list of unknown properties
 *
 * \endcode
 *
 * \sa QVersitDocument, QVersitReader, QVersitContactImporterPropertyHandler
 */

/*! Constructs a new importer */
QVersitContactImporter::QVersitContactImporter() 
    : d(new QVersitContactImporterPrivate)
{
}

/*! Frees the memory used by the importer */
QVersitContactImporter::~QVersitContactImporter()
{
    delete d;
}

/*!
 * Creates a QContact from \a versitDocument.
 */
QList<QContact> QVersitContactImporter::importContacts(const QList<QVersitDocument>& documents)
{
    QList<QContact> list;
    foreach (QVersitDocument document, documents) {
        list.append(d->importContact(document));
    }

    return list;
}

/*!
 * Sets \a importer to be the handler for processing QVersitProperties
 */
void QVersitContactImporter::setPropertyHandler(QVersitContactImporterPropertyHandler* importer)
{
    d->mPropertyHandler = importer;
}

/*!
 * Gets the handler for processing QVersitProperties
 */
QVersitContactImporterPropertyHandler* QVersitContactImporter::propertyHandler() const
{
    return d->mPropertyHandler;
}

/*!
 * Sets \a saver to be the handler to save files with.
 */
void QVersitContactImporter::setResourceSaver(QVersitResourceSaver* saver)
{
    d->mResourceSaver = saver;
}

/*!
 * Returns the file saver.
 */
QVersitResourceSaver* QVersitContactImporter::resourceSaver() const
{
    return d->mResourceSaver;
}
