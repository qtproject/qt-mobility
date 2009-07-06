/*
 * Proposal #2.5: per-return type (Verbage) with inheritance
 *
 * This API proposal allows the selection criteria and actions to be
 * separated according to the type of data being operated on.
 * For each type of data that is being operated on, a subclass exists
 * which provides various selection and retrieval functions, along with
 * "verb" functions which perform the described action on the selection.
 *
 * Concerns:
 *   - invalid selection for particular verb?  return error in that case.
 *   - verbs: in base or sub classes?
 *
 *   - partial retrieval (also list of ids versus list of objects)
 *   - extra filtering
 */


/*
 * Filtering.. again..:
 *
 * - presence of details (AND? OR?)
 * - value of details (AND? OR? value ranges?)
 * - presence of details supporting an action (== presence of details, OR)
 * - value of details supporting an action (== value of details, OR again)
 * - last modified/created time? (== since.. not specific ranges?)
 */

class QContactFilter
{
    int type() const;
};

class QContactDetailFilter : public QContactFilter
{
    /* Mutators */
    void setDetailDefinitionId(const QString& id);
    void setDetailFieldId(const QString& field);

    /* ??? pick one */
    void setValue(const QVariant& value);
    void setValues(const QList<QVariant>& values);
    void setRange(const QVariant& min, const QVariant& max);

    /* Accessors */
    QString detailDefinitionId() const;
    QString detailFieldId() const;
    QVariant value() const;
    QVariant minValue() const;
    QVariant maxValue() const;
    QVariantList valueList() const;
};

class QContactChangeLogFilter : public QContactFilter
{
    enum {
        Added,
        Changed,
        Removed
    } ChangeType;

    void setChangeType(ChangeType type);
    void setSince(const QDateTime& since);

    QDateTime since() const;
    ChangeType type() const;
};

class QContactActionFilter : public QContactFilter
{
    void setActionId(const QString& action);
    void setValue(const QVariant& value);

    /* Accessors */
    QString actionId() const;
    QVariant value() const;
};

class QContactBooleanFilter : public QContactFilter
{
    enum {
        And,
        Or
    } Type;

    QContactBooleanFilter(Type type = And);
    void setType(Type type);
    void setFilters(const QList<QContactFilter>& filters);

    void append(const QContactFilter& filter);
    void remove(const QContactFilter& filter);

    QContactBooleanFilter& operator<<(const QContactFilter& filter);

    /* Accessors */
    Type type() const;
    QList<QContactFilter> filters() const;
};

/* ======= Sort order ======== */
class QContactSortOrder
{
    enum {
        BlanksFirst,
        BlanksLast,
    } BlankPolicy;

    void setDetail(const QString& definitionId, const QString& fieldId);
    void setDirection(Qt::SortOrder direction);
    void setBlankPolicy(BlankPolicy blankPolicy); // XXX might depend on SQL engine

    /* Accessors */
    QString detailDefinitionId() const;
    QString detailFieldId() const;
    Qt::SortOrder direction() const;
    BlankPolicy blankPolicy() const;
};

/* ======= Request classes ======== */
class QContactAbstractRequest : public QObject
{
public:
    QContactAbstractRequest(QContactManager *m);

    enum Status {
        Inactive = 0,
        Pending,
        Finished,
        Cancelling,
        Cancelled
    };

    bool isFinished() const;

    QContactManager::Error error() const;
    Status status() const;

public slots:
    /* waiting for stuff */
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0); /* also waits for progress emitted by finished */

    /* Verbs */
    void cancel();
};

/*
 * Fetch/save/remove contacts, contact ids and contact changelogs
 */
class QContactRequest : public QContactAbstractRequest
{
public:
    /* Filtering */
    void selectById(const QList<QUniqueId>& contactIds);
    void selectByObject(const QList<QContact>& contacts);
    void selectByFilter(const QContactFilter& filter);

    /* Sorting */
    void setSortOrder(const QContactSortOrder& order);
    void setSortOrder(const QList<QContactSortOrder>& order);
    QList<QContactSortOrder> sortOrder() const;

