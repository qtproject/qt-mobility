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
#include "interfacewidget.h"
#include "mandatorylineedit.h"
#include "errorcollector.h"

#include <qserviceinterfacedescriptor_p.h>

#include <QSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QStringList>
#include <QHash>
#include <QListView>
#include <QPushButton>
#include <QStringListModel>
#include <QDebug>

QTM_USE_NAMESPACE

class EditableListWidgetGroup : public QWidget
{
    Q_OBJECT
public:
    EditableListWidgetGroup(QWidget *parent = 0);

signals:
    void dataChanged();

protected:
    virtual int addItem() = 0;
    virtual bool removeItemAt(int index) = 0;
    virtual QStringList listValues() const = 0;

    void initLayout(QLayout *addItemLayout, const QString &buttonAddText);
    void refreshView();
    void setAddButtonEnabled(bool enabled);

private slots:
    void clickedAdd();
    void clickedRemove();
    void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QString defaultValue() const;

    QStringListModel *m_model;
    QListView *m_listView;
    QPushButton *m_buttonAdd;
    QPushButton *m_buttonDel;
    QGridLayout *m_grid;
};

EditableListWidgetGroup::EditableListWidgetGroup(QWidget *parent)
    : QWidget(parent),
      m_model(new QStringListModel(this)),
      m_listView(new QListView),
      m_buttonAdd(new QPushButton),
      m_buttonDel(new QPushButton(tr("Delete"))),
      m_grid(new QGridLayout)
{
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void EditableListWidgetGroup::initLayout(QLayout *addItemLayout, const QString &buttonAddText)
{
    m_buttonAdd->setText(buttonAddText);

    m_grid->setContentsMargins(0, 0, 0, 0);
    m_grid->addWidget(m_listView, 0, 0);
    m_grid->addWidget(m_buttonDel, 0, 1, Qt::AlignTop);
    m_grid->addWidget(m_buttonAdd, 1, 1);

    if (addItemLayout)
        m_grid->addLayout(addItemLayout, 1, 0);

    m_model->setStringList(QStringList(defaultValue()));
    m_listView->setModel(m_model);

    m_buttonAdd->setEnabled(false);
    m_buttonDel->setEnabled(false);

    connect(m_buttonAdd, SIGNAL(clicked()), SLOT(clickedAdd()));
    connect(m_buttonDel, SIGNAL(clicked()), SLOT(clickedRemove()));
    connect(m_listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(currentRowChanged(QModelIndex,QModelIndex)));

    setLayout(m_grid);
}

void EditableListWidgetGroup::refreshView()
{
    QStringList values = listValues();
    if (values.isEmpty())
        m_model->setStringList(QStringList(defaultValue()));
    else
        m_model->setStringList(values);
}

void EditableListWidgetGroup::setAddButtonEnabled(bool enabled)
{
    m_buttonAdd->setEnabled(enabled);
}

void EditableListWidgetGroup::clickedAdd()
{
    int index = addItem();
    if (index >= 0) {
        refreshView();
        m_listView->setCurrentIndex(m_model->index(index));
        emit dataChanged();
    }
}

void EditableListWidgetGroup::clickedRemove()
{
    QModelIndex index = m_listView->currentIndex();
    if (index.isValid() && removeItemAt(index.row())) {
        refreshView();
        m_listView->setCurrentIndex(m_model->index(index.row() == 0 ? 0 : index.row()-1));
        emit dataChanged();
    }
}

void EditableListWidgetGroup::currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    m_buttonDel->setEnabled(current.isValid() && listValues().count() > 0);
}

QString EditableListWidgetGroup::defaultValue() const
{
    return tr("(None)");
}


class CapabilitiesWidget : public EditableListWidgetGroup
{
    Q_OBJECT
public:
    CapabilitiesWidget(QWidget *parent = 0);

    void setCapabilities(const QStringList &values);
    const QStringList &capabilities() const { return m_values; }

protected:
    virtual int addItem();
    virtual bool removeItemAt(int index);
    virtual QStringList listValues() const;

private slots:
    void valueToAddChanged(const QString &text);

private:
    QStringList m_values;
    QLineEdit *m_newValueEdit;
};

