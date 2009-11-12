#include "messagingex.h"
#include "QMessageStore.h"
#include "QMessageFilter.h"
#include <QFileDialog>

MessagingEx::MessagingEx(QWidget* parent)
     : QMainWindow(parent)
{
    setupUi(this);
    createMenus();
    m_store = QMessageStore::instance();
    connect(m_store, SIGNAL(messageAdded(const QMessageId&, const QMessageStore::NotificationFilterIdSet&)), this, SLOT(messageReceived(const QMessageId&)));
    connect(m_store, SIGNAL(messageRemoved(const QMessageId&, const QMessageStore::NotificationFilterIdSet&)), this, SLOT(messageRemoved(const QMessageId&)));
    connect(m_store, SIGNAL(messageUpdated(const QMessageId&, const QMessageStore::NotificationFilterIdSet&)), this, SLOT(messageUpdated(const QMessageId&)));
    m_store->registerNotificationFilter(QMessageFilter::byStandardFolder(QMessage::DraftsFolder/*InboxFolder*/));    
    connect(&m_serviceaction, SIGNAL(messagesFound(const QMessageIdList&)), this, SLOT(messagesFound(const QMessageIdList&)));
    m_accountList = m_store->queryAccounts(QMessageAccountFilter(), QMessageAccountOrdering(), 10 , 0);
    for(int i = 0; i < m_accountList.count(); ++i){
        QMessageAccount account = QMessageAccount(m_accountList[i]);
        accountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		accountComboBox_2->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		folderAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		timeAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		timestampAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		subjectAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		recipientAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		statusAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		senderAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
		priorityAccountComboBox->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.id().toString());
    }
    connect(accountComboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(sortParentAccountId()));
}

