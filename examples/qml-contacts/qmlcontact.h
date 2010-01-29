#ifndef QMLCONTACT_H
#define QMLCONTACT_H

#include <QObject>
#include <QContact>
#include <qml.h>


QTM_BEGIN_NAMESPACE
class QmlContact : public QObject {
Q_OBJECT
Q_PROPERTY(QContact contact READ contact WRITE setContact NOTIFY contactChanged)
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(QString test READ test)
Q_PROPERTY(QStringList availableActions READ availableActions)
Q_PROPERTY(QStringList details READ details)
Q_PROPERTY(QStringList contexts READ contexts)
public:
    QmlContact(QContact& contact, QObject *parent = 0);
    QmlContact();
    ~QmlContact();

    QContact& contact();
    void setContact(QContact& contact);

    QString name();
    void setName(QString name);

    QStringList availableActions();
    QStringList details();

    QStringList contexts();

    Q_INVOKABLE QVariantMap values(QString definitionId);

    QString test() { return "test string"; }

signals:
    void contactChanged(QmlContact* qmlcontact);
    void nameChanged(QmlContact* qmlcontact);

private:
    QContact m_contact;
};

QML_DECLARE_TYPE(QmlContact);

QTM_END_NAMESPACE

#endif // QMLCONTACT_H