CapabilitiesWidget::CapabilitiesWidget(QWidget *parent)
    : EditableListWidgetGroup(parent),
      m_newValueEdit(new QLineEdit)
{
    connect(m_newValueEdit, SIGNAL(textChanged(QString)),
            SLOT(valueToAddChanged(QString)));

    QHBoxLayout *box = new QHBoxLayout;
    box->addWidget(m_newValueEdit);
    initLayout(box, tr("Add"));
}

void CapabilitiesWidget::setCapabilities(const QStringList &values)
{
    m_values = values;
    refreshView();
}

int CapabilitiesWidget::addItem()
{
    if (!m_newValueEdit->text().isEmpty()) {
        m_values << m_newValueEdit->text();
        m_newValueEdit->clear();
        return m_values.count() - 1;
    }
    return -1;
}

bool CapabilitiesWidget::removeItemAt(int index)
{
    if (index >= 0 && index < m_values.count()) {
        m_values.removeAt(index);
        return true;
    }
    return false;
}

QStringList CapabilitiesWidget::listValues() const
{
    return m_values;
}

void CapabilitiesWidget::valueToAddChanged(const QString &text)
{
    setAddButtonEnabled(!text.isEmpty());
}


class CustomPropertiesWidget : public EditableListWidgetGroup
{
    Q_OBJECT
public:
    CustomPropertiesWidget(QWidget *parent = 0);

    void setProperties(const QHash<QString, QString> &values);

    const QStringList &propertyKeys() const { return m_keys; }
    const QHash<QString, QString> &properties() const { return m_pairs; }

protected:
    virtual int addItem();
    virtual bool removeItemAt(int index);
    virtual QStringList listValues() const;

private slots:
    void keyToAddChanged(const QString &text);

private:
    QStringList m_keys;
    QHash<QString, QString> m_pairs;
    QLineEdit *m_newKeyEdit;
    QLineEdit *m_newValueEdit;
};

CustomPropertiesWidget::CustomPropertiesWidget(QWidget *parent)
    : EditableListWidgetGroup(parent),
      m_newKeyEdit(new QLineEdit),
      m_newValueEdit(new QLineEdit)
{
    connect(m_newKeyEdit, SIGNAL(textChanged(QString)),
            SLOT(keyToAddChanged(QString)));

    QHBoxLayout *box = new QHBoxLayout;
    box->addWidget(m_newKeyEdit);
    box->addWidget(new QLabel(QLatin1String("=>")));
    box->addWidget(m_newValueEdit);
    initLayout(box, tr("Add / Modify"));
}

void CustomPropertiesWidget::setProperties(const QHash<QString, QString> &values)
{
    m_pairs = values;
    m_keys = m_pairs.keys();
    refreshView();
}

int CustomPropertiesWidget::addItem()
{
    int index = -1;
    QString key = m_newKeyEdit->text();
    if (!key.isEmpty()) {
        index = m_keys.indexOf(key);
        if (index < 0) {
            m_keys << key;
            index = m_keys.count() - 1;
        }
        m_pairs[key] = m_newValueEdit->text();
        m_newKeyEdit->clear();
        m_newValueEdit->clear();
    }
    return index;
}

bool CustomPropertiesWidget::removeItemAt(int index)
{
    if (index >= 0 && index < m_keys.count()) {
        m_pairs.remove(m_keys.takeAt(index));
        return true;
    }
    return false;
}

QStringList CustomPropertiesWidget::listValues() const
{
    QStringList values;
    for (int i=0; i<m_keys.count(); i++)
        values << (m_keys[i] + " => " + m_pairs[m_keys[i]]);
    return values;
}

void CustomPropertiesWidget::keyToAddChanged(const QString &text)
{
    setAddButtonEnabled(!text.isEmpty());
}


