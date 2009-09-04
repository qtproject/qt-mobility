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

#include "testmodelui.h"
#include "qtcontacts.h"
#include "qcontactlistmodel.h"
#include "qcontactfetchrequest.h"

#include <QtGui>

TestModelView::TestModelView(QContactManager* manager)
        : QListView()
{
    model = new QContactListModel(manager);
    setModel(model);
}

TestModelView::~TestModelView()
{
    delete model;
}

QUniqueId TestModelView::currentId() const
{
    return QUniqueId(model->data(currentIndex(), QContactListModel::IdRole).toUInt());
}

QVariant TestModelView::currentData(QContactListModel::ContactDataRole role) const
{
    return model->data(currentIndex(), role);
}

TestModelUi::TestModelUi()
        : QWidget()
{
    manager = new QContactManager("memory");
    populateContacts();
    nbrMissedCalls = 0;
    talkingToNumber = "";

    fetchRequest = new QContactFetchRequest;
    fetchRequest->setManager(manager);
    connect(fetchRequest, SIGNAL(progress(QContactFetchRequest*,bool)), this, SLOT(dataAvailable(QContactFetchRequest*,bool)));

    incomingCallTimer = new QTimer;
    incomingCallTimer->setInterval(15000); // 15 seconds between incoming calls.
    dialTimer = new QTimer;
    dialTimer->setInterval(3000); // they answer after 3 seconds
    answerTimer = new QTimer;
    answerTimer->setInterval(6000); // missed call after 6 seconds

    connect(incomingCallTimer, SIGNAL(timeout()), this, SLOT(incoming()));
    connect(dialTimer, SIGNAL(timeout()), this, SLOT(talking()));
    connect(answerTimer, SIGNAL(timeout()), this, SLOT(missedCall()));
    incomingCallTimer->start();

    list = new TestModelView(manager);
    textEdit = new QTextEdit;
    viewArea = new QStackedWidget;
    viewArea->addWidget(list);
    viewArea->addWidget(textEdit);
    viewArea->setCurrentIndex(0);

    missedCalls = new QLabel;
    missedCalls->setAlignment(Qt::AlignLeft);
    missedCalls->setText(QString(tr("# Missed Calls:")));
    missedCallsNbr = new QLabel;
    missedCallsNbr->setAlignment(Qt::AlignRight);
    missedCallsNbr->setText(QString::number(nbrMissedCalls));

    leftButton = new QPushButton(tr("Dial"));
    rightButton = new QPushButton(tr("Quit"));

    connect(leftButton, SIGNAL(clicked()), this, SLOT(dial()));
    connect(rightButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *missedLayout = new QHBoxLayout;
    missedLayout->addWidget(missedCalls);
    missedLayout->addWidget(missedCallsNbr);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(leftButton);
    btnLayout->addWidget(rightButton);

    QVBoxLayout *listLayout = new QVBoxLayout;
    listLayout->addLayout(missedLayout);
    listLayout->addWidget(viewArea);
    listLayout->addLayout(btnLayout);

    setLayout(listLayout);
}

TestModelUi::~TestModelUi()
{
    delete leftButton;
    delete rightButton;
    delete list;
    delete textEdit;
    delete viewArea;
    delete manager;
}

void TestModelUi::populateContacts()
{
    // generate some fictional contacts.
    QStringList nameFirst;
    nameFirst << "Adam" << "Adrianna" << "Alex" << "Brett" << "Bob"
              << "Carina" << "Christina" << "Carl" << "Daniel" << "Denise"
              << "Eric" << "Fred" << "Mario" << "William" << "Zandi";
    QStringList nameLast;
    nameLast << "Citizen" << "Civilian" << "Doe" << "Generic" << "Public" << "Unlikely";

    QStringList phoneFirst;
    phoneFirst << "111" << "222" << "333" << "444" << "555" << "666" << "777" << "888"
               << "123" << "234" << "345" << "456" << "567" << "678" << "789";
    QStringList phoneLast;
    phoneLast << "9876" << "8765" << "7654" << "6543" << "5432" << "4321";

    QStringList emailFirst;
    emailFirst << "testPersonOne" << "testPersonTwo" << "testPersonThree" << "testPersonFour" << "testPersonFive"
               << "testPersonSix" << "testPersonSeven" << "testPersonEight" << "testPersonNine" << "testPersonTen"
               << "testPersonEleven" << "testPersonTwelve" << "testPersonThirteen" << "testPersonFourteen" << "testPersonFifteen";
    QStringList emailLast;
    emailLast << "@test1.nokia.com" << "@test2.nokia.com" << "@test3.nokia.com"
              << "@test4.nokia.com" << "@test5.nokia.com" << "@test6.nokia.com";

    QStringList avatarFirst;
    avatarFirst << "party" << "celebration" << "happyface" << "grinning" << "smile"
                << "laughing" << "dance" << "serious" << "angry" << "sadface"
                << "tree" << "mountain" << "ocean" << "city" << "bridge";
    QStringList avatarLast;
    avatarLast << ".png" << ".bmp" << ".jpg" << ".gif" << ".avi" << ".mpg";

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 6; j++) {
            QContact c;
            QContactName n;
            QContactPhoneNumber p;
            QContactEmailAddress e;
            QContactAvatar a;

            n.setFirst(nameFirst.at(i));
            n.setLast(nameLast.at(j));
            p.setNumber(phoneFirst.at(i) + phoneLast.at(j));
            e.setEmailAddress(emailFirst.at(i) + emailLast.at(j));
            a.setAvatar(avatarFirst.at(i) + avatarLast.at(j));

            c.saveDetail(&n);
            c.saveDetail(&p);
            c.saveDetail(&e);
            c.saveDetail(&a);

            manager->saveContact(&c);
        }
    }
}

