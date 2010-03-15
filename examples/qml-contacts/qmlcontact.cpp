#include "qmlcontact.h"
#include <qcontactname.h>
#include <QtDebug>


QT_USE_NAMESPACE
QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

QmlContact::QmlContact(QContact& contact, QObject *parent)
    : QObject(parent), m_contact(contact)
{   
}

QmlContact::QmlContact()
{

}

QmlContact::~QmlContact()
{

}

QContact &QmlContact::contact()
{
    return m_contact;
}

void QmlContact::setContact(QContact& contact)
{
    m_contact = contact;
    emit contactChanged(this);
}

QString QmlContact::name()
{
    QList<QContactDetail> allNames = m_contact.details(QContactName::DefinitionName);

    const QLatin1String space(" ");

    // synthesise the display label from the name.
    for (int i=0; i < allNames.size(); i++) {
        const QContactName& name = allNames.at(i);

        QString result;
        if (!name.value(QContactName::FieldPrefix).trimmed().isEmpty()) {
           result += name.value(QContactName::FieldPrefix);
        }

        if (!name.value(QContactName::FieldFirst).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldFirst);
        }

        if (!name.value(QContactName::FieldMiddle).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldMiddle);
        }

        if (!name.value(QContactName::FieldLast).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldLast);
        }

        if (!name.value(QContactName::FieldSuffix).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldSuffix);
        }

        if (!result.isEmpty()) {
            return result;
        }
    }


    return QString("noName");
}

void QmlContact::setName(QString name)
{
    Q_UNUSED(name);
    qWarning() << "Not implemented yet";
    emit nameChanged(this);
}

QStringList QmlContact::availableActions()
{
    QList<QContactActionDescriptor> actions =  m_contact.availableActions();
    QStringList names;

    foreach (const QContactActionDescriptor& action, actions) {
        names << action.actionName();
    }
    return names;
}
QStringList QmlContact::details()
{
    QStringList dets;
    QList<QContactDetail> ld = m_contact.details();
    QContactDetail d;
    foreach(d, ld){
        dets += d.definitionName();
    }
    return dets;
}

QStringList QmlContact::contexts()
{
    QStringList dets;
    QList<QContactDetail> ld = m_contact.details();
    QContactDetail d;
    foreach(d, ld){
        dets += d.contexts();
    }
    return dets;
}

//QStringList QmlContact::values(QString definitionId)
QVariantMap QmlContact::values(QString definitionId)
{
    QStringList strlist;
    QContactDetail detail = m_contact.detail(definitionId);

    QVariantMap map = detail.values();
    //qWarning() << "Number of e: " << map.count();
    return map;

//    QMap<QString, QVariant>::const_iterator i = map.constBegin();
//    while (i != map.constEnd()) {
//        qWarning() << "Key: " << i.key() << " Value: " << i.value();
//        strlist += i.key() + ": " + i.value().toString();
//        ++i;
//    }
//
//    return strlist;
}

#include "moc_qmlcontact.cpp"
QTM_END_NAMESPACE
QML_DEFINE_TYPE(QmlContact, 1, 0, QmlContact, QmlContact)
