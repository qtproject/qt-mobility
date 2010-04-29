/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "attachmentlistwidget.h"
#include <QStringListModel>
#include <QListView>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QTreeView>
#include <QFileInfo>
#include <QItemDelegate>
#include <QPainter>
#include <QPointer>
#include <QMouseEvent>
#include <QHeaderView>
//#include <support/qmailnamespace.h>

class AttachmentListWidget;

static QString sizeString(uint size)
{
    if(size < 1024)
        return QObject::tr("%n byte(s)", "", size);
    else if(size < (1024 * 1024))
        return QObject::tr("%1 KB").arg(((float)size)/1024.0, 0, 'f', 1);
    else if(size < (1024 * 1024 * 1024))
        return QObject::tr("%1 MB").arg(((float)size)/(1024.0 * 1024.0), 0, 'f', 1);
    else
        return QObject::tr("%1 GB").arg(((float)size)/(1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
}

static QStringList headers(QStringList() << "Attachment" << "Size" << "Type" << "");

class AttachmentListHeader : public QHeaderView
{
    Q_OBJECT
public:
    AttachmentListHeader(AttachmentListWidget* parent);

signals:
    void clear();

protected:
    void paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const;
#ifndef QT_NO_CURSOR    
    bool viewportEvent(QEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
#endif    
    void mousePressEvent(QMouseEvent* e);
    bool overRemoveLink(QMouseEvent* e);


private:
    AttachmentListWidget* m_parent;
    mutable QRect m_removeButtonRect;

};

AttachmentListHeader::AttachmentListHeader(AttachmentListWidget* parent)
:
QHeaderView(Qt::Horizontal,parent),
m_parent(parent)
{
}

void AttachmentListHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    if(logicalIndex == 3 && m_parent->attachments().count() > 1)
    {
        painter->save();
        QFont font = painter->font();
        font.setUnderline(true);
        painter->setFont(font);
        painter->drawText(rect,Qt::AlignHCenter | Qt::AlignVCenter,"Remove All",&m_removeButtonRect);
        painter->restore();
    }
    else
        QHeaderView::paintSection(painter,rect,logicalIndex);
}

#ifndef QT_NO_CURSOR    
bool AttachmentListHeader::viewportEvent(QEvent* e)
{
    if(e->type() == QEvent::Leave)
        setCursor(QCursor());
    return QAbstractItemView::viewportEvent(e);
}

void AttachmentListHeader::mouseMoveEvent(QMouseEvent* e)
{
    QHeaderView::mouseMoveEvent(e);
    if(overRemoveLink(e))
    {
        QCursor handCursor(Qt::PointingHandCursor);
        setCursor(handCursor);
    }
    else if(cursor().shape() == Qt::PointingHandCursor)
        setCursor(QCursor());
}
#endif

void AttachmentListHeader::mousePressEvent(QMouseEvent* e)
{
    if(overRemoveLink(e))
        emit clear();
    QHeaderView::mousePressEvent(e);
}

bool AttachmentListHeader::overRemoveLink(QMouseEvent* e)
{
    return m_removeButtonRect.contains(e->pos());
}

class AttachmentListDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    AttachmentListDelegate(AttachmentListWidget* parent = 0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    bool isOverRemoveLink(const QRect& parentRect, const QPoint& pos) const;

protected slots:
    bool helpEvent(QHelpEvent * event, QAbstractItemView * view, const QStyleOptionViewItem & option, const QModelIndex & index);

private:
    QPointer<AttachmentListWidget> m_parent;
};

AttachmentListDelegate::AttachmentListDelegate(AttachmentListWidget* parent)
:
QItemDelegate(parent),
m_parent(parent)
{
}

void AttachmentListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if(index.isValid() && index.column() == 3)
    {
        painter->save();
        QFont font = painter->font();
        QColor c = option.palette.brush(QPalette::Link).color();
        font.setUnderline(true);
        painter->setPen(c);
        painter->setFont(font);
        painter->drawText(option.rect,Qt::AlignHCenter,"Remove");
        painter->restore();
    }
    else
        QItemDelegate::paint(painter,option,index);
}

bool AttachmentListDelegate::isOverRemoveLink(const QRect& parentRect, const QPoint& pos) const
{
    QFont font;
    font.setUnderline(true);
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(parentRect,Qt::AlignHCenter,"Remove");
    return textRect.contains(pos);
}

bool AttachmentListDelegate::helpEvent(QHelpEvent *, QAbstractItemView *view, const QStyleOptionViewItem &, const QModelIndex &index)
{
    if (!index.isValid()) {
        view->setToolTip(QString());
        return false;
    }

    QString attachment = m_parent->attachmentAt(index.row());
    view->setToolTip(attachment);
    return false;
}

class AttachmentListView : public QTreeView
{
    Q_OBJECT

public:
    AttachmentListView(QWidget* parent = 0);

signals:
    void removeAttachmentAtIndex(int index);

protected:
#ifndef QT_NO_CURSOR    
    bool viewportEvent(QEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
#endif    
    void mousePressEvent(QMouseEvent* e);
    bool overRemoveLink(QMouseEvent* e);
};

AttachmentListView::AttachmentListView(QWidget* parent)
:
QTreeView(parent)
{
    setMouseTracking(true);
    installEventFilter(this);
}

#ifndef QT_NO_CURSOR    
bool AttachmentListView::viewportEvent(QEvent* e)
{
    if(e->type() == QEvent::Leave)
        setCursor(QCursor());
    return QAbstractItemView::viewportEvent(e);
}

void AttachmentListView::mouseMoveEvent(QMouseEvent* e)
{
    if(overRemoveLink(e))
    {
        QCursor handCursor(Qt::PointingHandCursor);
        setCursor(handCursor);
    }
    else if(cursor().shape() == Qt::PointingHandCursor)
        setCursor(QCursor());
    QTreeView::mouseMoveEvent(e);
}
#endif

void AttachmentListView::mousePressEvent(QMouseEvent* e)
{
    if(overRemoveLink(e))
    {
        QModelIndex index = indexAt(e->pos());
        emit removeAttachmentAtIndex(index.row());
    }
    QTreeView::mousePressEvent(e);
}

bool AttachmentListView::overRemoveLink(QMouseEvent* e)
{
    QModelIndex index = indexAt(e->pos());
    if(index.isValid() && index.column() == 3)
    {
        AttachmentListDelegate* delegate = static_cast<AttachmentListDelegate*>(itemDelegate());
        return delegate->isOverRemoveLink(visualRect(index),e->pos());
    }
    return false;
}

class AttachmentListModel : public QAbstractListModel
{
public:
    AttachmentListModel(QWidget* parent );
    QVariant headerData(int section,Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    bool isEmpty() const;

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    QStringList attachments() const;
    void setAttachments(const QStringList& attachments);

private:
    QStringList m_attachments;
};

AttachmentListModel::AttachmentListModel(QWidget* parent)
:
QAbstractListModel(parent)
{
}

QVariant AttachmentListModel::headerData(int section, Qt::Orientation o, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if(section < headers.count())
            return headers.at(section);
    }

   return QAbstractListModel::headerData(section,o,role);
}

bool AttachmentListModel::isEmpty() const
{
    return m_attachments.isEmpty();
}

int AttachmentListModel::columnCount(const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return headers.count();
}

int AttachmentListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_attachments.count();
}

QVariant AttachmentListModel::data( const QModelIndex & index, int role) const
{
    if(index.isValid())
    {
        if(role == Qt::DisplayRole && index.isValid())
        {
            QString path = m_attachments.at(index.row());
            QFileInfo fi(path);

            switch(index.column())
            {
            case 0:
                return fi.fileName();
                break;
            case 1:
                return sizeString(fi.size());
                break;
            case 2:
                //QString mimeType = QMail::mimeTypeFromFileName(path);
                //if(mimeType.isEmpty()) mimeType = "Unknown";
                //return mimeType;
                return "Unknown";
                break;
            }
        }
        else if((role == Qt::DecorationRole  || role == Qt::CheckStateRole )&& index.column() > 0)
            return QVariant();
            else if(role == Qt::DecorationRole)
        {
            static QIcon attachIcon( ":icon/attach" );
            return attachIcon;
        }
    }
    return QVariant();
}

QStringList AttachmentListModel::attachments() const
{
    return m_attachments;
}

void AttachmentListModel::setAttachments(const QStringList& attachments)
{
    m_attachments.clear();

    foreach (const QString &att, attachments) {
        if (!att.startsWith("ref:") && !att.startsWith("partRef:")) {
            m_attachments.append(att);
        }
    }

    reset();
}

AttachmentListWidget::AttachmentListWidget(QWidget* parent)
:
QWidget(parent),
m_listView(new AttachmentListView(this)),
m_model(new AttachmentListModel(this)),
m_delegate(new AttachmentListDelegate(this)),
m_clearLink(new QLabel(this))
{
    m_clearLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    m_clearLink->setTextFormat(Qt::RichText);

    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::NoSelection);
    AttachmentListHeader* header = new AttachmentListHeader(this);
    connect(header,SIGNAL(clear()),this,SLOT(clearClicked()));
    m_listView->setHeader(header);
    m_listView->header()->setStretchLastSection(true);
    m_listView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_listView->header()->setDefaultSectionSize(180);
    m_listView->setUniformRowHeights(true);
    m_listView->setRootIsDecorated(false);
    m_listView->setItemDelegate(m_delegate);


    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_listView);

    connect(m_clearLink,SIGNAL(linkActivated(QString)),this,SLOT(clearClicked()));
    connect(m_listView,SIGNAL(removeAttachmentAtIndex(int)),this,SLOT(removeAttachmentAtIndex(int)));
}

