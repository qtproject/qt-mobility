@echo off
rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem
@echo on

if "%1"=="" set _whatvar_=udeb
call ..\tsrc\depend run
for %%i in (%_depend%) do \epoc32\release\wins\%_whatvar_%\%%i.exe
set _depend=
set _whatvar_=
