// lucky-two
// this file contains a proposal for per-datatype, per-operation
// asynchronous request classes.

#ifndef QCONTACTABSTRACTREQUEST_H
#define QCONTACTABSTRACTREQUEST_H

#include "qcontactmanager.h"
#include <QObject>

class QContactAbstractRequestData;
class QTCONTACTS_EXPORT QContactAbstractRequest : public QObject
{
    Q_OBJECT

public:
    QContactAbstractRequest() {}
    virtual ~QContactAbstractRequest() {}

    enum Status {
        Inactive = 0,   // operation not yet started
        Pending,        // operation started, not yet finished
        Cancelling,     // operation started then cancelled, not yet finished
        Cancelled,      // operation is finished due to cancellation
        Finished        // operation successfully completed
    };

    Status status() const;
    bool isActive() const;
    bool isFinished() const;
    QList<QContactManager::Error> errors();
    QContactManager::Error error() const;

    enum Type {
        Invalid = 0,
        Contact,
        DetailDefinition,
        Group
    };

    virtual Type type() const = 0;

    /* Which manager we want to perform the asynchronous request */
    QContactManager* manager() const;
    void setManager(QContactManager* manager);

public slots:
    /* Verbs */
    virtual bool start() = 0;
    virtual bool cancel() = 0;

    /* waiting for stuff */
    virtual bool waitForFinished(int msecs = 0) = 0;
    virtual bool waitForProgress(int msecs = 0) = 0; /* also waits for progress emitted by finished */

protected:
    QContactAbstractRequest(QContactAbstractRequestData* otherd);

private:
    Q_DISABLE_COPY(QContactAbstractRequest);
    QContactAbstractRequestData* d;
};

#endif






#ifndef QCONTACTABSTRACTREQUEST_P_H
#define QCONTACTABSTRACTREQUEST_P_H

#include "qcontactmanager.h"
#include "qcontactabstractrequest.h"

#include <QList>

class QContactAbstractRequestData
{
public:
    QContactAbstractRequestData()
        : m_error(QContactManager::NoError),
            m_status(QContactAbstractRequest::Inactive),
            m_manager(0)
    {
    }

    ~QContactAbstractRequestData()
    {
    }

    QContactManager::Error m_error;
    QContactAbstractRequest::Status m_status;
    QContactManager* m_manager;
    QList<QContactManager::Error> m_errors;
};

#endif



// qcontactabstractrequest.cpp
#include "qcontactabstractrequest.h"
#include "qcontactabstractrequest_p.h"
#include "qcontactmanager.h"
QContactAbstractRequest::QContactAbstractRequest(QContactAbstractRequest* otherd)
    : d(otherd)
{
}

bool QContactAbstractRequest::isActive() const
{
    return (d->m_status == QContactAbstractRequest::Pending
            || d->m_status == QContactAbstractRequest::Cancelling);
}

bool QContactAbstractRequest::isFinished() const
{
    return (d->m_status == QContactAbstractRequest::Finished
            || d->m_status == QContactAbstractRequest::Cancelled);
}

QContactManager::Error QContactAbstractRequest::error() const
{
    return d->m_error;
}

QContactAbstractRequest::Status QContactAbstractRequest::status() const
{
    return d->m_status;
}

QContactManager* QContactAbstractRequest::manager() const
{
    return d->m_manager;
}

void QContactAbstractRequest::setManager(QContactManager* manager)
{
    d->m_manager = manager;
}
//-----------end.





#ifndef QCONTACTSAVEREQUEST_H
#define QCONTACTSAVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontact.h"

#include <QList>

class QTCONTACTS_EXPORT QContactSaveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactSaveRequest();
    ~QContactSaveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::ContactSave;}

    /* Selection */
    void setContacts(const QList<QContact>& contacts);

    /* Results */
    QList<QContact> contacts() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactSaveRequest* self);

private:
    Q_DISABLE_COPY(QContactSaveRequest);
};

#endif

#ifndef QCONTACTSAVEREQUEST_P_H
#define QCONTACTSAVEREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontact.h"
#include <QList>

class QContactSaveRequestData : public QContactAbstractRequestData
{
public:
    QContactSaveRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactSaveRequestData()
    {
    }

    QList<QContact> m_contacts;
};

#endif


// qcontactsaverequest.cpp
#include "qcontactsaverequest.h"
#include "qcontactsaverequest_p.h"
QContactSaveRequest::QContactSaveRequest()
    : QContactAbstractRequest(new QContactSaveRequestData)
{
}

QContactSaveRequest::~QContactSaveRequest()
{
}

void QContactSaveRequest::setContacts(const QList<QContact>& contacts)
{
    d->m_contacts = contacts;
}

QList<QContact> QContactSaveRequest::contacts() const
{
    return d->m_contacts;
}

bool QContactSaveRequest::start()
{
    return false;
}

bool QContactSaveRequest::cancel()
{
    return false;
}

bool QContactSaveRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactSaveRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//----------------end



#ifndef QCONTACTFETCHREQUEST_H
#define QCONTACTFETCHREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactsortorder.h"
#include "qcontactfilter.h"
#include "qcontact.h"

#include <QList>
#include <QStringList>

class QTCONTACTS_EXPORT QContactFetchRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactFetchRequest();
    ~QContactFetchRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::ContactFetch;}

    /* Selection, restriction and sorting */
    void setFilter(const QContactFilter& filter);
    void setSorting(const QList<QContactSortOrder>& sorting);
    void setDefinitionRestrictions(const QStringList& definitionNames);
    QContactFilter filter() const;
    QList<QContactSortOrder> sorting() const;
    QStringList definitionRestrictions() const;

    /* Results */
    QList<QUniqueId> ids() const;
    QList<QContact> contacts() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactFetchRequest* self, bool appendOnly);

