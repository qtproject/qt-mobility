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

#include "filterdialog.h"

#include "qtcontacts.h"
#include <QtGui>

FilterDialog::FilterDialog(QWidget* parent)
        : QWidget(parent)
{
	vertLayout = new QVBoxLayout;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumSize(320,320);

    state = -1; // not yet opened.
    noFiltersYet = true;
    expressionSoFar = "";

    setWindowTitle(tr("Contact Search"));
    value = new QLineEdit;
    field = new QComboBox;
    match = new QComboBox;
    join = new QComboBox;
    expression = new QLabel(tr("Cumulative Expression:"));
    expression->setWordWrap(true);
  
    cancel = new QPushButton(tr("Cancel"));
    add = new QPushButton(tr("Add Filter"));
    done = new QPushButton(tr("Done"));

	connect(cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(add, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(done, SIGNAL(clicked()), this, SLOT(doneClicked()));

    field->addItem(tr("Name"));
    field->addItem(tr("Phone Number"));
    field->addItem(tr("Email Address"));

    match->addItem(tr("Exact Match"));
    match->addItem(tr("Starts With"));
    match->addItem(tr("Contains"));
    match->addItem(tr("Ends With"));

    join->addItem(tr("AND"));
    join->addItem(tr("OR"));

    formLayout = new QFormLayout;
    formLayout->addRow("Search String:", value);
    formLayout->addRow("In Field:", field);
    formLayout->addRow("With Criteria:", match);
    formLayout->addRow("Join Method:", join);

    btnLayout = new QHBoxLayout;
    btnLayout->addWidget(cancel);
    btnLayout->addWidget(add);
    btnLayout->addWidget(done);
	
	vertLayout->addLayout(formLayout);
	vertLayout->addWidget(expression);
	formLayout->addRow(btnLayout);

    setLayout(vertLayout);
}

FilterDialog::~FilterDialog()
{
}

QContactFilter FilterDialog::filter() const
{
    return total;
}

int FilterDialog::status() const
{
    // -1 = cancelled
    //  0 = in progress
    //  1 = accepted
    return state;
}

void FilterDialog::cancelClicked()
{
    total = QContactIntersectionFilter();
    noFiltersYet = true;
    state = -1;
    hide();
    emit hidden();
}

void FilterDialog::addClicked()
{
    state = 0;

    QString exprName;
    QString exprMatch;
    QString exprJoin;

    QContactDetailFilter fil;
    QString defName;
    QString fieldName;
    switch (field->currentIndex()) {
        case 0:
        {
            // name
            defName = QString(QLatin1String(QContactDisplayLabel::DefinitionName));
            fieldName = QString(QLatin1String(QContactDisplayLabel::FieldLabel));

            exprName = "NAME";
        }
        break;

        case 1:
        {
            // phone number
            defName = QString(QLatin1String(QContactPhoneNumber::DefinitionName));
            fieldName = QString(QLatin1String(QContactPhoneNumber::FieldNumber));

            exprName = "PHONENUMBER";
        }
        break;

        default:
        {
            // email address
            defName = QString(QLatin1String(QContactEmailAddress::DefinitionName));
            fieldName = QString(QLatin1String(QContactEmailAddress::FieldEmailAddress));

            exprName = "EMAILADDRESS";
        }
        break;

    }
    fil.setDetailDefinitionName(defName, fieldName);
    fil.setValue(value->text());

    Qt::MatchFlags matchFlags;
    switch (match->currentIndex()) {
        case 0:
        {
            matchFlags |= Qt::MatchExactly;

            exprMatch = "EQUALS";
        }
        break;

        case 1:
        {
            matchFlags |= Qt::MatchStartsWith;

            exprMatch = "STARTSWITH";
        }
        break;

        case 2:
        {
            matchFlags |= Qt::MatchContains;

            exprMatch = "CONTAINS";
        }
        break;

        default:
        {
            matchFlags |= Qt::MatchEndsWith;

            exprMatch = "ENDSWITH";
        }
        break;
    }
    fil.setMatchFlags(matchFlags);

    // if OR then join with OR
    noFiltersYet = false; // either way, have added a filter.
    if (join->currentIndex() == 1) {
        QContactUnionFilter ufil;
        ufil << total << fil;
        QContactIntersectionFilter ifil;
        ifil << ufil;
        total = ifil;

        exprJoin = "OR";
    } else {
        // otherwise, just AND.
        total << fil;

        exprJoin = "AND";
    }

    // set the expression so far
    if (!expressionSoFar.isEmpty())
        expressionSoFar += " " + exprJoin + " ";
    expressionSoFar += exprName + " " + exprMatch + " \"" + value->text() + "\"";
    expression->setText(tr("Cumulative Expression:") + expressionSoFar);

    // now reset the other UI elements.
    value->setText("");
    field->setCurrentIndex(0);
    match->setCurrentIndex(0);
    join->setCurrentIndex(0);
}

void FilterDialog::doneClicked()
{
    state = 1;
    hide();
    emit hidden();
}

void FilterDialog::showDialog()
{
    total = QContactIntersectionFilter();
    noFiltersYet = true;
    state = 0;
    expressionSoFar = "";

    // now reset the UI elements.
    value->setText("");
    field->setCurrentIndex(0);
    match->setCurrentIndex(0);
    join->setCurrentIndex(0);
    expression->setText(tr("Cumulative Expression:"));
	show();
}
