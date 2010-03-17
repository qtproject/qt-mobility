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

#include "viewer.h"

#include <QtGui/qformlayout.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qspinbox.h>
#include <QtGui/qtableview.h>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , m_typeInput(0)
    , m_offsetInput(0)
    , m_limitInput(0)
{
    m_request.setGallery(&m_gallery);
    m_request.setFields(QStringList() << QDocumentGallery::title << QDocumentGallery::duration);
    connect(&m_request, SIGNAL(documentsChanged()), this, SLOT(documentsChanged()));

    m_model.setColumnCount(2);
    m_model.setColumnField(0, QDocumentGallery::title);
    m_model.setColumnField(1, QDocumentGallery::duration);

    m_typeInput = new QLineEdit;

    m_offsetInput = new QSpinBox;
    m_offsetInput->setRange(0, 1000);

    m_limitInput = new  QSpinBox;
    m_limitInput->setRange(0, 1000);
    m_limitInput->setValue(100);

    QPushButton *executeButton = new QPushButton(tr("Execute"));
    connect(executeButton, SIGNAL(clicked()), this, SLOT(execute()));

    QTableView *view = new QTableView;
    view->setModel(&m_model);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Type:"), m_typeInput);
    layout->addRow(tr("Offset:"), m_offsetInput);
    layout->addRow(tr("Limit:"), m_limitInput);
    layout->addRow(executeButton);
    layout->addRow(view);

    setLayout(layout);
}

Viewer::~Viewer()
{
}

void Viewer::execute()
{
    m_request.setItemType(m_typeInput->text());
    m_request.setStartIndex(m_offsetInput->value());
    m_request.setMaximumCount((m_limitInput->value()));
    m_request.execute();
}

void Viewer::documentsChanged()
{
    m_model.setList(m_request.items());
}
