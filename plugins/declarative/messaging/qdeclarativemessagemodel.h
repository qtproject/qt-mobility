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
#include <qmessageservice.h>

QTM_USE_NAMESPACE

class QDeclarativeMessageFilterBase;

class QDeclarativeMessageModelPrivate;
class QDeclarativeMessageModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeMessageFilterBase *filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(SortKey sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged)
    Q_PROPERTY(SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_ENUMS(SortKey);
    Q_ENUMS(SortOrder);

public:
    QDeclarativeMessageModel(QObject *parent=0);
    ~QDeclarativeMessageModel();

    QDeclarativeMessageFilterBase *filter() const;
    void setFilter(QDeclarativeMessageFilterBase *filter);

    enum SortKey {
        Priority,
        Sender,
        Size,
        StatusRead,
        StatusIncoming,
        Subject,
        Timestamp,
        ReceptionTimestamp,
        Recipients,
        Type
    };

    SortKey sortBy() const;
    void setSortBy(SortKey k);

    enum SortOrder {
        AscendingOrder = Qt::AscendingOrder,
        DescendingOrder = Qt::DescendingOrder
    };

    SortOrder sortOrder() const;
    void setSortOrder(SortOrder o);

    enum Roles {
        SubjectRole = Qt::DisplayRole,
        TypeRole,
        SizeRole,
        SenderRole,
        ToRole,
        DateRole,
        ReceivedDateRole,
        BodyRole,
        PriorityRole,
        ReadyRole
    };

    Q_INVOKABLE void showMessage(int index) const;
    Q_INVOKABLE void removeMessage(int index);

    int limit() const;
    void setLimit(int l);

    int count() const { return rowCount(); }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    virtual void classBegin() {}
    virtual void componentComplete();

protected:
    void scheduleUpdate();

signals:
    void filterChanged();
    void sortByChanged();
    void sortOrderChanged();
    void limitChanged();
    void countChanged();
    void messageAdded();

private slots:
    void messagesFound(const QMessageIdList &ids);
    void serviceProgressChanged(uint value, uint total);
    void messageUpdated(const QMessageId&);
    void messageRemoved(const QMessageId&);
    void updateFilter();

private:
    friend class QDeclarativeMessageModelPrivate;
    QDeclarativeMessageModelPrivate *d;
};

QML_DECLARE_TYPE(QDeclarativeMessageModel)