    /* What to get/store */
    void clearRestrictions(); // full contact
    void restrictToIds(); // just ids
    void restrictToDetails(QStringList detailDefinitionIds); // partial contact

public slots:
    void startSave();
    void startRemove();
    void startRetrieve();

public:
    /* Results */
    QList<QUniqueId> ids();     // if restrictToIds is set
    QList<QContact> contacts(); // otherwise (full and partial contacts)
    QList<QContactManager::Error> errors();

signals:
    void finished(QContactRequest* self);
    void progress(QContactRequest* self, bool appendOnly); // appendOnly true if ids/contacts order has not changed, just stuff added
};

/*
 * Fetch/save/remove groups, group ids and group changelogs.
 */
class QContactGroupRequest : public QContactAbstractRequest
{
public:
    void selectById(const QList<QUniqueId>& groupIds);
    void selectByObject(const QList<QContactGroup>& groups);

    /* Groups are sorted by name */

    /* can just fetch ids */
    void clearRestrictions(); // full contact
    void restrictToIds(); // just ids

public slots:
    void startSave();
    void startRemove();
    void startRetrieve();

public:
    /* Results */
    QList<QUniqueId> ids();
    QList<QContactGroup> groups();
    QList<QContactManager::Error> errors();

signals:
    void finished(QContactGroupRequest* self);
    void progress(QContactGroupRequest* self, bool appendOnly); // appendOnly true if ids/contacts order has not changed, just stuff added
};

/*
 * Fetch/save/remove detail definition
 */
class QContactDetailDefinitionRequest : public QContactAbstractRequest
{
public:
    void selectById(const QList<QString>& definitionIds);
    void selectByObject(const QList<QContactDetailDefinition>& definitions);

public slots:
    void startSave();
    void startRemove();
    void startRetrieve();

public:
    /* Results */
    QList<QContactDetailDefinition> definitions();
    QList<QContactManager::Error> errors(); // XXX Not needed?

signals:
    void finished(QContactDetailDefinitionRequest* self);
    void progress(QContactDetailDefinitionRequest* self, bool appendOnly); // appendOnly true if ids/contacts order has not changed, just stuff added
};

/* Fetch the capabilities of a manager */
class QContactCapabilitiesRequest : public QContactRequest
{
public slots:
    void startRetrieve();

public:
    /* Results */
    QContactManagerCapabilities capabilities();

signals:
    void finished(QContactCapabilitiesRequest* self);
    // no progress signal
};

/* ========================= Manager changes ========================= */

class QTCONTACTS_EXPORT QContactManager : public QObject
{
    /* Contacts - Accessors and Mutators */
    QContact contact(const QUniqueId& contactId) const;  // retrieve a contact
    QList<QUniqueId> contacts() const;                  // retrieve contact ids
    // removed - QList<QUniqueId> contacts(const QString& definitionId, const QVariant& value = QVariant()) const;
    // removed - QList<QUniqueId> contactsWithAction(const QString& actionId, const QVariant& value = QVariant()) const;
    QList<QUniqueId> contacts(const QContactFilter& filter) const;

    QList<QUniqueId> groups(const QContactFilter& filter) const; // doesn't like all filters

    /* Changelog Functions also covered by contacts & groups */
    //QList<QUniqueId> contactsAddedSince(const QDateTime& timestamp) const;
    //QList<QUniqueId> contactsModifiedSince(const QDateTime& timestamp) const;
    //QList<QUniqueId> contactsRemovedSince(const QDateTime& timestamp) const;
    //QList<QUniqueId> groupsAddedSince(const QDateTime& timestamp) const;
    //QList<QUniqueId> groupsModifiedSince(const QDateTime& timestamp) const;
    //QList<QUniqueId> groupsRemovedSince(const QDateTime& timestamp) const;

};

/* ========================= Alternative Manager changes ========================= */

/* Remove nearly everything and use QContactRequest for all access */

/* ========================= Sample Code ========================= */
class SampleApp
{
public:
    SampleApp();
    ~SampleApp();

    void syncOvi();
    void matchIncoming(const QString& incoming);

private:
    void listOvi();
    void emailableContacts();

private slots:
    void gotContacts(QContactRequest* r);
    void gotOviContacts(QContactRequest* r);
    void gotIncoming(QContactRequest* r);
    void gotEmailContacts(QContactRequest* r);

private:
    // request objects must be allocated on heap to ensure signal/slot connections.
    QContactRequest *cr;
};

