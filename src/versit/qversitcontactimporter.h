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
#include "qversitresourcehandler.h"
#include "qversitproperty.h" // XXX: make this a forward declaration when removing deprecated functions

#include <qcontact.h>

#include <QList>

QTM_BEGIN_NAMESPACE

class QVersitDocument;
class QVersitContactImporterPrivate;

class Q_VERSIT_EXPORT QVersitContactImporterPropertyHandler
{
public:
    virtual ~QVersitContactImporterPropertyHandler() {}
    virtual bool preProcessProperty(const QVersitDocument& document,
                                    const QVersitProperty& property,
                                    int contactIndex,
                                    QContact* contact) = 0;
    virtual bool postProcessProperty(const QVersitDocument& document,
                                     const QVersitProperty& property,
                                     bool alreadyProcessed,
                                     int contactIndex,
                                     QContact* contact) = 0;
};

class Q_VERSIT_EXPORT QVersitContactImporter
{
public:
    QVersitContactImporter();
    ~QVersitContactImporter();

    // XXX We need some way of importing/exporting groups and "self-contact" from vCard.
    QList<QContact> importContacts(const QList<QVersitDocument>& documents);

    void setPropertyHandler(QVersitContactImporterPropertyHandler* handler);
    QVersitContactImporterPropertyHandler* propertyHandler() const;
    
    void setResourceHandler(QVersitResourceHandler* handler);
    QVersitResourceHandler* resourceHandler() const;


    // Deprecated
    void Q_DECL_DEPRECATED setImagePath(const QString& path)
    {
        qWarning("QVersitContactImporter::setImagePath(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  A QVersitResourceHandler should be used to handle images.");
        Q_UNUSED(path)
    }
    QString Q_DECL_DEPRECATED imagePath() const
    {
        qWarning("QVersitContactImporter::imagePath(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  A QVersitResourceHandler should be used to handle images.");
        return QString();
    }

    void Q_DECL_DEPRECATED setAudioClipPath(const QString& path)
    {
        qWarning("QVersitContactImporter::setAudioClipPath(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  A QVersitResourceHandler should be used to handle audio.");
        Q_UNUSED(path)
    }
    QString Q_DECL_DEPRECATED audioClipPath() const
    {
        qWarning("QVersitContactImporter::audioClipPath(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  A QVersitResourceHandler should be used to handle audio.");
        return QString();
    }

    QContact Q_DECL_DEPRECATED importContact(const QVersitDocument& versitDocument)
    {
        qWarning("QVersitContactImporter::importContact(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  importContacts() should be used instead.");
        QList<QVersitDocument> list;
        list.append(versitDocument);
        return importContacts(list).first();
    }

    QList<QVersitProperty> Q_DECL_DEPRECATED unknownVersitProperties()
    {
        qWarning("QVersitContactImporter::unknownVersitProperties(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  A QVersitContactImporterPropertyHandler should be used to discover and handle unknown details.");
        return QList<QVersitProperty>();
    }

private:
    QVersitContactImporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTIMPORTER_H

