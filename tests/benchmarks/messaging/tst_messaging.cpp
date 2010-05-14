/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QApplication>
#include <QtTest/QtTest>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#include "qmessagemanager.h"
#include "qmessage.h"

#ifdef Q_OS_SYMBIAN
#include <msvapi.h>
#include <msvids.h>  // KMsvGlobalInBoxIndexEntryId
#include <mtclreg.h>  // CClientMtmRegistry
#include <smsclnt.h>  // CSmsClientMtm
#include <smut.h>  // KUidMsgTypeSMS
#include <txtrich.h>  // CRichText
#include <cemailaccounts.h>
#include <mmsconst.h>
#include <miutmsg.h>
#include <msvfind.h>  
#include <impcmtm.h>
#include <mmsclient.h>
#include <etelmm.h>
#include <msvapi.h>
#include <iapprefs.h>
#include <smtcmtm.h>
#include <smuthdr.h>
#endif

#include <qdebug.h>

QTM_USE_NAMESPACE

class OsNative;

#if defined(Q_OS_SYMBIAN)
class OsNative : public MMsvSessionObserver {
public:
  OsNative()
  {    
  };

  void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3) { };
  
  void getFolders(RArray<TMsvId> &folders);
  
  int countMessages();
  void deleteAllMessages();
  void createMailAccountL();
  void printAccounts();
  void deleteAccounts();
  void deleteAllFolders();
  TInt createFolder(TPtrC name);
  
  TMsvId makeSMS(TMsvId folder);
  TMsvId makeIMAP4(TMsvId folder);
  
  CMsvSession *iSession;
  CClientMtmRegistry *iMtmRegistry;
  CSmsClientMtm *iSmsMtm;
  CImap4ClientMtm *iImap4Mtm;
  CSmtpClientMtm *iSmtpMtm;
  CMmsClientMtm *iMmsMtm;
   
  RArray<TMsvId> iFolders;
  RArray<TMsvId> iNewMsgs;
  TMsvId iMsvImapService;
  TMsvId iMsvSmtpService;

};

TMsvId OsNative::makeSMS(TMsvId folder){    
    TBuf<10> aAddress(_L("Nokia"));
    TBuf<20> aDescription(_L("Important Message"));
 
    _LIT(KTxt1,"Hi phone owner, how r u?");
 
    TBuf<150> iMessage; 
    iMessage.Copy(KTxt1);
  
    iSmsMtm->SwitchCurrentEntryL(folder); //inbox
// Clean phones without a SIM doesn't have a default service
//    TMsvId serviceId;
//    TRAPD(err, serviceId = iSmsMtm->DefaultServiceL());
//    if(err) {
//      qDebug() << "No default service";
//      return 0;
//    }
      
    
    TRAPD(err1, iSmsMtm->SwitchCurrentEntryL( folder ));
    if(err1){
        qDebug() << "Failed to create SMS1: " << err1;
        return 0;
    }

    TRAPD(err, iSmsMtm->CreateMessageL( iSmsMtm->ServiceId() )); 
    
    //TRAPD(err, iSmsMtm->CreateMessageL(KMsvLocalServiceIndexEntryId));
    if(err){
        qDebug() << "Failed to create SMS: " << err;
        return 0;
    }
    
    CSmsHeader& iHeader = iSmsMtm->SmsHeader();
    TRAP(err, iHeader.SetFromAddressL(aAddress));
    if(err)
      qDebug() << "Failed to set from address: " << err;    
 
    CRichText& body = iSmsMtm->Body();
    body.Reset();
    TRAP(err, body.InsertL(0, iMessage));
    if(err)
      qDebug() << "Failed to add body: " << err;
 
    TMsvEntry entry = iSmsMtm->Entry().Entry();
    entry.SetInPreparation(EFalse);
    entry.SetVisible(ETrue);
    entry.iDate.HomeTime();
    entry.iDescription.Set(aDescription);
    entry.iDetails.Set(aAddress);
    entry.SetUnread(ETrue);
 
    TRAP(err,
        iSmsMtm->Entry().ChangeL(entry);
        iSmsMtm->SaveMessageL();
        );
    if(err)
      qDebug() << "Failed to save changes: " << err;
    
   return entry.Id();
}

void OsNative::printAccounts(){
  RArray<TPopAccount> aPop;
  RArray<TImapAccount> aImap;
  RArray<TSmtpAccount> aSmtp;
  CEmailAccounts* account = CEmailAccounts::NewLC();
  TRAPD(err, 
      account->GetPopAccountsL(aPop);
      account->GetImapAccountsL(aImap);
      account->GetSmtpAccountsL(aSmtp);
      );
  if(err){
    qDebug()<< "Can't get all accounts";
  }
  
  for(int i = 0; i < aPop.Count(); i++){
    QString name  = QString::fromUtf16(aPop[i].iPopAccountName.Ptr(), aPop[i].iPopAccountName.Length());
    qDebug() << "Pop account: " << name << hex << " SMTP: " << aPop[i].iSmtpService;
//    account->DeletePopAccountL(aPop[i]);
  }
  for(int i = 0; i < aImap.Count(); i++){
    QString name  = QString::fromUtf16(aImap[i].iImapAccountName.Ptr(), aImap[i].iImapAccountName.Length());
    qDebug() << "Imap account: " << name << hex << " SMTP: " << aImap[i].iSmtpService;
//    account->DeleteImapAccountL(aImap[i]);
  }
  for(int i = 0; i < aSmtp.Count(); i++){
    QString name  = QString::fromUtf16(aSmtp[i].iSmtpAccountName.Ptr(), aSmtp[i].iSmtpAccountName.Length());
    qDebug() << "SMTP account: " << name << hex << " Related: " << aSmtp[i].iRelatedService;
//    account->DeleteSmtpAccountL(aSmtp[i]);
  }
  aPop.Close();
  aSmtp.Close();
  aImap.Close();
  CleanupStack::PopAndDestroy(account);
}

void OsNative::deleteAccounts(){
  RArray<TPopAccount> aPop;
  RArray<TImapAccount> aImap;
  RArray<TSmtpAccount> aSmtp;
  CEmailAccounts* account = CEmailAccounts::NewLC();
  TRAPD(err, 
      account->GetPopAccountsL(aPop);
      account->GetImapAccountsL(aImap);
      account->GetSmtpAccountsL(aSmtp);
      );
  if(err){
    qDebug()<< "Can't get all accounts";
  }
  
  for(int i = 0; i < aPop.Count(); i++){
    QString name  = QString::fromUtf16(aPop[i].iPopAccountName.Ptr(), aPop[i].iPopAccountName.Size());
    if(name.startsWith("BMTestAccount"))
      account->DeletePopAccountL(aPop[i]);
  }
  for(int i = 0; i < aImap.Count(); i++){
    QString name  = QString::fromUtf16(aImap[i].iImapAccountName.Ptr(), aImap[i].iImapAccountName.Size());
    if(name.startsWith("BMTestAccount"))
      account->DeleteImapAccountL(aImap[i]);

  }
  for(int i = 0; i < aSmtp.Count(); i++){
    QString name  = QString::fromUtf16(aSmtp[i].iSmtpAccountName.Ptr(), aSmtp[i].iSmtpAccountName.Size());
    if(name.startsWith("BMTestAccount"))
      account->DeleteSmtpAccountL(aSmtp[i]);
  }
  aPop.Close();
  aSmtp.Close();
  aImap.Close();
  CleanupStack::PopAndDestroy(account);
}

