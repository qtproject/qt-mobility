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

#include "qversitresourcehandler.h"
#include "qversitproperty.h"
#include "qversitdefaultresourcehandler_p.h"
#include "qversitdefs_p.h"
#include <QFile>

QTM_USE_NAMESPACE

/*!
  \class QVersitResourceHandler
  \preliminary
  \brief The QVersitResourceHandler class is an interface for clients wishing to implement custom
  behaviour for loading and saving files to disk when exporting and importing.
  \ingroup versit

  \sa QVersitContactImporter
  \sa QVersitContactExporter
  \sa QVersitDefaultResourceHandler
 */

/*!
  \fn virtual QVersitResourceHandler::~QVersitResourceHandler()
  Frees any memory used by the handler.
 */

/*!
  \fn virtual bool QVersitResourceHandler::saveResource(const QByteArray& contents, const QVersitProperty& property, QString* location) = 0;
  Saves the binary data \a contents to a file on a persistent storage medium.
 
  \a property holds the QVersitProperty which is the context in which the binary is coming from.
  The QVersitResourceHandler can use this, for example, to determine file extension it should choose.
  *\a location is filled with the contents of the file.
  Returns true on success, false on failure.
 */

/*!
 \fn virtual bool QVersitResourceHandler::loadResource(const QString& location, QByteArray* contents, QString* mimeType) = 0
 Loads a file from \a location.
 *\a contents is filled with the contents of the file and *\a mimeType is set to the MIME
 type that it is determined to be.
 Returns true on success, false on failure.
*/

/*!
  \class QVersitDefaultResourceHandler
 
  \brief The QVersitDefaultResourceHandler class provides a default implementation of a Versit
  resource handler.
 
  An example resource handler implementation:
  \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Resource handler
  \ingroup versit
 
  \sa QVersitContactImporter, QVersitContactExporter, QVersitResourceHandler
 */

/*!
  Constructs a QVersitDefaultResourceHandler.
 */
QVersitDefaultResourceHandler::QVersitDefaultResourceHandler()
    : d(new QVersitDefaultResourceHandlerPrivate)
{
    // File extension mappings
    int fileExtensionCount = sizeof(versitFileExtensionMappings)/sizeof(VersitMapping);
    for (int i = 0; i < fileExtensionCount; i++) {
        d->mFileExtensionMapping.insert(
            QLatin1String(versitFileExtensionMappings[i].contactString),
            QLatin1String(versitFileExtensionMappings[i].versitString));
    }
}

/*!
  Frees any memory used by the resource handler.
 */
QVersitDefaultResourceHandler::~QVersitDefaultResourceHandler()
{
    delete d;
}

/*!
  Default resource loader.
  Loads file from given \a location into \a contents and returns true if successful.
  Sets the \a mimeType based on the file extension.
 */
bool QVersitDefaultResourceHandler::loadResource(const QString& location,
                                                 QByteArray* contents,
                                                 QString* mimeType)
{
    QString extension = location.split(QLatin1Char('.')).last().toLower();
    *mimeType = d->mFileExtensionMapping.value(extension);
    if (location.isEmpty())
        return false;
    QFile file(location);
    file.open(QIODevice::ReadOnly);
    if (!file.isReadable())
        return false;
    *contents = file.readAll();
    return contents->size() > 0;
}

/*!
  Default resource saver.
  Does nothing and returns false, ignoring \a contents, \a property and \a location.  By default,
  resources aren't persisted because we don't know when it is safe to remove them.
 */
bool QVersitDefaultResourceHandler::saveResource(const QByteArray& contents,
                                                 const QVersitProperty& property,
                                                 QString* location)
{
    Q_UNUSED(contents)
    Q_UNUSED(property)
    Q_UNUSED(location)
    return false;
}
