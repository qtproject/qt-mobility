/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32test.h>
#include <cntitem.h>
#include <cntfldst.h>  //to enable casting from ccontactfieldstorage* to ccontacttextfield* 



struct SFlagsDict
	{
	TInt iFlag;
	const TText* iDescript;
	};

#define FF(x) {x, _S(#x)}
#define NbTestLast 0x00000000

	//Contact field flag struct
	enum // er5 hard coded enum most of which have been used but not documented ;o(
		{
		EHidden				=0x00000001,
		EReadOnly			=0x00000002,
		ESynchronize		=0x00000004,
		EDisabled			=0x00000008,
		/* 0x00000010,-> 0x00000080 are used for 
			the user defined attribs / categories like work /
			Personal / Other / None - used in er5 UI */
		EUserMask			=0x000000F0,
		ETemplateMask		=EUserMask|ESynchronize|EReadOnly|EHidden,
		EOverRidesLabel		=0x00000100,
		EUsesTemplateData	=0x00000200,
		EUserAddedField		=0x00000400,
		ETemplate			=0x00000800,
		/* 0x00001000,-> 0x0000F000 are used for 
			the storage type of the contact field */
		ELabelUnspecified	=0x40000000,
		EDeleted			=0x80000000
		};
	enum 
		// new extended enum to allow cntmodel to have
		// more attributes per field
		{
		EPrivate			=0x00000001,
		ESpeedDial			=0x00000002,
		EUserDefinedFilter	=0x00000004,
		EUserDefinedFilter1 =0x00000008,
		EUserDefinedFilter2	=0x00000010,
		EUserDefinedFilter3	=0x00000020,
		EUserDefinedFilter4	=0x00000040
		};


struct SDescArr 
	{
	operator const TText*() const
		{
		return iDesc;
		}

	const TText* iDesc;
	};



#define VM(x) case x: return _S(#x);

