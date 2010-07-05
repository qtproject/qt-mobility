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
#ifndef QMESSAGEFILTERPRIVATE_H
#define QMESSAGEFILTERPRIVATE_H
#include "qmessagefilter.h"
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include <qvariant.h>
#endif
#if defined(Q_OS_WIN)
#include "qmessagemanager.h"
#include "winhelpers_p.h"
#include <qvariant.h>
#include <qset.h>
#endif

QTM_BEGIN_NAMESPACE

#if 0
#define QSTRING_FOLDER_ID
#endif
#ifndef QSTRING_FOLDER_ID
typedef QSet<QMessageFolderId> FolderIdSet;
#else
typedef QSet<QString> FolderIdSet;
#endif

#if defined(Q_OS_WIN)
class MapiFolderIterator {
public:
    MapiFolderIterator();
    MapiFolderIterator(MapiStorePtr store, 
                       MapiFolderPtr root, 
                       QSet<QMessage::StandardFolder> standardFoldersInclude, 
                       QSet<QMessage::StandardFolder> standardFoldersExclude,
                       FolderIdSet parentInclude,
                       FolderIdSet parentExclude,
                       FolderIdSet ancestorInclude,
                       FolderIdSet ancestorExclude);
    MapiFolderPtr next();
private:
    QList<MapiFolderPtr> _folders; 
    MapiStorePtr _store;
    QSet<QMessage::StandardFolder> _standardFoldersInclude;
    QSet<QMessage::StandardFolder> _standardFoldersExclude;
    FolderIdSet _parentInclude;
    FolderIdSet _parentExclude;
    FolderIdSet _ancestorInclude;
    FolderIdSet _ancestorExclude;
};

class MapiStoreIterator {
public:
    MapiStoreIterator();
    MapiStoreIterator(QList<MapiStorePtr> stores, QSet<QMessageAccountId> accountsInclude, QSet<QMessageAccountId> accountsExclude);
    MapiStorePtr next();
private:
    QList<MapiStorePtr> _stores; 
    QSet<QMessageAccountId> _accountsInclude;
    QSet<QMessageAccountId> _accountsExclude;
};
#endif

class QMessageFilterPrivate
{
    Q_DECLARE_PUBLIC(QMessageFilter)

public:
    QMessageFilterPrivate(QMessageFilter *messageFilter);
    ~QMessageFilterPrivate();

    QMessageFilter *q_ptr;
    QMessageDataComparator::MatchFlags _matchFlags;

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    typedef QList<QMessageFilter> SortedMessageFilterList;
    
    bool filter(const QMessage &message) const;
    static bool filter(const QMessage &message, const QMessageFilterPrivate &filter);
    
    static void changeComparatorValuesToOpposite(QMessageFilter& filter);
    static void changeANDsAndORsToOpposite(QMessageFilter& filter);
    static void applyNot(QMessageFilter& filter);
    static bool lessThan(const QMessageFilter filter1, const QMessageFilter filter2); 
    static QMessageFilterPrivate* implementation(const QMessageFilter &filter);

    enum Field {None = 0, ParentAccountIdFilter, ParentFolderIdFilter, Id, ParentFolderId, AncestorFolderIds, ParentAccountId, Type, StandardFolder, TimeStamp, ReceptionTimeStamp, Sender, Recipients, Subject, Status, Priority, Size};
    enum Comparator {Equality = 0, Relation, Inclusion};

    bool _valid;
    bool _notFilter;
    bool _notFilterForComparator;

    QMessageIdList _ids;
    QVariant _value;
    QMessageAccountFilter *_accountFilter;
    QMessageFolderFilter *_folderFilter;
    QMessageFilterPrivate::Field _field;

    Comparator _comparatorType;
    int _comparatorValue;
    