void MessagingEx::createMenus()
{
    QAction* sendAction = new QAction(tr("Send"), this);
    connect(sendAction, SIGNAL(triggered()), this, SLOT(send()));
    
    m_createEmail = new QAction(tr("Create Message"), this);
    m_createSms = new QAction(tr("Create Text Message"), this);
    m_createMms = new QAction(tr("Create Multimedia Message"), this);
    m_sortId = new QAction(tr("By message id"), this);
    m_sortType = new QAction(tr("By priority"), this);
    m_sortSender = new QAction(tr("By sender"), this);
    m_sortRecipient = new QAction(tr("By recipient"), this);
    m_sortSubject = new QAction(tr("By subject"), this);
    m_sortTimestamp = new QAction(tr("By timestamp"), this);
    m_sortReceptiontimestamp = new QAction(tr("By receptiontimestamp"), this);
    m_sortStatus = new QAction(tr("By status"), this);
    m_sortParentAccountId = new QAction(tr("Messages by account"), this);
    m_sortStandardFolder = new QAction(tr("By standardFolder"), this);
    m_result = new QAction(tr("Results"), this);
    
    connect(m_createEmail, SIGNAL(triggered(bool)), m_createEmail, SLOT(setEnabled(bool)));
    connect(m_createEmail, SIGNAL(triggered(bool)), m_createSms, SLOT(setDisabled(bool)));
    connect(m_createEmail, SIGNAL(triggered(bool)), m_createMms, SLOT(setDisabled(bool)));
    
    connect(m_createSms, SIGNAL(triggered(bool)), m_createSms, SLOT(setEnabled(bool)));
    connect(m_createSms, SIGNAL(triggered(bool)), m_createEmail, SLOT(setDisabled(bool)));
    connect(m_createSms, SIGNAL(triggered(bool)), m_createMms, SLOT(setDisabled(bool)));
    
    connect(m_createMms, SIGNAL(triggered(bool)), m_createMms, SLOT(setEnabled(bool)));
    connect(m_createMms, SIGNAL(triggered(bool)), m_createEmail, SLOT(setDisabled(bool)));
    connect(m_createMms, SIGNAL(triggered(bool)), m_createSms, SLOT(setDisabled(bool)));
    
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(m_createSms, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_createSms, 0);
    connect(m_createEmail, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_createEmail, 1);
    connect(m_createMms, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_createMms, 2);
    connect(m_sortType, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortType, 3);
    connect(m_sortSender, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortSender, 4);
    connect(m_sortRecipient, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortRecipient, 5);
    connect(m_sortSubject, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortSubject, 6);
    connect(m_sortStatus, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortStatus, 7);
    connect(m_sortTimestamp, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortTimestamp, 8);
    connect(m_sortReceptiontimestamp, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortReceptiontimestamp, 9);
    connect(m_sortStandardFolder, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortStandardFolder, 10);
    connect(m_sortParentAccountId, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_sortParentAccountId, 11);
    connect(m_result, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_result, 12);
    
    connect(signalMapper, SIGNAL(mapped(int)), stackedWidget, SLOT(setCurrentIndex(int)));
    
    connect(appendAttachment,SIGNAL(clicked(bool)),this,SLOT(addAttachment()));
    connect(addEmailAttachmentButton,SIGNAL(clicked(bool)),this,SLOT(addAttachment()));
    connect(removeAttachmentButton,SIGNAL(clicked(bool)),this,SLOT(removeAttachment()));
    connect(removeAttachmentButton_2,SIGNAL(clicked(bool)),this,SLOT(removeAttachment()));
    connect(addMessageButton,SIGNAL(clicked(bool)),this,SLOT(addMessage()));
    
    filterMenu = menuBar()->addMenu(tr("&Filter"));
    filterMenu->addAction(m_sortId);
    m_sortId->setDisabled(true);
    // TODO: add filter by id functionality
    filterMenu->addAction(m_sortParentAccountId);
    connect(findMessagesButton, SIGNAL(clicked(bool)),this,SLOT(findMessages()));
    filterMenu->addAction(m_sortType);
    connect(typeButton, SIGNAL(clicked(bool)),this,SLOT(sortType()));
    filterMenu->addAction(m_sortSender);
    connect(senderButton, SIGNAL(clicked(bool)),this,SLOT(sortSender()));
    filterMenu->addAction(m_sortRecipient);
    connect(recipientButton, SIGNAL(clicked(bool)),this,SLOT(sortRecipient()));
    filterMenu->addAction(m_sortSubject);
    connect(subjectButton, SIGNAL(clicked(bool)),this,SLOT(sortSubject()));
    filterMenu->addAction(m_sortTimestamp);
    connect(timestampButton, SIGNAL(clicked(bool)),this,SLOT(sortTimestamp()));
    filterMenu->addAction(m_sortReceptiontimestamp);
    connect(receptionButton, SIGNAL(clicked(bool)),this,SLOT(sortReceptiontimestamp()));
    filterMenu->addAction(m_sortStatus);
    connect(statusButton, SIGNAL(clicked(bool)),this,SLOT(sortStatus()));
    filterMenu->addAction(m_sortStandardFolder);
    connect(folderButton, SIGNAL(clicked(bool)),this,SLOT(sortStandardFolder()));
    
    composeMenu = menuBar()->addMenu(tr("&Compose"));

    m_composeSMS = new QAction(tr("Compose SMS"), this);
    composeMenu->addAction(m_composeSMS);
    connect(m_composeSMS, SIGNAL(triggered()), this, SLOT(composeSMS()));
    
    m_composeMMS = new QAction(tr("Compose MMS"), this);
    composeMenu->addAction(m_composeMMS);
    connect(m_composeMMS, SIGNAL(triggered()), this, SLOT(composeMMS()));
    
    m_composeEmail = new QAction(tr("Compose Email"), this);
    composeMenu->addAction(m_composeEmail);
    connect(m_composeEmail, SIGNAL(triggered()), this, SLOT(composeEmail()));
     
    QAction* exitAct = new QAction(tr("Exit"), this);
    menuBar()->addAction(sendAction);
    QMenu* listMenu = menuBar()->addMenu(tr("Email accounts"));

    menuBar()->addAction(m_createEmail);
    menuBar()->addAction(m_createSms);
    menuBar()->addAction(m_createMms);
    menuBar()->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MessagingEx::send()
{
    if ( stackedWidget->currentIndex() == 0 )
    	on_sendSmsButton_clicked();
    if ( stackedWidget->currentIndex() == 1 )
    	on_sendEmailButton_clicked();
    if ( stackedWidget->currentIndex() == 2 )
    	on_sendMmsButton_clicked();
}

void MessagingEx::addAttachment()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Message Example"), "c:\\Data");
    QString labelText;
    if (!fileName.isNull()) 
    {
        m_attachments.append(fileName);
        m_fileNames.append(fileName);
        m_fileNames.append("\n");
    } 
    attachmentLabel->setText(m_fileNames);
    emailAttachmentLabel->setText(m_fileNames);
}

