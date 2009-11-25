/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTMODELUI_H
#define TESTMODELUI_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QWidget>
#include <QListView>

#include "qtcontactsglobal.h"  // for QContactLocalId...
#include "qcontactlistmodel.h"
#include "filterdialog.h"

class QStackedWidget;
class QTextEdit;
class QLabel;
class QPushButton;

QTM_BEGIN_NAMESPACE
class QContactFetchRequest;
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
class TestModelView : public QListView
{
    Q_OBJECT

public:
    TestModelView(QContactManager* manager = 0);
    ~TestModelView();

    QContactLocalId currentId() const;
    QVariant currentData(QContactListModel::ContactDataRole role) const;

private:
    QContactListModel *model;
};

class QPushButton;
class TestModelUi : public QWidget
{
    Q_OBJECT

public:
    TestModelUi();
    ~TestModelUi();

public slots:
    void populateContacts();
    void dataAvailable(QContactFetchRequest* request, bool appendOnly);
    void filterResults(QContactFetchRequest* request, bool appendOnly);
    void filterFound();

    void findContact();
    void showFilterDialog();
    void finishedFindContact();
    void dial();
    void incoming();
    void talking();
    void hangup();
    void missedCall();

private:
    QContactManager *manager;
    FilterDialog *dialog;
    TestModelView *list;
    QTextEdit *textEdit;
    QStackedWidget *viewArea;
    QPushButton *leftButton;
    QPushButton *middleButton;
    QPushButton *rightButton;

    QTimer *incomingCallTimer; // we generate incoming calls from this
    QTimer *dialTimer;         // we simulate them answering after x seconds
    QTimer *answerTimer;       // incoming calls are ignored after y seconds
    QLabel *missedCalls;       // the display label for missed calls
    QLabel *missedCallsNbr;    // the count is displayed in this widget
    int nbrMissedCalls;

    enum CallState {
        WaitingState = 0,
        IncomingState,
        DialingState,
        TalkingState
    };
    CallState currentState;

    QContactFetchRequest* fetchRequest;
    QContactFetchRequest* filterRequest;
    QString talkingToName;
    QString talkingToNumber;
    QString talkingToDetails;
    QString talkingToFirstLine;
};

#endif
