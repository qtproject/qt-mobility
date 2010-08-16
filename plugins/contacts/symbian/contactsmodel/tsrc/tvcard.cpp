/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 

#if !defined(__T_VERS_H__)

    #include "t_vers.h"

#endif



void CVCardTest::GetSampleVersitL()

	{



	iParser=CParserVCard::NewL();

	iVersit.Zero();

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(_L8("BEGIN:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;HOME:+1-919-672-473"));

	iVersit.Append(KVersitTokenCRLF);

//QUOTED-P

	iVersit.Append(_L8("NOTE;ENCODING=QUOTED-PRINTABLE:Don’t remember to order Girl="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("  Scout cookies from Stacey today!")); 

    iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("A.TEL;HOME:+1-213-672-474"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("A.NOTE:This is my vacation home."));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("PHOTO:http://www.abc.com/dir_photos/my_photo.gif"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("SOUND;VALUE=CONTENT-ID:<jsmith.part3.960817T083000.xyzMail@host1.com"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("FN;CHARSET=ISO-8859-8;VALUE=URL;TYPE=GIF:Mr. John Q. Public, Esq."));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("N:Public;John;Quinlan;Mr.;Esq."));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("N:Veni, Vidi, Vici The Restaurant."));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("PHOTO;VALUE=URL:file:///jqpublic.gif"));

	iVersit.Append(KVersitTokenCRLF);

//BASE 64

	iVersit.Append(_L8("PHOTO;ENCODING=BASE64;TYPE=GIF:R0lGODdhfgA4AOYAAAAAAK+vr62trVIxa6WlpZ+fnzEpCEpzlAha/0Kc74+PjyGMSuecKRhrtX9/fzExORBSjCEYCGtra2NjYyF7nDGE50JrhAg51qWtOTl7vee1MWu150o5e3PO/3sxcwAx/4R7GBgQOcDAwFoAQt61hJyMGHuUSpRKIf8A/wAY54yMjHtz"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(KVersitTokenCRLF); //BASE 64 (needs double CRLF)

	iVersit.Append(_L8("BDAY:19950415"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("BDAY:1995-04-15"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("ADR;DOM;HOME:P.O. Box 101;Suite 101;123 Main Street;Any Town;CA;91921-1234;"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("ADR;DOM;WORK;HOME;POSTAL:P.O. Box 101;;;Any Town;CA;91921-1234;"));

	iVersit.Append(KVersitTokenCRLF);

//QUOTED-P

	iVersit.Append(_L8("LABEL;DOM;POSTAL;ENCODING=QUOTED-PRINTABLE:P. O. Box 456=0D=0A="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("123 Main Street=0D=0A="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("Any Town, CA 91921-1234"));

	iVersit.Append(KVersitTokenCRLF);

	/*iVersit.Append(_L8("LABEL;INTL;PARCEL,ENCODING=QUOTED-PRINTABLE:Suite 101=0D=0A="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("123 Main Street=0D=0A="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("Any Town, CA 91921-1234=0D=0A="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("U.S.A.")); 

	iVersit.Append(KVersitTokenCRLF);*/

	iVersit.Append(_L8("TEL;PREF;WORK;MSG;FAX:+1-800-208-1866"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;WORK;HOME;VOICE;FAX:+1-800-672-473"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("EMAIL;INTERNET:john.public@abc.com"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("MAILER:ccMail 2.2"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TZ:-0500"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TZ:-08:00"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("GEO:37.24,-17.87"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TITLE:V.P., Research and Development"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("ROLE:Executive"));

	iVersit.Append(KVersitTokenCRLF);

//BASE 64

	/*iVersit.Append(_L8("LOGO;ENCODING=BASE64;TYPE=GIF:R0lGODdhfgA4AOYAAAAAAK+vr62trVIxa6WlpZ+fnzEpCEpzlAha/0Kc74+PjyGMSuecKRhrtX9/fzExORBSjCEYCGtra2NjYyF7nDGE50JrhAg51qWtOTl7vee1MWu150o5e3PO/3sxcwAx/4R7GBgQOcDAwFoAQt61hJyMGHuUSpRKIf8A/wAY54yMjHtz..."));

	iVersit.Append(KVersitTokenCRLF); */

	

//AGENT 

	iVersit.Append(_L8("AGENT:"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("BEGIN:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("N:Friday;Fred"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;WORK;VOICE:+1-213-555-1234"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;WORK;FAX:+1-213-555-5678"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("UID:0-555-1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("END:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	

	iVersit.Append(_L8("ORG:ABC, Inc.;North American Division;Marketing"));

	iVersit.Append(KVersitTokenCRLF);

	 

//QUOTED-P

	iVersit.Append(_L8("NOTE;ENCODING=QUOTED-PRINTABLE:This facsimile machine if operational="));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("0830 to 1715 hours"));

	iVersit.Append(KVersitTokenCRLF);



	iVersit.Append(_L8("REV:19951031T222710"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("REV:1995-10-31T22:27:10Z"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("SOUND:JON Q PUBLIK"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("SOUND;VALUE=URL:file///multimed/audio/jqpublic.wav"));

	iVersit.Append(KVersitTokenCRLF);

// BASE 64

	/*iVersit.Append(_L8("SOUND;WAVE;BASE64:UklGRhAsAABXQVZFZm10IBAAAAABAAEAESsAABErAAABAAgAZGF0YesrAACAg4eCeXR4e3uAhoiIiYmKjIiDfnx5eX6CgoKEhYWDenV5fH6BhISGiIiDfHZ2eXt/hIiKjY2IhH12d3Vyc3uDiIiFf3l7fn18eXl+houFf319fnyAgHl5eoCIiISChIeAfnt2..."));*/

	iVersit.Append(_L8("URL:http://abc.com/pub/directory/northam/jpublic.ecd"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("X-ABC-VIDEO;MPEG2:http://lonestar.bubbas.org/billibob.mpg"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("X-DL;X-DesignWorkGroup:List Item 1;List Item 2;List Item 3"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("END:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("BEGIN:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("N:A;B;C;;."));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL:1111"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("END:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	}



void CVCardTest::GetSampleVersit2L(TInt aContactId)

	{

	// this will delete the agent

	iVersit.Zero();

	iParser=CParserVCard::NewL();

	iVersit.Append(_L8("BEGIN:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	TBuf<32> buf;

	buf.Format(_L("UID:0-555-%d"),aContactId);

	iVersit.Append(buf);

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("END:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	}



void CVCardTest::GetSampleVersit3L()

	{

	iParser=CParserVCard::NewL();

	iVersit.Zero();

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(_L8("BEGIN:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;HOME:+1-919-672-473"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("A.NOTE:This is my vacation home."));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("PHOTO:http://www.abc.com/dir_photos/my_photo.gif"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("END:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

//

	iVersit.Append(_L8(" "));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("BEGIN:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("VERSION:2.1"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("     "));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;HOME:+1-123-456"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("TEL;WORK;VOICE:+1-213-555-1234"));

	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("END:VCARD"));

	iVersit.Append(KVersitTokenCRLF);

	}