TMsvId OsNative::makeIMAP4(TMsvId folder){

  // create a new object to access an existing entry
  CMsvEntry* msvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
  CleanupStack::PushL(msvEntry); // 1
              
  msvEntry->SetEntryL(folder);
  
  // mtm takes ownership of entry context     
  CleanupStack::Pop(msvEntry); // 0
  iSmtpMtm->SetCurrentEntryL(msvEntry);
  
  // create a new message
  TMsvId defaultServiceId(0);
  defaultServiceId = iSmtpMtm->DefaultServiceL();
  CMsvEntry *pE = iSession->GetEntryL(defaultServiceId);
  CleanupStack::PushL(pE);  
  QString name = QString::fromUtf16(pE->Entry().iDetails.Ptr(), pE->Entry().iDetails.Length());
  CleanupStack::PopAndDestroy(pE);
  iSmtpMtm->CreateMessageL(defaultServiceId);
  
  // set subject
  iSmtpMtm->SetSubjectL( _L("Test Message") );
  // set body
  iSmtpMtm->Body().Reset();
  iSmtpMtm->Body().InsertL(0, _L("This is the text body")); 

  // get the entry of the message
  TMsvEntry messageEntry = iSmtpMtm->Entry().Entry();
//      AddSenderToMtmAddresseeL(*iSmtpMtm, parameters, ETrue );
//      SetSenderToEntryDetails(messageEntry, parameters, ETrue);
  messageEntry.iMtm = KUidMsgTypeIMAP4;  // or any other than KUidMsgTypeSMTP to display 'from' field instead of 'to' field 

  // set the description field same as the message subject
  messageEntry.iDescription.Set( _L("test") );  
  messageEntry.iDetails.Set( _L("test subject") );

  // save the changes done above
  iSmtpMtm->Entry().ChangeL(messageEntry);
  // get an access to the message store
  CMsvStore* store = msvEntry->EditStoreL();
  CleanupStack::PushL(store); // 1
  
  // save the attachments

  // save the changes made to the message store
  store->CommitL();
  CleanupStack::PopAndDestroy(store); // 0 
  // save the message      
  iSmtpMtm->SaveMessageL();
  // final fine tuning
  TMsvEmailEntry temailEntry = static_cast<TMsvEmailEntry>(messageEntry);
  temailEntry.SetMessageFolderType(EFolderTypeUnknown);
  temailEntry.SetDisconnectedOperation(ENoDisconnectedOperations);
  temailEntry.SetEncrypted(EFalse);
  temailEntry.SetSigned(EFalse);
  temailEntry.SetVCard(EFalse);
  temailEntry.SetVCalendar(EFalse);
  temailEntry.SetReceipt(EFalse);
  temailEntry.SetMHTMLEmail(EFalse);
  temailEntry.SetBodyTextComplete(ETrue);
  
  temailEntry.SetAttachment(EFalse);
      
  temailEntry.iDate.HomeTime();
  temailEntry.SetVisible(ETrue);
  temailEntry.SetInPreparation(EFalse);
  temailEntry.SetUnread(ETrue);
  temailEntry.SetNew(ETrue);

  temailEntry.SetComplete(ETrue);
  temailEntry.iServiceId = iMsvSmtpService;
  temailEntry.iRelatedId = 0;
  
  iSmtpMtm->Entry().ChangeL(temailEntry);

  // reset email headers
  CImHeader* header = CImHeader::NewLC(); // 1
  CMsvStore* msvStore = msvEntry->EditStoreL();
  CleanupStack::PushL(msvStore); // 2
  header->RestoreL(*msvStore);
  header->SetSubjectL( _L("test email") );
  header->SetFromL( _L("blah@blah.com") );
  header->SetReceiptAddressL( _L("Foo@bar.com") );
  header->StoreL( *msvStore );
  msvStore->CommitL();
  CleanupStack::PopAndDestroy(2); // msvStore, header // 0
  
  // id has been generated, store it for being able to delete
  // only entries created with Creator
  TMsvId id = messageEntry.Id();
  return id;
}


void OsNative::deleteAllFolders() {

    RArray<TMsvId> aFolders;
    
    TRAP_IGNORE(getFolders(aFolders));   
    
    TMsvSelectionOrdering sort;
    sort.SetSorting(EMsvSortByNone);
    sort.SetShowInvisibleEntries(ETrue);

    for(int i = 0; i < aFolders.Count(); i++) {
        CMsvEntry* inboxContext = CMsvEntry::NewL(*iSession, aFolders[i], sort);                
        CleanupStack::PushL(inboxContext);
    
        // Get all entries in the folder
        CMsvEntrySelection* entries = inboxContext->ChildrenL();
        CleanupStack::PushL(entries);
        QString desc = QString::fromUtf16(inboxContext->Entry().iDetails.Ptr(), inboxContext->Entry().iDetails.Length());
        
        for (TInt j = 0; j < entries->Count(); j++) {
            CMsvEntry* pEntry = CMsvEntry::NewL(*iSession, entries->At(j), sort);
            CleanupStack::PushL(pEntry);
            QString desc2 = QString::fromUtf16(pEntry->Entry().iDetails.Ptr(), pEntry->Entry().iDetails.Length());

            const TMsvEntry &entry = pEntry->Entry();            
            if((desc != "My folders" || desc2 == "Templates")) {
              CleanupStack::PopAndDestroy(pEntry);
              continue;
            }
                        
            CleanupStack::PopAndDestroy(pEntry);
            
            TMsvId id = entries->At(j);
            qDebug() << "Deleting: " << desc << "/" << desc2;
            TRAPD(ierr, iSession->RemoveEntry(id)); // works
            if(ierr){
                qDebug() << "Failed to delete: " << ierr << " " << hex << entries->At(j);
            }            
        }        
        CleanupStack::PopAndDestroy(2, inboxContext);
    }
}

void OsNative::createMailAccountL() {
  CImIAPPreferences* outgoingIAPSet = CImIAPPreferences::NewLC(); // 1
  CImIAPPreferences* incomingIAPSet = CImIAPPreferences::NewLC(); // 2
  CEmailAccounts* account = CEmailAccounts::NewLC(); // 3

  CImImap4Settings* imap4Set = new(ELeave) CImImap4Settings;
  CleanupStack::PushL(imap4Set); // 4
  CImSmtpSettings* smtpSet = new(ELeave) CImSmtpSettings;
  CleanupStack::PushL(smtpSet); // 5

  account->PopulateDefaultSmtpSettingsL(*smtpSet, *outgoingIAPSet);
  account->PopulateDefaultImapSettingsL(*imap4Set, *outgoingIAPSet);
  
 imap4Set->SetDisconnectedUserMode(ETrue);
 imap4Set->SetSynchronise(EUseLocal);
 imap4Set->SetSubscribe(EUpdateNeither);
 imap4Set->SetAutoSendOnConnect(EFalse);
 imap4Set->SetGetMailOptions(EGetImap4EmailBodyText);
 imap4Set->SetUpdatingSeenFlags(ETrue);
 imap4Set->SetServerAddressL(_L("test.sdfsdf.wer.sadazsdsddf.com"));

 TImapAccount imapAccountId = account->CreateImapAccountL(_L("BMTestAccount"), *imap4Set, *incomingIAPSet, EFalse);
 TSmtpAccount smtpAccountId = account->CreateSmtpAccountL(imapAccountId, *smtpSet, *outgoingIAPSet, EFalse);
 account->SetDefaultSmtpAccountL(smtpAccountId);

 iMsvImapService = imapAccountId.iImapService;
 iMsvSmtpService = smtpAccountId.iSmtpAccountId;
 
 // Fails with NotSupported 
 //iImap4Mtm->ChangeDefaultServiceL(iMsvImapService);
 // Failed with invalid arg
 // iSmtpMtm->ChangeDefaultServiceL(iMsvSmtpService);
 CleanupStack::PopAndDestroy(5);
 
}

int OsNative::countMessages() {

    RArray<TMsvId> aFolders;
    int total = 0;

    TRAPD(err, getFolders(aFolders));
    if(err){
        qDebug() << "getFodlers faield: " << err;
        return 0;
    }

    for(int i = 0; i < aFolders.Count(); i++){
        TMsvSelectionOrdering sort;
        sort.SetSorting(EMsvSortByNone);
        sort.SetShowInvisibleEntries(ETrue);
        CMsvEntry* inboxContext = CMsvEntry::NewL(*iSession, 
                aFolders[i], sort);                
        CleanupStack::PushL(inboxContext); // 1
    
        // Get all entries in the Inbox
        CMsvEntrySelection* entries = inboxContext->ChildrenL();
        CleanupStack::PushL(entries); // 2
        for (TInt i = 0; i < entries->Count(); i++) {              
    
            TMsvId entryID = entries->At(i);
        
            const TUid mtm = inboxContext->ChildDataL(entryID).iMtm;
            if (mtm == KUidMsgTypeSMS) {
                total++;
            }
            else if ((mtm == KUidMsgTypeIMAP4) || (mtm == KUidMsgTypeSMTP)) {
                total++;                
            }
            else {
            }           
        }
        CleanupStack::PopAndDestroy(2, inboxContext);
    }
    return total;
}

void OsNative::deleteAllMessages() {

    RArray<TMsvId> aFolders;    

    TRAPD(err, getFolders(aFolders));
    if(err){
        qDebug() << "getFodlers faield: " << err;
        return;
    }

    for(int i = 0; i < aFolders.Count(); i++){
        TMsvSelectionOrdering sort;
        CMsvEntry* inboxContext = CMsvEntry::NewL(*iSession, aFolders[i], sort);                
        CleanupStack::PushL(inboxContext); // 1
    
        // Get all entries in the Inbox
        CMsvEntrySelection* entries = inboxContext->ChildrenL();
        CleanupStack::PushL(entries); // 2
        for (TInt i = 0; i < entries->Count(); i++) {              
    
            TMsvId entryID = entries->At(i);        
            const TUid mtm = inboxContext->ChildDataL(entryID).iMtm;
            
            if ((mtm == KUidMsgTypeSMS) || (mtm == KUidMsgTypeIMAP4) || (mtm == KUidMsgTypeSMTP)) {
                iSession->RemoveEntry(entryID);                
            }
        }
        CleanupStack::PopAndDestroy(2, inboxContext);
    }
}


#define KMsvMyFoldersEntryIdValue 0x1008
const TMsvId KMsvMyFoldersEntryId=KMsvMyFoldersEntryIdValue;

