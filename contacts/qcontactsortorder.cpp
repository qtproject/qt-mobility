#include "qcontactsortorder.h"
#include "qcontactsortorder_p.h"

QContactSortOrder::QContactSortOrder() : d(new QContactSortOrderPrivate)
{
}

QContactSortOrder::~QContactSortOrder()
{
}

QContactSortOrder::QContactSortOrder(const QContactSortOrder& other)
    : d(other.d)
{

}
QContactSortOrder& QContactSortOrder::operator=(const QContactSortOrder& other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}


void QContactSortOrder::setType(QContactSortOrder::Type type)
{
    d->m_type = type;
}

void QContactSortOrder::setDetail(const QString& definitionId, const QString& fieldId)
{
    d->m_definitionId = definitionId;
    d->m_fieldId = fieldId;
}

void QContactSortOrder::setBlankPolicy(BlankPolicy blankPolicy)
{
    d->m_blankPolicy = blankPolicy;
}

void QContactSortOrder::setDirection(Qt::SortOrder direction)
{
    d->m_direction = direction;
}

QContactSortOrder::Type QContactSortOrder::type() const
{
    return d->m_type;
}

QString QContactSortOrder::detailDefinitionId() const
{
    return d->m_definitionId;
}

QString QContactSortOrder::detailFieldId() const
{
    return d->m_fieldId;
}

QContactSortOrder::BlankPolicy QContactSortOrder::blankPolicy() const
{
    return d->m_blankPolicy;
}

Qt::SortOrder QContactSortOrder::direction() const
{
    return d->m_direction;
}
