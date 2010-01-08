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

#ifndef QVERSITCONTACTEXPORTER_H
#define QVERSITCONTACTEXPORTER_H

#include "qmobilityglobal.h"
#include "qversitdocument.h"

#include <qcontact.h>

#include <QObject>
#include <QImage>

QTM_BEGIN_NAMESPACE

class QVersitContactExporterPrivate;

class QVersitContactDetailExporter
{
protected:
    /*!
     * Converts \a detail into a QVersitProperty.
     *
     * \a *ok is set to true on success, false on failure.
     *
     * This function is called on every QContactDetail encountered during an export.  Supply this
     * function and set *ok to true to implement custom export behaviour.
     */
    virtual QVersitProperty processDetail(const QContactDetail& detail, bool* ok) = 0;
     
    /*!
     * Converts \a detail into a QVersitProperty.
     *
     * \a *ok is set to true on success, false on failure.
     *
     * This function is called on every QContactDetail encountered during an export which is not
     * handled by either \l processDetail() or by QVersitContactExporter.  This can be used to
     * implement support for QContactDetails not supported by QVersitContactExporter.
     *
     * Note that QVersitContactExporter doesn't support avatars or audio clips and this function
     * must be supplied to support these.
     *
     * An example for processing avatars:
     * QVersitProperty CustomExporter::processUnknownDetail(const QContactDetail& detail, bool* ok)
     * {
     *     if (detail.definitionName() == QContactAvatar::DefinitionName) {
     *         QContactAvatar avatar = static_cast<QContactAvatar>(detail);
     *         // Process avatar and return a corresponding QVersitProperty
     *     } else {
     *         *ok = false;
     *         return QVersitProperty();
     *     }
     * }
     *
     */
    virtual QVersitProperty processUnknownDetail(const QContactDetail& detail, bool* ok) = 0;
};

class Q_VERSIT_EXPORT QVersitContactExporter : public QObject
{
    Q_OBJECT

public:
    QVersitContactExporter();
    virtual ~QVersitContactExporter();

    QList<QVersitDocument> exportContacts(
        const QList<QContact>& contacts,
        QVersitDocument::VersitType versitType=QVersitDocument::VCard21);

    void setDetailExporter(QVersitContactDetailExporter* exporter);
    QVersitContactDetailExporter* detailExporter() const;

private:
    QVersitContactExporterPrivate* d;    
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTEXPORTER_H
