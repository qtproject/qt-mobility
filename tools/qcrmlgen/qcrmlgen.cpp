/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qcrmlgen.h"
#include "qcrmlparser_p.h"

#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QPushButton>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegExpValidator>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDebug>
#include <QCloseEvent>

QTM_USE_NAMESPACE

bool checkID(const QString &id)
{
    if (id.length() > 8 || id.length() < 0)
        return false;
    bool ok = false;
    id.toUInt(&ok, 16);
    return ok;
}

#ifdef INCL_TYPE
TypeDelegate::TypeDelegate(QTableWidget *parent)
    : QItemDelegate(parent), m_parentTable(parent)
{
}

QWidget *TypeDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &,
        const QModelIndex &) const {
    QComboBox *editor = new QComboBox(parent);
    editor->addItem("int");
    editor->addItem("real");
    editor->addItem("string");
    editor->addItem("string8");
    editor->addItem("binary");

    return editor;
}

void TypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTableWidgetItem * item = m_parentTable->item(index.row(), index.column());
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!item->text().isEmpty() && comboBox->findText(item->text()) != -1) {
        comboBox->setCurrentIndex(comboBox->findText(item->text()));
    }
}

void TypeDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *,
                                const QModelIndex &index) const{

    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    m_parentTable->item(index.row(), index.column())->setText(comboBox->currentText());
}
#endif

KeyIdDelegate::KeyIdDelegate(QTableWidget *parent)
    : QItemDelegate(parent), m_parentTable(parent)
{
}

QWidget *KeyIdDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &,
        const QModelIndex &) const {
    QLineEdit *editor = new QLineEdit(parent);
    QTableWidgetItem *item = m_parentTable->currentItem();
    if (!item->text().isEmpty()) {
        editor->setText(item->text());
    }
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("([0-9]|[A-F]|[a-f]){1,8}"), editor);
    editor->setValidator(validator);

    return editor;
}

void KeyIdDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *,
                                const QModelIndex &index) const{

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    m_parentTable->item(index.row(), index.column())->setText(lineEdit->text());
}

PathDelegate::PathDelegate(QTableWidget *parent)
    : QItemDelegate(parent), m_parentTable(parent)
{
}

QWidget *PathDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &,
        const QModelIndex &) const {
    QLineEdit *editor = new QLineEdit(parent);
    QTableWidgetItem *item = m_parentTable->currentItem();
    if (!item->text().isEmpty()) {
        editor->setText(item->text());
    }

    return editor;
}

void PathDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *,
                                const QModelIndex &index) const{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    m_parentTable->item(index.row(), index.column())->setText(lineEdit->text());
}

EditorWidget::EditorWidget():m_isModified(false) {
    m_RPropRadio = new QRadioButton("RProperty");
    m_CRepRadio = new QRadioButton("CRepository");
    QButtonGroup *targetGroup = new QButtonGroup(this);
    targetGroup->addButton(m_CRepRadio);
    targetGroup->addButton(m_RPropRadio);
    targetGroup->setExclusive(true);
    connect(targetGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(targetChanged(QAbstractButton *)));

    QHBoxLayout *targetLayout = new QHBoxLayout;
    targetLayout->addWidget(m_RPropRadio);
    targetLayout->addWidget(m_CRepRadio);

    m_RPropRadio->setChecked(true);
    m_repoLabel = new QLabel("Category ID", this);
    m_repoUID = new QLineEdit(this);
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("([0-9]|[A-F]|[a-f]){1,8}"), this);
    m_repoUID->setValidator(validator);
    connect(m_repoUID, SIGNAL(textEdited(const QString &)), this, SLOT(setModified()));
    m_repoUID->setToolTip("Must be a hexidecimal number no longer than  8 digits");

    QHBoxLayout *repoLayout = new QHBoxLayout;
    repoLayout->addWidget(m_repoLabel);
    repoLayout->addWidget(m_repoUID);

    QStringList headers;
    headers << tr("Key ID")  << tr("Key path");

#ifdef INCL_TYPE
    headers << tr("Type");
#endif

    m_tableWidget = new QTableWidget(0,headers.count());
    connect(m_tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(setModified()));
    m_tableWidget->setHorizontalHeaderLabels(headers);
    m_tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    m_keyIdDelegate = new KeyIdDelegate(m_tableWidget);
    m_tableWidget->setItemDelegateForColumn(EditorWidget::KeyId, m_keyIdDelegate);

    m_pathDelegate = new PathDelegate(m_tableWidget);
    m_tableWidget->setItemDelegateForColumn(EditorWidget::Path, m_pathDelegate);