TInt OsNative::createFolder(TPtrC name){
    TMsvId msvServId;                                                                       
    CMsvEntry *entry1 = NULL;                                                                
    CMsvEntry * rootEntry = NULL;                                                           
    CMsvEntry *entryRootService = NULL;                                                     
     
    TInt newFldrID = 0;                                                                     
     
    //Get the entry (whatever is passed in)                                                    
    entry1 = iSession->GetEntryL(KMsvMyFoldersEntryIdValue);   
    CleanupStack::PushL(entry1);                                                             
     
    if (entry1)                                                                              
    {                          
        msvServId = entry1->OwningService();                                                    
     
        entryRootService = iSession->GetEntryL(msvServId);                            
        CleanupStack::PushL(entryRootService);                                                 
     
        rootEntry = iSession->GetEntryL(msvServId);                                   
        CleanupStack::PushL(rootEntry);                                                        
     
        rootEntry->SetEntryL(KMsvMyFoldersEntryIdValue); // set to parent
     
        TMsvEntry newServiceEntry;                                                             

        newServiceEntry.iDate.HomeTime();
        newServiceEntry.iSize = 0;
        newServiceEntry.iType = KUidMsvFolderEntry;                                            
        newServiceEntry.iMtm = KUidMsvLocalServiceMtm;
//        newServiceEntry.iServiceId = iSmsMtm->ServiceId();
//        newServiceEntry.iServiceId = KMsvLocalServiceIndexEntryId;
        newServiceEntry.iServiceId = iMsvImapService;
        newServiceEntry.iDetails.Set(name);                                             
        newServiceEntry.iDescription.Set(KNullDesC);
        newServiceEntry.SetVisible(ETrue);        

        TRAPD(err, rootEntry->CreateL(newServiceEntry));
        if(err){
            QString n = QString::fromUtf16(name.Ptr(), name.Size());
            qDebug() << "Failed to create folder "<< n << ": " << err;
            User::Panic(_L("Fail"), err);            
        }
//        qDebug() << "iError: " << newServiceEntry.iError << " complete: " << newServiceEntry.Complete() << " prot: " << newServiceEntry.DeleteProtected();
//        newFldrID = newServiceEntry.Id();                                                      
        CleanupStack::PopAndDestroy();                                                         
        CleanupStack::PopAndDestroy();                                                         
    }
    CleanupStack::PopAndDestroy(entry1);
    
    return newFldrID;
}


#else
    class OsNative {
      
    };
#endif
    


    
//Q_DECLARE_METATYPE(QSystemInfo::Feature);

class tst_messaging : public QObject
{
    Q_OBJECT

public:
    enum platform {
        QMessaging = 0,
        Native = 1
    };
    
    enum bases {
      Accounts = 0,
      AccountsImap,
      AccountsPop,
      AccountsSmtp,
      Folders,
      FoldersEmail,
      FoldersSMS,
      FoldersMMS,
      Messages,
      MessagesAlt,
      
    };

    enum filter {
      Id = 0,
      Type,
      Sender,
      Recipients,
      Subject,
      TimeStamp,
      Status,
      Priority,
      Size,
      AllId,
      AllSender
    };
    
    enum types {
      Sms = 0,
      Mms,
      Email          
    };

    
private slots:
    void initTestCase();
    void cleanupTestCase();
                
    void tst_createTime_data();
    void tst_createTime();

    void tst_counts_data();
    void tst_counts();

    void tst_query_data();
    void tst_query();

    void tst_fetch_data();
    void tst_fetch();

    void tst_fetchAll_data();
    void tst_fetchAll();

    void tst_fetchFilter_data();
    void tst_fetchFilter();

    void tst_addMessage_data();
    void tst_addMessage();

    void tst_removeMessage_data();
    void tst_removeMessage();
    
    void tst_removeAllMessage_data();
    void tst_removeAllMessage();
    
private:
    QMessage *createMessage();
    QMessage *messageTemplate();
    QMessageId addMessage(QMessage *);     

    void clearMessages();
    int countMessages();

    QMessageManager *m_mgr;
    
    QMessageIdList msg_list;
    
    QMessageAccount m_act;
    //QMessageFolder m_fol;
    

    OsNative *m_native;
};

Q_DECLARE_METATYPE(tst_messaging::platform);
Q_DECLARE_METATYPE(tst_messaging::bases);
Q_DECLARE_METATYPE(tst_messaging::filter);
Q_DECLARE_METATYPE(tst_messaging::types);

void tst_messaging::initTestCase()
{

#if defined(Q_WS_MAEMO_6)
    QFAIL("Maemo 6 not supported by tst_messaging");
#elif defined(Q_WS_MAEMO_5)
    QFAIL("Maemo 5 does not support tst_messageing");
#elif defined(Q_OS_SYMBIAN)
    
#else
    QFAIL("Platform not supported");
#endif


//    QMessageFolderIdList flist = m_mgr->queryFolders();
//    while(!flist.empty()) {      
//      QMessageFolder fol = m_mgr->folder(flist.takeFirst());
//      qDebug() << "Folder: " << fol.name();
//#if defined(Q_OS_SYMBIAN)      
//      if(fol.name() == "Inbox"){
//        m_fol = fol;
//        break;
//      }     
//#endif      
//    }
    
#if defined(Q_OS_SYMBIAN)
    // define/init native features we need native
    m_native = new OsNative;
    m_native->iSession = CMsvSession::OpenSyncL(*m_native);
    m_native->iMtmRegistry = CClientMtmRegistry::NewL(*m_native->iSession);
    m_native->iSmsMtm = STATIC_CAST(CSmsClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeSMS));
    m_native->iImap4Mtm = STATIC_CAST(CImap4ClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeIMAP4));
    m_native->iMmsMtm = STATIC_CAST(CMmsClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeMultimedia));
    m_native->iSmtpMtm = STATIC_CAST(CSmtpClientMtm*,(m_native->iMtmRegistry->NewMtmL(KUidMsgTypeSMTP)));  

    qDebug() << "Print accounts";
    m_native->printAccounts();
    qDebug() << "Delete accounts";
    m_native->deleteAccounts();
    qDebug() << "Delete all folders and messages";
    m_native->deleteAllFolders();
    m_native->deleteAllMessages();
    qDebug() << "Create accounts";
    TRAPD(err, m_native->createMailAccountL());
    if(err)
      qDebug() << "Account create failed: " << err;
    qDebug() << "Oustanding ops: " << m_native->iSession->OutstandingOperationsL();
    m_native->printAccounts();
    qDebug() << "Create 3 folders";
    m_native->createFolder(_L("Test1"));    
    m_native->createFolder(_L("Test2"));
    m_native->createFolder(_L("Test3"));
    
// Created messages with QMF, our symbian messages aren't very good    
//    qDebug() << "Create 15 emails";
//    for(int count = 0; count < 15; count++){
//      TRAP(err, TMsvId id = m_native->makeIMAP4(KMsvGlobalInBoxIndexEntryId);
//      m_native->iNewMsgs.Append(id));
//      if(err){
//        qDebug() << "Error: " << err;
//      }
//    }
    
   
// Can't create SMS messages on a phone with no msg center
//    qDebug() << "Create 15 SMS: " << m_native->iNewMsgs.Count();
//    TRAPD(err, 
//    for(int count = 0; count < 15; count++){
//        TMsvId id = m_native->makeSMS(KMsvDraftEntryId); 
//        m_native->iNewMsgs.Append(id);
//    }
//    );    
    qDebug() << "Initial state is set: " << m_native->iNewMsgs.Count();
    RArray<TMsvId> folders;
    m_native->getFolders(folders);
    qDebug() << "Number of folders: " << folders.Count();
    folders.Close();    
    qDebug() << "Number of messages: " << m_native->countMessages();    
#else
// this doesn't really create testable messages on symbian
    for(int i = 0; i < 30; i++)
      createMessage();
#endif

    m_mgr = new QMessageManager();
    
    QMessageAccountIdList list;
    list = m_mgr->queryAccounts();
    while(!list.empty()) {      
      QMessageAccount act = m_mgr->account(list.takeFirst());
      qDebug() << "Account: " << act.name();
#if defined(Q_OS_SYMBIAN)           
      if(act.name() == "BMTestAccount"){
        qDebug() << "Using this account";
        m_act = act;        
        break;
      }     
#endif      
    }

    
    for(int i = 0; i < 30; i++)
      createMessage();
}

void tst_messaging::cleanupTestCase()
{
  
  clearMessages();
  
#if defined(Q_OS_SYMBIAN)
  qDebug() << "Delete: Oustanding ops: " << m_native->iSession->OutstandingOperationsL() << " outstanding messages: " << m_native->countMessages();
  m_native->deleteAccounts();
  m_native->deleteAllFolders();
  m_native->deleteAllMessages();
  m_native->iSession->RemoveEntry(m_native->iMsvImapService);
  m_native->iSession->RemoveEntry(m_native->iMsvSmtpService);
  delete m_native->iMmsMtm;
  delete m_native->iImap4Mtm;
  delete m_native->iSmsMtm;
  delete m_native->iMtmRegistry;
  delete m_native->iSession;
  delete m_native;
#endif
    
  delete m_mgr;


}


void tst_messaging::tst_createTime_data()
{
    QTest::addColumn<tst_messaging::platform>("platform");

    QTest::newRow("Qt-Create") << tst_messaging::QMessaging;
    QTest::newRow("Native-Create") << tst_messaging::Native;
}


