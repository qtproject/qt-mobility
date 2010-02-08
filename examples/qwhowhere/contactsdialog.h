/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include <QtGui/QDialog>
#include <QListWidget>
#include <QPointer>

// QtMobility API headers
// Contacts
#include <qcontactmanager.h>
#include <qcontactphonenumber.h>
#include <qcontactsortorder.h>
#include <qcontact.h>
#include <qcontactname.h>

// QtMobility namespace
QTM_USE_NAMESPACE

class ContactsDialog: public QDialog
{
    Q_OBJECT

public:
    ContactsDialog(QWidget *parent = 0);
    ~ContactsDialog();

public slots:
    void itemClicked(QListWidgetItem *item);

signals:
    void contactSelected(QString phoneNumber);

private:
    void createContactManager();
    void searchContact();

private:
    QPointer<QContactManager> m_contactManager;
    QListWidget* m_listWidget;
};

#endif // CONTACTSDIALOG_H