extern const TText* VCardMaping(const TFieldType& type)
	{
	switch (type.iUid)
		{
	//case KUidContactFieldAddressValue: return _S("KUidContactFieldAddressValue");
	VM(KUidContactFieldAddressValue			);                
	VM(KUidContactFieldPostOfficeValue		);            
	VM(KUidContactFieldExtendedAddressValue	);       
	VM(KUidContactFieldLocalityValue		);            
	VM(KUidContactFieldRegionValue			);            
	VM(KUidContactFieldPostCodeValue		);            
	VM(KUidContactFieldCountryValue			);                

	VM(KUidContactFieldCompanyNameValue );                
	VM(KUidContactFieldCompanyNamePronunciationValue );
	VM(KUidContactFieldPhoneNumberValue );                
	VM(KUidContactFieldGivenNameValue   );                
	VM(KUidContactFieldFamilyNameValue  );                
	VM(KUidContactFieldGivenNamePronunciationValue );   
	VM(KUidContactFieldFamilyNamePronunciationValue );    

	VM(KUidContactFieldAdditionalNameValue);              
	VM(KUidContactFieldSuffixNameValue  );                
	VM(KUidContactFieldPrefixNameValue  );                
	VM(KUidContactFieldHiddenValue      );                
	VM(KUidContactFieldEMailValue       );                
	VM(KUidContactFieldMsgValue         );                
	VM(KUidContactFieldSmsValue         );                
	VM(KUidContactFieldFaxValue         );                

	VM(KUidContactFieldDefinedTextValue );                
	VM(KUidContactFieldNoteValue		);            
	VM(KUidContactFieldBirthdayValue	);            
	VM(KUidContactFieldUrlValue			);                
	VM(KUidContactFieldStorageInlineValue);            
	VM(KUidContactFieldTemplateLabelValue);            
	VM(KUidContactFieldPictureValue		);                
	VM(KUidContactFieldRingToneValue	);            
	VM(KUidContactFieldDTMFValue		);            
	VM(KUidContactsVoiceDialFieldValue	);            
	VM(KUidContactFieldNoneValue		);			
	VM(KUidContactFieldJobTitleValue	);            
	VM(KUidContactFieldICCSlotValue		);		
	VM(KUidContactFieldICCPhonebookValue);	
	VM(KUidContactFieldICCGroupValue	);		
	VM(KUidContactFieldIMAddressValue	);	
	VM(KUidContactFieldSecondNameValue	);	
	VM(KUidContactFieldSIPIDValue		);	

	VM(KIntContactFieldVCardMapWORK );                      
	VM(KIntContactFieldVCardMapHOME );                      
	VM(KIntContactFieldVCardMapMSG  );                      
	VM(KIntContactFieldVCardMapVOICE);                      
	VM(KIntContactFieldVCardMapFAX  );                      
	VM(KIntContactFieldVCardMapPREF );                      
	VM(KIntContactFieldVCardMapCELL );                      
	VM(KIntContactFieldVCardMapPAGER);                      
	VM(KIntContactFieldVCardMapBBS  );                      
	VM(KIntContactFieldVCardMapMODEM);                      
	VM(KIntContactFieldVCardMapCAR  );                      
	VM(KIntContactFieldVCardMapISDN );                      
	VM(KIntContactFieldVCardMapVIDEO);                      
	VM(KIntContactFieldVCardMapDOM  );                      

	VM(KIntContactFieldVCardMapADR          );          
	VM(KIntContactFieldVCardMapPOSTOFFICE	);          
	VM(KIntContactFieldVCardMapEXTENDEDADR	);              
	VM(KIntContactFieldVCardMapLOCALITY		);              
	VM(KIntContactFieldVCardMapREGION		);          
	VM(KIntContactFieldVCardMapPOSTCODE		);               
	VM(KIntContactFieldVCardMapCOUNTRY		);           
	VM(KIntContactFieldVCardMapAGENT        );               
	VM(KIntContactFieldVCardMapBDAY         );               
	VM(KIntContactFieldVCardMapEMAILINTERNET);               
	VM(KIntContactFieldVCardMapGEO          );               
	VM(KIntContactFieldVCardMapLABEL        );               
	VM(KIntContactFieldVCardMapLOGO         );               
	VM(KIntContactFieldVCardMapMAILER       );               
	VM(KIntContactFieldVCardMapNOTE         );               
	VM(KIntContactFieldVCardMapORG          );               
	VM(KIntContactFieldVCardMapORGPronunciation );           
	VM(KIntContactFieldVCardMapPHOTO        );               
	VM(KIntContactFieldVCardMapROLE         );               
	VM(KIntContactFieldVCardMapSOUND        );               
	VM(KIntContactFieldVCardMapTEL          );               
	VM(KIntContactFieldVCardMapTELFAX       );               
	VM(KIntContactFieldVCardMapTITLE        );               
	VM(KIntContactFieldVCardMapURL          );               
	VM(KIntContactFieldVCardMapUnusedN      );               
	VM(KIntContactFieldVCardMapUnusedFN     );               
	VM(KIntContactFieldVCardMapNotRequired  );               
	VM(KIntContactFieldVCardMapUnknownXDash );               
	VM(KIntContactFieldVCardMapUnknown      );               
	VM(KIntContactFieldVCardMapUID          );               
	VM(KIntContactFieldVCardMapINTL         );               
	VM(KIntContactFieldVCardMapPOSTAL       );               
	VM(KIntContactFieldVCardMapPARCEL       );               
	VM(KIntContactFieldVCardMapGIF		    );              
	VM(KIntContactFieldVCardMapCGM          );               
	VM(KIntContactFieldVCardMapWMF          );               
	VM(KIntContactFieldVCardMapBMP          );               
	VM(KIntContactFieldVCardMapMET          );               
	VM(KIntContactFieldVCardMapPMB          );               
	VM(KIntContactFieldVCardMapDIB          );               
	VM(KIntContactFieldVCardMapPICT         );               
	VM(KIntContactFieldVCardMapTIFF         );               
	VM(KIntContactFieldVCardMapPDF          );               
	VM(KIntContactFieldVCardMapPS           );               
	VM(KIntContactFieldVCardMapJPEG         );               
	VM(KIntContactFieldVCardMapMPEG         );               
	VM(KIntContactFieldVCardMapMPEG2        );               
	VM(KIntContactFieldVCardMapAVI          );               
	VM(KIntContactFieldVCardMapQTIME        );               
	VM(KIntContactFieldVCardMapTZ			);           
	VM(KIntContactFieldVCardMapKEY			);           

	VM(KIntContactFieldVCardMapX509			);               
	VM(KIntContactFieldVCardMapPGP			);           
	VM(KIntContactFieldVCardMapSMIME		);           
	VM(KIntContactFieldVCardMapWV			);		
	VM(KIntContactFieldVCardMapSECONDNAME	);	
	VM(KIntContactFieldVCardMapSIPID		);		
	VM(KIntContactFieldVCardMapPOC			);	
	VM(KIntContactFieldVCardMapSWIS			);	
	VM(KIntContactFieldVCardMapVOIP			);	
	default: return _S("Unknown");
		};
	};