private:
    Q_DISABLE_COPY(QContactFetchRequest);
};

#endif


#ifndef QCONTACTFETCHREQUEST_P_H
#define QCONTACTFETCHREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactfilter.h"
#include "qcontactsortorder.h"
#include "qtcontactsglobal.h"
#include <QStringList>
#include <QList>

class QContactFetchRequestData : public QContactAbstractRequestData
{
public:
    QContactFetchRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactFetchRequestData()
    {
    }

    QContactFilter m_filter;
    QList<QContactSortOrder> m_sorting;
    QStringList m_definitionRestrictions;

    QList<QUniqueId> m_ids;
    QList<QContact> m_contacts;
};

#endif

// qcontactfetchrequest.cpp
#include "qcontactfetchrequest.h"
#include "qcontactfetchrequest_p.h"

QContactFetchRequest::QContactFetchRequest()
    : QContactAbstractRequest(new QContactFetchRequestData)
{
}

QContactFetchRequest::~QContactFetchRequest()
{
}

void QContactFetchRequest::setFilter(const QContactFilter& filter)
{
    d->m_filter = filter;
}

void QContactFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    d->m_sorting = sorting;
}

void QContactFetchRequest::setDefinitionRestrictions(const QStringList& definitionNames)
{
    d->m_definitionRestrictions = definitionNames;
}

QContactFilter QContactFetchRequest::filter() const
{
    return d->m_filter;
}

QList<QContactSortOrder> QContactFetchRequest::sorting() const
{
    return d->m_sorting;
}

QStringList QContactFetchRequest::definitionRestrictions() const
{
    return d->m_definitionRestrictions;
}

QList<QUniqueId> QContactFetchRequest::ids() const
{
    return d->m_ids;
}

QList<QContact> QContactFetchRequest::contacts() const
{
    return d->m_contacts;
}

bool QContactFetchRequest::start()
{
    return false;
}

bool QContactFetchRequest::cancel()
{
    return false;
}

bool QContactFetchRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactFetchRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//----------------end





#ifndef QCONTACTREMOVEREQUEST_H
#define QCONTACTREMOVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactfilter.h"

#include <QList>

class QTCONTACTS_EXPORT QContactRemoveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactRemoveRequest();
    ~QContactRemoveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::ContactRemove;}

    /* Selection */
    void setFilter(const QContactFilter& filter);
    QContactFilter filter() const;

    /* Results */
    QList<QUniqueId> ids() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactRemoveRequest* self);

private:
    Q_DISABLE_COPY(QContactRemoveRequest);
};

#endif

#ifndef QCONTACTREMOVEREQUEST_P_H
#define QCONTACTREMOVEREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactfilter.h"
#include "qtcontactsglobal.h"
#include <QList>

class QContactRemoveRequestData : public QContactAbstractRequestData
{
public:
    QContactRemoveRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactRemoveRequestData()
    {
    }

    QContactFilter m_filter;
    QList<QUniqueId> m_ids;
};

#endif

// qcontactremoverequest.cpp
#include "qcontactremoverequest.h"
#include "qcontactremoverequest_p.h"

QContactRemoveRequest::QContactRemoveRequest()
    : QContactAbstractRequest(new QContactRemoveRequestData)
{
}

QContactRemoveRequest::~QContactRemoveRequest()
{
}

void QContactRemoveRequest::setFilter(const QContactFilter& filter)
{
    d->m_filter = filter;
}

QContactFilter QContactRemoveRequest::filter() const
{
    return d->m_filter;
}

QList<QUniqueId> QContactRemoveRequest::ids() const
{
    return d->m_ids;
}

bool QContactRemoveRequest::start()
{
    return false;
}

bool QContactRemoveRequest::cancel()
{
    return false;
}

bool QContactRemoveRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactRemoveRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//---------------------end






#ifndef QCONTACTIDFETCHREQUEST_H
#define QCONTACTIDFETCHREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactsortorder.h"
#include "qcontactfilter.h"

#include <QList>
#include <QStringList>

class QTCONTACTS_EXPORT QContactIdFetchRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactIdFetchRequest();
    ~QContactIdFetchRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::ContactIdFetch;}

    /* Selection, restriction and sorting */
    void setFilter(const QContactFilter& filter);
    void setSorting(const QList<QContactSortOrder>& sorting);
    void setDefinitionRestrictions(const QStringList& definitionNames);
    QContactFilter filter() const;
    QList<QContactSortOrder> sorting() const;
    QStringList definitionRestrictions() const;

    /* Results */
    QList<QUniqueId> ids() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactIdFetchRequest* self, bool appendOnly);

private:
    Q_DISABLE_COPY(QContactIdFetchRequest);
};

#endif


#ifndef QCONTACTIDFETCHREQUEST_P_H
#define QCONTACTIDFETCHREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactsortorder.h"
#include "qcontactfilter.h"
#include "qtcontactsglobal.h"
#include <QList>
#include <QStringList>

class QContactIdFetchRequestData : public QContactAbstractRequestData
{
public:
    QContactIdFetchRequestData
        : QContactAbstractRequestData()
    {
    }

    ~QContactIdFetchRequestData
    {
    }

