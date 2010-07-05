/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVERSITCONTACTEXPORTER_H
#define QVERSITCONTACTEXPORTER_H

#include "qmobilityglobal.h"
#include "qversitresourcehandler.h"
#include "qversitdocument.h"

#include <qcontact.h>

#include <QImage>

QTM_BEGIN_NAMESPACE

class QVersitContactExporterPrivate;

class Q_VERSIT_EXPORT QVersitContactExporterDetailHandler
{
public:
    virtual ~QVersitContactExporterDetailHandler() {}
    virtual bool preProcessDetail(const QContact& contact,
                                  const QContactDetail& detail,
                                  QVersitDocument* document) = 0;
    virtual bool postProcessDetail(const QContact& contact,
                                   const QContactDetail& detail,
                                   bool alreadyProcessed,
                                   QVersitDocument* document) = 0;
};

class Q_VERSIT_EXPORT QVersitContactExporter
{
public:
    enum Error {
        NoError = 0,
        EmptyContactError,
        NoNameError
    };

    QVersitContactExporter();
    ~QVersitContactExporter();

    bool exportContacts(const QList<QContact>& contacts,
        QVersitDocument::VersitType versitType);
    QList<QVersitDocument> documents() const;
    QMap<int, Error> errors() const;

    void setDetailHandler(QVersitContactExporterDetailHandler* handler);
    QVersitContactExporterDetailHandler* detailHandler() const;

    void setResourceHandler(QVersitResourceHandler* handler);
    QVersitResourceHandler* resourceHandler() const;

private:
    QVersitContactExporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTEXPORTER_H
