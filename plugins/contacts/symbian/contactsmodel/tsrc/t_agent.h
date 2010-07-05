/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef __T_AGENT_H__
#define __T_AGENT_H__

class CAgentTest : public CBase
	{
public:
	static CAgentTest* NewL(CCntTest& aTestLibrary);
	~CAgentTest();
	void RunTestsL();
private:
	CAgentTest(CCntTest& aTestLibrary);
	void ConstructL();
	void AgentExportL();
	void CompareFilesL(const TDesC& aOutputFile,const TDesC& aCompareFile);
	TBool CompareLines(const TDesC8& aLine1, const TDesC8& aLine2);
	void ImportContactsL(const TDesC& aFileName);
	void ExportContactsL(const TDesC& aFileName);
	void ImportContactsFromDescriptorL(const TDesC8& aVCard);
	void ImportAgentToEmptyDatabaseL();
	void ImportAgentForMergeL();
	void ImportAgentMiscL();
	void ExportWithDeletedAgentL();
	void MultipleAgentExportL();
	void AddContactItemsL(TInt aCount);
	void MakeAgentL(TContactItemId aMainContactId, TContactItemId aAgentId);
	TInt GetAgentCountL(TContactItemId aContactId);
	TBool RemoveFieldFromContactL(TContactItemId aContactId, TUid aContactField);
private:
	CCntTest& iTest;
	CContactDatabase* iDb;
	CContactIdArray* iIdArray;
	};

_LIT8(KAgentvCard, "BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b20762-1\r\n"
					"N:Morpheous;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755501\r\n"
					"END:VCARD\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b9494b-2\r\n"
					"N:Neo;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755502\r\n"
					"AGENT:\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b20762-1\r\n"
					"N:AgentSmith;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755501\r\n"
					"END:VCARD\r\n"
					"\r\n"
					"END:VCARD\r\n");

_LIT8(KMultipleAgentvCard, "BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b20762-1\r\n"
					"N:Morpheous;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755501\r\n"
					"AGENT:\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b20762-5\r\n"
					"N:AgentSmith;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755501\r\n"
					"END:VCARD\r\n"
					"\r\n"					
					"END:VCARD\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b9494b-2\r\n"
					"N:Neo;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755502\r\n"
					"AGENT:\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b20762-3\r\n"
					"N:Oracle;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755501\r\n"
					"END:VCARD\r\n"
					"\r\n"
					"AGENT:\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20061211T050714Z\r\n"
					"UID:86454f285939ae79-00e1020032b20762-4\r\n"
					"N:Architect;;;;\r\n"
					"TEL;HOME;VOICE;CELL:755501\r\n"
					"END:VCARD\r\n"
					"\r\n"				
					"END:VCARD\r\n");

#endif