    QContactFilter m_filter;
    QList<QContactSortOrder> m_sorting;
    QStringList m_definitionRestrictions;

    QList<QUniqueId> m_ids;
};

#endif


// qcontactidfetchrequest.cpp
#include "qcontactidfetchrequest.h"
#include "qcontactidfetchrequest_p.h"

QContactIdFetchRequest::QContactIdFetchRequest()
    : QContactAbstractRequest(new QContactIdFetchRequestData)
{
}

QContactIdFetchRequest::~QContactIdFetchRequest()
{
}


void QContactIdFetchRequest::setFilter(const QContactFilter& filter)
{
    d->m_filter = filter;
}

void QContactIdFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    d->m_sorting = sorting;
}

void QContactIdFetchRequest::setDefinitionRestrictions(const QStringList& definitionNames)
{
    d->m_definitionRestrictions = definitionNames;
}

QContactFilter QContactIdFetchRequest::filter() const
{
    return d->m_filter;
}

QList<QContactSortOrder> QContactIdFetchRequest::sorting() const
{
    return d->m_sorting;
}

QStringList QContactIdFetchRequest::definitionRestrictions() const
{
    return d->m_definitionRestrictions;
}

QList<QUniqueId> QContactIdFetchRequest::ids() const
{
    return d->m_ids;
}


bool QContactIdFetchRequest::start()
{
    return false;
}

bool QContactIdFetchRequest::cancel()
{
    return false;
}

bool QContactIdFetchRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactIdFetchRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//-----------------------end






#ifndef QCONTACTGROUPFETCHREQUEST_H
#define QCONTACTGROUPFETCHREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactgroup.h"

#include <QList>

class QTCONTACTS_EXPORT QContactGroupFetchRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactGroupFetchRequest();
    ~QContactGroupFetchRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::GroupFetch;}

    /* Selection */
    void setIds(const QList<QUniqueId>& ids);
    QList<QUniqueId> ids() const;

    /* Results */
    QList<QContactGroup> groups() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactGroupFetchRequest* self, bool appendOnly);

private:
    Q_DISABLE_COPY(QContactGroupFetchRequest);
};

#endif


#ifndef QCONTACTGROUPFETCHREQUEST_P_H
#define QCONTACTGROUPFETCHREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactgroup.h"
#include "qtcontactsglobal.h"
#include <QList>

class QContactGroupFetchRequestData : public QContactAbstractRequestData
{
    QContactGroupFetchRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactGroupFetchRequestData()
    {
    }

    QList<QUniqueId> m_ids;
    QList<QContactGroup> m_groups;
};

#endif

// qcontactgroupfetchrequest.cpp
#include "qcontactgroupfetchrequest.h"
#include "qcontactgroupfetchrequest_p.h"

QContactGroupFetchRequest::QContactGroupFetchRequest()
    : QContactAbstractRequest(new QContactGroupFetchRequestData)
{
}

QContactGroupFetchRequest::~QContactGroupFetchRequest()
{
}

void QContactGroupFetchRequest::setIds(const QList<QUniqueId>& ids)
{
    d->m_ids = ids;
}

QList<QUniqueId> QContactGroupFetchRequest::ids() const
{
    return d->m_ids;
}

QList<QContactGroup> QContactGroupFetchRequest::groups() const
{
    return d->m_groups;
}

bool QContactGroupFetchRequest::start()
{
    return false;
}

bool QContactGroupFetchRequest::cancel()
{
    return false;
}

bool QContactGroupFetchRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactGroupFetchRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//-----------------------end





#ifndef QCONTACTGROUPSAVEREQUEST_H
#define QCONTACTGROUPSAVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactgroup.h"

#include <QList>

class QTCONTACTS_EXPORT QContactGroupSaveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactGroupSaveRequest();
    ~QContactGroupSaveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::GroupSave;}

    /* Selection */
    void setGroups(const QList<QContactGroup>& groups);

    /* Results */
    QList<QContactGroup> groups() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactGroupSaveRequest* self);

private:
    Q_DISABLE_COPY(QContactGroupSaveRequest);
};

#endif



#ifndef QCONTACTGROUPSAVEREQUEST_P_H
#define QCONTACTGROUPSAVEREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactgroup.h"
#include <QList>

class QContactGroupSaveRequestData : public QContactAbstractRequestData
{
public:
    QContactGroupSaveRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactGroupSaveRequestData()
    {
    }

    QList<QContactGroup> m_groups;

};

#endif

// qcontactgroupsaverequest.cpp
#include "qcontactgroupsaverequest.h"
#include "qcontactgroupsaverequest_p.h"

QContactGroupSaveRequest::QContactGroupSaveRequest()
    : QContactAbstractRequest(new QContactGroupSaveRequestData)
{
}

QContactGroupSaveRequest::~QContactGroupSaveRequest()
{
}

void QContactGroupSaveRequest::setGroups(const QList<QContactGroup>& groups)
{
    d->m_groups = groups;
}

QList<QContactGroup> QContactGroupSaveRequest::groups() const
{
    return d->m_groups;
}

bool QContactGroupSaveRequest::start()
{
    return false;
}

bool QContactGroupSaveRequest::cancel()
{
    return false;
}

bool QContactGroupSaveRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactGroupSaveRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//------------------end







#ifndef QCONTACTGROUPREMOVEREQUEST_H
#define QCONTACTGROUPREMOVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"

#include <QList>