class CMyField : public CBase
	{
public:
	CContentType *iContentType;
	TStorageType iStorageType;
	HBufC* iLabel;
	TInt iId;
	TUint32 iAttributes;
	TUint32 iExtendedAttributes;
	CContactFieldStorage* iStorage;
	TInt iTemplateFieldId;
	};


void PrintFieldsTableL(RFile& outFile, 	const CContactItemFieldSet& fldSet, TBuf8<255>& buf)
	{
	SDescArr sTStorageType[] = {
		_S("KStorageTypeText"),
		_S("KStorageTypeStore"),
		_S("KStorageTypeContactIt"),
		_S("KStorageTypeDateTime")
		};

	SFlagsDict sFieldFlag[]= {
		FF(EHidden),
		FF(EReadOnly),
		FF(ESynchronize),
		FF(EDisabled),
		FF(EUserMask),
		FF(EOverRidesLabel),
		FF(EUsesTemplateData),
		FF(EUserAddedField),
		FF(ETemplate),
		FF(ELabelUnspecified),
		FF(EDeleted),
		FF(NbTestLast)
		};

	outFile.Write(_L8("<html><body><table width=100% cellpadding=1 cellspacing=1 border=1>"));
	outFile.Write(_L8("<tr><td>Fld</td><td>Label</td><td>Field types:</td><td>Storg type</td>" \
		"<td>Field</td><td>Attr</td><td>Ext.Att</td><td>Templ</td></tr>\r\n"));

	const TInt numOfFields = fldSet.Count();
	for (TInt i=0; i < numOfFields; ++i)
		{
		const CContactItemField& field = fldSet[i];
		const CMyField& myField = reinterpret_cast<const CMyField&>(field);
		const CContentType& conType = *myField.iContentType;

		buf.Format(_L8("<tr><td>%d</td>"), myField.iId);
		outFile.Write(buf);

		outFile.Write(_L8("<td>"));
		if (myField.iLabel)
			{
			buf.Copy(*myField.iLabel);
			outFile.Write(buf);
			}
		outFile.Write(_L8("</td>\r\n"));

		//test.Printf(_L("Field types are:"));
		outFile.Write(_L8("<td>"));

		for(TInt j=0; j < conType.FieldTypeCount(); ++j)
			{
			const TFieldType& fldType = conType.FieldType(j);
			//test.Printf(_L(" %s"), VCardMaping(fldType));
			const TPtrC str = VCardMaping(fldType);
			buf.Copy(str);
			outFile.Write(buf);
			outFile.Write(_L8(",<br>"));
			}

		//Print Mapping of the field 
		outFile.Write(_L8("MP>: "));
		const TPtrC mapp = VCardMaping(conType.Mapping());
		buf.Copy(mapp);
		outFile.Write(buf);

		outFile.Write(_L8("</td>\r\n<td>"));

		//test.Printf(_L("Storg type: %s"), sTStorageType[myField.iStorageType]);
		const TPtrC str (sTStorageType[myField.iStorageType]);
		buf.Copy(str);
		outFile.Write(buf);

		outFile.Write(_L8("</td>\r\n<td>"));
		switch(myField.iStorageType)
			{
			case KStorageTypeText:  
				{
				const TPtrC& textFld = static_cast<CContactTextField*>(field.Storage())->Text();
				//test.Printf(_L("Text	  : %S"),  textFld);
				buf.Copy(textFld);
				outFile.Write(buf);
				}
			break;
			default: 
			break;
			}

		outFile.Write(_L8("</td>\r\n"));

		//test.Printf(_L("Attributes: 0x%08x"), myField.iAttributes);
		buf.Format(_L8("<td>0x%08x"), myField.iAttributes);
		outFile.Write(buf);

		for (TInt i = 0;; ++i)
			{
			const SFlagsDict& aFlag = sFieldFlag[i];

			if (aFlag.iFlag & myField.iAttributes)
				{
				TPtrC str(aFlag.iDescript);
				buf.Copy(str);

				outFile.Write(_L8(",<br>"));
				outFile.Write(buf);
				}


			if (aFlag.iFlag == NbTestLast)
				{
				break;
				}
			}

		outFile.Write(_L8("</td>\r\n"));

		//test.Printf(_L("Ext Attrib: 0x%08x"), myField.iExtendedAttributes);
		buf.Format(_L8("<td>0x%08x</td>\r\n"), myField.iExtendedAttributes);
		outFile.Write(buf);

		//test.Printf(_L("Temp fldId: 0x%08x"), myField.iTemplateFieldId);
		buf.Format(_L8("<td>%d</td>\r\n"), myField.iTemplateFieldId);
		outFile.Write(buf);

		
		outFile.Write(_L8("</tr>\r\n\r\n"));
		}

	outFile.Write(_L8("</table></body></html>\r\n"));
	}



