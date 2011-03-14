/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include <QAbstractListModel>
#include <qmessage.h>
#include <qmessagefilter.h>

QTM_USE_NAMESPACE

class QDeclarativeMessageFilterBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool negated READ negated WRITE setNegated NOTIFY negatedChanged)

public:
    QDeclarativeMessageFilterBase(QObject *parent=0);

    bool negated() const;
    void setNegated(bool);

    virtual QMessageFilter filter() = 0;

signals:
    void negatedChanged();

protected:
    bool m_negated;
};

class QDeclarativeMessageIntersectionFilter : public QDeclarativeMessageFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeMessageFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    QDeclarativeMessageIntersectionFilter(QObject *parent=0);

    QDeclarativeListProperty<QDeclarativeMessageFilterBase> filters();

    virtual QMessageFilter filter();

private:
    QList<QDeclarativeMessageFilterBase*> m_filters;
};

class QDeclarativeMessageUnionFilter : public QDeclarativeMessageFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeMessageFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    QDeclarativeMessageUnionFilter(QObject *parent=0);

    QDeclarativeListProperty<QDeclarativeMessageFilterBase> filters();

    virtual QMessageFilter filter();

private:
    QList<QDeclarativeMessageFilterBase*> m_filters;
};


class QDeclarativeMessageFilterPrivate;
class QDeclarativeMessageFilter : public QDeclarativeMessageFilterBase
{
    Q_OBJECT

    Q_PROPERTY(FilterType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Comparator comparator READ comparator WRITE setComparator NOTIFY comparatorChanged)

    Q_ENUMS(FilterType);
    Q_ENUMS(Priority);
    Q_ENUMS(Comparator);
    Q_ENUMS(Type);
    Q_ENUMS(Status);

public:
    QDeclarativeMessageFilter(QObject *parent=0);

    enum FilterType {
        AncestorFolder,
        ParentFolder,
        Priority,
        Recipients,
        Sender,
        Size,
        StandardFolder,
        Status,
        Subject,
        Timestamp,
        ReceptionTimestamp,
        Type
    };

    FilterType type() const;
    void setType(FilterType type);

    QVariant value() const;
    void setValue(const QVariant &value);

    enum Comparator {
        // Inclusion
        Includes,
        Excludes,
        // Equality
        Equal,
        NotEqual,
        // Relation
        LessThan,
        LessThanEqual,
        GreaterThan,
        GreaterThanEqual
    };

    Comparator comparator() const;
    void setComparator(Comparator);

    QMessageFilter filter();

    enum Priority {
        HighPriority = QMessage::HighPriority,
        NormalPriority = QMessage::NormalPriority,
        LowPriority = QMessage::LowPriority
    };

    enum Type {
        Mms = QMessage::Mms,
        Sms = QMessage::Sms,
        Email = QMessage::Email,
        InstantMessage = QMessage::InstantMessage,
        AnyType = QMessage::AnyType
    };

    enum Status {
        Read = QMessage::Read,
        HasAttachments = QMessage::HasAttachments,
        Incoming = QMessage::Incoming,
        Removed = QMessage::Removed
    };

    enum StandardFolder {
        InboxFolder = QMessage::InboxFolder,
        DraftsFolder = QMessage::DraftsFolder,
        OutboxFolder = QMessage::OutboxFolder,
        SentFolder = QMessage::SentFolder,
        TrashFolder = QMessage::TrashFolder
    };

signals:
    void typeChanged();
    void valueChanged();
    void comparatorChanged();

private:
    QDeclarativeMessageFilterPrivate *d;
};


QML_DECLARE_TYPE(QDeclarativeMessageFilter)
QML_DECLARE_TYPE(QDeclarativeMessageIntersectionFilter)
QML_DECLARE_TYPE(QDeclarativeMessageUnionFilter)
QML_DECLARE_TYPE(QDeclarativeMessageFilterBase)