class QTCONTACTS_EXPORT QContactGroupRemoveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactGroupRemoveRequest();
    ~QContactGroupRemoveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::GroupRemove;}

    /* Selection */
    void setIds(const QList<QUniqueId>& ids);

    /* Results */
    QList<QUniqueId> ids() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactGroupRemoveRequest* self);

private:
    Q_DISABLE_COPY(QContactGroupRemoveRequest);
};

#endif


#ifndef QCONTACTGROUPREMOVEREQUEST_P_H
#define QCONTACTGROUPREMOVEREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qtcontactsglobal.h"
#include <QList>

class QContactGroupRemoveRequestData : public QContactAbstractRequestData
{
public:
    QContactGroupRemoveRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactGroupRemoveRequestData()
    {
    }

    QList<QUniqueId> m_ids;
};

#endif

// qcontactgroupremoverequest.cpp
#include "qcontactgroupremoverequest.h"
#include "qcontactgroupremoverequest_p.h"

QContactGroupRemoveRequest::QContactGroupRemoveRequest()
    : QContactAbstractRequest(new QContactGroupRemoveRequestData)
{
}

QContactGroupRemoveRequest::~QContactGroupRemoveRequest()
{
}

void QContactGroupRemoveRequest::setIds(const QList<QUniqueId>& ids)
{
    d->m_ids = ids;
}

QList<QUniqueId> QContactGroupRemoveRequest::ids() const
{
    return d->m_ids;
}

bool QContactGroupRemoveRequest::start()
{
    return false;
}

bool QContactGroupRemoveRequest::cancel()
{
    return false;
}

bool QContactGroupRemoveRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactGroupRemoveRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//---------------------end




#ifndef QCONTACTDETAILDEFINITIONFETCHREQUEST_H
#define QCONTACTDETAILDEFINITIONFETCHREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactdetaildefinition.h"

#include <QList>
#include <QStringList>

class QTCONTACTS_EXPORT QContactDetailDefinitionFetchRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactDetailDefinitionFetchRequest();
    ~QContactDetailDefinitionFetchRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::DetailDefinitionFetch;}

    /* Selection */
    void setNames(const QStringList& names);
    QStringList names() const;

    /* Results */
    QList<QContactDetailDefinition> definitions() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactDetailDefinitionFetchRequest* self, bool appendOnly);

private:
    Q_DISABLE_COPY(QContactDetailDefinitionFetchRequest);
};

#endif


#ifndef QCONTACTDETAILDEFINITIONFETCHREQUEST_P_H
#define QCONTACTDETAILDEFINITIONFETCHREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactdetaildefinition.h"
#include <QStringList>
#include <QList>

class QContactDetailDefinitionFetchRequestData : public QContactAbstractRequestData
{
public:
    QContactDetailDefinitionFetchRequest()
        : QContactAbstractRequest()
    {
    }

    ~QContactDetailDefinitionFetchRequest()
    {
    }

    QStringList m_names;
    QList<QContactDetailDefinition> m_definitions;
};

#endif

// qcontactdetaildefinitionfetchrequest.cpp
#include "qcontactdetaildefinitionfetchrequest.h"
#include "qcontactdetaildefinitionfetchrequest_p.h"

QContactDetailDefinitionFetchRequest::QContactDetailDefinitionFetchRequest()
    : QContactAbstractRequest(new QContactDetailDefinitionFetchRequestData)
{
}

QContactDetailDefinitionFetchRequest::~QContactDetailDefinitionFetchRequest()
{
}

void QContactDetailDefinitionFetchRequest::setNames(const QStringList& names)
{
    d->m_names = names;
}

QStringList QContactDetailDefinitionFetchRequest::names() const
{
    return d->m_names;
}

QList<QContactDetailDefinition> QContactDetailDefinitionFetchRequest::definitions() const
{
    return d->m_definitions;
}

bool QContactDetailDefinitionFetchRequest::start()
{
    return false;
}

bool QContactDetailDefinitionFetchRequest::cancel()
{
    return false;
}

bool QContactDetailDefinitionFetchRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactDetailDefinitionFetchRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//--------------------------------end





#ifndef QCONTACTDETAILDEFINITIONSAVEREQUEST_H
#define QCONTACTDETAILDEFINITIONSAVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactdetaildefinition.h"

#include <QList>
#include <QStringList>

class QTCONTACTS_EXPORT QContactDetailDefinitionSaveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactDetailDefinitionSaveRequest();
    ~QContactDetailDefinitionSaveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::DetailDefinitionSave;}

    /* Selection */
    void setDefinitions(const QList<QContactDetailDefinition>& definitions);

    /* Results */
    QList<QContactDetailDefinition> definitions() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactDetailDefinitionSaveRequest* self);

private:
    Q_DISABLE_COPY(QContactDetailDefinitionSaveRequest);
};

#endif


#ifndef QCONTACTDETAILDEFINITIONSAVEREQUEST_P_H
#define QCONTACTDETAILDEFINITIONSAVEREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include "qcontactdetaildefinition.h"
#include <QList>

class QContactDetailDefinitionSaveRequestData : public QContactAbstractRequestData
{
public:
    QContactDetailDefinitionSaveRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactDetailDefinitionSaveRequestData()
    {
    }

    QList<QContactDetailDefinition> m_definitions;
};

#endif

// qcontactdetaildefinitionsaverequest.cpp
#include "qcontactdetaildefinitionsaverequest.h"
#include "qcontactdetaildefinitionsaverequest_p.h"

QContactDetailDefinitionSaveRequest::QContactDetailDefinitionSaveRequest()
    : QContactAbstractRequest(new QContactDetailDefinitionSaveRequestData)
{
}

