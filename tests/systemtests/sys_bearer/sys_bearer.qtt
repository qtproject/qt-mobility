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
        if (!checkPlatform(testcase.qtuitest_platform)){
            fail("TESTPLATFORM "+ testcase.qtuitest_platform +" not valid");
        }
        if (testcase.platform == "linux" || testcase.platform == "mac") {
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
        if (testcase.platform != "symbian" && testcase.platform != "maemo") {
            skip("This test only applies to Symbian platform");
        }
        prompt(twiki('---++++ Interfaces are used in order of priority

    1. Pre: GPRS is available to the device
    1. Pre: WLAN is available to the device

    1. Press the Menu key
    1. Select Settings->Connectivity->Destinations
    1. Select Option->New Destination, enter a name
    1. Select a picture
    1. Select the destination
    1. Select Option->New access point
    1. Select Yes
    1. Verify that the WLAN "x networks avail." is greater than 0
    1. Select Wireless LAN
    1. Select the test WLAN (and enter a key if necessary)
    1. Select Option->New access point
    1. Select Yes
    1. Select Packet data
    1. Enter a name and commit
    1. Press Back
    1. Select Options->Default connection
    1. Select the new Destination
    1. Verify the Destination has a check mark
    1. Start the bearermonitor example application
    1. In the list, select the new Destination
    1. Verify "Roaming" is shown as "Available"
    1. Move the device into an area not covered by the WLAN or simulate an uncovered environment
    1. Verify the application indicates loss of WLAN network (may take 1 - 2 minutes)
    1. Verify the application indicates a change of bearer to the GPRS network - may take 1 - 2 minutes
    1. Move the device into or simulate the area covered by both networks
    1. Verify the device connects to the network using the WLAN bearer
    '));
    }

}

function checkPlatform(platform)
{
    if (platform == undefined){
        return false;
    }
    if (platform.contains("S60") || platform.contains("Symbian")) {
        testcase.platform = "symbian";
        return true;
    } else if (platform.contains("maemo")) {
        testcase.platform = "maemo";
    } else if (platform.contains("linux") || platform.contains("mac")) {
        testcase.platform = "unix";
        return true;
    } else if (platform.contains("windows")) {
        testcase.platform = "windows";
        return true;
    } else if (platform.contains("wince")) {
        testcase.platform = "wince";
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
