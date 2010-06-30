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

		
#ifndef __T_TESTDATA_H__
#define __T_TESTDATA_H__


//Export of CLASS,NICKNAME,CATEGORIES,PRODID,SORT-STRING is not supported in vCard2.1.
//Export of AGENT is not supported even for vCard2.1
_LIT8(KAllSupportedFields,"BEGIN:VCARD\r\n"
						  "VERSION:2.1\r\n"
						  "REV:20061005T200000\r\n"
						  "UID:d4a5b62eb04ab57b-00e0fcd667a3bab2-1\r\n"
						  "N:Anderson;Thomas\r\n"
						  "FN:Neo\r\n"
						  "TITLE:Software Engineer\r\n"
						  "TZ:+00\r\n"
						  "BDAY:20060928\r\n"
						  "TEL;HOME;VOICE:087715826\r\n"
						  "ADR;DOM;HOME:P.O. Box 101;Suite 101;Any Town;CA;91921-1234;\r\n"
						  "LABEL;WORK;ENCODING=QUOTED-PRINTABLE:DP/ECS=0D=0ABuilding 3, Daten Park\r\n"
						  "EMAIL;PREF;INTERNET:neo@matrixworld.com\r\n"
						  "ORG:Psion;Last Human Establishment\r\n"
						  "NOTE;X-EPOCCNTMODELFIELDLABEL=Memo:System Test Engineer\r\n"
						  "ROLE:The One\r\n"
						  "GEO:96.96963,32.86593\r\n"
						  "URL:http://morpheous.psion.com/trinity\r\n"
						  "KEY:12345\r\n"
						  "SOUND;X-IRMC-ORG;X-PALM-ORG:Eye w-o-r-k h-e-r-e\r\n"
						  "MAILER:MS-Outlook\r\n"
						  "PHOTO:TextPHOTO\r\n"
						  "LOGO:TextLogo\r\n"
						  "END:VCARD\r\n");

_LIT8(KAllExpectedExport30,"BEGIN:VCARD\r\n"
							"VERSION:3.0\r\n"
							"REV:20061009T085023Z\r\n"
							"UID:d4a5b62eb04ab57b-00e0fcd667a3bab2-1\r\n"
							"N:Anderson;Thomas;;;\r\n"
							"FN:Neo\r\n"
							"ADR;TYPE=HOME:P.O. Box 101;Suite 101;Any Town;CA;91921-1234;;\r\n"
							"ORG:Psion;Last Human Establishment\r\n"
							"TEL;TYPE=HOME;TYPE=VOICE:087715826\r\n"
							"TITLE:Software Engineer\r\n"
							"EMAIL;TYPE=PREF;TYPE=INTERNET:neo@matrixworld.com\r\n"
							"URL:http://morpheous.psion.com/trinity\r\n"
							"LOGO;ENCODING=b:VGV4dExvZ28=\r\n"
							"BDAY:20060928\r\n"
							"NOTE:System Test Engineer\r\n"
							"PHOTO;ENCODING=b:VGV4dFBIT1RP\r\n"
							"LABEL;TYPE=WORK:DP/ECS\\nBuilding 3\\, Daten Park\r\n"
							"ROLE:The One\r\n"
							"KEY:12345\r\n"
							"MAILER:MS-Outlook\r\n"
							"END:VCARD\r\n");

_LIT8(KAllExpectedExport21,"BEGIN:VCARD\r\n"
							"VERSION:2.1\r\n"
							"REV:20061009T095320Z\r\n"
							"UID:d4a5b62eb04ab57b-00e0fcd667a3bab2-1\r\n"
							"N:Anderson;Thomas;;;\r\n"
							"FN:Neo\r\n"
							"ADR;HOME:P.O. Box 101;Suite 101;Any Town;CA;91921-1234;;\r\n"
							"ORG:Psion;Last Human Establishment\r\n"
							"TEL;HOME;VOICE:087715826\r\n"
							"TITLE:Software Engineer\r\n"
							"EMAIL;PREF;INTERNET;ENCODING=QUOTED-PRINTABLE:neo=40matrixworld.com\r\n"
							"URL:http://morpheous.psion.com/trinity\r\n"
							"LOGO;ENCODING=BASE64:\r\n"
							"    VGV4dExvZ28=\r\n"
							"\r\n"
							"BDAY:20060928\r\n"
							"NOTE:System Test Engineer\r\n"
							"PHOTO;ENCODING=BASE64:\r\n"
							"    VGV4dFBIT1RP\r\n"
							"\r\n"
							"LABEL;WORK;ENCODING=QUOTED-PRINTABLE:DP/ECS=0D=0ABuilding=203,=20Daten=20Park\r\n"
							"SOUND;X-IRMC-ORG:Eye w-o-r-k h-e-r-e\r\n"
							"ROLE:The One\r\n"
							"GEO:96.96963,32.86593\r\n"
							"KEY:12345\r\n"
							"MAILER:MS-Outlook\r\n"
							"END:VCARD\r\n");
							