QContactDetailDefinitionSaveRequest::~QContactDetailDefinitionSaveRequest()
{
}

void QContactDetailDefinitionSaveRequest::setDefinitions(const QList<QContactDetailDefinition>& definitions)
{
    d->m_definitions = definitions;
}

QList<QContactDetailDefinition> QContactDetailDefinitionSaveRequest::definitions() const
{
    return d->m_definitions;
}

bool QContactDetailDefinitionSaveRequest::start()
{
    return false;
}

bool QContactDetailDefinitionSaveRequest::cancel()
{
    return false;
}

bool QContactDetailDefinitionSaveRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactDetailDefinitionSaveRequest::waitForProgress(int msecs = 0)
{
    return false;
}
//-----------------------------end






#ifndef QCONTACTDETAILDEFINITIONREMOVEREQUEST_H
#define QCONTACTDETAILDEFINITIONREMOVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"

#include <QStringList>

class QTCONTACTS_EXPORT QContactDetailDefinitionRemoveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactDetailDefinitionRemoveRequest();
    ~QContactDetailDefinitionRemoveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::DetailDefinitionRemove;}

    /* Selection */
    void setNames(const QStringList& names);

    /* Results */
    QStringList names() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactDetailDefinitionRemoveRequest* self);

private:
    Q_DISABLE_COPY(QContactDetailDefinitionRemoveRequest);
};

#endif


#ifndef QCONTACTDETAILDEFINITIONREMOVEREQUEST_P_H
#define QCONTACTDETAILDEFINITIONREMOVEREQUEST_P_H

#include "qcontactabstractrequest_p.h"
#include <QStringList>

class QContactDetailDefinitionRemoveRequestData : public QContactAbstractRequestData
{
public:
    QContactDetailDefinitionRemoveRequestData()
        : QContactAbstractRequestData()
    {
    }

    ~QContactDetailDefinitionRemoveRequestData()
    {
    }

    QStringList m_names;
};

#endif

// qcontactdetaildefinitionremoverequest.cpp
#include "qcontactdetaildefinitionremoverequest.h"
#include "qcontactdetaildefinitionremoverequest_p.h"

QContactDetailDefinitionRemoveRequest::QContactDetailDefinitionRemoveRequest()
    : QContactAbstractRequest(new QContactDetailDefinitionRemoveRequestData)
{
}

QContactDetailDefinitionRemoveRequest::~QContactDetailDefinitionRemoveRequest()
{
}

void QContactDetailDefinitionRemoveRequest::setNames(const QStringList& names)
{
    d->m_names = names;
}

QStringList QContactDetailDefinitionRemoveRequest::names() const
{
    return d->m_names;
}

bool QContactDetailDefinitionRemoveRequest::start()
{
    return false;
}

bool QContactDetailDefinitionRemoveRequest::cancel()
{
    return false;
}

bool QContactDetailDefinitionRemoveRequest::waitForFinished(int msecs = 0)
{
    return false;
}

bool QContactDetailDefinitionRemoveRequest::waitForProgress(int msecs = 0)
{
    return false;
}

//---------------------------------end


//------------------------------------------------new manager engine signature required:
void QContactMemoryEngine::requestDestroyed(QContactAbstractRequest* req)
{
    switch (req->type()) {
        case ContactFetch:
        {
            // delete my internal datastructure which is mapped to this request
        }
        break;

        case ContactIdFetch:
        case ContactSave:
        case ContactRemove:
        //...
        default:
        break;
    }
}

void QContactMemoryEngine::startRequest(QContactAbstractRequest* req)
{
    // add req to the operation queue, or whatever.
}

void QContactMemoryEngine::cancelRequest(QContactAbstractRequest* req)
{
    // remove req from the operation queue, or whatever.
}

void QContactMemoryEngine::waitForRequestProgress(QContactAbstractRequest* req)
{

}

void QContactMemoryEngine::waitForRequestFinished(QContactAbstractRequest* req)
{

}

//---- now the update request trampoline in base class: performs switch, depending on type updates member variable.
void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QUniqueId>& result)
{
    switch (req->type()) {
        case ContactFetch:
        {
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(req);
            r->d->m_ids = result;
        }
        break;

        case ContactIdFetch:
        {
            QContactIdFetchRequest* r = static_cast<QContactIdFetchRequest*>(req);
            r->d->m_ids = result;
        }
        break;

        case ContactRemove:
        {
            ContactRemoveRequest* r = static_cast<ContactRemoveRequest*>(req);
            r->d->m_ids = result;
        }
        break;

        case GroupFetch:
        {
            QContactGroupFetchRequest* r = static_cast<QContactGroupFetchRequest*>(req);
            r->d->m_ids = result;
        }
        break;

        case GroupRemove:
        {
            QContactGroupRemoveRequest* r = static_cast<QContactGroupRemoveRequest*>(req);
            r->d->m_ids = result;
        }
        break;

        default:
        {
            // this request type does not have a list of ids to update...
            return;
        }
    }
}

void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QContact>& result)
{
    switch (req->type()) {
        case ContactFetch:
        {
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(req);
            r->d->m_contacts = result;
        }
        break;

        case ContactSave:
        {
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(req);
            r->d->m_contacts = result;
        }
        break;

        default:
        {
            // this request type does not have a list of contacts to update...
            return;
        }
    }
}