void tst_messaging::tst_createTime()
{
    QFETCH(tst_messaging::platform, platform);
    
    //qDebug() << "Platform: " << platform;
    
    if(platform == tst_messaging::QMessaging){
      QMessageManager *mgr = 0x0;
      
      QBENCHMARK {
        mgr = new QMessageManager();        
      }
      
      delete mgr;
        
    }
    else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      
      __UHEAP_MARK;
      
      if(m_native){
        delete m_native->iSmtpMtm;
        delete m_native->iMmsMtm;
        delete m_native->iImap4Mtm;
        delete m_native->iSmsMtm;
        delete m_native->iMtmRegistry;
        delete m_native->iSession;
        delete m_native;
      }

      QBENCHMARK {
        // define/init native features we need native
        m_native = new OsNative;
        m_native->iSession = CMsvSession::OpenSyncL(*m_native);
        m_native->iMtmRegistry = CClientMtmRegistry::NewL(*m_native->iSession);
        m_native->iSmsMtm = STATIC_CAST(CSmsClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeSMS));
        m_native->iImap4Mtm = STATIC_CAST(CImap4ClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeIMAP4));
        m_native->iMmsMtm = STATIC_CAST(CMmsClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeMultimedia));
        m_native->iSmtpMtm = STATIC_CAST(CSmtpClientMtm*, m_native->iMtmRegistry->NewMtmL(KUidMsgTypeSMTP));
      }
      
      __UHEAP_MARKEND;
#endif
    }

}

void tst_messaging::tst_counts_data()
{
    QTest::addColumn<tst_messaging::platform>("platform");
    QTest::addColumn<tst_messaging::bases>("base");

    QTest::newRow("Qt-Accounts") << tst_messaging::QMessaging << tst_messaging::Accounts;
//    QTest::newRow("Native-Accounts") << tst_messaging::Native << tst_messaging::Accounts;
    QTest::newRow("Qt-Folders") << tst_messaging::QMessaging << tst_messaging::Folders;
    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::Folders;
    QTest::newRow("Qt-Messages") << tst_messaging::QMessaging << tst_messaging::Messages;
    QTest::newRow("Qt-MessagesAlt") << tst_messaging::QMessaging << tst_messaging::MessagesAlt;
    QTest::newRow("Native-Messages") << tst_messaging::Native << tst_messaging::Messages;

}

void tst_messaging::tst_counts()
{
    QFETCH(tst_messaging::platform, platform);
    QFETCH(tst_messaging::bases, base);
    
    if(platform == tst_messaging::QMessaging){
      int cnt;
      Q_UNUSED(cnt);
      if(base == tst_messaging::Accounts){
        QBENCHMARK {
          cnt = m_mgr->countAccounts();
        }
      }
      else if(base == tst_messaging::Folders){
        QBENCHMARK {
          cnt = m_mgr->countFolders();
        }
      }
      else if(base == tst_messaging::Messages){
        QBENCHMARK {
          cnt = m_mgr->countMessages();
        }        
      }
      else if(base == tst_messaging::MessagesAlt){
        QMessageIdList list;
        QBENCHMARK {
          list = m_mgr->queryMessages();
          cnt = list.count();
        }        
      }
      //qDebug() << "Got cnt: " << cnt;      
    }
    else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      QBENCHMARK {
        // Access the Inbox
        TMsvSelectionOrdering sort;
        CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, KMsvGlobalInBoxIndexEntryId, sort);
        CleanupStack::PushL(inboxContext);

        // Get all entries in the Inbox
        CMsvEntrySelection* entries = inboxContext->ChildrenL();
        CleanupStack::PushL(entries);


        TInt messages = entries->Count();

        CleanupStack::PopAndDestroy(2, inboxContext);
      }           
#endif
    }

}

void tst_messaging::tst_query_data()
{
    QTest::addColumn<tst_messaging::platform>("platform");
    QTest::addColumn<tst_messaging::bases>("base");

    QTest::newRow("Qt-Accounts") << tst_messaging::QMessaging << tst_messaging::Accounts;    
//    QTest::newRow("Qt-Folders") << tst_messaging::QMessaging << tst_messaging::Folders;    
    QTest::newRow("Qt-Messages") << tst_messaging::QMessaging << tst_messaging::Messages;
    
    QTest::newRow("Native-Accounts") << tst_messaging::Native << tst_messaging::Accounts;
    QTest::newRow("Native-Accounts-Imap") << tst_messaging::Native << tst_messaging::AccountsImap;
    QTest::newRow("Native-Accounts-Pop") << tst_messaging::Native << tst_messaging::AccountsPop;
    QTest::newRow("Native-Accounts-Smtp") << tst_messaging::Native << tst_messaging::AccountsSmtp;
    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::Folders;
//    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::FoldersEmail;
//    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::FoldersSMS;
//    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::FoldersMMS;
    QTest::newRow("Native-Messages") << tst_messaging::Native << tst_messaging::Messages;

}

void tst_messaging::tst_query()
{
    QFETCH(tst_messaging::platform, platform);
    QFETCH(tst_messaging::bases, base);
    
    if(platform == tst_messaging::QMessaging){
//      int cnt;
      if(base == tst_messaging::Accounts){
        QMessageAccountIdList list;
        QBENCHMARK {
          list = m_mgr->queryAccounts();
        }
      }
      else if(base == tst_messaging::Folders){
        QMessageFolderIdList list;
        QBENCHMARK {
          list = m_mgr->queryFolders();
        }
//        XXX The library returns inbox/drafts/sent for all default accounts and greatly exagerates the number of folders
//        qDebug() << "Number of folders: " << list.count();
//        while(!list.empty()){
//          QMessageFolder f(list.takeFirst());
//          qDebug() << "Folder: " << f.name(); 
//        }
      }
      else if(base == tst_messaging::Messages){
        QMessageIdList list;
        QBENCHMARK {
          list = m_mgr->queryMessages();
        }        
      }
      //qDebug() << "Got cnt: " << cnt;      
    }
    else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      if(base == tst_messaging::Accounts){
        CEmailAccounts *email = CEmailAccounts::NewLC();
        RArray<TPopAccount> aPop;
        RArray<TImapAccount> aImap;
        RArray<TSmtpAccount> aSmtp;
        
        QBENCHMARK {
          email->GetPopAccountsL(aPop);
          email->GetImapAccountsL(aImap);
          email->GetSmtpAccountsL(aSmtp);
        }
        CleanupStack::PopAndDestroy(email);        
      }
      else if(base == tst_messaging::AccountsImap) {
        CEmailAccounts *email = CEmailAccounts::NewLC();
        RArray<TImapAccount> aImap;        
        
        QBENCHMARK {          
          email->GetImapAccountsL(aImap);          
        }
        CleanupStack::PopAndDestroy(email);        
        
      }
      else if(base == tst_messaging::AccountsPop) {
        CEmailAccounts *email = CEmailAccounts::NewLC();
        RArray<TPopAccount> aPop;
        
        QBENCHMARK {
          email->GetPopAccountsL(aPop);
        }
        CleanupStack::PopAndDestroy(email);        
        
      }
      else if(base == tst_messaging::AccountsSmtp) {
        CEmailAccounts *email = CEmailAccounts::NewLC();
        RArray<TSmtpAccount> aSmtp;
        
        QBENCHMARK {
          email->GetSmtpAccountsL(aSmtp);
        }
        CleanupStack::PopAndDestroy(email);        
        
      }
      else if(base == tst_messaging::Folders){
          RArray<TMsvId> aFolders;
          QBENCHMARK {
              m_native->getFolders(aFolders);
              aFolders.Close();              
          }
      }
      else if(base == tst_messaging::Messages) {
        
        RArray<TMsvId> aFolders;
        
//        aFolders.Append(KMsvGlobalInBoxIndexEntryId);
//        aFolders.Append(KMsvGlobalOutBoxIndexEntryId);
//        aFolders.Append(KMsvDraftEntryId);
//        aFolders.Append(KMsvSentEntryId);
//        for(int i = 0; i < m_native->iFolders.Count(); i++)
//          aFolders.Append(m_native->iFolders[i]);
        m_native->getFolders(aFolders);

        // Access the Inbox
        QBENCHMARK {

          for(int i = 0; i < aFolders.Count(); i++){
            TMsvSelectionOrdering sort;
            CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, aFolders[i], sort);
            CleanupStack::PushL(inboxContext);

            // Get all entries in the Inbox
            CMsvEntrySelection* entries = inboxContext->ChildrenL();
            CleanupStack::PushL(entries);

            TInt messages = entries->Count();
            for (TInt i = 0; i < messages; i++) {
              TMsvId entryID = entries->At(i);
              m_native->iSmsMtm->SwitchCurrentEntryL(entryID);

              CMsvEntry* entry;

              entry = m_native->iSession->GetEntryL((*entries)[i]);

              delete entry;

            }

            CleanupStack::PopAndDestroy(2, inboxContext);
          }
        }
      }
#endif
    }

}