void TestModelUi::dataAvailable(QContactFetchRequest* request, bool appendOnly)
{
    Q_UNUSED(appendOnly);

    // first, make sure we can use the data.
    if (currentState == TestModelUi::WaitingState || request->status() != QContactAbstractRequest::Finished)
        return;

    // we assume that we need the extra details.
    QString text;

    QList<QContact> requestData = request->contacts();
    if (requestData.isEmpty() || requestData.at(0).isEmpty()) {
        text += "Unknown Contact";
        talkingToDetails = text;
        return;
    }

    QContact curr = request->contacts().at(0);
    QList<QContactDetail> allDetails = curr.details();
    foreach (const QContactDetail& det, allDetails) {
        QString defName = det.definitionName();
        text += defName + ":" + "\n";
        QList<QString> fieldKeys = det.values().keys();
        foreach (const QString& key, fieldKeys) {
            text += "\t" + key + " = " + det.value(key) + "\n";
        }
        text += "\n";
    }

    talkingToName = curr.displayLabel().label();
    if (currentState == TestModelUi::DialingState) {
        talkingToNumber = curr.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber);
    }

    if (!text.isEmpty())
        text.chop(1); // kill unneeded newline.
    talkingToDetails = text;
    textEdit->setText(talkingToFirstLine + " " + talkingToName + "\n\n" + talkingToDetails);
}

void TestModelUi::dial()
{
    // get current index id from view
    // change current widget to text
    // change buttons to <gray> <hangup>
    // ...
    incomingCallTimer->stop();
    answerTimer->stop();
    currentState = TestModelUi::DialingState;

    QContactIdListFilter fil;
    QList<QUniqueId> fetchIds;
    fetchIds << list->currentId();
    fil.setIds(fetchIds);
    fetchRequest->cancel(); // if not already stopped.
    fetchRequest->setFilter(fil);
    fetchRequest->start();

    talkingToFirstLine = "Dialing";
    talkingToName = list->currentData(QContactListModel::DisplayLabelRole).toString();
    textEdit->setText(talkingToFirstLine + " " + talkingToName + "\n\n" + talkingToDetails);
    leftButton->disconnect();
    rightButton->disconnect();
    leftButton->setText(tr("Dial"));
    rightButton->setText(tr("Cancel"));
    connect(rightButton, SIGNAL(clicked()), this, SLOT(hangup()));
    leftButton->setEnabled(false);
    viewArea->setCurrentIndex(1);
    dialTimer->start();
}

