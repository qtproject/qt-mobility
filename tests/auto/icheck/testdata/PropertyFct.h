/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "PropertyBase.h"

class IPropertyImpl : public IPropertyBaseImpl
{
    //assign enum to metadata
    Q_ENUMS(ConnectionState)
    Q_PROPERTY( ConnectionState state READ readState WRITE writeState NOTIFY notifyState RESET resetState);
public:
    enum ConnectionState {
        Disconnected = 0,
        Connecting,
        Connected,
        Engaged
    };
    ConnectionState readState() const
    {
        return Disconnected;
    }
    void writeState(ConnectionState state)
    {
    }
    void notifyState()
    {
    }
    void resetState()
    {
    }

    //different function names 
    Q_ENUMS(Property2Type)
    Q_PROPERTY( Property2Type property2 READ readProperty2wn WRITE writeProperty2wn NOTIFY notifyProperty2wn RESET resetProperty2wn);
    enum Property2Type{
        prop20 = 0,
        prop21,
        prop22
    };
    Property2Type readProperty2wn();
    void writeProperty2wn(Property2Type val);
    void notifyProperty2wn();
    void resetProperty2wn();

     //--- failing part
    //wrong enum values
    //different function names and missing functions
    Q_ENUMS(Property3Type)
    Q_PROPERTY( Property3Type property3 READ readProperty3 WRITE writeProperty3 NOTIFY notifyProperty3 RESET resetProperty3);
    enum Property3Type{
        prop30 = 0,
        prop31,
        prop32
    };
    Property3Type readProperty3();
    void writeProperty3(Property3Type val);
    void notifyProperty3();
    void resetProperty3();

    //functions missing
    Q_ENUMS(Property4Type)
    Q_PROPERTY( Property4Type property4 READ readProperty4 WRITE writeProperty4 NOTIFY notifyProperty4 RESET resetProperty4);
    enum Property4Type{
        prop40 = 0,
        prop41,
        prop42
    };
    /*
    Property4Type readProperty4();
    void writeProperty4(Property4Type val);
    void notifyProperty4();
    void resetProperty4();
    */

    //wrong enum name
    Q_ENUMS(Property5Typewn)
    Q_PROPERTY( Property5Typewn property5 READ readProperty5 WRITE writeProperty5 NOTIFY notifyProperty5 RESET resetProperty5);
    enum Property5Typewn{
        prop50 = 0,
        prop51,
        prop52
    };
    Property5Typewn readProperty5();
    void writeProperty5(Property5Typewn val);
    void notifyProperty5();
    void resetProperty5();

    //using property type without Q_ENUMS
    //Q_ENUMS(Property6Type)
    Q_PROPERTY( Property6Type property6 READ readProperty6 WRITE writeProperty6 NOTIFY notifyProperty6 RESET resetProperty6);
    enum Property6Type{
        prop60 = 0,
        prop61,
        prop62
    };
    Property6Type readProperty6();
    void writeProperty6(Property6Type val);
    void notifyProperty6();
    void resetProperty6();

    //using wrong flag name
    Q_FLAGS(MyFlags1wn)
    enum enumFlag1 {
        enumFlag10 = 0,
        enumFlag11,
        enumFlag12,
        enumFlag13
    };
    //define a enum as a flag
    Q_DECLARE_FLAGS(MyFlags1wn, enumFlag1)
    Q_PROPERTY( MyFlags1wn Flag1 READ readFlag1 NOTIFY notifyFlag1);
    MyFlags1wn readFlag1() const;
    void notifyFlag1();

        //using wrong flag value
    Q_FLAGS(MyFlags2)
    enum enumFlag2 {
        enumFlag20 = 0,
        enumFlag21,
        enumFlag22,
        enumFlag23
    };
    //define a enum as a flag
    Q_DECLARE_FLAGS(MyFlags2, enumFlag2)
    Q_PROPERTY( MyFlags2 Flag2 READ readFlag2 NOTIFY notifyFlag2);
    MyFlags2 readFlag2() const;
    void notifyFlag2();

};
