#include "ut_qtcontacts_common.h"

#include <QContactFetchRequest>
#include <QContactLocalIdFilter>
#include <QContactRemoveRequest>
#include <QContactSaveRequest>

ut_qtcontacts_common::ut_qtcontacts_common()
  : mContactManager(0)
{
}

void ut_qtcontacts_common::initTestCase()
{
}

void ut_qtcontacts_common::cleanupTestCase()
{
}

void ut_qtcontacts_common::init()
{
    QVERIFY(0 == mContactManager);
    mContactManager = new QContactManager("tracker");
    QVERIFY(0 != mContactManager);
}

void ut_qtcontacts_common::cleanup()
{
    if (mContactManager) {
        if (not mLocalIds.isEmpty()) {
#if 0 // FIXME: qtcontacts-tracker doesn't implement QContactRemoveRequest yet
            QContactRemoveRequest request;
            request.setManager(mContactManager);
            request.setContactIds(mLocalIds);

            if (not request.start())
                qDebug() << "error code" << request.error();
            //QVERIFY(request.start());

            waitForRequest(request);
            CHECK_CURRENT_TEST_FAILED;

            QVERIFY(request.isFinished());
            QCOMPARE(request.error(), QContactManager::NoError);
#else
            QMap<int, QContactManager::Error> errors;
            bool success = mContactManager->removeContacts(&mLocalIds, &errors);
            QCOMPARE(mContactManager->error(), QContactManager::NoError);
            QVERIFY(errors.isEmpty());
            QVERIFY(success);
#endif

            mLocalIds.clear();
        }

        mContactManager->deleteLater();
        mContactManager = 0;
    }
}

void ut_qtcontacts_common::waitForRequest(QContactAbstractRequest &request, int ms)
{
    if (request.isFinished())
        return;

    // check pre-conditions
    QCOMPARE((int) request.state(), (int) QContactAbstractRequest::ActiveState);

    // wait for the request to do its work (or get canceled)
    QTime timer;
    timer.start();

    while (request.isActive() && timer.elapsed() < ms) {
        QTest::qWait(10);
    }

    // check post-conditions
    QVERIFY2(not request.isActive(), "timeout expired");

    qDebug() << request.metaObject()->className() << "finished after" << timer.elapsed() << "ms";
}

// FIXME: remove again once QtMobility provides more verbose contact validation utilities
static bool validateContact(QContactManager *manager, const QContact &contact, QContactManager::Error &error_, QString &what)
{
    QList<QString> uniqueDefinitionIds;

    // check that each detail conforms to its definition as supported by this manager.
    for (int i=0; i < contact.details().count(); i++) {
        const QContactDetail& d = contact.details().at(i);
        QVariantMap values = d.variantValues();
        QContactDetailDefinition def = manager->detailDefinition(d.definitionName(), contact.type());
        // check that the definition is supported
        if (def.isEmpty()) {
            error_ = QContactManager::InvalidDetailError;
            what = "Unsupported definition: " + d.definitionName();
            return false;
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.name())) {
                error_ = QContactManager::AlreadyExistsError;
                what = "Detail must be unique: " + d.definitionName();
                return false;
            }
            uniqueDefinitionIds.append(def.name());
        }

        QList<QString> keys = values.keys();
        for (int i=0; i < keys.count(); i++) {
            const QString& key = keys.at(i);
            // check that no values exist for nonexistent fields.
            if (!def.fields().contains(key)) {
                error_ = QContactManager::InvalidDetailError;
                what = "Value for nonexistent field: " + d.definitionName() + "::" + key;
                return false;
            }

            QContactDetailFieldDefinition field = def.fields().value(key);
            // check that the type of each value corresponds to the allowable field type
            if (static_cast<int>(field.dataType()) != values.value(key).userType()) {
                error_ = QContactManager::InvalidDetailError;
                what = "Type doesn't match: " + d.definitionName() + "::" + key;
                return false;
            }

            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues().isEmpty()) {
                // if the field datatype is a list, check that it contains only allowable values
                if (field.dataType() == QVariant::List || field.dataType() == QVariant::StringList) {
                    QList<QVariant> innerValues = values.value(key).toList();
                    for (int i = 0; i < innerValues.size(); i++) {
                        if (!field.allowableValues().contains(innerValues.at(i))) {
                            error_ = QContactManager::InvalidDetailError;
                            what = QString("Value not allowed: %1 (%2)").
                                   arg(d.definitionName() + "::" + key,
                                       innerValues.at(i).toString());
                            return false;
                        }
                    }
                } else if (!field.allowableValues().contains(values.value(key))) {
                    // the datatype is not a list; the value wasn't allowed.
                    error_ = QContactManager::InvalidDetailError;
                    what = QString("Value not allowed: %1 (%2)").
                           arg(d.definitionName() + "::" + key,
                               values.value(key).toString());
                    return false;
                }
            }
        }
    }

    return true;
}