void MessagingEx::removeAttachment()
{
	m_attachments.clear();
	emailAttachmentLabel->clear();
	m_fileNames.clear();
}
void MessagingEx::composeSMS()
{
	QMessage message;
	message.setType(QMessage::Sms);
    m_serviceaction.compose(message);
}

void MessagingEx::composeMMS()
{
	QMessage message;
	message.setType(QMessage::Mms);
    m_serviceaction.compose(message);
}

void MessagingEx::composeEmail()
{
	QMessage message;
	message.setType(QMessage::Email);
    m_serviceaction.compose(message);
}

void MessagingEx::addMessage()
{
	QMessage message;
	int index = accountComboBox->currentIndex();
	m_account = QMessageAccount(m_accountList[index]).id();
	QMessage::TypeFlags types = m_account.messageTypes();
	
	if (!emailAddressEdit->text().isEmpty()){
		message.setTo(QMessageAddress(emailAddressEdit->text(), QMessageAddress::Email));
	}
	
	message.setParentAccountId(m_account.id());
	message.setSubject(subjectEdit->text());
	message.setBody(QString(emailMessageEdit->toPlainText()));
	message.appendAttachments(m_attachments);
	
	if (!m_attachments.isEmpty() && types == QMessage::Sms){
		QMessageBox msgBox;
		msgBox.setText(tr("Cannot send attachments via Sms!"));
		msgBox.exec();
		m_attachments.clear();
		m_fileNames.clear();
		attachmentLabel->clear();
    } else {
		m_store->addMessage(&message);
		emailAddressEdit->clear();
		subjectEdit->clear();
		emailMessageEdit->clear();  
		m_attachments.clear();
		m_fileNames.clear();
		emailAttachmentLabel->clear();
		attachmentLabel->clear();
		}   	
}


void MessagingEx::on_sendSmsButton_clicked()
{
    QMessage message;
    message.setType(QMessage::Sms);
    message.setTo(QMessageAddress(phoneNumberEdit->text(), QMessageAddress::Email));
    message.setBody(QString(smsMessageEdit->toPlainText()));
    
    if (!QString(phoneNumberEdit->text()).isEmpty())
    {
        if (!m_serviceaction.send(message))
        {
            QMessageBox msgBox;
            msgBox.setText(tr("SMS was successfully sent"));
            msgBox.exec();
        }
        phoneNumberEdit->clear();
        smsMessageEdit->clear();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Set Phone number!"));
        msgBox.exec();
    }
}


void MessagingEx::on_sendMmsButton_clicked()
{
    QMessage message;
    message.setType(QMessage::Mms);
    
    message.setTo(QMessageAddress(mmsPhoneNumberEdit->text(), QMessageAddress::Email));
    message.setSubject(mmsSubjectEdit->text());
    message.setBody(QString(mmsBodyEdit->toPlainText()));
    message.appendAttachments(m_attachments);
    
    if (!QString(mmsPhoneNumberEdit->text()).isEmpty())
    {
        if (!m_serviceaction.send(message))
        {
            QMessageBox msgBox;
            msgBox.setText(tr("MMS was successfully sent"));
            msgBox.exec();
        }
        mmsPhoneNumberEdit->clear();
        mmsSubjectEdit->clear();
        mmsBodyEdit->clear();
        attachmentLabel->clear();
        m_attachments.clear();
        m_fileNames.clear();
        emailAttachmentLabel->clear();
        
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Set Phone number!"));
        msgBox.exec();
    }
}

