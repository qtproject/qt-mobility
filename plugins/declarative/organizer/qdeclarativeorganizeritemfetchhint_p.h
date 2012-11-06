/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEORGANIZERITEMFETCHHINT_H
#define QDECLARATIVEORGANIZERITEMFETCHHINT_H

#include <QtDeclarative>
#include "qorganizeritemfetchhint.h"

QTM_USE_NAMESPACE

class QDeclarativeOrganizerItemFetchHint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(OptimizationHints optimizationHints READ optimizationHints WRITE setOptimizationHints NOTIFY fetchHintChanged)
    Q_ENUMS(OptimizationHint)
    Q_FLAGS(OptimizationHints)
public:
    QDeclarativeOrganizerItemFetchHint(QObject* parent = 0)
        :QObject(parent)
    {}

    enum OptimizationHint {
        AllRequired = QOrganizerItemFetchHint::AllRequired,
        NoActionPreferences = QOrganizerItemFetchHint::NoActionPreferences,
        NoBinaryBlobs = QOrganizerItemFetchHint::NoBinaryBlobs
    };
    Q_DECLARE_FLAGS(OptimizationHints, OptimizationHint)

    OptimizationHints optimizationHints() const
    {
        OptimizationHints hints;
        hints = ~hints & (int)d.optimizationHints();
        return hints;
    }

    void setOptimizationHints(OptimizationHints hints)
    {
        if (hints != d.optimizationHints()) {
            QOrganizerItemFetchHint::OptimizationHints newHints;
            newHints = ~newHints & (int)hints;
            d.setOptimizationHints(newHints);
            emit fetchHintChanged();
        }
    }

    QOrganizerItemFetchHint fetchHint() const
    {
        return d;
    }

    void setFetchHint(const QOrganizerItemFetchHint& fetchHint)
    {
        d = fetchHint;
        emit fetchHintChanged();
    }

signals:
    void fetchHintChanged();
private:
    QOrganizerItemFetchHint d;
};

QML_DECLARE_TYPE(QDeclarativeOrganizerItemFetchHint)

#endif

