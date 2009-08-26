#ifndef TESTMODELUI
#define TESTMODELUI

#include <QWidget>
#include <QListView>

class QContactListModel;
class QContactManager;
class TestModelView : public QListView
{
    Q_OBJECT

public:
    TestModelView(QContactManager* manager = 0);
    ~TestModelView();

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
    void generateMoreContacts();

private:
    QContactManager *manager;
    TestModelView *list;
    QPushButton *contacts;
    QPushButton *close;
};

#endif