#ifdef INCL_TYPE
    m_typeDelegate = new TypeDelegate(m_tableWidget);
    m_tableWidget->setItemDelegateForColumn(EditorWidget::Type,m_typeDelegate);
#endif

    m_addRowButton = new QPushButton(tr("Add Row"), this);
    connect(m_addRowButton, SIGNAL(clicked()), this, SLOT(addRow()));
    m_removeRowButton = new QPushButton(tr("Remove Row"), this);
    connect(m_removeRowButton, SIGNAL(clicked()), this, SLOT(removeRow()));
    QPushButton *upButton = new QPushButton(tr("/\\"), this);
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveRowUp()));
    QPushButton *downButton = new QPushButton(tr("\\/"), this);
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveRowDown()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_addRowButton);
    buttonsLayout->addWidget(m_removeRowButton);
    buttonsLayout->addWidget(upButton);
    buttonsLayout->addWidget(downButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(targetLayout);
    mainLayout->addLayout(repoLayout);
    mainLayout->addWidget(m_tableWidget);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    addRow();
    setModified(false);
}

void EditorWidget::addRow()
{
    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);

    QTableWidgetItem *item;
    item = new QTableWidgetItem;
    item->setToolTip("Must be a hexidecimal number no longer than 8 digits");
    m_tableWidget->setItem(row, EditorWidget::KeyId, item);

    item = new QTableWidgetItem("/");
    item->setToolTip("Must not be empty and must start with a /");
    m_tableWidget->setItem(row, EditorWidget::Path, item);

#ifdef INCL_TYPE
    item = new QTableWidgetItem("int");
    m_tableWidget->setItem(row, EditorWidget::Type, item);
#endif

    setModified(true);
}

void EditorWidget::removeRow()
{
    int row = m_tableWidget->currentRow();
    m_tableWidget->removeRow(row);
    row = m_tableWidget->currentRow();
    m_tableWidget->setCurrentCell(row, 0);
    setModified(true);
}

void EditorWidget::initNew()
{
    m_repoUID->clear();

    for(int i = m_tableWidget->rowCount() -1;  i >= 0; --i) {
        m_tableWidget->removeRow(i);
    }
    addRow();
    setModified(false);
}

void EditorWidget::save(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadWrite)) {
        QMessageBox::warning(this, tr("File Error"),
                            tr("%1 could not be opened").arg(filePath));
        return;

    }
    file.resize(0);

    QString repoUID = m_repoUID->text();
    QString keyId;
    QString path;
#ifdef INCL_TYPE
    QString type;
#endif
    QString documentStart("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    file.write(documentStart.toUtf8());

    QString repositoryElementStart("<repository target=\"%1\" uidValue=\"0x%2\">\n");
    QString target = m_RPropRadio->isChecked()?"RProperty":"CRepository";
    file.write(repositoryElementStart.arg(target).arg(repoUID).toUtf8());
#ifdef INCL_TYPE
    QString keyElementStart("    <key int=\"0x%1\" type=\"%2\" ref=\"%3\">\n");
#else

    QString keyElementStart("    <key int=\"0x%1\" ref=\"%2\">\n");
#endif
    QString keyElementEnd("    </key>\n");

    for(int i=0; i < m_tableWidget->rowCount(); i++) {
        keyId = m_tableWidget->item(i, EditorWidget::KeyId)->text();
        path = m_tableWidget-> item(i, EditorWidget::Path)->text().remove(0,1);
#ifdef INCL_TYPE
        type = m_tableWidget->item(i, EditorWidget::Type)->text();
        file.write(keyElementStart.arg(keyId).arg(type).arg(path).toUtf8());
#else
        file.write(keyElementStart.arg(keyId).arg(path).toUtf8());
#endif

        file.write(keyElementEnd.toUtf8());
    }

    QString repositoryElementEnd("</repository>\n");
    file.write(repositoryElementEnd.toUtf8());
    file.close();
    setModified(false);
}