void TestModelUi::incoming()
{
    // change current widget to text
    // change buttons to <answer> <hangup>
    // ...
    incomingCallTimer->stop();
    dialTimer->stop();
    currentState = TestModelUi::IncomingState;

    // create some phone numbers.  about half appear in our contacts, half do not.
    QStringList phoneFirst;
    phoneFirst << "111" << "222" << "337" << "944" << "555" << "611" << "777" << "855"
               << "123" << "234" << "385" << "456" << "587" << "688" << "788";
    QStringList phoneLast;
    phoneLast << "9876" << "8765" << "7654" << "9543" << "5432" << "9321";

    int firstIndex = qrand() % phoneFirst.size();
    int lastIndex = qrand() % phoneLast.size();
    talkingToNumber = phoneFirst.at(firstIndex) + phoneLast.at(lastIndex);
    talkingToFirstLine = "Incoming call from";
    textEdit->setText(talkingToFirstLine + " " + talkingToNumber + "\n\n" + talkingToDetails);

    QContactDetailFilter fil;
    fil.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    fil.setValue(talkingToNumber);
    fetchRequest->cancel(); // if not already stopped.
    fetchRequest->setFilter(fil);
    fetchRequest->start();

    leftButton->disconnect();
    rightButton->disconnect();
    leftButton->setText(tr("Answer"));
    rightButton->setText(tr("Hang Up"));
    connect(leftButton, SIGNAL(clicked()), this, SLOT(talking()));
    connect(rightButton, SIGNAL(clicked()), this, SLOT(hangup()));
    leftButton->setEnabled(true);
    viewArea->setCurrentIndex(1);

    answerTimer->start();
}

void TestModelUi::talking()
{
    // get id of contact from incoming?
    // using async contact request with filter?
    // change current widget to text
    // change buttons to <gray> <hangup>
    // ...
    incomingCallTimer->stop();
    dialTimer->stop();
    answerTimer->stop();
    currentState = TestModelUi::TalkingState;

    talkingToFirstLine = "Talking to";
    textEdit->setText(talkingToFirstLine + " " + (talkingToName.isEmpty() ? talkingToNumber : talkingToName) + "\n\n" + talkingToDetails);
    leftButton->disconnect();
    rightButton->disconnect();
    leftButton->setText(tr("Answer"));
    rightButton->setText(tr("Hang Up"));
    connect(rightButton, SIGNAL(clicked()), this, SLOT(hangup()));
    leftButton->setEnabled(false);
}

void TestModelUi::hangup()
{
    // change current widget to list
    // change buttons to <dial> <exit>
    // ...
    incomingCallTimer->stop();
    dialTimer->stop();
    answerTimer->stop();
    currentState = TestModelUi::WaitingState;

    talkingToName = "";
    talkingToNumber = "";
    talkingToDetails = "";
    talkingToFirstLine = "";
    textEdit->setText("");
    leftButton->disconnect();
    rightButton->disconnect();
    leftButton->setText(tr("Dial"));
    rightButton->setText(tr("Quit"));
    connect(leftButton, SIGNAL(clicked()), this, SLOT(dial()));
    connect(rightButton, SIGNAL(clicked()), this, SLOT(close()));
    leftButton->setEnabled(true);
    viewArea->setCurrentIndex(0);
    incomingCallTimer->start(); // restart the incoming call timer.
}

void TestModelUi::missedCall()
{
    // increment missed call count
    // change current widget to list
    // change buttons to <dial> <exit>
    // ...
    incomingCallTimer->stop();
    dialTimer->stop();
    answerTimer->stop();
    currentState = TestModelUi::WaitingState;

    talkingToName = "";
    talkingToNumber = "";
    talkingToDetails = "";
    talkingToFirstLine = "";
    textEdit->setText("");
    nbrMissedCalls += 1;
    missedCallsNbr->setText(QString::number(nbrMissedCalls));
    leftButton->disconnect();
    rightButton->disconnect();
    leftButton->setText(tr("Dial"));
    rightButton->setText(tr("Quit"));
    connect(leftButton, SIGNAL(clicked()), this, SLOT(dial()));
    connect(rightButton, SIGNAL(clicked()), this, SLOT(close()));
    leftButton->setEnabled(true);
    viewArea->setCurrentIndex(0);
    incomingCallTimer->start(); // restart the incoming call timer.
}
