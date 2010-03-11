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
    m_cumulativeExpressionLabel = new QLabel("Match All Contacts", this);
    m_cumulativeExpressionLabel->setWordWrap(true);
    m_cumulativeExpressionLabel->setFocusPolicy(Qt::StrongFocus);
    
#ifdef Q_OS_SYMBIAN
    // In symbian use softkeys instead of normal buttons
    m_addBtn = new QAction("Add", this);
    m_addBtn->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_addBtn);
    connect(m_addBtn, SIGNAL(triggered(bool)), this, SLOT(addClicked()));    
    m_filterBtn = new QAction("Filter", this);
    m_filterBtn->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(m_filterBtn);
    connect(m_filterBtn, SIGNAL(triggered(bool)), this, SLOT(filterClicked()));
#else
    m_addBtn = new QPushButton("Add", this);
    connect(m_addBtn, SIGNAL(clicked()), this, SLOT(addClicked()));
    m_filterBtn = new QPushButton("Filter", this);
    connect(m_filterBtn, SIGNAL(clicked()), this, SLOT(filterClicked()));
#endif
    m_clearBtn = new QPushButton("Clear", this);
    connect(m_clearBtn, SIGNAL(clicked()), this, SLOT(clearClicked()));

    QStringList filterableFields;
    filterableFields.append("Name");
    filterableFields.append("Phone Number");
    filterableFields.append("Email");
    m_fieldCriteriaCombo->addItems(filterableFields);

    m_criteriaTypeCombo->addItem("Equals", QContactFilter::MatchExactly);
    m_criteriaTypeCombo->addItem("Contains", QContactFilter::MatchContains);
    m_criteriaTypeCombo->addItem("Starts with", QContactFilter::MatchStartsWith);
    m_criteriaTypeCombo->addItem("Ends with", QContactFilter::MatchEndsWith);

    QStringList joinTypes;
    joinTypes.append("AND");
    joinTypes.append("OR");
    m_joinMethodCombo->addItems(joinTypes);
    
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(new QLabel("Search String:", this));
    formLayout->addRow(m_valueCriteriaEdit);
    formLayout->addRow(new QLabel("Search Field:", this));
    formLayout->addRow(m_fieldCriteriaCombo);
    formLayout->addRow(new QLabel("Criteria Type:", this));
    formLayout->addRow(m_criteriaTypeCombo);
    formLayout->addRow(new QLabel("Join Method:", this));
    formLayout->addRow(m_joinMethodCombo);
    QFrame* separatorFrame = new QFrame(this);
    separatorFrame->setFrameShape(QFrame::HLine);
    separatorFrame->setFrameShadow(QFrame::Plain);
    separatorFrame->setLineWidth(2);
    formLayout->addRow(separatorFrame);
    formLayout->addRow(new QLabel("Filter Expression:", this));
    formLayout->addRow(m_cumulativeExpressionLabel);
#ifdef Q_OS_SYMBIAN
    formLayout->addRow(m_clearBtn);
#endif    
    
    QVBoxLayout *pageLayout = new QVBoxLayout;
    
    QScrollArea *formScrollArea = new QScrollArea(this);
    formScrollArea->setWidgetResizable(true);
    QWidget *formContainer = new QWidget(formScrollArea);
    formContainer->setLayout(formLayout);
    formScrollArea->setWidget(formContainer);
    pageLayout->addWidget(formScrollArea);
    
#ifndef Q_OS_SYMBIAN
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_clearBtn);
    btnLayout->addWidget(m_filterBtn);
    pageLayout->addLayout(btnLayout);
#endif

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

    int flag = m_criteriaTypeCombo->itemData(m_criteriaTypeCombo->currentIndex()).toInt();
    fil.setMatchFlags(QContactFilter::MatchFlags(flag));
    exprMatch = m_criteriaTypeCombo->currentText().toLower();

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

void FilterPage::filterClicked()
{
    emit showListPage(m_cumulativeFilter);
}