void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QContactGroup>& result)
{
    switch (req->type()) {
        case GroupFetch:
        {
            QContactGroupFetchRequest* r = static_cast<QContactGroupFetchRequest*>(req);
            r->d->m_groups = result;
        }
        break;

        case GroupSave:
        {
            QContactGroupSaveRequest* r = static_cast<QContactGroupSaveRequest*>(req);
            r->d->m_groups = result;
        }
        break;

        default:
        {
            // this request type does not have a list of groups to update...
            return;
        }
    }
}

void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QStringList& result)
{
    switch (req->type()) {
        case DetailDefinitionFetch:
        {
            QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(req);
            r->d->m_names = result;
        }
        break;

        case DetailDefinitionRemove:
        {
            QContactDetailDefinitionRemoveRequest* r = static_cast<QContactDetailDefinitionRemoveRequest*>(req);
            r->d->m_names = result;
        }
        break;

        default:
        {
            // this request type does not have a list of definition names to update...
            return;
        }
    }
}

void QContactManagerEngine::updateRequest(QContactAbstractRequest* req, const QList<QContactDetailDefinition>& result)
{
    switch (req->type()) {
        case DetailDefinitionFetch:
        {
            QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(req);
            r->d->m_definitions = result;
        }
        break;

        case DetailDefinitionSave:
        {
            QContactDetailDefinitionSaveRequest* r = static_cast<QContactDetailDefinitionSaveRequest*>(req);
            r->d->m_definitions = result;
        }
        break;

        default:
        {
            // this request type does not have a list of definitions to update...
            return;
        }
    }
}
















//////-------------------------------------------- all the OLD memory engine async stuff (for reference):
/*!
 * \reimp
 */
void QContactMemoryEngine::asynchronousRequestDestroyed(QContactAbstractRequest* req)
{
    // delete the result associated with the request and remove it from the map.
    // note that a "real implementation" that does result sharing amongst requests
    // would simply call removeRequest on the result, and then, iff the count
    // of requests for which the result is valid is zero, delete the result.
    if (d->m_asynchronousRequests.contains(req)) {
        QContactAbstractRequestResult* carr = d->m_asynchronousRequests.take(req);
        delete carr;
    }
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::asynchronousRequestWaitForFinished(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);

    return false;
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::asynchronousRequestWaitForProgress(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);

    return false;
}

/*!
 * \reimp
 */
void QContactMemoryEngine::cancelAsynchronousRequest(QContactAbstractRequest* req)
{
    // check to see that the request is able to be cancelled
    if (req->status() != QContactAbstractRequest::Pending)
        return;

    // we can start the request.
    QContactAbstractRequestResult *requestResult;
    switch (req->type()) {
        case QContactAbstractRequest::Contact:
        {
            requestResult = new QContactRequestResult;
            QContactRequest *creq = static_cast<QContactRequest*>(req);
            static_cast<QContactRequestResult*>(requestResult)->updateRequest(creq, QContactAbstractRequest::Cancelling);
        }
        break;

        case QContactAbstractRequest::DetailDefinition:
        {
            requestResult = new QContactDetailDefinitionRequestResult;
            QContactDetailDefinitionRequest *dreq = static_cast<QContactDetailDefinitionRequest*>(req);
            static_cast<QContactDetailDefinitionRequestResult*>(requestResult)->updateRequest(dreq, QContactAbstractRequest::Cancelling);
        }
        break;

        case QContactAbstractRequest::Group:
        {
            requestResult = new QContactGroupRequestResult;
            QContactGroupRequest *greq = static_cast<QContactGroupRequest*>(req);
            static_cast<QContactGroupRequestResult*>(requestResult)->updateRequest(greq, QContactAbstractRequest::Cancelling);
        }
        break;

        default: // unknown request type
        return;
    }

    // clean up memory in use from previous operation
    if (d->m_asynchronousRequests.contains(req)) {
        delete d->m_asynchronousRequests.value(req);
    }

    // and insert our new (cancelling) operation.
    d->m_asynchronousRequests.insert(req, requestResult);
}

/*!
 * \reimp
 */