void MessagingEx::messageReceived(const QMessageId& aId)
{
	//QMessage message = m_store->message(aId);
	m_serviceaction.show(aId);
}

void MessagingEx::messageRemoved(const QMessageId& aId)
{
	Q_UNUSED(aId);
	QMessageBox msgBox;
	msgBox.setText(tr("Message removed."));
	msgBox.exec();
}

void MessagingEx::messageUpdated(const QMessageId& aId)
{
	Q_UNUSED(aId);
}

void MessagingEx::queryMessages()
{	

}

void MessagingEx::accountSelected(int index)
{   
	Q_UNUSED(index); 
}

void MessagingEx::on_sendEmailButton_clicked()
{
	QMessage message;
	int index = accountComboBox->currentIndex();
	m_account = QMessageAccount(m_accountList[index]).id();
	QMessage::TypeFlags types = m_account.messageTypes();
	
	if (!emailAddressEdit->text().isEmpty()){
		message.setTo(QMessageAddress(emailAddressEdit->text(), QMessageAddress::Email));
	}
	
	message.setParentAccountId(m_account.id());
	message.setSubject(subjectEdit->text());
	message.setBody(QString(emailMessageEdit->toPlainText()));
	message.appendAttachments(m_attachments);
	
	if (!m_attachments.isEmpty() && types == QMessage::Sms){
		QMessageBox msgBox;
		msgBox.setText(tr("Cannot send attachments via Sms!"));
		msgBox.exec();
		m_attachments.clear();
		m_fileNames.clear();
		attachmentLabel->clear();
	} else if (QString(emailAddressEdit->text()).isEmpty()){	
    	QMessageBox msgBox;
		msgBox.setText(tr("Set address!"));
		msgBox.exec();
    } else {
		if (m_serviceaction.send(message)){
			emailAddressEdit->clear();
			subjectEdit->clear();
			emailMessageEdit->clear();  
			m_attachments.clear();
			m_fileNames.clear();
			emailAttachmentLabel->clear();
			attachmentLabel->clear();
		}
	}   
}

