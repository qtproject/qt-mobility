/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of QtUiTest.
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

//TESTED_COMPONENT=src/bearer

testcase = {

    initTestCase: function()
    {
        testcase.qtuitest_platform = getLocalEnv("TESTPLATFORM");
        if (!checkPlatform("symbian") &&
            !checkPlatform("windows") &&
            !checkPlatform("linux") &&
            !checkPlatform("maemo") &&
            !checkPlatform("mac") &&
            !checkPlatform("wince")) {
            fail("TESTPLATFORM not valid");
        }
        if (testcase.platform == "linux" || testcase.platform == "maemo" || testcase.platform == "mac") {
            testcase.platform = "unix";
        }
    },

    enumeration: function()
    {
        prompt(twiki(readLocalFile(baseDataPath() + "testtext/" + testcase.platform + "/" + currentTestFunction())));
    },

    added_interfaces: function()
    {
        prompt(twiki(readLocalFile(baseDataPath() + "testtext/" + testcase.platform + "/" + currentTestFunction())));
    },

    removed_interfaces: function()
    {
        prompt(twiki(readLocalFile(baseDataPath() + "testtext/" + testcase.platform + "/" + currentTestFunction())));
    },

    change_status: function()
    {
        prompt(twiki(readLocalFile(baseDataPath() + "testtext/" + testcase.platform + "/" + currentTestFunction())));
    },

    interface_priority: function()
    {
        if (testcase.platform != "symbian") {
            skip("This test only applies to Symbian platform");
        }
        prompt(twiki(readLocalFile(baseDataPath() + "testtext/" + testcase.platform + "/" + currentTestFunction())));
    }

}

function checkPlatform(platform)
{
    if (testcase.qtuitest_platform != undefined &&
        testcase.qtuitest_platform.indexOf(platform) != -1) {
        testcase.platform = platform;
        return true;
    }
    return false;
}

function getLocalEnv(name)
{
    var env = QProcess.systemEnvironment();
    var re = new RegExp("^" + name + "=(.*)$");
    for (var i=0; i<env.length; ++i) {
        var res = env[i].match(re);
        if (res != null) {
            return res[1];
        }
    }
}
