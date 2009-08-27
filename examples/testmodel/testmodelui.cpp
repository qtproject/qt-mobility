#include "testmodelui.h"
#include "qtcontacts.h"
#include "qcontactlistmodel.h"

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

TestModelUi::TestModelUi()
        : QWidget()
{
    manager = new QContactManager("memory");
    generateMoreContacts(); // only one generate = should be able to cache whole lot.
    generateMoreContacts();
    generateMoreContacts();
    generateMoreContacts();
    generateMoreContacts();
    generateMoreContacts();
    generateMoreContacts();
    generateMoreContacts();
    generateMoreContacts();

    list = new TestModelView(manager);
    contacts = new QPushButton(tr("Generate more contacts"));
    close = new QPushButton(tr("Exit"));

    connect(contacts, SIGNAL(clicked()), this, SLOT(generateMoreContacts()));
    connect(close, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(contacts);
    btnLayout->addWidget(close);

    QVBoxLayout *listLayout = new QVBoxLayout;
    listLayout->addWidget(list);
    listLayout->addLayout(btnLayout);

    setLayout(listLayout);
}

TestModelUi::~TestModelUi()
{
    delete close;
    delete contacts;
    delete list;
    delete manager;
}

void TestModelUi::generateMoreContacts()
{
    // create some contacts, add to manager.
    for (int i = 0; i < 10; i++) {
        QContact c;
        QString name;
        QString phn;
        for (int j = 0; j < 5; j++) {
            int zma = 'z' - 'a';
            int fnameChar = (qrand() % zma) + 'a';
            name += QChar(fnameChar);
        }
        name += " ";
        for (int j = 0; j < 4; j++) {
            int zma = 'z' - 'a';
            int lnameChar = (qrand() % zma) + 'a';
            name += QChar(lnameChar);
        }

        for (int j = 0; j < 7; j++) {
            int zma = '9' - '1';
            int phnChar = (qrand() % zma) + '1';
            phn += QChar(phnChar);
        }

        c.setDisplayLabel(name);
        QContactPhoneNumber n;
        n.setNumber(phn);
        c.saveDetail(&n);

        QContactAvatar a;
        a.setAvatar(QString::number(i) + QString(".png"));
        c.saveDetail(&a);

        if (!manager->saveContact(&c))
            qDebug() << "Failed to add generated contact:" << c.displayLabel().label();
    }
}