void tst_messaging::tst_fetch_data()
{
  //qDebug() << "Start fetch_data";
    QTest::addColumn<tst_messaging::platform>("platform");
    QTest::addColumn<tst_messaging::bases>("base");

    QTest::newRow("Qt-Accounts") << tst_messaging::QMessaging << tst_messaging::Accounts;
    QTest::newRow("Native-Accounts") << tst_messaging::Native << tst_messaging::Accounts;
    QTest::newRow("Qt-Folders") << tst_messaging::QMessaging << tst_messaging::Folders;
    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::Folders;
    QTest::newRow("Qt-Messages") << tst_messaging::QMessaging << tst_messaging::Messages;
    QTest::newRow("Native-Messages") << tst_messaging::Native << tst_messaging::Messages;
}

void tst_messaging::tst_fetch()
{
  
    QFETCH(tst_messaging::platform, platform);
    QFETCH(tst_messaging::bases, base);
    
    //qDebug() << "Start fetch" << platform << " base: " << base;  
    if(platform == tst_messaging::QMessaging){
      int cnt;
      Q_UNUSED(cnt);
      if(base == tst_messaging::Accounts){
        QMessageAccountIdList list;
        QMessageAccount acc;
        list = m_mgr->queryAccounts();

        if(!list.empty()){
          QBENCHMARK_ONCE { // XXX Maybe a bug here, if you call account a lot system crashes        
            acc = m_mgr->account(list.takeFirst());
          }
        }
        if(m_mgr->error()){
          QFAIL(QString("Failed fetching accounts: %1").arg(m_mgr->error()).toAscii());
        }
      }
      else if(base == tst_messaging::Folders){
        QMessageFolderIdList list;
        QMessageFolder fol;
        list = m_mgr->queryFolders();

        if(!list.empty()) {
          QBENCHMARK {          
            fol = m_mgr->folder(list.takeFirst());          
          }
        }        
      }
      else if(base == tst_messaging::Messages){
        QMessageIdList list;
        QMessage msg;
        list = m_mgr->queryMessages();
        
        if(!list.empty()){
          QBENCHMARK {
            msg = m_mgr->message(list.takeFirst());
          }        
        }
      }
      //qDebug() << "Got cnt: " << cnt;      
    }
    else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      if(base == tst_messaging::Messages){
        __UHEAP_MARK;
        // Access the Inbox
        QBENCHMARK {
          TMsvSelectionOrdering sort;
          sort.SetSorting(EMsvSortByDateReverse);
          sort.SetShowInvisibleEntries(ETrue);

          CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, 
                  KMsvDraftEntryIdValue /*KMsvGlobalInBoxIndexEntryId*/, sort);                
          CleanupStack::PushL(inboxContext);

          // Get all entries in the Inbox
          CMsvEntrySelection* entries = inboxContext->ChildrenL();
          CleanupStack::PushL(entries);

          if(entries->Count() == 0){
              CleanupStack::PopAndDestroy(2, inboxContext);
              QSKIP("No messages on device to list", SkipAll);              
          }
          TMsvId entryID = entries->At(0);
          const TUid mtm = inboxContext->ChildDataL(entryID).iMtm;

          if (mtm == KUidMsgTypeSMS) {
            m_native->iSmsMtm->SwitchCurrentEntryL(entryID);
            TRAPD(err, m_native->iSmsMtm->LoadMessageL());
            if(err){
              qDebug() << "LoadMessageL failed: " << err;
              continue;
            }
            CSmsHeader& header = m_native->iSmsMtm->SmsHeader();
          }
          else if (mtm == KUidMsgTypeMultimedia) {
            // TODO None of these have a data store...skip until it can be fixed
            QFAIL("MMS message handeling is broken, change setup to use non-MMS type");
          }
          else if (mtm == KUidMsgTypeIMAP4) {
            m_native->iImap4Mtm->SwitchCurrentEntryL(entryID);
            m_native->iImap4Mtm->LoadMessageL();            
          }
          else if (mtm == KUidMsgTypeSMTP){
            m_native->iSmtpMtm->SwitchCurrentEntryL(entryID);
            m_native->iSmtpMtm->LoadMessageL();
          }
          else {
            qDebug() << "Got Type: " << hex << mtm.iUid;
          }
          CleanupStack::PopAndDestroy(2, inboxContext);
        }
        __UHEAP_MARKEND;
      }

#endif
    }
//qDebug() <<"End fetch";
}

void tst_messaging::tst_fetchAll_data()
{
    QTest::addColumn<tst_messaging::platform>("platform");
    QTest::addColumn<tst_messaging::bases>("base");

    QTest::newRow("Qt-Accounts") << tst_messaging::QMessaging << tst_messaging::Accounts;
    QTest::newRow("Native-Accounts") << tst_messaging::Native << tst_messaging::Accounts;
//    QTest::newRow("Qt-Folders") << tst_messaging::QMessaging << tst_messaging::Folders;
    QTest::newRow("Native-Folders") << tst_messaging::Native << tst_messaging::Folders;
    QTest::newRow("Qt-Messages") << tst_messaging::QMessaging << tst_messaging::Messages;
    QTest::newRow("Native-Messages") << tst_messaging::Native << tst_messaging::Messages;

}


void tst_messaging::tst_fetchAll()
{
    QFETCH(tst_messaging::platform, platform);
    QFETCH(tst_messaging::bases, base);
    
    if(platform == tst_messaging::QMessaging){      
      if(base == tst_messaging::Accounts){
        QMessageAccountIdList list;
        QMessageAccount acc;
        list = m_mgr->queryAccounts();

        QBENCHMARK {        
          while(!list.empty())
            acc = m_mgr->account(list.takeFirst());
        }
      }
      else if(base == tst_messaging::Folders){
        QMessageFolderIdList list;
        QMessageFolder fol;
        list = m_mgr->queryFolders();

        QBENCHMARK {
          while(!list.empty())
            fol = m_mgr->folder(list.takeFirst());          
        }
      }
      else if(base == tst_messaging::Messages){
        QMessageIdList list;
        QMessage msg;
        list = m_mgr->queryMessages();
        if(list.count() == 0)
          QFAIL("No messages to fetch");

        qDebug() << "Total fetched messages: " << list.count(); 
        QBENCHMARK {
          while(!list.empty())
            msg = m_mgr->message(list.takeFirst());
        }        
      }
      //qDebug() << "Got cnt: " << cnt;      
    }
    else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      if(base == tst_messaging::Messages){
        
        __UHEAP_MARK;
        
        RArray<TMsvId> aFolders;
        int total = 0;
        int skipped = 0;
        
        m_native->getFolders(aFolders);

        // Access the Inbox
        QBENCHMARK {

          for(int i = 0; i < aFolders.Count(); i++){
            TMsvSelectionOrdering sort;
            //sort.SetSorting(EMsvSortByDateReverse);
            sort.SetSorting(EMsvSortByNone);
            sort.SetShowInvisibleEntries(ETrue);
            
            CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, 
                aFolders[i], sort);                
            CleanupStack::PushL(inboxContext);

            // Get all entries in the Inbox
            CMsvEntrySelection* entries = inboxContext->ChildrenL();
            CleanupStack::PushL(entries);
            
            for (TInt i = 0; i < entries->Count(); i++) {              
              
              TMsvId entryID = entries->At(i);
              
              const TUid mtm = inboxContext->ChildDataL(entryID).iMtm;
              
//              qDebug() << "Entry: " << i << " mtm: " << hex << mtm.iUid;
              
              if (mtm == KUidMsgTypeSMS) {

                TRAP_IGNORE(m_native->iSmsMtm->SwitchCurrentEntryL(entryID));                
                TRAPD(err, m_native->iSmsMtm->LoadMessageL());
                if(err){
                  qDebug() << "LoadMessageL failed: " << err;
                  continue;
                }                
                //CSmsHeader& header = m_native->iSmsMtm->SmsHeader();                
                total++;
              }
              else if (mtm == KUidMsgTypeMultimedia) {

                // TODO None of these have a data store...skip until it can be fixed 
                
//                                 CMsvEntry* pEntry = NULL;        
//                                 pEntry = m_native->iSession->GetEntryL(entryID);        
//                                 const TMsvEntry& entry = pEntry->Entry();    
//                                 
//                                 QString details = QString::fromUtf16(entry.iDetails.Ptr(),entry.iDetails.Length());
//                                 QString desc = QString::fromUtf16(entry.iDescription.Ptr(),entry.iDescription.Length());
//                                 qDebug() << "Nothing returned for entry: " << entryID << "/" << entry.Id() << " " << details << " - " << desc << " Has store: " << pEntry->HasStoreL();
////                                 m_native->iImap4Mtm->SwitchCurrentEntryL(entry.Id());

                
//                m_native->iMmsMtm->SwitchCurrentEntryL(entryID);
//                TRAPD(err, m_native->iMmsMtm->LoadMessageL());
//                if(err){
//                  qDebug() << "LoadMessageL failed: " << err << "entryId/mtm" << entryID << "/" << mtm.iUid;
//                  continue;
//                }
                skipped++;
              }
              else if (mtm == KUidMsgTypeIMAP4 || mtm == KUidMsgTypeSMTP) {                 
                 CMsvEntry* pEntry = NULL;        
                 pEntry = m_native->iSession->GetEntryL(entryID);

                 CImEmailMessage *pMsg = CImEmailMessage::NewLC(*pEntry);                 

                 CMsvStore *store = 0x0;
                 TRAPD(err, store = pEntry->ReadStoreL());
                 if(err){
//                   TPtrC sub;
//                   m_native->iImap4Mtm->SwitchCurrentEntryL(entryID);
//                   m_native->iImap4Mtm->LoadMessageL();                   
//                   TRAP(err,sub.Set(m_native->iImap4Mtm->SubjectL()));
//                   if(err){
//                     qDebug() << "No subject either: " << err;                     
//                   }                   
//                   qDebug() << "Can't read store: " << err << hex << entryID << " Details: " << QString::fromUtf16(sub.Ptr(), sub.Length());
                   skipped++;
                   CleanupStack::PopAndDestroy(pMsg);
                   continue;
                 }
                 CleanupStack::PushL(store);
                 
                 CImHeader* header = CImHeader::NewLC();                 
                 header->RestoreL(*store);
                 
                 //subject buffer contains the "subject" of the mail.
                 TBuf<50> subject = header->Subject();
                  
                 //header buffer contains the "header" of the mail.
                 TBuf<50> from = header->From();

                 CleanupStack::PopAndDestroy(header);
                 CleanupStack::PopAndDestroy(store);
                 CleanupStack::PopAndDestroy(pMsg);
                                  
                 total++;
               }              
              else {
//                qDebug() << "Got Type: " << mtm.iUid;
              }              
            }
            CleanupStack::PopAndDestroy(2, inboxContext);           
          }          
        }
//        qDebug() << "Total messages fetched: " << total << " skipped: " << skipped;
        __UHEAP_MARKEND;
      }            