void EditorWidget::open(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadWrite)) {
        QMessageBox::warning(this, tr("File Error"),
                            tr("%1 could not be opened").arg(filePath));
        return;
    }

    for(int i = m_tableWidget->rowCount() - 1; i >=0; i--) {
        m_tableWidget->removeRow(i);
    }

    QList<KeyData> keyData;
    QCrmlParser parser;
    keyData = parser.parseQCrml(filePath);
    if (parser.error() != QCrmlParser::NoError) {
        QMessageBox::warning(this, tr("Parse Error"),
                            tr("%1 is not a valid qcrml file").arg(filePath));
        qWarning() << "Parsing error:" << parser.errorString();
        return;
    }

    for (int i = 0; i < keyData.count(); ++i) {
        if (i == 0) {
            m_repoUID->setText(QString::number(keyData.at(i).repoId(),16));
            if (keyData.at(i).target() == KeyData::RProperty)
                m_RPropRadio->click();
            else 
                m_CRepRadio->click();
        }

        addRow();
        m_tableWidget->item(i, EditorWidget::KeyId)->setText(QString::number(keyData.at(i).keyId(),16));
        m_tableWidget->item(i, EditorWidget::Path)->setText(keyData.at(i).path());
#ifdef INCL_TYPE
        m_tableWidget->item(i, EditorWidget::Type)->setText("int");
#endif
    }
    setModified(false);
}

bool EditorWidget::isModified()
{
    if (!m_isModified) {
        //check if the user has started a cell editor but hadn't
        //committed it's contents to the table by pressing enter
        QWidget *editorWidget = m_tableWidget->cellWidget(m_tableWidget->currentRow(),
                                            m_tableWidget->currentColumn());
        if (editorWidget) {
            switch(m_tableWidget->currentColumn()) {
                case(EditorWidget::KeyId):
                case(EditorWidget::Path): {
                    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editorWidget);
                    if (lineEdit->text() != m_tableWidget->currentItem()->text())
                        m_isModified = true;
                    break;
                }
#ifdef INCL_TYPE
                case(EditorWidget::Type): {
                    QComboBox *comboBox = qobject_cast<QComboBox *>(editorWidget);
                    if (comboBox->currentText() != m_tableWidget->currentItem()->text())
                        m_isModified = true;
                    break;
                }
#endif
                default:
                    qWarning() << "EditorWidget::isModified():- unknown widget type";
            }
        }
    }
    return m_isModified;
}

bool EditorWidget::verifyContents()
{
    //If any cells are currently edited add their contents
    //to the tableWidget
    QWidget *cellWidget = m_tableWidget->cellWidget(m_tableWidget->currentRow(), m_tableWidget->currentColumn());
    if (cellWidget) {
        QAbstractItemDelegate *delegate = m_tableWidget->itemDelegateForColumn(m_tableWidget->currentColumn());
        if (delegate) {
            delegate->setModelData(cellWidget,m_tableWidget->model(),m_tableWidget->currentIndex());
        }
    }

    QString repoUID = m_repoUID->text();
    if (!checkID(repoUID)) {
        QMessageBox::warning(this, "Invalid input", m_repoLabel->text() + "field is invalid, it must be a hexidecimal number no longer than 8 digits");
        m_repoUID->setFocus();
        return false;
    }

    QString keyId;
    QString path;
#ifdef INCL_TYPE
    QString type;
#endif
    for(int i=0; i < m_tableWidget->rowCount(); i++) {
        keyId = m_tableWidget->item(i, EditorWidget::KeyId)->text();
        if (!checkID(keyId)) {
            QMessageBox::warning(this, tr("Invalid Key ID"),
                    tr("The Key ID field is invalid, it must be a hexidecimal number no longer than 8 digits"));
            m_tableWidget->setCurrentCell(i, EditorWidget::KeyId);
            m_tableWidget->setFocus();
            return false;
        }

        path = m_tableWidget->item(i, EditorWidget::Path)->text();
        if (path.isEmpty() || !path.startsWith("/")) {
            QMessageBox::warning(this, tr("Invalid Path"),
                            tr("The Key Path field is invalid, it must not be empty and start with a /"));
            m_tableWidget->setCurrentCell(i, EditorWidget::Path);
            m_tableWidget->setFocus();
            return false;
        }

#ifdef INCL_TYPE
        type = m_tableWidget->item(i, EditorWidget::Type)->text();
        if (type.isEmpty()) {
            QMessageBox::warning(this, tr("Invalid Type"),
                                tr("The Type field is invalid, it must not be empty"));
            m_tableWidget->setCurrentCell(i, EditorWidget::Type);
            m_tableWidget->setFocus();
            return false;
        }
#endif
    }
    return true;
}