void PrintIdArrayL(RFile& outFile, 	const CContactIdArray* aArray, const TDesC8& aLabel, TBuf8<255>& buf)
	{
	outFile.Write(_L8("<tr><td>"));
	outFile.Write(aLabel);
	outFile.Write(_L8("</td><td>"));
	if (aArray) //If array exists
		{
		for (TInt i = 0; i < aArray->Count(); ++i)
			{
			const TContactItemId id = (*aArray)[i];
			if (i)
				{
				outFile.Write(_L8("<br/>"));
				}
			
			buf.Format(_L8("%d (%x)"), id,  id);
			outFile.Write(buf);
			}
		}
	else
		{
		outFile.Write(_L8("Not Initialised"));
		}
	outFile.Write(_L8("</td></tr>\r\n"));
	}


class CMyContact : public CBase
	{
public:
	CContactItemFieldSet* iFieldSet;
	TUint32 iAttributes;
	TContactItemId iId;
	TContactItemId iTemplateRefId;
	TTime iLastModified;
	TTime iCreationDate;
	TUint32 iAccessCount;
	HBufC* iGuid;
	};

class CMyContactPlusGroup : public CMyContact
	{
public:
	CContactIdArray* iGroups;
	};

class CMyContactGroup : public CMyContactPlusGroup
	{
public:
	CContactIdArray* iItems;
	};

EXPORT_C void PrintContactL(const TDesC& aFilename, const CContactItem& aContact)
	{
	TBuf8<255> buf;
	TBuf16<100> buf16;

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	fs.MkDirAll(aFilename);

	RFile outFile;
	User::LeaveIfError(outFile.Replace(fs, aFilename, EFileWrite | EFileStreamText));
	CleanupClosePushL(outFile);

	const CMyContact& myContact = reinterpret_cast<const CMyContact&>(aContact);

	buf.Format(_L8("<table><tr><td>Attributes:</td><td>0x%08x</td></tr>\r\n"), myContact.iAttributes);
	outFile.Write(buf);

	buf.Format(_L8("<tr><td>Contact ID:</td><td>%d (%x)</td></tr>\r\n"), myContact.iId,  myContact.iId);
	outFile.Write(buf);

	buf.Format(_L8("<tr><td>Template Ref ID:</td><td> %d (%x)</td></tr>\r\n"), myContact.iTemplateRefId,  myContact.iTemplateRefId);
	outFile.Write(buf);


/*	outFile.Write(_L8("<tr><td>Last Modified:</td><td>"));
	  myContact.iLastModified.FormatL(buf16,KDateTimeFormat);
	  buf.Copy(buf16);
	  outFile.Write(buf);
	outFile.Write(_L8("</td></tr>\r\n"));

	outFile.Write(_L8("<tr><td>Creation Date:</td><td>"));
	  myContact.iCreationDate.FormatL(buf16,KDateTimeFormat);
	  buf.Copy(buf16);
	  outFile.Write(buf);
	outFile.Write(_L8("</td></tr>\r\n"));*/

	buf.Format(_L8("<tr><td>Access count:</td><td> %d</td></tr>\r\n"), myContact.iTemplateRefId,  myContact.iAccessCount);
	outFile.Write(buf);


	if (aContact.Type() == KUidContactCard || 
		aContact.Type() == KUidContactGroup || 
		aContact.Type() == KUidContactOwnCard)
		{
		const CMyContactPlusGroup& myContactPlusGroup = reinterpret_cast<const CMyContactPlusGroup&>(aContact);
		PrintIdArrayL(outFile, myContactPlusGroup.iGroups, _L8("Belongs to"), buf);

		if (aContact.Type() == KUidContactGroup)
			{
			const CMyContactGroup& myContactGrp = reinterpret_cast<const CMyContactGroup&>(aContact);
			PrintIdArrayL(outFile, myContactGrp.iItems, _L8("Contains"), buf);
			}
		}

	outFile.Write(_L8("</table><br/>\r\n"));

	PrintFieldsTableL(outFile, aContact.CardFields(), buf);

	CleanupStack::PopAndDestroy(&outFile);
	CleanupStack::PopAndDestroy(&fs);
	}