InterfaceWidget::InterfaceWidget(QWidget *parent)
    : QWidget(parent),
      m_name(new MandatoryLineEdit(tr("(Name required)"))),
      m_verMajor(new QSpinBox),
      m_verMinor(new QSpinBox),
      m_desc(new QLineEdit),
      m_capWidget(new CapabilitiesWidget),
      m_customPropWidget(new CustomPropertiesWidget)
{
    m_verMajor->setMinimum(1);  // min version as documented for interfaces

    QHBoxLayout *versions = new QHBoxLayout;
    versions->addWidget(m_verMajor);
    versions->addWidget(new QLabel(tr("(Major)")));
    versions->addWidget(m_verMinor);
    versions->addWidget(new QLabel(tr("(Minor)")));

    QFormLayout *form = new QFormLayout;
    form->addRow(tr("* Name"), m_name);
    form->addRow(tr("* Version"), versions);
    form->addRow(tr("Description"), m_desc);
    form->addRow(tr("Capabilities"), m_capWidget);
    form->addRow(tr("Custom attributes"), m_customPropWidget);
    setLayout(form);

    connect(m_name, SIGNAL(textEdited(QString)), SLOT(changedNameOrVersion()));
    connect(m_verMajor, SIGNAL(valueChanged(int)), SLOT(changedNameOrVersion()));
    connect(m_verMinor, SIGNAL(valueChanged(int)), SLOT(changedNameOrVersion()));

    connect(m_desc, SIGNAL(textEdited(QString)), SIGNAL(dataChanged()));
    connect(m_capWidget, SIGNAL(dataChanged()), SIGNAL(dataChanged()));
    connect(m_customPropWidget, SIGNAL(dataChanged()), SIGNAL(dataChanged()));
}

void InterfaceWidget::init()
{
    m_name->setFocus();
}

void InterfaceWidget::load(const QServiceInterfaceDescriptor &info)
{
    const QServiceInterfaceDescriptorPrivate *p = QServiceInterfaceDescriptorPrivate::getPrivate(&info);
    if (!p)
        return;

    m_name->setText(p->interfaceName);
    m_verMajor->setValue(p->major);
    m_verMinor->setValue(p->minor);
    if (p->attributes.contains(QServiceInterfaceDescriptor::InterfaceDescription))
        m_desc->setText(p->attributes[QServiceInterfaceDescriptor::InterfaceDescription].toString());

    if (p->attributes.contains(QServiceInterfaceDescriptor::Capabilities))
        m_capWidget->setCapabilities(p->attributes[QServiceInterfaceDescriptor::Capabilities].toStringList());
    if (!p->customAttributes.isEmpty())
        m_customPropWidget->setProperties(p->customAttributes);
}

void InterfaceWidget::validate(ErrorCollector *errors)
{
    m_name->validate(errors);
}

void InterfaceWidget::writeXml(QXmlStreamWriter *writer) const
{
    writer->writeTextElement(QLatin1String("name"), !m_name->hasText() ? QString() : m_name->text());
    writer->writeTextElement(QLatin1String("version"),
            QString("%1.%2").arg(m_verMajor->value()).arg(m_verMinor->value()));
    writer->writeTextElement(QLatin1String("description"), m_desc->text());
    writer->writeTextElement(QLatin1String("capabilities"),
            m_capWidget->capabilities().join(QLatin1String(",")));

    const QStringList &customPropKeys = m_customPropWidget->propertyKeys();
    const QHash<QString, QString> &customProps = m_customPropWidget->properties();
    for (int i=0; i<customPropKeys.count(); i++) {
        writer->writeStartElement(QLatin1String("customproperty"));
        writer->writeAttribute(QLatin1String("key"), customPropKeys[i]);
        writer->writeCharacters(customProps[customPropKeys[i]]);
        writer->writeEndElement();
    }
}

QString InterfaceWidget::title() const
{
    QString name = m_name->text().isEmpty() ? tr("[New Interface]") : m_name->text();
    return QString("%1 %2.%3").arg(name).arg(m_verMajor->value()).arg(m_verMinor->value());
}

void InterfaceWidget::changedNameOrVersion()
{
    emit titleChanged(title());
    emit dataChanged();
}

#include "interfacewidget.moc"
