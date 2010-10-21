/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    Q_PROPERTY(OptimizationHints optimizationHints READ optimizationHints WRITE setOptimizationHints NOTIFY valueChanged)
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
        hints &= 0xFFFFFFFF;
        hints &= (int)d.optimizationHints();
        return hints;
    }

    void setOptimizationHints(OptimizationHints hints)
    {
        if (hints != d.optimizationHints()) {
            QOrganizerItemFetchHint::OptimizationHints newHints;
            newHints &= 0xFFFFFFFF;
            newHints &= (int)hints;
            d.setOptimizationHints(newHints);
            emit valueChanged();
        }
    }

    QOrganizerItemFetchHint fetchHint() const
    {
        return d;
    }

    void setFetchHint(const QOrganizerItemFetchHint& fetchHint)
    {
        d = fetchHint;
    }

signals:
    void valueChanged();
private:
    QOrganizerItemFetchHint d;
};

QML_DECLARE_TYPE(QDeclarativeOrganizerItemFetchHint)

#endif

