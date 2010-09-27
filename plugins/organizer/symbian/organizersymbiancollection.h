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
#ifndef ORGANIZERSYMBIANCOLLECTION_H_
#define ORGANIZERSYMBIANCOLLECTION_H_

#include <calchangecallback.h>
#include <calprogresscallback.h>
#include "qorganizercollection.h"

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE
class QOrganizerItemManagerEngine;
QTM_END_NAMESPACE
class CCalSession;
class CCalEntryView;
class CCalInstanceView;
class CCalCalendarInfo;

class OrganizerSymbianCollectionPrivate : public QSharedData,
                                          public MCalProgressCallBack,
                                          public MCalChangeCallBack2
{
public:
    OrganizerSymbianCollectionPrivate();
    ~OrganizerSymbianCollectionPrivate();
    
    void Progress(TInt aPercentageCompleted);
    void Completed(TInt aError);
    TBool NotifyProgress();
    void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);
    
    QOrganizerItemManagerEngine *m_engine;
    CCalSession *m_calSession;
    CCalEntryView *m_calEntryView;
    CCalInstanceView *m_calInstanceView;
    QOrganizerCollectionId m_id;
    quint64 m_calCollectionId;
    CActiveSchedulerWait *m_activeSchedulerWait;
    QString m_fileName;
    int m_error;
};

class OrganizerSymbianCollection
{
public:
    OrganizerSymbianCollection();
    OrganizerSymbianCollection(QOrganizerItemManagerEngine *engine);
    OrganizerSymbianCollection(const OrganizerSymbianCollection &other);
    
#ifdef SYMBIAN_CALENDAR_V2
    void openL(const TDesC &fileName, const CCalCalendarInfo *calInfo = 0);
#else
    void openL(const TDesC &fileName);
#endif
    void createViewsL();
    QOrganizerCollectionId id() const;
    QOrganizerCollectionLocalId localId() const;
    quint64 calCollectionId() const;
    CCalSession *calSession() const; 
    CCalEntryView *calEntryView() const;
    CCalInstanceView *calInstanceView() const;
    QString fileName() const;
    bool isValid() const;
    bool isMarkedForDeletionL() const;
    QOrganizerCollection toQOrganizerCollectionL() const;
    
private:
    QExplicitlySharedDataPointer<OrganizerSymbianCollectionPrivate> d;
};

#endif // ORGANIZERSYMBIANCOLLECTION_H_
