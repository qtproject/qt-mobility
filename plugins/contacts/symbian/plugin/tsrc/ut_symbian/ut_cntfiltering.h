/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QObject>
#include <QHash>
#include <qcontactfilter.h>
#include <cntdb.h>
#include "cntsymbianengine.h"


QTM_USE_NAMESPACE
QTM_BEGIN_NAMESPACE
typedef struct {
    QContactFilter filter;
    QString name;
    int result;
    int error;
} TFilter;
QTM_END_NAMESPACE

class TestFiltering : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
	void initTestCase();
	void cleanupTestCase();

private:
    void parseFilters();
    void addFilter(QVector<QString> param);
    void createContacts();
    QContactFilter::MatchFlags getMatchFlag(QString& inputflag);

    void createContact_1();
    void createContact_2();
    void createContact_3();
    void createContact_4();
    void createContact_5();
    void createContact_6();
    void createContact_7();
    void createContact_8();
    void createContact_9();
private slots:  // Test cases
    void testContactDetailFilter();
    void testRelationshipFilter();
    void testIntersectionFilter();
    void testUnionFilter();
    void testDefaultFilter();
   	void testDefaultFilterWithPredictiveSearch();
    void testDefaultFilterWithPredictiveSearch2();
    void testDefaultFilterWithPredictiveSearch3();
    void testDefaultFilterWithPredictiveSearch4();
    void testDefaultFilterWithPredictiveSearch5();
    void testDefaultFilterWithPredictiveSearch6();
    void testDefaultFilterWithPredictiveSearch7();
    void testDefaultFilterWithPredictiveSearch8();
    void testDefaultFilterWithPredictiveSearch9();
    void testFilterSupported();
    void testCreateSelectQuery();
    void testFavoriteDetailFilter();
    void testMatchFlags();
    void testContactTypeFilter();
private:
    void  testIntersectionFilter_1();
    void  testIntersectionFilter_2();
    void  testIntersectionFilter_3();
    void  testIntersectionFilter_4();
    void  testUnionFilter_1();
    void  testUnionFilter_2();
    void  testUnionFilter_3();
    void  testUnionFilter_4();
    void  testContactDetailFilter_1();
    void  testContactDetailFilter_2();
    void  testContactDetailFilter_3();
    void  testContactDetailFilter_4();
    void  testContactDetailFilter_5();
    void  testContactDetailFilter_6();
    void  testContactDetailFilter_7();
    void  testContactDetailFilter_8();
    void  testContactDetailFilter_9();
private:
    QHash<QContactFilter::FilterType, TFilter>  *mFilters;
    CContactDatabase *m_database;
    CntSymbianEngine *m_engine;
};
