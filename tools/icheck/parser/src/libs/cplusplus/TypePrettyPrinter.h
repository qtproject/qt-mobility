/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TYPEPRETTYPRINTER_H
#define TYPEPRETTYPRINTER_H

#include <TypeVisitor.h>
#include <FullySpecifiedType.h>
#include <QString>
#include <QList>

namespace CPlusPlus {

class Overview;
class FullySpecifiedType;

class CPLUSPLUS_EXPORT TypePrettyPrinter: protected TypeVisitor
{
public:
    TypePrettyPrinter(const Overview *overview);
    virtual ~TypePrettyPrinter();

    const Overview *overview() const;

    QString operator()(const FullySpecifiedType &type);
    QString operator()(const FullySpecifiedType &type, const QString &name);

protected:
    QString switchText(const QString &text = QString());
    QList<FullySpecifiedType> switchPtrOperators(const QList<FullySpecifiedType> &ptrOperators);
    QString switchName(const QString &name);

    void applyPtrOperators(bool wantSpace = true);
    void acceptType(const FullySpecifiedType &ty);

    virtual void visit(UndefinedType *type);
    virtual void visit(VoidType *type);
    virtual void visit(IntegerType *type);
    virtual void visit(FloatType *type);
    virtual void visit(PointerToMemberType *type);
    virtual void visit(PointerType *type);
    virtual void visit(ReferenceType *type);
    virtual void visit(ArrayType *type);
    virtual void visit(NamedType *type);
    virtual void visit(Function *type);
    virtual void visit(Namespace *type);
    virtual void visit(Class *type);
    virtual void visit(Enum *type);

    void space();
    void outPlain(const QString &text);
    void out(const QString &text);
    void out(const QChar &ch);
    void outCV(const FullySpecifiedType &ty);

private:
    const Overview *_overview;
    QString _name;
    QString _text;
    FullySpecifiedType _fullySpecifiedType;
    QList<FullySpecifiedType> _ptrOperators;
};

} // end of namespace CPlusPlus

#endif // TYPEPRETTYPRINTER_H