    QList<SortedMessageFilterList> _filterList;
#endif
#if defined(Q_OS_WIN)
    enum Field { None = 0, Id, Type, Sender, SenderName, SenderAddress, Recipients, RecipientName, RecipientAddress, Subject, TimeStamp, ReceptionTimeStamp, Status, Priority, Size, ParentAccountId, ParentFolderId, AncestorFolderIds, MessageFilter, AccountFilter, FolderFilter, AncestorFilter };
    enum Comparator { Equality = 0, Relation, Inclusion };
    enum Operator { Identity = 0, And, Or, Not, Nand, Nor, OperatorEnd };
    QMessageFilterPrivate::Field _field;
    QVariant _value;
    Comparator _comparatorType;
    int _comparatorValue; //TODO: Consider using a union here to get rid of ugly static casts in implementation
    Operator _operator;
    QMessageFilter *_left;
    QMessageFilter *_right;
    bool _valid;
    bool _matchesRequired;
    bool _restrictionPermitted;
    QMessageFilter *_messageFilter;
    QMessageAccountFilter *_accountFilter;
    QMessageFolderFilter *_folderFilter;

    QSet<QMessage::StandardFolder> _standardFoldersInclude; // only match messages directly in one of these folders
    QSet<QMessage::StandardFolder> _standardFoldersExclude; // only match messages not directly in any of these folders
    QSet<QMessageAccountId> _accountsInclude; // only match messages in one of these accounts
    QSet<QMessageAccountId> _accountsExclude; // only match messages not in any of these accounts
    FolderIdSet _parentInclude; // only match messages directly in one of these folders
    FolderIdSet _parentExclude; // only match messages not directly in any of these folders
    FolderIdSet _ancestorInclude; // only match messages (directly or recursively) in one of these folders
    FolderIdSet _ancestorExclude; // only match messages not (directly or recursively) in any of these folders
    bool _complex; // true iff operator is Or and left or right terms contain non-null containerFilters

    bool containerFiltersAreEmpty(); // returns true IFF above QSets are empty
    bool nonContainerFiltersAreEmpty();
    QMessageFilter containerFiltersPart(); // returns a filter comprised of just the container filters
    QMessageFilter nonContainerFiltersPart(); // returns a filter comprised of everything but the container filters

    static QMessageFilter from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilter from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::RelationComparator cmp);
    static QMessageFilter from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::InclusionComparator cmp);
    static QMessageFilterPrivate* implementation(const QMessageFilter &filter);

    static MapiFolderIterator folderIterator(const QMessageFilter &filter, QMessageManager::Error *error, const MapiStorePtr &store);
    static MapiStoreIterator storeIterator(const QMessageFilter &filter, QMessageManager::Error *error, const MapiSessionPtr &session);
    static QList<QMessageFilter> subfilters(const QMessageFilter &filter);

    static QMessageFilter preprocess(QMessageManager::Error *error, MapiSessionPtr session, const QMessageFilter &filter);
    static bool preprocess(QMessageManager::Error *error, MapiSessionPtr session, QMessageFilter *filter);
    static bool isNonMatching(const QMessageFilter &filter); // Possibly should be in public QMessageFilter API
    static bool matchesMessageSimple(const QMessageFilter &filter, const QMessage &message);
    static bool matchesMessage(const QMessageFilter &filter, const QMessage &message, MapiStore *store);

    static bool QMessageFilterPrivate::restrictionPermitted(const QMessageFilter &filter);
    static bool QMessageFilterPrivate::matchesMessageRequired(const QMessageFilter &filter);
    static bool QMessageFilterPrivate::containsSenderSubfilter(const QMessageFilter &filter);

    static QMessageFilter QMessageFilterPrivate::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp);
    static QMessageFilter QMessageFilterPrivate::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static void QMessageFilterPrivate::debug(const QMessageFilter &filter, const QString &indent = QString());
#endif
};

#if defined(Q_OS_WIN)
class MapiRestriction {
public:
    MapiRestriction(const QMessageFilter &filter);
    ~MapiRestriction();
    void complement();
    SRestriction *sRestriction();
    bool isValid() { return _valid; }
    bool isEmpty() { return _empty; }

private:
    SRestriction _restriction;
    SRestriction _subRestriction[2];
    SPropValue _keyProp;
    SPropValue _keyProp2;
    SRestriction *_notRestriction;
    SRestriction *_recipientRestriction;
    SPropValue *_keyProps;
    SRestriction *_restrictions;
#ifdef _WIN32_WCE
    MapiEntryId *_recordKeys;
#else
    MapiRecordKey *_recordKeys;
#endif
    wchar_t *_buffer;
    wchar_t *_buffer2;
    bool _valid;
    bool _empty;
    MapiRestriction *_left;
    MapiRestriction *_right;
};
#endif

QTM_END_NAMESPACE
#endif
