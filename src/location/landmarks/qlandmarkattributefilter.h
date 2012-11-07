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

#ifndef QLANDMARKATTRIBUTEFILTER_H
#define QLANDMARKATTRIBUTEFILTER_H

#include "qlandmarkfilter.h"
#include <QVariant>
#include <QString>
#include <QStringList>

QTM_BEGIN_NAMESPACE
class QLandmark;

class QLandmarkAttributeFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkAttributeFilter : public QLandmarkFilter
{
public:
    QLandmarkAttributeFilter();
    QLandmarkAttributeFilter(const QLandmarkFilter &other);
    virtual ~QLandmarkAttributeFilter();

    enum OperationType{AndOperation, OrOperation};

    QVariant attribute(const QString &key) const;
    void setAttribute(const QString &key, const QVariant &value, QLandmarkFilter::MatchFlags flags =0);
    void setAttributes(const QStringList &key, const QVariant &value, QLandmarkFilter::MatchFlags flags=0);
    void removeAttribute(const QString &key);
    void clearAttributes();
    QStringList attributeKeys() const;

    QLandmarkFilter::MatchFlags matchFlags(const QString &key) const;

    OperationType operationType() const;
    void setOperationType(OperationType operationType);

private:
    Q_DECLARE_LANDMARKFILTER_PRIVATE(QLandmarkAttributeFilter)
};

QTM_END_NAMESPACE

#endif
