/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVERSITORGANIZEREXPORTER_H
#define QVERSITORGANIZEREXPORTER_H

#include "qmobilityglobal.h"
#include "qversitdocument.h"

#include <qorganizeritem.h>

QTM_BEGIN_NAMESPACE

class QVersitOrganizerExporterPrivate;

class Q_VERSIT_ORGANIZER_EXPORT QVersitOrganizerExporterDetailHandler
{
public:
    virtual ~QVersitOrganizerExporterDetailHandler() {}
    virtual void detailProcessed(const QOrganizerItem& item,
                                 const QOrganizerItemDetail& detail,
                                 const QVersitDocument& document,
                                 QSet<QString>* processedFields,
                                 QList<QVersitProperty>* toBeRemoved,
                                 QList<QVersitProperty>* toBeAdded) = 0;
    virtual void itemProcessed(const QOrganizerItem& item,
                               QVersitDocument* document) = 0;
};

class Q_VERSIT_ORGANIZER_EXPORT QVersitOrganizerExporter
{
public:
    enum Error {
        NoError = 0,
        EmptyOrganizerError,
        UnknownComponentTypeError,
        UnderspecifiedOccurrenceError
    };

    QVersitOrganizerExporter();
    explicit QVersitOrganizerExporter(const QString& profile);
    ~QVersitOrganizerExporter();

    bool exportItems(const QList<QOrganizerItem>& items,
            QVersitDocument::VersitType versitType = QVersitDocument::ICalendar20Type);
    QVersitDocument document() const;
    QMap<int, Error> errorMap() const;

    void setDetailHandler(QVersitOrganizerExporterDetailHandler* handler);

private:
    QVersitOrganizerExporterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITORGANIZEREXPORTER_H
