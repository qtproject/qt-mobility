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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "finddialog.h"
#include "qtcontacts.h"

#include <QWidget>
#include <QMap>

QT_BEGIN_NAMESPACE
class QListWidget;
class QPushButton;
class QComboBox;
class QLabel;
class QLineEdit;
class QTextEdit;
QT_END_NAMESPACE

class PhoneBook : public QWidget
{
    Q_OBJECT

public:
    PhoneBook(QWidget *parent = 0);

public slots:
    void addContact();
    void saveContact();
    void removeContact();
    void findContact();
    void next();
    void previous();
    void importFromVCard();
    void exportAsVCard();
    void selectAvatar();
    void contactSelected(int row);

private slots:
    void backendChanged(const QList<QUniqueId>& changed);
    void backendSelected(const QString& backend);

private:
    QContact buildContact() const;
    void displayContact();
    bool eventFilter(QObject* watched, QEvent* event);
    void populateList(const QContact& currentContact);

    QListWidget *contactsList;

    QPushButton *addButton;
    QPushButton *saveButton;
    QPushButton *removeButton;
    QPushButton *findButton;
    QPushButton *importButton;
    QPushButton *exportButton;
    QPushButton *quitButton;
    QLabel *currentIndexLabel;
    QLabel *currentBackendLabel;

    QComboBox *backendCombo;

    QLineEdit *nameLine;
    QPushButton *avatarButton;
    QLineEdit *emailLine;
    QLineEdit *homePhoneLine;
    QLineEdit *workPhoneLine;
    QLineEdit *mobilePhoneLine;
    QTextEdit *addressText;

    FindDialog *dialog;

    int currentIndex;
    QList<QContact> contacts;

    QContactManager *cm;
};

#endif //PHONEBOOK_H
