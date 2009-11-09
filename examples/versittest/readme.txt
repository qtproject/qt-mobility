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

Following configurations should be made to run the testdriver application.

Test folder 					: <home>\testvcards
Test files 						: Should be located under testfolder and should have extension .vcf     
Note 								  : <home> : Windows : Check for HOME environmental variable
                                 Symbian : c:\data       
                     
Excluding some fields : File excludefields.txt should be located under testfolder
                        If you want to exclude vCard field "X-MYFIELD" from the comparsion, add line X-MYFIELD to the excludefields.txt.
                        Each field name should be located in a separate line.
                        
Configuration file    : versittestconfig.xml  ( Detailed explanation can be found as description element inside the file )
Note                  : Configuration file settings can be overridden by commandline arguments temporarily
                        CommandLine Arguments :
                        sc -> save to contact
                        nsc -> no save to contact 
                        p -> performance benchmarking
                        np -> no performance benchmarking , so normal test.
                        xml -> output to QVersitTestResults.xml in xml format
                        v1 -> verbose output level 1
                        v2 -> verbose output level 2