void ut_qtcontacts_common::saveContact(QContact &contact, int timeout)
{
    QList<QContact> contactList;
    contactList.append(contact);

    saveContacts(contactList, timeout);

    QCOMPARE(contactList.count(), 1);
    contact = contactList[0];
}

void ut_qtcontacts_common::saveContacts(QList<QContact> &contacts, int timeout)
{
    QVERIFY(not contacts.isEmpty());

    foreach(const QContact &contact, contacts) {
        QContactManager::Error error;
        QString what;

        if (not validateContact(mContactManager, contact, error, what)) {
            foreach(const QContactDetail &d, contact.details()) {
                qDebug() << d.definitionName() << d.variantValues();
            }

            QFAIL(qPrintable(QString("error %1: %2").arg(error).arg(what)));
        }
    }

    // add the contact to database
    QContactSaveRequest request;
    request.setManager(mContactManager);
    request.setContacts(contacts);
    QVERIFY(request.start());

    qDebug() << "saving" << request.contacts().count() << "contacts";
    waitForRequest(request, timeout);
    CHECK_CURRENT_TEST_FAILED;

    // verify the contact got saved
    QVERIFY(request.isFinished());

    QCOMPARE((int) request.error(),
             (int) QContactManager::NoError);

    // copy back the saved contacts
    contacts = request.contacts();

    // remember the local id so that we can remove the contact from database later
    foreach(const QContact &contact, contacts) {
        QVERIFY(contact.localId());
        mLocalIds.append(contact.localId());
    }
}

void ut_qtcontacts_common::fetchContact(const QContactLocalId &id,
                                        QContact &result, int timeout)
{
    QList<QContact> contactList;
    fetchContacts(QList<QContactLocalId>() << id, contactList, timeout);
    QCOMPARE(contactList.count(), 1);
    result = contactList[0];
}

void ut_qtcontacts_common::fetchContact(const QContactFilter &filter,
                                        QContact &result, int timeout)
{
    QList<QContact> contactList;
    fetchContacts(filter, contactList, timeout);
    QCOMPARE(contactList.count(), 1);
    result = contactList[0];
}

void ut_qtcontacts_common::fetchContacts(const QList<QContactLocalId> &ids,
                                         QList<QContact> &result, int timeout)
{
    QContactLocalIdFilter filter;

    filter.setIds(ids);
    fetchContacts(filter, result, timeout);
    CHECK_CURRENT_TEST_FAILED;

    QCOMPARE(result.count(), ids.count());
}

void ut_qtcontacts_common::fetchContacts(const QContactFilter &filter,
                                         QList<QContact> &result, int timeout)
{
    QContactFetchRequest request;

    request.setManager(mContactManager);

    if (QContactFilter::InvalidFilter != filter.type())
        request.setFilter(filter);

    QVERIFY(request.start());

    qDebug() << "fetching contacts";
    waitForRequest(request, timeout);
    CHECK_CURRENT_TEST_FAILED;

    QVERIFY(request.isFinished());
    result = request.contacts();
}

QSet<QString> ut_qtcontacts_common::testSlotNames()
{
    QSet<QString> testSlots;

    for(int i = 0; i < metaObject()->methodCount(); ++i) {
        const QMetaMethod &method = metaObject()->method(i);

        if (QMetaMethod::Private != method.access() ||
            QMetaMethod::Slot != method.methodType()) {
            continue;
        }

        const char *signature = method.signature();
        const char *parenthesis = strchr(signature, '(');

        if (0 != qstrcmp(parenthesis, "()")) {
            continue;
        }

        testSlots.insert(QString::fromLatin1(signature, parenthesis - signature));
    }

    return testSlots;
}
