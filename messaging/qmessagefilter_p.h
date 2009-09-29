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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagefilter.h"
#if defined(Q_OS_WIN)
#include "qmessagestore.h"
#include "winhelpers_p.h"
#include <qvariant.h>
#include <qset.h>
#endif

#if defined(Q_OS_WIN)
class MapiFolderIterator {
public:
    MapiFolderIterator();
    MapiFolderIterator(MapiStorePtr store, MapiFolderPtr root);
    MapiFolderIterator(MapiStorePtr store, QMessageFolderIdList ids);
    MapiFolderPtr next();
private:
    QList<MapiFolderPtr> _folders; 
#ifdef QMESSAGING_OPTIONAL_FOLDER
    MapiStorePtr _store;
    QMessageFolderIdList _ids;
#endif
};

class MapiStoreIterator {
public:
    MapiStoreIterator();
    MapiStoreIterator(QList<MapiStorePtr> stores);
    MapiStorePtr next();
private:
    QList<MapiStorePtr> _stores; 
};
#endif

class QMessageFilterPrivate
{
    Q_DECLARE_PUBLIC(QMessageFilter)

public:
    QMessageFilterPrivate(QMessageFilter *messageFilter);
    ~QMessageFilterPrivate();

    QMessageFilter *q_ptr;
    QMessageDataComparator::Options _options;
#if defined(Q_OS_WIN)
    enum Field { None = 0, Id, Type, Sender, Recipients, Subject, TimeStamp, ReceptionTimeStamp, Status, Priority, Size, CustomField, ParentAccountId, ParentFolderId, AncestorFolderIds };
    enum Comparator { Equality = 0, Relation, Inclusion };
    enum Operator { Identity = 0, And, Or, Not, Nand, Nor, OperatorEnd };
    QMessageFilterPrivate::Field _field;
    QVariant _value;
    Comparator _comparatorType;
    int _comparatorValue; //TODO: Consider using a union here to get rid of ugly static casts in implementation
    Operator _operator;
    QMessageFilter *_left;
    QMessageFilter *_right;
    wchar_t *_buffer; // TODO: Consider trying to move these into MapiRestriction
    wchar_t *_buffer2;
    bool _valid;

    QSet<QMessage::StandardFolder> _standardFoldersInclude; // only match messages directly in one of these folders
    QSet<QMessage::StandardFolder> _standardFoldersExclude; // only match messages not directly in any of these folders
    QSet<QMessageAccountId> _accountsInclude; // only match messages in one of these accounts
    QSet<QMessageAccountId> _accountsExclude; // only match messages not in any of these accounts
    bool _complex; // true iff operator is Or and left or right terms contain non-null containerFilters

    bool containerFiltersAreEmpty(); // returns true IFF above QSets are empty
    QMessageFilter containerFiltersPart(); // returns a filter comprised of just the container filters
    QMessageFilter nonContainerFiltersPart(); // returns a filter comprised of everything but the container filters

    static void filterTable(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter, LPMAPITABLE);
    static QMessageFilter from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilter from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::RelationComparator cmp);
    static QMessageFilter from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::InclusionComparator cmp);
    static QMessageFilterPrivate* implementation(const QMessageFilter &filter);

    static MapiFolderIterator folderIterator(const QMessageFilter &filter, QMessageStore::ErrorCode *lastError, MapiStorePtr store);
    static MapiStoreIterator storeIterator(const QMessageFilter &filter, QMessageStore::ErrorCode *lastError, MapiSessionPtr session);


#endif
};