_LIT8(KMandatoryPopulatedProperties21,"BEGIN:VCARD\r\n"
										"VERSION:2.1\r\n"
										"N:Anderson;Thomas;;;\r\n"
										"TEL;HOME;VOICE:087715826\r\n"
										"END:VCARD\r\n");

_LIT8(KMandatoryPopulatedProperties30,"BEGIN:VCARD\r\n"
										"VERSION:3.0\r\n"
										"N:Anderson;Thomas;;;\r\n"
										"FN:Neo\r\n"
										"TEL;TYPE=HOME;TYPE=VOICE:087715826\r\n"
										"END:VCARD\r\n");
										
_LIT8(KvCardWithoutMandatoryProperties,"BEGIN:VCARD\r\n"
										"VERSION:2.1\r\n"
										"BDAY:20060928\r\n"
										"END:VCARD\r\n");
										
_LIT8(KEmptyMandatoryProperties30,"BEGIN:VCARD\r\n"
									"VERSION:3.0\r\n"
									"N:\r\n"
									"FN:\r\n"
									"TEL:\r\n"
									"END:VCARD\r\n");
									
									
_LIT8(KEmptyMandatoryProperties21,"BEGIN:VCARD\r\n"
									"VERSION:2.1\r\n"
									"N:\r\n"
									"TEL:\r\n"
									"END:VCARD\r\n");
									
_LIT8(KTELPropertyDisabled30,"BEGIN:VCARD\r\n"
									"VERSION:3.0\r\n"
									"N:\r\n"
									"FN:\r\n"
									"END:VCARD\r\n");
																			
_LIT8(KIntraContactExport,"BEGIN:VCARD\r\n"
							"VERSION:3.0\r\n"
							"N:Anderson;Thomas;;;\r\n"
							"FN:Neo\r\n"
							"TEL;TYPE=HOME;TYPE=VOICE:087715826\r\n"
							"TITLE:Software Engineer\r\n"
							"NOTE:System Test Engineer\r\n"
							"LABEL;TYPE=WORK:DP/ECS\\nBuilding 3\\, Daten Park\r\n"
							"X-IRMC-CALL-DATETIME;TYPE=MISSED:20061011T103000Z\r\n"
							"END:VCARD\r\n");	
							
_LIT8(KTestTypePrefix,"BEGIN:VCARD\r\n"
						"VERSION:2.1\r\n"
						"N:Thomas Anderson\r\n"
						"FN:Neo\r\n"
						"TITLE:Software Engineer\r\n"
						"TEL;HOME;VOICE:087715826\r\n"
						"ADR;DOM;HOME:P.O. Box 101;Suite 101;123;Any Town;CA;91921-1234;\r\n"
						"LABEL;WORK;ENCODING=QUOTED-PRINTABLE:DP/ECS=0D=0ABuilding 3, Daten Park\r\n"
						"EMAIL;PREF;INTERNET:neo@matrixworld.com\r\n"
						"ORG:Psion;Last Human Establishment\r\n"
						"NOTE;X-EPOCCNTMODELFIELDLABEL=Memo:System Test Engineer\r\n"
						"TEL;WORK:456745766\r\n"
						"TEL;HOME:5678957656\r\n"
						"TEL;CELL:56765767\r\n"
						"TEL;FAX;WORK:23453245\r\n"
						"SOUND;X-IRMC-ORG;X-PALM-ORG:Eye w-o-r-k h-e-r-e\r\n"
						"MAILER:MS-Outlook\r\n"
						"END:VCARD\r\n");
						
