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

#include "filterpage.h"

#include <QtGui>

FilterPage::FilterPage(QWidget* parent)
        : QWidget(parent)
{
    m_valueCriteriaEdit = new QLineEdit(this);
    m_fieldCriteriaCombo = new QComboBox(this);
    m_criteriaTypeCombo = new QComboBox(this);
    m_joinMethodCombo = new QComboBox(this);
    m_cumulativeExpressionLabel = new QLabel(this);
    m_cumulativeExpressionLabel->setText("Match All Contacts");

    QStringList filterableFields;
    filterableFields.append("Name");
    filterableFields.append("Phone Number");
    filterableFields.append("Email");
    m_fieldCriteriaCombo->addItems(filterableFields);

    QStringList criteriaTypes;
    criteriaTypes.append("Exact Match");
    criteriaTypes.append("Starts With");
    criteriaTypes.append("Ends With");
    criteriaTypes.append("Contains");
    m_criteriaTypeCombo->addItems(criteriaTypes);

    QStringList joinTypes;
    joinTypes.append("AND");
    joinTypes.append("OR");
    m_joinMethodCombo->addItems(joinTypes);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Search String: ", m_valueCriteriaEdit);
    formLayout->addRow("Search Field: ", m_fieldCriteriaCombo);
    formLayout->addRow("Criteria Type: ", m_criteriaTypeCombo);
    formLayout->addRow("Join Method: ", m_joinMethodCombo);
    // TODO: add separator.
    formLayout->addRow("Filter Expression: ", m_cumulativeExpressionLabel);

    m_add = new QPushButton("Add", this);
    connect(m_add, SIGNAL(clicked()), this, SLOT(addClicked()));
    m_clear = new QPushButton("Clear", this);
    connect(m_clear, SIGNAL(clicked()), this, SLOT(clearClicked()));
    m_done = new QPushButton("Filter", this);
    connect(m_done, SIGNAL(clicked()), this, SLOT(doneClicked()));

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_add);
    btnLayout->addWidget(m_clear);
    btnLayout->addWidget(m_done);

    QVBoxLayout *pageLayout = new QVBoxLayout;
    pageLayout->addLayout(formLayout);
    pageLayout->addLayout(btnLayout);

    setLayout(pageLayout);
}

FilterPage::~FilterPage()
{
}

void FilterPage::addClicked()
{
    QContactDetailFilter fil;
    QString defName;
    QString fieldName;
    QString exprName;
    QString exprMatch;
    QString exprJoin;
    switch (m_fieldCriteriaCombo->currentIndex()) {
        case 0:
        {
            // name
            defName = QString(QLatin1String(QContactDisplayLabel::DefinitionName));
            fieldName = QString(QLatin1String(QContactDisplayLabel::FieldLabel));

            exprName = "Name";
        }
        break;

        case 1:
        {
            // phone number
            defName = QString(QLatin1String(QContactPhoneNumber::DefinitionName));
            fieldName = QString(QLatin1String(QContactPhoneNumber::FieldNumber));

            exprName = "Phone Number";
        }
        break;

        default:
        {
            // email address
            defName = QString(QLatin1String(QContactEmailAddress::DefinitionName));
            fieldName = QString(QLatin1String(QContactEmailAddress::FieldEmailAddress));

            exprName = "Email Address";
        }
        break;

    }
    fil.setDetailDefinitionName(defName, fieldName);
    fil.setValue(m_valueCriteriaEdit->text());

    QContactFilter::MatchFlags matchFlags;
    switch (m_criteriaTypeCombo->currentIndex()) {
        case 0:
        {
            matchFlags |= QContactFilter::MatchExactly;
            exprMatch = "equals";
        }
        break;

        case 1:
        {
            matchFlags |= QContactFilter::MatchStartsWith;
            exprMatch = "starts with";
        }
        break;

        case 2:
        {
            matchFlags |= QContactFilter::MatchEndsWith;
            exprMatch = "ends with";
        }
        break;

        default:
        {
            matchFlags |= QContactFilter::MatchContains;
            exprMatch = "contains";
        }
        break;
    }
    fil.setMatchFlags(matchFlags);

    // if OR then join with OR
    if (m_joinMethodCombo->currentIndex() == 1) {
        QContactUnionFilter ufil;
        ufil << m_cumulativeFilter << fil;
        QContactIntersectionFilter ifil;
        ifil << ufil;
        m_cumulativeFilter = ifil;
        exprJoin = "OR";
    } else {
        // otherwise, just AND.
        QContactIntersectionFilter ifil(m_cumulativeFilter);
        ifil << fil;
        m_cumulativeFilter = ifil;
        exprJoin = "AND";
    }

    // set the expression so far
    if (!m_cumulativeExpression.isEmpty())
        m_cumulativeExpression += " " + exprJoin + " ";
    m_cumulativeExpression += exprName + " " + exprMatch + " \"" + m_valueCriteriaEdit->text() + "\"";

    // and clear the UI ready for the next filter expression.
    m_valueCriteriaEdit->setText("");
    m_fieldCriteriaCombo->setCurrentIndex(0);
    m_criteriaTypeCombo->setCurrentIndex(0);
    m_joinMethodCombo->setCurrentIndex(0);
    m_cumulativeExpressionLabel->setText(m_cumulativeExpression);
}

void FilterPage::clearClicked()
{
    m_cumulativeExpression = QString();
    m_valueCriteriaEdit->setText("");
    m_fieldCriteriaCombo->setCurrentIndex(0);
    m_criteriaTypeCombo->setCurrentIndex(0);
    m_joinMethodCombo->setCurrentIndex(0);
    m_cumulativeExpressionLabel->setText("Match All Contacts");
    m_cumulativeFilter = QContactFilter();
}

void FilterPage::doneClicked()
{
    emit showListPage(m_cumulativeFilter);
}