void QContactMemoryEngine::startAsynchronousRequest(QContactAbstractRequest* req, QContactAbstractRequest::Operation operation)
{
    // check to see that the request isn't already started
    if (req->status() == QContactAbstractRequest::Pending || req->status() == QContactAbstractRequest::Cancelling)
        return;

    // we can start the request.  First, create a result of the correct type.
    QContactAbstractRequestResult *requestResult;
    switch (req->type()) {
        case QContactAbstractRequest::Contact:
        {
            requestResult = new QContactRequestResult;
        }
        break;

        case QContactAbstractRequest::DetailDefinition:
        {
            requestResult = new QContactDetailDefinitionRequestResult;
        }
        break;

        case QContactAbstractRequest::Group:
        {
            requestResult = new QContactGroupRequestResult;
        }
        break;

        default: // unknown request type
        return;
    }

    // add the request to this result's list of requests, and update the request.
    requestResult->addRequest(req);
    requestResult->updateRequest(req, QContactAbstractRequest::Pending);

    // clean up memory in use from previous operation
    if (d->m_asynchronousRequests.contains(req))
        delete d->m_asynchronousRequests.value(req);

    // and start the new operation
    d->m_asynchronousRequests.insert(req, requestResult);
    d->m_asynchronousOperations.enqueue(QPair<QContactAbstractRequest*, QContactAbstractRequest::Operation>(req, operation));
    QTimer::singleShot(2000, this, SLOT(performAsynchronousOperation()));
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void QContactMemoryEngine::performAsynchronousOperation()
{
    QContactAbstractRequest *currentRequest;
    QContactAbstractRequest::Operation operation;

    // take the first pending, non-destroyed request and finish it
    while (true) {
        currentRequest = d->m_asynchronousOperations.head().first;
        operation = d->m_asynchronousOperations.dequeue().second;
        if (d->m_asynchronousRequests.contains(currentRequest))
            break;
        if (d->m_asynchronousOperations.isEmpty())
            return;
    }

    QSet<QUniqueId> removedContacts;
    QSet<QUniqueId> changedContacts;
    QSet<QUniqueId> addedContacts;
    QSet<QUniqueId> removedGroups;
    QSet<QUniqueId> changedGroups;
    QSet<QUniqueId> addedGroups;

    if (currentRequest->status() == QContactAbstractRequest::Pending) {
        switch (currentRequest->type()) {
            case QContactAbstractRequest::Contact:
            {
                QContactRequest *cr = static_cast<QContactRequest*>(currentRequest);
                QContactRequestResult *crr = static_cast<QContactRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactManager::Error asynchronousError = QContactManager::UnspecifiedError;
                if (operation == QContactAbstractRequest::SaveOperation) {
                    // save
                    QList<QContact> selection = cr->contactSelection();
                    crr->setErrors(saveContacts(&selection, addedContacts, changedContacts, changedGroups, asynchronousError));
                } else {
                    // retrieve or remove
                    QList<QContact> result;
                    QList<QUniqueId> translatedRequest;
                    QList<QContactManager::Error> errors;
                    asynchronousError = QContactManager::DoesNotExistError;
                    if (cr->selectionType() == QContactRequest::SelectByIds) {
                        translatedRequest = cr->idSelection();
                    } else if (cr->selectionType() == QContactRequest::SelectByFilter) {
                        QContactFilter fil = cr->filterSelection();
                        QList<QContactSortOrder> defaultSortOrder;
                        translatedRequest = contacts(fil, defaultSortOrder, asynchronousError);
                    } else if (cr->selectionType() == QContactRequest::SelectAll) {
                        translatedRequest = contacts(QList<QContactSortOrder>(), asynchronousError);
                    } else {
                        // invalid selection type...
                        asynchronousError = QContactManager::BadArgumentError;
                    }

                    if (operation == QContactAbstractRequest::RetrieveOperation) {
                        // retrieve the specified contacts
                        for (int i = 0; i < translatedRequest.size(); i++) {
                            result.append(contact(translatedRequest.at(i), asynchronousError));
                            errors.append(asynchronousError);
                        }
                        crr->setErrors(errors);
                        crr->setContacts(result);
                    } else {
                        // remove the specified contacts
                        crr->setErrors(removeContacts(&translatedRequest, removedContacts, changedGroups, asynchronousError));
                    }
                }

                crr->setError(asynchronousError);
                crr->updateRequest(cr, QContactAbstractRequest::Finished);
            }
            break;

            case QContactAbstractRequest::DetailDefinition:
            {
                QContactDetailDefinitionRequest *dr = static_cast<QContactDetailDefinitionRequest*>(currentRequest);
                QContactDetailDefinitionRequestResult *drr = static_cast<QContactDetailDefinitionRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactManager::Error asynchronousError = QContactManager::UnspecifiedError;
                if (operation == QContactAbstractRequest::SaveOperation) {
                    // save
                    QList<QContactDetailDefinition> selection = dr->definitionSelection();
                    QList<QContactManager::Error> errors;
                    for (int i = 0; i < selection.size(); i++) {
                        saveDetailDefinition(selection.at(i), asynchronousError);
                        errors.append(asynchronousError);
                        if (asynchronousError != QContactManager::NoError) {
                            drr->setError(asynchronousError);
                        }
                    }

                    drr->setErrors(errors);
                } else if (operation == QContactAbstractRequest::RetrieveOperation) {
                    // retrieve
                    QStringList defNames = dr->nameSelection();
                    QMap<QString, QContactDetailDefinition> allDefs = detailDefinitions(asynchronousError);
                    QList<QContactDetailDefinition> defs;
                    QList<QContactManager::Error> errors;
                    for (int i = 0; i < defNames.size(); i++) {
                        if (allDefs.contains(defNames.at(i))) {
                            defs.append(allDefs.value(defNames.at(i)));
                            errors.append(QContactManager::NoError);
                        } else {
                            defs.append(QContactDetailDefinition());
                            errors.append(QContactManager::DoesNotExistError);
                        }
                    }

                    drr->setError(asynchronousError);
                    drr->setErrors(errors);
                    drr->setDefinitions(defs);
                } else {
                    // remove.
                    QStringList defNames = dr->nameSelection();
                    QList<QContactManager::Error> errors;
                    for (int i = 0; i < defNames.size(); i++) {
                        removeDetailDefinition(defNames.at(i), asynchronousError);
                        errors.append(asynchronousError);
                    }
                    drr->setErrors(errors);
                    drr->setError(QContactManager::NoError);
                }

                drr->updateRequest(dr, QContactAbstractRequest::Finished);
            }
            break;

            case QContactAbstractRequest::Group:
            {
                QContactGroupRequest *gr = static_cast<QContactGroupRequest*>(currentRequest);
                QContactGroupRequestResult *grr = static_cast<QContactGroupRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactManager::Error asynchronousError = QContactManager::UnspecifiedError;
                if (operation == QContactAbstractRequest::SaveOperation) {
                    // save
                    QList<QContactGroup> selection = gr->groupSelection();
                    QList<QContactManager::Error> errors;
                    for (int i = 0; i < selection.size(); i++) {
                        QContactGroup g = selection.at(i);
                        saveGroup(&g, addedGroups, changedGroups, changedContacts, asynchronousError);
                        errors.append(asynchronousError);
                    }
                    grr->setErrors(errors);
                } else {
                    // retrieve or remove
                    QList<QContactGroup> result;
                    QList<QUniqueId> translatedRequest;
                    QList<QContactManager::Error> errors;
                    asynchronousError = QContactManager::DoesNotExistError;
                    if (gr->selectionType() == QContactGroupRequest::SelectByIds) {
                        translatedRequest = gr->idSelection();
                    } else if (gr->selectionType() == QContactGroupRequest::SelectAll) {
                        translatedRequest = groups(asynchronousError);
                    } else {
                        // invalid selection type...
                        asynchronousError = QContactManager::BadArgumentError;
                    }

                    if (operation == QContactAbstractRequest::RetrieveOperation) {
                        // retrieve the specified groups
                        for (int i = 0; i < translatedRequest.size(); i++) {
                            result.append(group(translatedRequest.at(i), asynchronousError));
                            errors.append(asynchronousError);
                        }
                        grr->setErrors(errors);
                        grr->setGroups(result);
                    } else {
                        // remove the specified groups
                        for (int i = 0; i < translatedRequest.size(); i++) {
                            removeGroup(translatedRequest.at(i), removedGroups, changedGroups, asynchronousError);
                            errors.append(asynchronousError);
                        }
                        grr->setErrors(removeContacts(&translatedRequest, removedContacts, changedGroups, asynchronousError));
                    }
                }

                grr->setError(asynchronousError);
                grr->updateRequest(gr, QContactAbstractRequest::Finished);
            }
            break;

            default: // unknown request type...
            return;
        }
    } else if (currentRequest->status() == QContactAbstractRequest::Cancelling) {
        switch (currentRequest->type()) {
            case QContactAbstractRequest::Contact:
            {
                QContactRequestResult *crr = static_cast<QContactRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactRequest *creq = static_cast<QContactRequest*>(currentRequest);
                crr->updateRequest(creq, QContactAbstractRequest::Cancelled);
            }
            break;

            case QContactAbstractRequest::DetailDefinition:
            {
                QContactDetailDefinitionRequestResult *drr = static_cast<QContactDetailDefinitionRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactDetailDefinitionRequest *dreq = static_cast<QContactDetailDefinitionRequest*>(currentRequest);
                drr->updateRequest(dreq, QContactAbstractRequest::Cancelled);
            }
            break;

            case QContactAbstractRequest::Group:
            {
                QContactGroupRequestResult *grr = static_cast<QContactGroupRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactGroupRequest *greq = static_cast<QContactGroupRequest*>(currentRequest);
                grr->updateRequest(greq, QContactAbstractRequest::Cancelled);
            }
            break;

            default:
            break;
        }
    }

    // now emit any signals we have to emit
    QList<QUniqueId> currEmit = removedContacts.toList();
    if (!currEmit.isEmpty())
        emit contactsRemoved(currEmit);
    currEmit = changedContacts.toList();
    if (!currEmit.isEmpty())
        emit contactsChanged(currEmit);
    currEmit = addedContacts.toList();
    if (!currEmit.isEmpty())
        emit contactsAdded(currEmit);
    currEmit = removedGroups.toList();
    if (!currEmit.isEmpty())
        emit groupsRemoved(currEmit);
    currEmit = changedGroups.toList();
    if (!currEmit.isEmpty())
        emit groupsChanged(currEmit);
    currEmit = addedGroups.toList();
    if (!currEmit.isEmpty())
        emit groupsAdded(currEmit);
}


// and, what the inidividual requests did to call into this:

/* =============== Dynamic functions below (trampoline to the engine) */

bool QContactDetailDefinitionRequest::waitForFinished(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForFinished(this, msecs);
    return false;
}

bool QContactDetailDefinitionRequest::waitForProgress(int msecs)
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        return engine->asynchronousRequestWaitForProgress(this, msecs);
    return false;
}

