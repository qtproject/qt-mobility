/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import QtMobility.contacts 1.1
import "contents"

Rectangle {
    id: topItem
    width: 360
    height: 640
    x: 0
    y: 0

    Column {
        spacing:4

        Text { text:"Name(from property name):" + myContact.name.firstName + " " + myContact.name.lastName }
        Text { text:"Name(from detail type):" + myContact.detail(ContactDetail.Name).firstName + " " + myContact.name.lastName }
        Text { text:"Name(from detail name):" + myContact.detail("Name").firstName + " " + myContact.name.lastName }
        Text { text:"Address:" + myContact.address.street + " " + myContact.address.locality + " " + myContact.address.region + " " + myContact.address.postcode }
        Text { text:"Email:" + myContact.email.emailAddress }
        Text { text:"phone number[0]:" + myContact.phoneNumbers[0].number }
        Text { text:"phone number[1]:" + myContact.phoneNumbers[1].number }
        Text { text:"phone number[2]:" + myContact.phoneNumbers[2].number }
    }


    Contact {
        id: myContact
        type:"Contact"
            Name {
                firstName:"Charles"
                lastName:"Yin"
            }

            EmailAddress {
                emailAddress:"charles.yin@nokia.com"
            }

            address.street:"53 Brandl St"
            address.locality: "Eight Mile Plains"
            address.region: "QLD"
            address.postcode:"4111"
            address.country:"Australia"
            address.subTypes:["Office"]
            address.postOfficeBox:"1111"

            Nickname {
                nickname:"Charles"
            }

            PhoneNumber {
                number: "1111111111"
                subTypes:["Office", "Mobile"]
            }

            PhoneNumber {
                number: "2222222222"
                subTypes:["Home"]
            }

            PhoneNumber {
                number: "3333333333"
                subTypes:["Mobile"]
            }
    }
    DetailFilter {
       id:filter
       detail:ContactDetail.Name
       field:Name.FirstName
       value:"Charles"
       matchFlags:Filter.MatchExactly
    }
}

// ![0]
