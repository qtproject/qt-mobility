/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTDETAILDEFINITIONREQUEST_H
#define QCONTACTDETAILDEFINITIONREQUEST_H

#include "qcontactabstractrequest.h"

#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactdetaildefinition.h"

#include <QSharedData>
#include <QString>
#include <QList>
#include <QStringList>

class QContactDetailDefinitionRequestData;
class QTCONTACTS_EXPORT QContactDetailDefinitionRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactDetailDefinitionRequest(QContactManager* manager);
    ~QContactDetailDefinitionRequest();

    /* First, the base class functionality */
    bool isFinished() const;
    QContactManager::Error error() const;
    QContactAbstractRequest::Status status() const;

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::DetailDefinition;}

    /* Filtering */
    enum SelectionType {
        SelectAll,
        SelectByNames,
        SelectByObject
    };
    void clearSelection();
    void selectByName(const QStringList& definitionNames);
    void selectByObject(const QList<QContactDetailDefinition>& definitions);

    SelectionType selectionType() const;
    QStringList nameSelection() const;
    QList<QContactDetailDefinition> definitionSelection() const;

    /* What to get/store */
    enum Restriction {
        NoRestriction,
        RestrictToNames
    };

    void clearRestrictions();                                // full definition
    void restrictToNames();                                  // just definition names
    Restriction restriction();

public slots:
    /* Base class slots */
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);
    void start(QContactAbstractRequest::Operation operation);
    void cancel();

public:
    /* Results */
    QStringList names();                                  // if restrictToIds is set
    QList<QContactDetailDefinition> definitions();        // otherwise (full and partial contacts)
    QList<QContactManager::Error> errors();

signals:
    void progress(QContactDetailDefinitionRequest* self, bool finished, bool appendOnly);   // appendOnly true if ids/contacts order has not changed, just stuff added

private:
    friend class QContactDetailDefinitionRequestResult;
    Q_PRIVATE_SLOT(d, void _q_statusUpdate(QContactAbstractRequest::Status, QContactManager::Error));
    QContactDetailDefinitionRequestData* d;
};

#endif