void QContactDetailDefinitionRequest::cancel()
{
    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->cancelAsynchronousRequest(this);
}

void QContactDetailDefinitionRequest::start(QContactManager *manager, QContactAbstractRequest::Operation operation)
{
    if (status() != QContactAbstractRequest::Inactive
        && status() != QContactAbstractRequest::Cancelled
        && status() != QContactAbstractRequest::Finished) {
        return; // unable to start operation; another operation already in progress.
    }

    QContactManagerEngine *engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->asynchronousRequestDestroyed(this);

    d->m_manager = manager;
    engine = QContactManagerData::engine(d->m_manager);
    if (engine)
        engine->startAsynchronousRequest(this, operation);
}

QStringList QContactDetailDefinitionRequest::names()
{
    if (!d->m_result)
        return QStringList();

    return d->m_result->definitionNames();
}

QList<QContactDetailDefinition> QContactDetailDefinitionRequest::definitions()
{
    if (!d->m_result)
        return QList<QContactDetailDefinition>();

    return d->m_result->definitions();
}

QList<QContactManager::Error> QContactDetailDefinitionRequest::errors()
{
    if (!d->m_result)
        return QList<QContactManager::Error>();

    return d->m_result->errors();
}

#include "moc_qcontactdetaildefinitionrequest.cpp"