_LIT8(KExpectedTypePrefix30,"BEGIN:VCARD\r\n"
							"VERSION:3.0\r\n"
							"REV:20061009T140617Z\r\n"
							"UID:d4a5b62eb04ab57b-00e0fcd667a3bab2-1\r\n"
							"N:Thomas Anderson;;;;\r\n"
							"FN:Neo\r\n"
							"ADR;TYPE=HOME:P.O. Box 101;Suite 101;123;Any Town;CA;91921-1234;\r\n"
							"ORG:Psion;Last Human Establishment\r\n"
							"TEL;TYPE=HOME;TYPE=VOICE:087715826\r\n"
							"TEL;TYPE=HOME:5678957656\r\n"
							"TITLE:Software Engineer\r\n"
							"TEL;TYPE=CELL:56765767\r\n"
							"TEL;TYPE=WORK:456745766\r\n"
							"TEL;TYPE=WORK;TYPE=FAX:23453245\r\n"
							"EMAIL;TYPE=PREF;TYPE=INTERNET:neo@matrixworld.com\r\n"
							"NOTE:System Test Engineer\r\n"
							"LABEL;TYPE=WORK:DP/ECS\\nBuilding 3\\, Daten Park\r\n"
							"MAILER:MS-Outlook\r\n"
							"END:VCARD\r\n");

_LIT8(KExpected1FilteredProp30,"BEGIN:VCARD\r\n"
								"VERSION:3.0\r\n"
								"N:Anderson;Thomas;;;\r\n"
								"FN:Neo\r\n"
								"ADR;TYPE=HOME:P.O. Box 101;Suite 101;Any Town;CA;91921-1234;;\r\n"
								"TEL;TYPE=HOME;TYPE=VOICE:087715826\r\n"
								"TITLE:Software Engineer\r\n"
								"NOTE:System Test Engineer\r\n"
								"LABEL;TYPE=WORK:DP/ECS\\nBuilding 3\\, Daten Park\r\n"
								"END:VCARD\r\n");	
								
_LIT8(KExpected2FilteredProp30,"BEGIN:VCARD\r\n"
								"VERSION:3.0\r\n"
								"N:Anderson;Thomas;;;\r\n"
								"FN:Neo\r\n"
								"ORG:Psion;Last Human Establishment\r\n"
								"EMAIL;TYPE=PREF;TYPE=INTERNET:neo@matrixworld.com\r\n"
								"URL:http://morpheous.psion.com/trinity\r\n"
								"ROLE:The One\r\n"
								"END:VCARD\r\n");	
								
_LIT8(KExpected3FilteredProp21,"BEGIN:VCARD\r\n"
								"VERSION:2.1\r\n"
								"N:Anderson;Thomas;;;\r\n"
								"LOGO;ENCODING=BASE64:\r\n"
								"    VGV4dExvZ28=\r\n"
								"\r\n"
								"BDAY:20060928\r\n"
								"PHOTO;ENCODING=BASE64:\r\n"
								"    VGV4dFBIT1RP\r\n"
								"\r\n"
								"SOUND;X-IRMC-ORG:Eye w-o-r-k h-e-r-e\r\n"
								"KEY:12345\r\n"
								"MAILER:MS-Outlook\r\n"
								"END:VCARD\r\n");																																																											
								
_LIT8(KExpectedStandard21vCard,"BEGIN:VCARD\r\n"
								"VERSION:2.1\r\n"
								"REV:20061108T120234Z\r\n"
								"UID:d4a5b62eb04ab57b-00e0fcd667a3bab2-1\r\n"
								"N:Anderson;Thomas;;;\r\n"
								"FN:Neo\r\n"
								"ADR;HOME:P.O. Box 101;Suite 101;Any Town;CA;91921-1234;;\r\n"
								"ORG:Psion;Last Human Establishment\r\n"
								"TEL;HOME;VOICE:087715826\r\n"
								"TITLE:Software Engineer\r\n"
								"EMAIL;PREF;INTERNET;ENCODING=QUOTED-PRINTABLE:neo=40matrixworld.com\r\n"
								"URL:http://morpheous.psion.com/trinity\r\n"
								"LOGO;ENCODING=BASE64:\r\n"
								"    VGV4dExvZ28=\r\n"
								"\r\n"
								"BDAY:20060928\r\n"
								"NOTE:System Test Engineer\r\n"
								"PHOTO;ENCODING=BASE64:\r\n"
								"    VGV4dFBIT1RP\r\n"
								"\r\n"
								"LABEL;WORK;ENCODING=QUOTED-PRINTABLE:DP/ECS=0D=0ABuilding=203,=20Daten=20Park\r\n"
								"SOUND;X-IRMC-ORG:Eye w-o-r-k h-e-r-e\r\n"
								"ROLE:The One\r\n"
								"GEO:96.96963,32.86593\r\n"
								"KEY:12345\r\n"
								"MAILER:MS-Outlook\r\n"
								"END:VCARD\r\n");																																																									
#endif