#endif
    }

}



void tst_messaging::tst_fetchFilter_data()
{
  QTest::addColumn<tst_messaging::platform>("platform");
  QTest::addColumn<tst_messaging::filter>("filter");
  
  QTest::newRow("Qt-Id") << tst_messaging::QMessaging << tst_messaging::Id;
  QTest::newRow("Qt-Type") << tst_messaging::QMessaging << tst_messaging::Type;
  QTest::newRow("Qt-Sender") << tst_messaging::QMessaging << tst_messaging::Sender;
  QTest::newRow("Qt-Subject") << tst_messaging::QMessaging << tst_messaging::Subject;
  QTest::newRow("Qt-TimeStamp") << tst_messaging::QMessaging << tst_messaging::TimeStamp;
  QTest::newRow("Qt-Status") << tst_messaging::QMessaging << tst_messaging::Status;
  QTest::newRow("Qt-Priority") << tst_messaging::QMessaging << tst_messaging::Priority;
  QTest::newRow("Qt-Size") << tst_messaging::QMessaging << tst_messaging::Size;
  // XXX Test is broken, User::Panic(47) on symbian
//  QTest::newRow("Qt-AllId") << tst_messaging::QMessaging << tst_messaging::AllId;
  QTest::newRow("Qt-AllSender") << tst_messaging::QMessaging << tst_messaging::AllSender;
  
  QTest::newRow("Native-Id") << tst_messaging::Native << tst_messaging::Id;
  QTest::newRow("Native-Type") << tst_messaging::Native << tst_messaging::Type;
  QTest::newRow("Native-Sender") << tst_messaging::Native << tst_messaging::Sender;
  QTest::newRow("Native-Subject") << tst_messaging::Native << tst_messaging::Subject;
  QTest::newRow("Native-TimeStamp") << tst_messaging::Native << tst_messaging::TimeStamp;
  QTest::newRow("Native-Status") << tst_messaging::Native << tst_messaging::Status;
  QTest::newRow("Native-Priority") << tst_messaging::Native << tst_messaging::Priority;
  QTest::newRow("Native-Size") << tst_messaging::Native << tst_messaging::Size;  
  QTest::newRow("Native-AllId") << tst_messaging::Native << tst_messaging::AllId;
  QTest::newRow("Native-AllSender") << tst_messaging::Native << tst_messaging::AllSender;
  
  // QTest::newRow("Native-Size") << tst_messaging::Native << tst_messaging::Size;
}