void MessagingEx::sortType()
{	
	QMessageFilter filter;
	int priorityindex = priorityComboBox->currentIndex();
	// 0 = No priority filtering 1 = High Priority, 2 = Normal Priority, 3 = Low Priority
	switch(priorityindex){
		case 0:
			break;
		case 1:
			filter = QMessageFilter::byPriority(QMessage::HighPriority, QMessageDataComparator::Equal);
			break;
		case 2:
			filter = QMessageFilter::byPriority(QMessage::NormalPriority, QMessageDataComparator::Equal);
			break;
		case 3:
			filter = QMessageFilter::byPriority(QMessage::LowPriority, QMessageDataComparator::Equal);
			break;
		case 4:
			filter = QMessageFilter::byPriority(QMessage::HighPriority, QMessageDataComparator::NotEqual);
			break;
		case 5:
			filter = QMessageFilter::byPriority(QMessage::NormalPriority, QMessageDataComparator::NotEqual);
			break;
		case 6:
			filter = QMessageFilter::byPriority(QMessage::LowPriority, QMessageDataComparator::NotEqual);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = priorityAccountComboBox->itemData(priorityAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);
	
}

void MessagingEx::sortSender()
{
	QString sender = senderEdit->text();
	QMessageFilter filter;
	int index = sendercomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::bySender(sender, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::bySender(sender, QMessageDataComparator::NotEqual);
			break;
		case 2:
			filter = QMessageFilter::bySender(sender, QMessageDataComparator::Includes);
			break;
		case 3:
			filter = QMessageFilter::bySender(sender, QMessageDataComparator::Excludes);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = senderAccountComboBox->itemData(senderAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);
	
}

void MessagingEx::sortRecipient()
{
	QString recipient = recipientEdit->text();
	QMessageFilter filter;
	int index = recipientcomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::byRecipients(recipient, QMessageDataComparator::Includes);
			break;
		case 1:
			filter = QMessageFilter::byRecipients(recipient, QMessageDataComparator::Excludes);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = recipientAccountComboBox->itemData(recipientAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);

}

void MessagingEx::sortSubject()
{
	QString subject = subjectEdit_2->text();
	QMessageFilter filter;
	int index = subjectcomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::bySubject(subject, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::bySubject(subject, QMessageDataComparator::NotEqual);
			break;
		case 2:
			filter = QMessageFilter::bySubject(subject, QMessageDataComparator::Includes);
			break;
		case 3:
			filter = QMessageFilter::bySubject(subject, QMessageDataComparator::Excludes);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = subjectAccountComboBox->itemData(subjectAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);
}

void MessagingEx::sortTimestamp()
{
	QDateTime time = timestampdateTimeEdit->dateTime();
	QMessageFilter filter;
	int index = timestampcomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::byTimeStamp(time, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::byTimeStamp(time, QMessageDataComparator::NotEqual);
			break;
		case 2:
			filter = QMessageFilter::byTimeStamp(time, QMessageDataComparator::LessThan);
			break;
		case 3:
			filter = QMessageFilter::byTimeStamp(time, QMessageDataComparator::LessThanEqual);
			break;
		case 4:
			filter = QMessageFilter::byTimeStamp(time, QMessageDataComparator::GreaterThan);
			break;
		case 5:
			filter = QMessageFilter::byTimeStamp(time, QMessageDataComparator::GreaterThanEqual);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = timestampAccountComboBox->itemData(timestampAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);

}

void MessagingEx::sortReceptiontimestamp()
{
	QDateTime time = receptiondateTimeEdit->dateTime();
	QMessageFilter filter;
	int index = receptioncomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::byReceptionTimeStamp(time, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::byReceptionTimeStamp(time, QMessageDataComparator::NotEqual);
			break;
		case 2:
			filter = QMessageFilter::byReceptionTimeStamp(time, QMessageDataComparator::LessThan);
			break;
		case 3:
			filter = QMessageFilter::byReceptionTimeStamp(time, QMessageDataComparator::LessThanEqual);
			break;
		case 4:
			filter = QMessageFilter::byReceptionTimeStamp(time, QMessageDataComparator::GreaterThan);
			break;
		case 5:
			filter = QMessageFilter::byReceptionTimeStamp(time, QMessageDataComparator::GreaterThanEqual);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = timeAccountComboBox->itemData(timeAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);	
}

void MessagingEx::sortStatus()
{
	QMessageFilter filter;
	int index = statuscomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::NotEqual);
			break;
		case 2:
			filter = QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Includes);
			break;
		case 3:
			filter = QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Excludes);
			break;
		case 4:
			filter = QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Equal);
			break;
		case 5:
			filter = QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::NotEqual);
			break;
		case 6:
			filter = QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Includes);
			break;
		case 7:
			filter = QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Excludes);
			break;
		case 8:
			filter = QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::Equal);
			break;
		case 9:
			filter = QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::NotEqual);
			break;
		case 10:
			filter = QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::Includes);
			break;
		case 11:
			filter = QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::Excludes);
			break;
		case 12:
			filter = QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Equal);
			break;
		case 13:
			filter = QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::NotEqual);
			break;
		case 14:
			filter = QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Includes);
			break;
		case 15:
			filter = QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Excludes);
			break;
	}
	
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	QString accountId = statusAccountComboBox->itemData(statusAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	m_serviceaction.queryMessages(filter, ordering, 100, 0);

}

void MessagingEx::sortParentAccountId()
{
	int index = accountComboBox_2->currentIndex();
	folderComboBox->clear ();
	QMessageFolderFilter filter = QMessageFolderFilter::byParentAccountId(m_accountList[index]);// & QMessageFolderFilter::byDisplayName("Inbox");
	QMessageFolderIdList ids = m_store->queryFolders(filter);
	for (int i=0; i < ids.count(); i++) {
	    QMessageFolder folder;
	    folder = m_store->folder(ids[i]);
	    QString name = folder.displayName();
	    folderComboBox->addItem(name, folder.id().toString());    
	}
}

