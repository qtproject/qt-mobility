#include <QMessageBox>

#include "categoryadddialog.h"

CategoryAddDialog::CategoryAddDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    setupUi(this);
}

CategoryAddDialog::~CategoryAddDialog()
{
}

QLandmarkCategory CategoryAddDialog::category()
{
    return m_category;
}

void CategoryAddDialog::accept()
{

    m_category.clear();
    QString name = nameLineEdit->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Invalid Name", "Category name must not be empty",
                             QMessageBox::Ok, QMessageBox::NoButton);
    }
    m_category.setName(nameLineEdit->text());
    QDialog::accept();
}

void CategoryAddDialog::reject()
{
    m_category.clear();
    QDialog::reject();
}
