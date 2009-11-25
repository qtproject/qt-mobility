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


#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QWidget>
#include "qcontactfilter.h"
#include "qcontactintersectionfilter.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;

QTM_USE_NAMESPACE

class FilterDialog : public QWidget
{
    Q_OBJECT

public:
    FilterDialog(QWidget *parent = 0);
    ~FilterDialog();

    QContactFilter filter() const;
    int status() const;

signals:
    void hidden();

public slots:
    void showDialog();

private slots:
    void cancelClicked();
    void addClicked();
    void doneClicked();

private:
    QLineEdit *value;
    QComboBox *field;
    QComboBox *match;
    QComboBox *join;
    QLabel *expression;
    QFormLayout *formLayout;
    QHBoxLayout *btnLayout;
    QVBoxLayout *vertLayout;

    QPushButton *cancel;
    QPushButton *add;
    QPushButton *done;

    mutable QContactIntersectionFilter total;
    mutable bool noFiltersYet;
    mutable int state;
    mutable QString expressionSoFar;
};


#endif
