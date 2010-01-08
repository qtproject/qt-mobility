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

#ifndef QVERSITCONTACTIMPORTER_H
#define QVERSITCONTACTIMPORTER_H

#include "qmobilityglobal.h"

#include <qcontact.h>

#include <QList>

QTM_BEGIN_NAMESPACE

class QVersitDocument;
class QVersitProperty;
class QVersitContactImporterPrivate;


class QVersitContactPropertyImporter
{
protected:
    /*!
     * Converts \a property into a QContactDetail.
     *
     * \a *ok is set to true on success, false on failure.
     *
     * This function is called on every QVersitProperty encountered during an import.  Supply this
     * function and set *ok to true to implement custom import behaviour.
     */
    virtual QContactDetail processProperty(const QVersitProperty& property, bool* ok);

    /*!
     * Converts \a property into a QContactDetail.
     *
     * \a *ok is set to true on success, false on failure.
     *
     * This function is called on every QVersitProperty encountered during an import which is not
     * handled by either \l processProperty() or by QVersitContactImporter.  Supply this
     * function and set *ok to true to implement support for QVersitProperties not supported by
     * QVersitContactImporter.
     *
     * Note that QVersitContactImporter doesn't support images or audio clips and this function
     * must be supplied to support these.
     *
     * An example for processing avatars:
     * QContactDetail CustomImporter::processProperty(const QVersitProperty& property, bool* ok)
     * {
     *     if (property.name() == QLatin1String("IMAGE")) {
     *         QVariant value = property.value();
     *         switch (value.type) {
     *         case QMetaType::QString:
     *             QString url = value.toString();
     *             // Interpret the value as a URL and return the converted QContactDetail
     *             // In reality, you probably want to check the property parameters to tell if
     *             // it's a URL or a base-64 encoded binary.
     *         case QMetaType::QImage:
     *             QImage image = value.value<QImage>();
     *             // Return the converted QContactDetail
     *         }
     *     } else {
     *         *ok = false;
     *         return QContactDetail();
     *     }
     * }
     *
     */
    virtual QContactDetail processUnknownProperty(const QVersitProperty& property, bool* ok);
};

class Q_VERSIT_EXPORT QVersitContactImporter
{
public:
    QVersitContactImporter();
    virtual ~QVersitContactImporter();

    QList<QContact> importContacts(const QList<QVersitDocument>& versitDocuments);

    void setPropertyImporter(QVersitContactPropertyImporter* importer);
    QVersitContactPropertyImporter* propertyImporter();
    
private:
    QVersitContactImporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTIMPORTER_H