QStringList AttachmentListWidget::attachments() const
{
    return m_attachments;
}

QString AttachmentListWidget::attachmentAt(int index) const
{
    return m_attachments.at(index);
}

int AttachmentListWidget::count() const
{
    return m_attachments.count();
}

bool AttachmentListWidget::isEmpty() const
{
    return m_attachments.isEmpty();
}

void AttachmentListWidget::addAttachment(const QString& attachment)
{
    if(m_attachments.contains(attachment))
        return;

    m_attachments.append(attachment);

    m_model->setAttachments(m_attachments);
    setVisible(!m_model->isEmpty());

    emit attachmentsAdded(QStringList() << attachment);
}

void AttachmentListWidget::addAttachments(const QStringList& attachments)
{
    QSet<QString> newAttachments = attachments.toSet() - m_attachments.toSet();

    if (!newAttachments.isEmpty()) {
        m_attachments += newAttachments.toList();

        m_model->setAttachments(m_attachments);
        setVisible(!m_model->isEmpty());

        emit attachmentsAdded(newAttachments.toList());
    }
}

void AttachmentListWidget::removeAttachment(const QString& attachment)
{
    if (!m_attachments.contains(attachment))
        return;

    m_attachments.removeAll(attachment);

    m_model->setAttachments(m_attachments);
    setVisible(!m_model->isEmpty());

    emit attachmentsRemoved(attachment);
}

void AttachmentListWidget::clear()
{
    m_attachments.clear();
    m_model->setAttachments(m_attachments);
    setVisible(false);
}

void AttachmentListWidget::clearClicked()
{
    if(QMessageBox::question(this,
                             "Remove attachments",
                             QString("Remove %1 attachments?").arg(m_attachments.count()),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        clear();
}

void AttachmentListWidget::removeAttachmentAtIndex(int index)
{
    if(index >= m_attachments.count())
        return;

    QString attachment = m_attachments.at(index);
    m_attachments.removeAt(index);

    m_model->setAttachments(m_attachments);
    setVisible(!m_model->isEmpty());

    emit attachmentsRemoved(attachment);
}

#include <attachmentlistwidget.moc>

