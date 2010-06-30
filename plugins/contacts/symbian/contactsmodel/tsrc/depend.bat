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

@echo off
REM lists all test code in \cntmodel\tsrc

set _depend=T_DBASE,T_ITEM,T_FIELD,T_MULTS,T_DBASE2,T_VIEW,T_VERS,T_TEMPL,T_EXPDEL,T_ERROR,t_ttvers,T_FERROR
if "%1"=="make" set _depend=CNTTUTIL,T_TIME,%_depend%