void tst_messaging::tst_fetchFilter()
{
  QFETCH(tst_messaging::platform, platform);
  QFETCH(tst_messaging::filter, filter);
  
  if(platform == tst_messaging::QMessaging){

    QMessageId msgId;
    QMessageFilter mf;
    QMessageId id;
    // let's assume we want equal tests for all the filters.
    // So let's try and filter out 1 message from each filter request.    
    if(filter == tst_messaging::Id){
      id = msg_list.takeFirst();
        mf = QMessageFilter::byId(id);
        msg_list.push_back(id);        
    }
    else if(filter == tst_messaging::Type){      
        QMessage *msg = messageTemplate();
        
        msgId = addMessage(msg);       
        mf = QMessageFilter::byType(QMessage::Sms);
        
    }  
    else if(filter == tst_messaging::Sender){
        QString email = "singletst@boo.com";
        QMessage *msg = messageTemplate();
        QMessageAddress addr;
        addr.setAddressee(email);
        msg->setFrom(addr);
        msgId = addMessage(msg);

        id = msg_list.takeFirst();
        mf = QMessageFilter::bySender(email);
    }
    else if(filter == tst_messaging::TimeStamp){
        QMessage *msg = messageTemplate();
        QDateTime dt = QDateTime::currentDateTime();
        dt.addDays(1);
        msg->setReceivedDate(dt);
        msg->setDate(dt);
        msgId = addMessage(msg);
        
        mf = QMessageFilter::byTimeStamp(dt, QMessageDataComparator::Equal);
    }
    else if(filter == tst_messaging::Status){
        QMessage *msg = messageTemplate();
        msg->setStatus(QMessage::Incoming);
        msgId = addMessage(msg);

        mf = QMessageFilter::byStatus(QMessage::Incoming);
    }
    else if(filter == tst_messaging::Subject){
      QMessage *msg = messageTemplate();
      QString subject = "skdflkdsjfl sdfklke  werewr";
      msg->setSubject(subject);
      msgId = addMessage(msg);
      
      mf = QMessageFilter::bySubject(subject);
    }
    else if(filter == tst_messaging::Priority){
        QMessage *msg = messageTemplate();
        msg->setPriority(QMessage::LowPriority);
        msgId = addMessage(msg);

        mf = QMessageFilter::byPriority(QMessage::LowPriority);
    }
    else if(filter == tst_messaging::Size){
        QString body;
        body.fill('x', 5120);
        QMessage *msg = messageTemplate();
        msg->setBody(body);
        msgId = addMessage(msg);

        mf = QMessageFilter::bySize(5000, QMessageDataComparator::GreaterThan);
    }
    else if(filter == tst_messaging::AllId){
//      mf = QMessageFilter::byId(msg_list.mid(0, 2)); works, mid(0,15) fails...
      mf = QMessageFilter::byId(msg_list);
    }
    else if(filter == tst_messaging::AllSender){
      id = msg_list.takeFirst();      
      msg_list.push_back(id);   
      QMessage msg = m_mgr->message(id);
      
      mf = QMessageFilter::bySender(msg.from().addressee());
    }
    if(!mf.isSupported()){
        QFAIL("QMessage filter returned unsupported");
    }
    else if(mf.isEmpty()){
      QFAIL("Empty filter provided");
    }
    else {
        QMessageIdList list;
        QMessageSortOrder sortOrder(QMessageSortOrder::byReceptionTimeStamp(Qt::DescendingOrder));
        QBENCHMARK {          
            list = m_mgr->queryMessages(mf, sortOrder, 100);
        }
        if(list.count() != 1 && filter != tst_messaging::AllId && filter != tst_messaging::AllSender)
            qDebug() << "Wanted 1 message got: " << list.count();
    }

    if(msgId.isValid()){
        m_mgr->removeMessage(msgId);
    }
  }
  else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
    if(filter == tst_messaging::Id){
      // Access the Inbox
      QBENCHMARK {
        TMsvSelectionOrdering sort;
        //sort.SetSorting(EMsvSortByDateReverse);
        sort.SetSorting(EMsvSortById);
        sort.SetShowInvisibleEntries(ETrue);
          
        CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, 
                KMsvDraftEntryIdValue /*KMsvGlobalInBoxIndexEntryId*/, sort);                
        CleanupStack::PushL(inboxContext); // 1

        // Get all entries in the Inbox
        CMsvEntrySelection* entries = inboxContext->ChildrenL();
        CleanupStack::PushL(entries); // 2
        if(entries->Count() == 0){
            CleanupStack::PopAndDestroy(2, inboxContext);
            QSKIP("No messages on device to list", SkipAll);                        
        }
        
        TMsvId entryID = entries->At(0);

        const TUid mtm = inboxContext->ChildDataL(entryID).iMtm;
      
        CleanupStack::PopAndDestroy(2, inboxContext);
      }
    }
    else if(filter == tst_messaging::Type){ // look for SMS messages
      // Access the Inbox
      RArray<TMsvId> msvids;
      RArray<TMsvId> aFolders;
    
      m_native->getFolders(aFolders);
      
      //qDebug() << "Total folders: " << aFolders.Count();
    
      QBENCHMARK {
        msvids.Close(); // zero it out
        for(int i = 0; i < aFolders.Count(); i++){

          TMsvSelectionOrdering sort;
          //sort.SetSorting(EMsvSortByDateReverse);
          sort.SetSorting(EMsvSortByNone);
          sort.SetShowInvisibleEntries(ETrue);

          CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, 
              aFolders[i], sort);                
          CleanupStack::PushL(inboxContext);

          // Get all entries in the Inbox
          CMsvEntrySelection* entries = inboxContext->ChildrenL();
          CleanupStack::PushL(entries);

          for (TInt j = 0; j < entries->Count(); j++) {
            TMsvId entryID = entries->At(j);
            const TUid mtm = inboxContext->ChildDataL(entryID).iMtm;

            if (mtm == KUidMsgTypeSMS) {
              msvids.AppendL(entryID);
            }
          }      
          CleanupStack::PopAndDestroy(2, inboxContext);
        }
      }
      msvids.Close();
    }
    else if(filter == tst_messaging::Sender || 
        filter == tst_messaging::Subject ||
        filter == tst_messaging::TimeStamp){
      // Access the Inbox
        
      TMsvPartList part = KMsvMessagePartDescription;
      TPtrC text = _L("tst");
      
      if(filter == tst_messaging::Sender){
        part = KMsvMessagePartOriginator;
        text.Set(_L("Millicent"));
      }
      else if(filter == tst_messaging::Subject){
        part = KMsvMessagePartDescription;
        text.Set(_L("Free Bottle"));
      }
      else if(filter == tst_messaging::TimeStamp){
        part = KMsvMessagePartDate;
        text.Set(_L("Sep"));
      }

      CMsvOperationActiveSchedulerWait* waiter=CMsvOperationActiveSchedulerWait::NewLC();
      
      CMsvFindOperation *finder = CMsvFindOperation::FindInChildrenL(*m_native->iSession, text, KMsvRootIndexEntryId, part, waiter->iStatus);
      CleanupStack::PushL(finder);

      QBENCHMARK {      
        waiter->Start();
      }
      if(finder->GetFindResult().Count() != 1)
        qDebug() << "Wanted 1 message, got: " << finder->GetFindResult().Count(); 
      
//      qDebug() << "Status: " << finder->GetFindResult().Count();
//      for(int i = 0; i < finder->GetFindResult().Count(); i++){
//        CMsvEntry *pEntry;
//        TRAPD(err, pEntry = m_native->iSession->GetEntryL(finder->GetFindResult().At(i).iId));
//        if(err){
//          qDebug() << "Failed to fetch: " << i << " - " << finder->GetFindResult().At(i).iId;
//          continue;
//        }
//        CleanupStack::PushL(pEntry);
//        TMsvEntry entry = pEntry->Entry();
//        QString details = QString::fromUtf16(entry.iDetails.Ptr(),entry.iDetails.Length());
//        QString desc = QString::fromUtf16(entry.iDescription.Ptr(),entry.iDescription.Length());
//        qDebug() << "Got entry: " << details << " - " << desc;
//        CleanupStack::PopAndDestroy(1); //
//      }
       
      CleanupStack::PopAndDestroy(2, waiter); 
      
    }
    else if(filter == tst_messaging::Priority) {
      RArray<TMsvId> msvids;

      CMsvOperationActiveSchedulerWait* waiter=CMsvOperationActiveSchedulerWait::NewLC();
      
      TBufC<10> null(_L(" "));
      CMsvFindOperation *finder = CMsvFindOperation::FindInChildrenL(*m_native->iSession, null, KMsvRootIndexEntryId, KMsvMessagePartDescription, waiter->iStatus);
      CleanupStack::PushL(finder);

      QBENCHMARK {      
        waiter->Start();
        
        const CMsvFindResultSelection &res = finder->GetFindResult();
        
        for(int i = 0; i < res.Count(); i++){          
          CMsvEntry *pEntry = 0x0;
          TRAPD(err, pEntry = m_native->iSession->GetEntryL(res.At(i).iId));
          if(err){
            qDebug() << "Failed to fetch: " << i << " - " << res.At(i).iId;
            continue;
          }
          CleanupStack::PushL(pEntry);
          TMsvEntry entry = pEntry->Entry();

          if(entry.Priority() == EMsvLowPriority) 
            msvids.Append(entry.Id());
          
          CleanupStack::PopAndDestroy(1); //
        }
      }
      msvids.Close();
      CleanupStack::PopAndDestroy(2, waiter);
    }
    else if(filter == tst_messaging::Size){
      RArray<TMsvId> msvids;

      CMsvOperationActiveSchedulerWait* waiter=CMsvOperationActiveSchedulerWait::NewLC();
      
      TBufC<10> null(_L(" "));
      CMsvFindOperation *finder = CMsvFindOperation::FindInChildrenL(*m_native->iSession, null, KMsvRootIndexEntryId, KMsvMessagePartDescription, waiter->iStatus);
      CleanupStack::PushL(finder);

      QBENCHMARK {      
        waiter->Start();
        
        const CMsvFindResultSelection &res = finder->GetFindResult();
        
        for(int i = 0; i < res.Count(); i++){          
          CMsvEntry *pEntry = 0x0;
          TRAPD(err, pEntry = m_native->iSession->GetEntryL(res.At(i).iId));
          if(err){
            qDebug() << "Failed to fetch: " << i << " - " << res.At(i).iId;
            continue;
          }
          CleanupStack::PushL(pEntry);
          TMsvEntry entry = pEntry->Entry();

          if(entry.iSize == 550)
            msvids.Append(entry.Id());          
          
          CleanupStack::PopAndDestroy(1); //
        }
      }
      msvids.Close();
      CleanupStack::PopAndDestroy(2, waiter);      
    }
    if(filter == tst_messaging::AllId){
      // Access the Inbox
      QBENCHMARK {
        RArray<TMsvId> msvids;
        
        TMsvSelectionOrdering sort;
        //sort.SetSorting(EMsvSortByDateReverse);
        sort.SetSorting(EMsvSortById);
        sort.SetShowInvisibleEntries(ETrue);
          
        CMsvEntry* inboxContext = CMsvEntry::NewL(*m_native->iSession, 
            KMsvGlobalInBoxIndexEntryId, sort);                
        CleanupStack::PushL(inboxContext);

        // Get all entries in the Inbox
        CMsvEntrySelection* entries = inboxContext->ChildrenL();
        CleanupStack::PushL(entries);
        
        for(int i = 0; i < entries->Count(); i++){
          msvids.Append(entries->At(i));
        }

        msvids.Close();       
        
        CleanupStack::PopAndDestroy(2, inboxContext);
      }
    }
    else if(filter == tst_messaging::AllSender){
      RArray<TMsvId> msvids;

      CMsvOperationActiveSchedulerWait* waiter=CMsvOperationActiveSchedulerWait::NewLC();
      
      TBufC<10> null(_L("@"));
      CMsvFindOperation *finder = CMsvFindOperation::FindInChildrenL(*m_native->iSession, null, KMsvRootIndexEntryId, KMsvMessagePartOriginator, waiter->iStatus);
      CleanupStack::PushL(finder);

      QBENCHMARK {      
        waiter->Start();
        
        const CMsvFindResultSelection &res = finder->GetFindResult();        
        TMsvSelectionOrdering order;
        
        CMsvEntry *pEntry = CMsvEntry::NewL(*m_native->iSession, KMsvRootIndexEntryId, TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone));
        CleanupStack::PushL(pEntry);
        
        for(int i = 0; i < res.Count(); i++){
          TRAPD(err, pEntry->SetEntryL(res.At(i).iId));          
          if(err){
            qDebug() << "Failed to fetch: " << i << " - " << res.At(i).iId;
            continue;
          }
          TMsvEntry entry = pEntry->Entry();
          
          msvids.Append(entry.Id());          
                  
        }
        
        CleanupStack::PopAndDestroy(pEntry);
      }
      msvids.Close();
      CleanupStack::PopAndDestroy(2, waiter);          
    }
#endif
  }      
}

void tst_messaging::clearMessages()
{
  QMessageId id;
  int num = 0;
  qDebug() << "Start QtMessaging removal";
  while(!msg_list.empty())
    m_mgr->removeMessage(msg_list.takeFirst()), num++;
  qDebug() << "Start Native removal";
#ifdef Q_OS_SYMBIAN
  while(m_native->iNewMsgs.Count())
    m_native->iSession->RemoveEntry(m_native->iNewMsgs[0]), m_native->iNewMsgs.Remove(0), num++;
#endif

  qDebug() << "Cleanup Removed: " << num;
}

QMessage *tst_messaging::messageTemplate()
{
  QMessage *msg = new QMessage;
  
  msg->setType(QMessage::Email);
  msg->setDate(QDateTime::currentDateTime());
  msg->setSubject("test");
  QMessageAddress addr;
  addr.setAddressee("abr@foo.com");
  addr.setType(QMessageAddress::Email);
  msg->setTo(addr);
  addr.setAddressee("from@bar.com");
  addr.setType(QMessageAddress::Email);
  msg->setFrom(addr);
  
  return msg;
}

QMessageId tst_messaging::addMessage(QMessage *msg)
{
  if(!m_mgr->addMessage(msg))
    qDebug() << "Failed to add message: " << m_mgr->error();  

  msg_list.append(msg->id());

  //qDebug() << "id: " << msg->id().toString();
  
  return msg->id();
}

