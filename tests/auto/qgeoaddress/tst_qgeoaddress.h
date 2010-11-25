/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TST_QGEOADDRESS_H
#define TST_QGEOADDRESS_H


#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include "qgeoaddress.h"

QTM_USE_NAMESPACE

class tst_QGeoAddress : public QObject
{
    Q_OBJECT

public:
    tst_QGeoAddress();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for qgeoaddress.h
    void t_qga_constructor();
    void t_qga_constructorCopy();
    void t_qga_city();
    void t_qga_country();
    void t_qga_countryCode();
    void t_qga_county();
    void t_qga_district();
    void t_qga_postcode();
    void t_qga_state();
    void t_qga_street();
//    void t_qga_streetNumber();
    void t_qga_emptyClear();
    void t_qga_emptyClear_data();
    void t_qga_operators();
    void t_qga_operators_data();
    //End Unit Tests for qgeoaddress.h

private:
    QGeoAddress *qgeoaddress;
};

#endif // TST_QGEOADDRESS_H

