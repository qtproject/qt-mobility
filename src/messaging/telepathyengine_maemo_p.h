#ifndef TELEPATHYENGINE_MAEMO_P_H
#define TELEPATHYENGINE_MAEMO_P_H
#include <tpsession.h>
#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessage.h"

QTM_BEGIN_NAMESPACE

class QMessageService;

class TelepathyEngine
{
public:
    static TelepathyEngine* instance();

    TelepathyEngine();
    ~TelepathyEngine();

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                        uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount(QMessage::Type type) const;

    bool queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool countMessages(QMessageService& messageService, const QMessageFilter &filter);

    bool sendMessage(QMessage &message);


private:

    void updateImAccounts() const;


private: //Data
    TpSession *tpSession;
    mutable QMessageAccountId defaultSmsAccountId;
    mutable QHash<QString, QMessageAccount> iAccounts;
};

QTM_END_NAMESPACE

#endif // TELEPATHYENGINE_MAEMO_P_H