QMessage *tst_messaging::createMessage()
{
    QMessage *msg = messageTemplate();
    addMessage(msg);
    return msg;
}

int tst_messaging::countMessages()
{
  QMessageIdList list;
  list = m_mgr->queryMessages();
  return list.count();
}

void tst_messaging::tst_addMessage_data()
{
    QTest::addColumn<tst_messaging::platform>("platform");    
    QTest::addColumn<tst_messaging::types>("type");
    
    QTest::newRow("Qt-Email") << tst_messaging::QMessaging << tst_messaging::Email;
// XXX SMS broken. On clean devices there is no SMSMC to create messages for.    
//    QTest::newRow("Native-SMS") << tst_messaging::Native << tst_messaging::Sms; 
    QTest::newRow("Native-Email") << tst_messaging::Native << tst_messaging::Email;
   
}

void tst_messaging::tst_addMessage()
{
    QFETCH(tst_messaging::platform, platform);
    QFETCH(tst_messaging::types, type);
    
    if(platform == tst_messaging::QMessaging){
      QBENCHMARK {
        QMessage *msg = createMessage();
        addMessage(msg);
      }
    }
    else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      if(type == tst_messaging::Sms){
        TMsvId id;
        QBENCHMARK {
          TRAPD(err, id = m_native->makeSMS(KMsvGlobalInBoxIndexEntryId));
          if(err){
              qDebug() << "Failed to create sms: " << err;
          }
        }
        m_native->iNewMsgs.Append(id);
      }
      else if(type == tst_messaging::Email){
        TMsvId id;
        QBENCHMARK {
          TRAPD(err, id = m_native->makeIMAP4(KMsvGlobalInBoxIndexEntryId));
          if(err){
              qDebug() << "Failed to create sms: " << err;
          }

        }
        m_native->iNewMsgs.Append(id);
      }
#else 
    Q_UNUSED(type);
#endif
    }

}

void tst_messaging::tst_removeMessage_data()
{
  QTest::addColumn<tst_messaging::platform>("platform");    

  QTest::newRow("Qt-remove") << tst_messaging::QMessaging;
  QTest::newRow("Native-remove") << tst_messaging::Native;  
}

void tst_messaging::tst_removeMessage() 
{
  QFETCH(tst_messaging::platform, platform);

  if(platform == tst_messaging::QMessaging){
//#ifdef Q_OS_SYMBIAN
//      TMsvId id;
//      id = m_native->makeIMAP4(KMsvDraftEntryIdValue);
//      QString str;
//      str.setNum(id);
//      QMessageId qmid = str;    
//#else
      QMessageId qmid = msg_list.takeFirst();
//#endif

   QBENCHMARK_ONCE {
          m_mgr->removeMessage(qmid);
    }
  }  
  else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
      TMsvId id;
      id = m_native->makeIMAP4(KMsvDraftEntryIdValue);      
      CMsvEntry *pEntry = CMsvEntry::NewL(*m_native->iSession, id, TMsvSelectionOrdering());
      CleanupStack::PushL(pEntry);
      QBENCHMARK_ONCE {    // We're only guaranteed one entry to delete.  Not a fast process anyways.s
          TRAPD(err, pEntry->DeleteL(id)); // slightly dangerous since we never want this failing, but it can fail too something, should debug TODO
          if(err){
          QFAIL(QString("Message delete failed with error: " + QString::number(err)).toAscii());
          }
      }
      CleanupStack::PopAndDestroy(pEntry);          
#endif
  }

}

void tst_messaging::tst_removeAllMessage_data()
{
  QTest::addColumn<tst_messaging::platform>("platform");    

  QTest::newRow("Qt-removeAll") << tst_messaging::QMessaging;
  QTest::newRow("Native-removeAll") << tst_messaging::Native;  
}

void tst_messaging::tst_removeAllMessage() 
{
  QFETCH(tst_messaging::platform, platform);

  if(platform == tst_messaging::QMessaging){
    QBENCHMARK {
      while(!msg_list.empty())
        m_mgr->removeMessage(msg_list.takeFirst());
    }
  }
  else if(platform == tst_messaging::Native){
#ifdef Q_OS_SYMBIAN
    // make sure there's enough messages to delete
    while(m_native->iNewMsgs.Count() < 30) {
      TMsvId id;
      id = m_native->makeIMAP4(KMsvDraftEntryIdValue);      
      m_native->iNewMsgs.Append(id);
    }
    
    QBENCHMARK {      
      CMsvEntry *pEntry = 0x0;
      TRAPD(err, pEntry = CMsvEntry::NewL(*m_native->iSession, KMsvRootIndexEntryId, TMsvSelectionOrdering()));
      if(err){
        qDebug() << "Failed to init CMsvEntryL " << err;
        return;
      }
      CleanupStack::PushL(pEntry);
      while(m_native->iNewMsgs.Count() != 0) {
        TMsvId id = m_native->iNewMsgs[0];
        m_native->iNewMsgs.Remove(0);    
        //pEntry->SetEntryL(id);
        TRAP_IGNORE(pEntry->DeleteL(id));                
      }
      CleanupStack::PopAndDestroy(pEntry);
    }
#endif
  }

}

#if defined(Q_OS_SYMBIAN)

void OsNative::getFolders(RArray<TMsvId> &folders) {
  __UHEAP_MARK;

  CEmailAccounts *email = CEmailAccounts::NewLC();
  RArray<TPopAccount> aPop;
  RArray<TImapAccount> aImap;
  RArray<TSmtpAccount> aSmtp;

#ifndef KDocumentsEntryIdValue
#define KDocumentsEntryIdValue    0x1008
#endif
  
  folders.Append(KMsvRootIndexEntryId);
  folders.Append(KMsvLocalServiceIndexEntryId);
  folders.Append(KMsvGlobalInBoxIndexEntryId);
  folders.Append(KMsvGlobalOutBoxIndexEntryId);
  folders.Append(KMsvDraftEntryId);
  folders.Append(KMsvSentEntryId);
  folders.Append(KMsvDeletedEntryFolderEntryId);
  folders.Append(KDocumentsEntryIdValue);
  folders.Append(KMsvMyFoldersEntryId);

  email->GetPopAccountsL(aPop);
  email->GetImapAccountsL(aImap);
  email->GetSmtpAccountsL(aSmtp);

  RArray<TMsvId> aService;
  
//  aService.Append(KMsvLocalServiceIndexEntryId);
//  aService.Append(KMsvRootIndexEntryId);

  for(int i = 0; i < aPop.Count(); i++){
    aService.Append(aPop[i].iPopService);
  }
  for(int i = 0; i < aImap.Count(); i++){
    aService.Append(aImap[i].iImapService);
  }
  for(int i = 0; i < aSmtp.Count(); i++){
    aService.Append(aSmtp[i].iSmtpService);
  }
  
  TSmtpAccount sacc;
  if(email->DefaultSmtpAccountL(sacc))
    aService.Append(sacc.iSmtpService);

  if(aService.Count() > 0) {
      CMsvEntry* pEntry = NULL;
      
      TRAPD(err, pEntry = iSession->GetEntryL(aService[0]));
      if(!err) {
          CleanupStack::PushL(pEntry);
          for(int i = 0; i < aService.Count(); i++){
              TMsvId msvid = aService[i];
              
              TRAP(err, pEntry->SetEntryL(msvid)); // faster to call set, saves .2ms out of 2.7ms.
              if(err)
                  continue;
        
              const TMsvEntry& entry = pEntry->Entry();
              CMsvEntryFilter* pFilter = CMsvEntryFilter::NewLC();
              pFilter->SetService(msvid);
              pFilter->SetType(KUidMsvFolderEntry);
              CMsvEntrySelection* pSelection = new(ELeave) CMsvEntrySelection;
              CleanupStack::PushL(pSelection);

              if (entry.iMtm == KUidMsgTypeSMS || entry.iMtm == KUidMsgTypeMultimedia || entry.iMtm == KUidMsgTypeSMTP) 
                pEntry->SetEntryL(KDocumentsEntryIdValue);          
              
              iSession->GetChildIdsL(msvid, *pFilter, *pSelection);
              for(TInt i = 0; i < pSelection->Count(); i++) {
                folders.Append(pSelection->At(i));
                pEntry->SetEntryL(pSelection->At(i));
                if(pEntry->Entry().iMtm == KUidMsvFolderEntry){
                    aService.Append(pSelection->At(i));
                }            
              }              
              CleanupStack::PopAndDestroy(pSelection);
              CleanupStack::PopAndDestroy(pFilter);
        
            }
          CleanupStack::PopAndDestroy(pEntry);
        }
  }
    aPop.Close();
    aImap.Close();
    aSmtp.Close();
    aService.Close();
        
    CleanupStack::PopAndDestroy(email);
  __UHEAP_MARKEND;
}
#endif 

int main(int argc, char **argv){

    QApplication app(argc, argv);
    
    tst_messaging test1;
    QTest::qExec(&test1, argc, argv);
    
}
//QTEST_MAIN(tst_messaging);


#include "tst_messaging.moc"

