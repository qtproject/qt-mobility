/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "organizersymbiancollection.h"

#include <QDebug>
#include <calsession.h>
#include <calentryview.h>
#include "organizersymbianutils.h"
#include "qorganizeritemchangeset.h"

using namespace OrganizerSymbianUtils;

OrganizerSymbianCollectionPrivate::OrganizerSymbianCollectionPrivate()
    :QSharedData(), 
    m_engine(0),
    m_calSession(0),
    m_calEntryView(0),
    m_localId(0),
    m_error(0)
{
    m_activeSchedulerWait = new CActiveSchedulerWait();
}

OrganizerSymbianCollectionPrivate::~OrganizerSymbianCollectionPrivate()
{
    //if (m_localId)
        //qDebug() << "collection close" << m_localId << m_fileName;

    if (m_calSession)
        m_calSession->StopChangeNotification();
    delete m_calEntryView;
    delete m_calSession;
    delete m_activeSchedulerWait;
}    

/*!
 * From MCalProgressCallBack
 */
void OrganizerSymbianCollectionPrivate::Progress(TInt /*aPercentageCompleted*/)
{
}

/*!
 * From MCalProgressCallBack
 */
void OrganizerSymbianCollectionPrivate::Completed(TInt aError)
{
    m_error = aError;
    // Let's continue the operation that started the calendar operation
    m_activeSchedulerWait->AsyncStop();
}

/*!
 * From MCalProgressCallBack
 */
TBool OrganizerSymbianCollectionPrivate::NotifyProgress()
{
    // No 
    return EFalse;
}

/*!
 * From MCalChangeCallBack2
 */
void OrganizerSymbianCollectionPrivate::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
{
    // NOTE: We will not be notified of a change if we are the source. So these events are
    // caused by something else than our manager instance.

    QOrganizerItemChangeSet changeSet;

    int count = aChangeItems.Count();
    for (int i=0; i<count; i++) 
    {
        QOrganizerItemLocalId entryId = QOrganizerItemLocalId(aChangeItems[i].iEntryId);
        switch(aChangeItems[i].iChangeType)
        {
        case MCalChangeCallBack2::EChangeAdd:       
            changeSet.insertAddedItem(entryId);
            break;

        case MCalChangeCallBack2::EChangeDelete:
            changeSet.insertRemovedItem(entryId);
            break;

        case MCalChangeCallBack2::EChangeModify:
            changeSet.insertChangedItem(entryId);
            break;

        case MCalChangeCallBack2::EChangeUndefined:
            // fallthrough
        default: 
            changeSet.setDataChanged(true);
            break;
        }
    }

    changeSet.emitSignals(m_engine);
}

OrganizerSymbianCollection::OrganizerSymbianCollection()
    :d(0)
{
    d = new OrganizerSymbianCollectionPrivate();
}

OrganizerSymbianCollection::OrganizerSymbianCollection(QOrganizerItemManagerEngine *engine)
    :d(0)
{
    d = new OrganizerSymbianCollectionPrivate();
    d->m_engine = engine;
}

OrganizerSymbianCollection::OrganizerSymbianCollection(const OrganizerSymbianCollection &other)
    :d(other.d)
{

}

#ifdef SYMBIAN_CALENDAR_V2
void OrganizerSymbianCollection::openL(const TDesC &fileName, const CCalCalendarInfo *calInfo)
#else
void OrganizerSymbianCollection::openL(const TDesC &fileName)
#endif
{
    // Free the old
    if (d->m_calSession)
        d->m_calSession->StopChangeNotification();
    delete d->m_calEntryView;
    d->m_calEntryView = 0;
    delete d->m_calSession;
    d->m_calSession = 0;
    
    // Create a new session
    d->m_calSession = CCalSession::NewL();
    
    // Try opening the calendar file
    TRAPD(err, d->m_calSession->OpenL(fileName));
    if (err == KErrNotFound) {
        // File not found
        // Create a new calendar file
#ifdef SYMBIAN_CALENDAR_V2
        if (calInfo)
            d->m_calSession->CreateCalFileL(fileName, *calInfo);
        else
            d->m_calSession->CreateCalFileL(fileName);
#else
        d->m_calSession->CreateCalFileL(fileName);
#endif
        // And open it
        d->m_calSession->OpenL(fileName);
    } else {
        User::LeaveIfError(err);
#ifdef SYMBIAN_CALENDAR_V2
        if (calInfo)
            d->m_calSession->SetCalendarInfoL(*calInfo);
#endif
    }

#ifdef SYMBIAN_CALENDAR_V2
    // Get the actual filename from the session (it may differ from the given filename)
    CCalCalendarInfo *info = d->m_calSession->CalendarInfoL();
    CleanupStack::PushL(info);
    d->m_fileName = toQString(info->FileNameL());
    CleanupStack::PopAndDestroy(info);
#else
    d->m_fileName = toQString(fileName);
#endif
    
    // Get collection id
#ifdef SYMBIAN_CALENDAR_V2
    d->m_localId = d->m_calSession->CollectionIdL();
#else
    // TODO: If multiple calendars are to be supported on without native support
    // we need to generate a real id here. Currently there will only be one
    // collection so does not matter if its just a magic number.
    d->m_localId = 1;
#endif
    
    // Start listening to calendar events
    TCalTime minTime;
    minTime.SetTimeUtcL(TCalTime::MinTime());
    TCalTime maxTime;
    maxTime.SetTimeUtcL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange calTimeRange(minTime, maxTime);
    CCalChangeNotificationFilter *filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, true, calTimeRange);
    d->m_calSession->StartChangeNotification(*d, *filter);
    delete filter;
    
    //qDebug() << "collection open" << d->m_localId << d->m_fileName;
}

void OrganizerSymbianCollection::createEntryViewL()
{
    // Create an entry view
    d->m_calEntryView = CCalEntryView::NewL(*d->m_calSession, *d);
    d->m_activeSchedulerWait->Start(); // stopped at Completed()
    User::LeaveIfError(d->m_error);
}

QOrganizerCollectionLocalId OrganizerSymbianCollection::localId() const 
{ 
    return d->m_localId; 
}

CCalSession *OrganizerSymbianCollection::calSession() const
{
    return d->m_calSession; 
}

CCalEntryView *OrganizerSymbianCollection::calEntryView() const
{ 
    return d->m_calEntryView; 
}

QString OrganizerSymbianCollection::fileName() const
{
    return d->m_fileName;
}

bool OrganizerSymbianCollection::isValid() const
{
    return (d->m_localId > 0);
}

bool OrganizerSymbianCollection::isMarkedForDeletionL() const
{
#ifdef SYMBIAN_CALENDAR_V2
    // Get EMarkAsDelete property
    CCalCalendarInfo *calInfo = d->m_calSession->CalendarInfoL();
    TBool markAsDelete = EFalse;
    // NOTE: trap ignore because property might not exist
    TRAP_IGNORE(markAsDelete = getCalInfoPropertyL<TBool>(*calInfo, EMarkAsDelete));
    delete calInfo;
    return markAsDelete;
#else
    return false;
#endif
}
