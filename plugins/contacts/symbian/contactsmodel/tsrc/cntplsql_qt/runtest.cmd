/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

call qmake
call sbs reallyclean
call qmake
call sbs -c winscw_udeb
call copy \epoc32\release\winscw\udeb\ut_cntsqlsearch.exe \epoc32\release\winscw\udeb\z\sys\bin\test.exe 
call epoc
