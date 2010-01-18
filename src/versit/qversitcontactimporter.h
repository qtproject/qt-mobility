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


// XXX I think these should be QVersitContactImporterPropertyHandler
// and QVersitContactImporterBinaryPropertyHandler
// actually the binary stuff is just another aspect of property handling, really,
// but QVCIPH is about converting a property to detail. hrm
// QVersitContactImporterDetailHandler

// I guess we still need filesaver for other blobs?
// hrm... more thought needed :)

class QVersitContactImporterPropertyHandler
{
public:
    virtual ~QVersitContactImporterPropertyHandler() {}
    virtual bool processProperty(const QVersitProperty& property, QContact* contact) = 0;
    virtual bool processUnknownProperty(const QVersitProperty& property, QContact* contact) = 0;
};

class QVersitResourceSaver // XXX this isn't really contacts specific
{
public:
    virtual ~QVersitResourceSaver() {}
    virtual bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                              QString* location) = 0;
};

class Q_VERSIT_EXPORT QVersitContactImporter
{
public:
    QVersitContactImporter();
    ~QVersitContactImporter();

    // XXX I'm not sure about importing vs converting (same for class name)
    QList<QContact> importContacts(const QList<QVersitDocument>& documents);

    void setPropertyHandler(QVersitContactImporterPropertyHandler* importer);
    QVersitContactImporterPropertyHandler* propertyHandler() const;
    
    void setResourceSaver(QVersitResourceSaver* saver);
    QVersitResourceSaver* resourceSaver() const;

private:
    QVersitContactImporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTIMPORTER_H