void MessagingEx::sortStandardFolder()
{
	QMessageFilter filter;
	int index = foldercomboBox->currentIndex();
	switch(index){
		case 0:
			filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::NotEqual);
			break;
		case 2:
			filter = QMessageFilter::byStandardFolder(QMessage::OutboxFolder, QMessageDataComparator::Equal);
			break;
		case 3:
			filter = QMessageFilter::byStandardFolder(QMessage::OutboxFolder, QMessageDataComparator::NotEqual);
			break;
		case 4:
			filter = QMessageFilter::byStandardFolder(QMessage::DraftsFolder, QMessageDataComparator::Equal);
			break;
		case 5:
			filter = QMessageFilter::byStandardFolder(QMessage::DraftsFolder, QMessageDataComparator::NotEqual);
			break;
		case 6:
			filter = QMessageFilter::byStandardFolder(QMessage::SentFolder, QMessageDataComparator::Equal);
			break;
		case 7:
			filter = QMessageFilter::byStandardFolder(QMessage::SentFolder, QMessageDataComparator::NotEqual);
			break;
		case 8:
			filter = QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::Equal);
			break;
		case 9:
			filter = QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::NotEqual);
			break;
	}
	QString accountId = folderAccountComboBox->itemData(folderAccountComboBox->currentIndex()).toString();
	filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	QMessageOrdering ordering;
	ordering.byTimeStamp(Qt::DescendingOrder);
	m_serviceaction.queryMessages(filter, ordering, 100, 0);
}

void MessagingEx::findMessages()
{
	QString accountId = accountComboBox_2->itemData(accountComboBox_2->currentIndex()).toString();
	int folderIndex = folderComboBox->currentIndex();
	int defaultIndex = defaultAccountBox->currentIndex();
	QMessageFilter filter;
	if (defaultIndex >= 0 && folderComboBox->count() == 0){
		switch(defaultIndex){
		case 0:
			filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::Equal);
			break;
		case 1:
			filter = QMessageFilter::byStandardFolder(QMessage::OutboxFolder, QMessageDataComparator::Equal);
			break;
		case 2:
			filter = QMessageFilter::byStandardFolder(QMessage::DraftsFolder, QMessageDataComparator::Equal);
			break;
		case 3:
			filter = QMessageFilter::byStandardFolder(QMessage::SentFolder, QMessageDataComparator::Equal);
			break;
		case 4:
			filter = QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::Equal);
			break;
		}
        filter &= QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	}
	else if (folderIndex >= 0) {
        QString folderId = folderComboBox->itemData(folderComboBox->currentIndex()).toString();
	    filter = QMessageFilter::byParentFolderId(QMessageFolderId(folderId), QMessageDataComparator::Equal);
	} else {
        filter = QMessageFilter::byParentAccountId(QMessageAccountId(accountId));
	}

	QMessageOrdering ordering = ordering.byTimeStamp(Qt::DescendingOrder);
	m_serviceaction.queryMessages(filter, ordering, 100, 0);
	
}

void MessagingEx::messagesFound(const QMessageIdList &ids)
{
	QString labelText;
	QStandardItemModel* standardModel = new QStandardItemModel(this);
	messageListView->setModel(standardModel);
	stackedWidget->setCurrentIndex(12);
    for (int i=0; i < ids.count(); i++) {
        QMessage message = m_store->message(ids[i]);
        QString from = message.from().recipient();
        QString subject = message.subject();
        if (subject.length() == 0) {
            subject = message.textContent();
        }
      //  QString body = QString(message.textContent().left(20));
       // labelText.append(from);
        QStandardItem* item1 = new QStandardItem(from);
        QStandardItem* item2 = new QStandardItem(subject);
        standardModel->appendRow(item1);
        standardModel->appendRow(item2);
        messageListView->setModel(standardModel);
    }
    
}


// End of file