void EditorWidget::setModified(bool b) {
    m_isModified = b;
}

void EditorWidget::moveRowUp()
{
    int currentRow = m_tableWidget->currentRow();
    int currentColumn = m_tableWidget->currentColumn();
    if (currentRow  < 1) {
        return;
    }

    m_tableWidget->insertRow(currentRow - 1);

    for (int i = 0; i < m_tableWidget->columnCount(); ++i) {
        m_tableWidget->setItem(currentRow - 1, i,
            m_tableWidget->takeItem(currentRow + 1, i));
    }

    m_tableWidget->setCurrentCell(currentRow -1, currentColumn);
    m_tableWidget->removeRow(currentRow+1);
}

void EditorWidget::moveRowDown()
{
    int currentRow = m_tableWidget->currentRow();
    int currentColumn = m_tableWidget->currentColumn();
    if (currentRow == -1 || currentRow == m_tableWidget->rowCount() - 1) {
        return;
    }
    m_tableWidget->insertRow(currentRow + 2);

    for (int i = 0; i < m_tableWidget->columnCount(); ++i) {
        m_tableWidget->setItem(currentRow + 2, i,
            m_tableWidget->takeItem(currentRow, i));
    }

    m_tableWidget->setCurrentCell(currentRow + 2, currentColumn);
    m_tableWidget->removeRow(currentRow);
}

void EditorWidget::targetChanged(QAbstractButton *button)
{
    if (button == m_RPropRadio)
        m_repoLabel->setText("Category ID");
    else
        m_repoLabel->setText("Repository ID");
}

QCrmlGenerator::QCrmlGenerator()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    newAction = new QAction(tr("&New"), this);
    connect(newAction, SIGNAL(triggered()) , this, SLOT(newFile()));

    openAction = new QAction(tr("&Open"), this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    saveAction = new QAction(tr("&Save"), this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    saveAsAction = new QAction(tr("Save &As..."), this);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    exitAction = new QAction(tr("&Exit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    m_editorWidget = new EditorWidget;
    setCentralWidget(m_editorWidget);
}

void QCrmlGenerator::newFile() {

    if (safeToClear()) {
        m_editorWidget->initNew();
        m_saveFile.clear();
    }
}

void QCrmlGenerator::openFile() {
    if (safeToClear()) {
        QFileDialog openDialog(this, tr("Open"), QString(), tr("QCrml (*.qcrml);;Any Files(*)"));
        openDialog.setFileMode(QFileDialog::ExistingFile);
        openDialog.setAcceptMode(QFileDialog::AcceptOpen);
        if (openDialog.exec() == QDialog::Accepted) {
            if (openDialog.selectedFiles().count() > 0) {
                m_saveFile = openDialog.selectedFiles().at(0);
                m_editorWidget->open(m_saveFile);
            }
        }
    }
}

void QCrmlGenerator::saveFile()
{
    if (m_saveFile.isEmpty()) {
        saveFileAs();
    }
    else {
        if (m_editorWidget->verifyContents())
            m_editorWidget->save(m_saveFile);
    }
}

void QCrmlGenerator::saveFileAs()
{
    if (m_editorWidget->verifyContents()) {
        QFileDialog saveDialog(this, tr("Save As"), QString(), tr("QCrml (*.qcrml);;Any Files (*)"));
        saveDialog.setDefaultSuffix("qcrml");
        saveDialog.setFileMode(QFileDialog::AnyFile);
        saveDialog.setAcceptMode(QFileDialog::AcceptSave);
        if (saveDialog.exec() == QDialog::Accepted) {
            if (saveDialog.selectedFiles().count() > 0) {
            m_saveFile = saveDialog.selectedFiles().at(0);
            m_editorWidget->save(m_saveFile);
            }
        }
    }
}

void QCrmlGenerator::closeEvent(QCloseEvent *event)
{
    if (safeToClear())
        event->accept();
    else
        event->ignore();
}

bool QCrmlGenerator::safeToClear()
{
    if (m_editorWidget->isModified()) {
        int ret = QMessageBox::warning(this, tr("Save changes?"),
                tr("Modifications have been made. Do you want to save your changes? "),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
        switch (ret) {
            case QMessageBox::Save:
                saveFile();
                return true;
            case QMessageBox::Discard:
                return true;
            default:
                return false;
        }
    }
    return true;
}
