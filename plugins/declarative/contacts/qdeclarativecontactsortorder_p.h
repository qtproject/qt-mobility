/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#ifndef QDECLARATIVECONTACTSORTORDER_P_H
#define QDECLARATIVECONTACTSORTORDER_P_H

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include <QDeclarativeParserStatus>

#include "qcontactsortorder.h"
#include "qdeclarativecontactdetail_p.h"

QTM_USE_NAMESPACE

class QDeclarativeContactSortOrder :public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant detail READ detail WRITE setDetail NOTIFY sortOrderChanged)
    Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY sortOrderChanged)
    Q_PROPERTY(Qt::SortOrder direction READ direction WRITE setDirection NOTIFY sortOrderChanged)
    Q_PROPERTY(BlankPolicy blankPolicy READ blankPolicy WRITE setBlankPolicy NOTIFY sortOrderChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY sortOrderChanged)
    Q_ENUMS(BlankPolicy)
    Q_INTERFACES(QDeclarativeParserStatus)
public:

    enum BlankPolicy {
        BlanksFirst = QContactSortOrder::BlanksFirst,
        BlanksLast = QContactSortOrder::BlanksLast
    };


    QDeclarativeContactSortOrder(QObject* parent = 0);

    void setDetail(const QVariant& detailType);

    QVariant detail() const;

    void setField(const QVariant& fieldType);

    QVariant field() const;

    //from QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete();

    BlankPolicy blankPolicy() const;
    void setBlankPolicy(BlankPolicy blankPolicy);
    Qt::SortOrder direction() const;
    void setDirection(Qt::SortOrder direction);
    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity sensitivity);
    QContactSortOrder sortOrder();
    void setSortOrder(const QContactSortOrder& sortOrder);

signals:
    void sortOrderChanged();
private:
    QVariant m_field;
    QVariant m_detail;
    QContactSortOrder m_sortOrder;
};

QML_DECLARE_TYPE(QDeclarativeContactSortOrder)

#endif