/* ========================= Use case #1 - synchronising Ovi contacts ========================= */
void SampleApp::syncOvi()
{
    /* Find out what Ovi contacts have changed since last sync - use synchronous methods for illumination */
    QContactRequest r;

    /* Grab the added ones first */
    QContactChangeLogFilter crlf;
    crlf.setSince(lastSyncTime);
    crlf.setChangeType(QContactChangeLogFilter::Added);

    QContactDetailFilter crdf;
    crdf.setDetailDefinitionId(QContactSyncTarget::DefinitionId);
    crdf.setValue("Ovi");

    QContactBooleanFilter caf(QContactBooleanFilter::And);
    caf << crlf << crdf;

    r.selectByFilter(caf);
    r.startRetrieve();

    /* Sync case: */
    r.waitForFinished();

    /* Get the results */
    QList<QContact> contacts = r.contacts();

    /* Do stuff with them etc */
    sendToOvi(contacts);

    /* Now get the removed ones */
    caf.remove(crlf);
    crlf.setChangeType(QContactChangeLogFilter::Removed);
    caf.append(crlf);

    r.selectByFilter(caf);
    // Just get ths ids this time
    r.restrictToIds();
    r.startRetrieve();

    r.waitForFinished();
    QList<QUniqueId> ids = r.ids();

    /* Process them somehow */
    sendDeletedtoOvi(ids);

    QList<QContact> addedOvi = getContactsAddedByOvi();
    QContactRequest r2;
    r2.selectByObject(addedOvi);
    r2.startSave();

    r.waitForFinished();

    if (r.error() == QContactManager::NoError) {
        // rejoice
    } else {
        // Check each error
        QList<QContactManager::Error> errors = r.errors();
    }
}

/* ========================= Use case #2 - collection list view retrieval ========================= */
void SampleApp::listOvi()
{
    /* Select all contacts whose syncTarget detail is set to "Ovi" */
    cr = new QContactRequest;

    QContactDetailFilter crdf;
    crdf.setDetailDefinitionId(QContactSyncTarget::DefinitionId);
    crdf.setValue("Ovi");

    cr->selectByFilter(crdf);

    connect(cr, SIGNAL(finished(QContactRequest*)), this, SLOT(gotOviContacts(QContactRequest*)));
    cr->startRetrieve();
}
void SampleApp::gotOviContacts(QContactRequest* request)
{
    QList<QContact> oviContacts = request->contacts();
    displayContacts(oviContacts); // do whatever with them

    /* clean up */
    delete request;
}

/* ========================= Use case #3 - match incoming call ========================= */
void SampleApp::matchIncoming(const QString& number)
{
    /* Select all contacts who have an action available for a detail whose value is the incoming number */
    cr = new QContactRequest;

    QContactActionFilter caf;
    caf.setActionId("com.nokia.gsm.dial");
    caf.setValue(number);

    cr->selectFilter(caf);

    connect(cr, SIGNAL(finished(QContactRequest*)), this, SLOT(gotIncoming(QContactRequest*)));
    cr->startRetrieve();
}
void SampleApp::gotIncoming(QContactRequest* request)
{
    QList<QContact> incoming = request->contacts();
    if (incoming.isEmpty())
        display("Unknown Caller!");
    else
        display("Incoming call from " + incoming.at(0).name().displayName());

    /* clean up */
    delete request;
}

/* ========================= Use case #4 - retrieve list of contacts I can email ========================= */
void SampleApp::emailableContacts()
{
    /* Select all contacts who have the "email" action available */
    cr = new QContactRequest;
    QContactActionFilter caf;
    caf.setActionId("com.nokia.messaging.email"); // default-constructed QVariant means "any value"

    cr->selectFilter(caf);

    connect(cr, SIGNAL(finished(QContactRequest*)), this, SLOT(gotEmailContacts(QContactRequest*)));
    cr->startRetrieve();
}
void SampleApp::gotEmailContacts(QContactRequest* request)
{
    QList<QContact> emailable = request->contacts();
    displayContacts(emailable); // do whatever with them

    /* clean up */
    delete request;
}
