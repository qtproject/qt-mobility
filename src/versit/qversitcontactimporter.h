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

#ifndef QVERSITCONTACTIMPORTER_H
#define QVERSITCONTACTIMPORTER_H

#include "qmobilityglobal.h"
#include "qversitresourcehandler.h"

#include <qcontact.h>

#include <QList>

QTM_BEGIN_NAMESPACE

class QVersitDocument;
class QVersitContactImporterPrivate;
class QVersitProperty;

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

class Q_VERSIT_EXPORT QVersitContactImporterPropertyHandlerV2
{
public:
    virtual ~QVersitContactImporterPropertyHandlerV2() {}
    virtual bool afterProcessProperty(const QVersitDocument& document,
                                      const QVersitProperty& property,
                                      bool alreadyProcessed,
                                      const QContact& contact,
                                      QList<QContactDetail>* updatedDetails) = 0;
    virtual void endDocument(const QVersitDocument& document,
                             QContact* contact) = 0;
    virtual int version() { return 2; }
};

class Q_VERSIT_EXPORT QVersitContactImporter
{
public:
    enum Error {
        NoError = 0,
        InvalidDocumentError,
        EmptyDocumentError
    };

    QVersitContactImporter();
    ~QVersitContactImporter();

    bool importDocuments(const QList<QVersitDocument>& documents);
    QList<QContact> contacts() const;
    QMap<int, Error> errors() const;

    void setPropertyHandler(QVersitContactImporterPropertyHandler* handler);
    void setPropertyHandler(QVersitContactImporterPropertyHandlerV2* handler);

    /* deprecated and internal */
    QVersitContactImporterPropertyHandler* propertyHandler() const;

    void setResourceHandler(QVersitResourceHandler* handler);
    QVersitResourceHandler* resourceHandler() const;

    QList<QContact> Q_DECL_DEPRECATED importContacts(const QList<QVersitDocument>& documents);

private:
    QVersitContactImporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTIMPORTER_H

