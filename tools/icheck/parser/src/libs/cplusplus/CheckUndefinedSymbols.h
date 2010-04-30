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

#ifndef CHECKUNDEFINEDSYMBOLS_H
#define CHECKUNDEFINEDSYMBOLS_H

#include "CppDocument.h"
#include "CppBindings.h"

#include <ASTVisitor.h>
#include <QtCore/QSet>
#include <QtCore/QByteArray>

namespace CPlusPlus {

class CPLUSPLUS_EXPORT CheckUndefinedSymbols: protected ASTVisitor
{
public:
    CheckUndefinedSymbols(Document::Ptr doc);
    virtual ~CheckUndefinedSymbols();

    void setGlobalNamespaceBinding(NamespaceBindingPtr globalNamespaceBinding);

    void operator()(AST *ast);

protected:
    using ASTVisitor::visit;

    bool isType(const Identifier *id) const;
    bool isType(const QByteArray &name) const;

    void addType(const Name *name);
    void buildTypeMap(Class *klass);
    void buildMemberTypeMap(Symbol *member);
    void buildTypeMap(NamespaceBinding *binding, QSet<NamespaceBinding *> *processed);
    void addProtocol(const Name *name);
    bool isProtocol(const QByteArray &name) const;

    FunctionDeclaratorAST *currentFunctionDeclarator() const;
    CompoundStatementAST *compoundStatement() const;
    bool qobjectCheck() const;

    QByteArray templateParameterName(NameAST *ast) const;
    QByteArray templateParameterName(DeclarationAST *ast) const;

    virtual bool visit(FunctionDeclaratorAST *ast);
    virtual void endVisit(FunctionDeclaratorAST *ast);

    virtual bool visit(TypeofSpecifierAST *ast);
    virtual bool visit(NamedTypeSpecifierAST *ast);

    virtual bool visit(TemplateDeclarationAST *ast);
    virtual void endVisit(TemplateDeclarationAST *);

    virtual bool visit(ClassSpecifierAST *ast);
    virtual void endVisit(ClassSpecifierAST *);

    virtual bool visit(FunctionDefinitionAST *ast);
    virtual void endVisit(FunctionDefinitionAST *ast);

    virtual bool visit(CompoundStatementAST *ast);
    virtual void endVisit(CompoundStatementAST *ast);

    virtual bool visit(SimpleDeclarationAST *ast);
    virtual bool visit(BaseSpecifierAST *base);
    virtual bool visit(UsingDirectiveAST *ast);
    virtual bool visit(QualifiedNameAST *ast);
    virtual bool visit(CastExpressionAST *ast);
    virtual bool visit(SizeofExpressionAST *ast);

    virtual bool visit(ObjCClassDeclarationAST *ast);
    virtual bool visit(ObjCProtocolRefsAST *ast);
    virtual bool visit(ObjCPropertyDeclarationAST *ast);

private:
    Document::Ptr _doc;
    NamespaceBindingPtr _globalNamespaceBinding;
    QList<bool> _qobjectStack;
    QList<FunctionDeclaratorAST *> _functionDeclaratorStack;
    QList<TemplateDeclarationAST *> _templateDeclarationStack;
    QList<CompoundStatementAST *> _compoundStatementStack;
    QSet<QByteArray> _types;
    QSet<QByteArray> _protocols;
    QSet<QByteArray> _namespaceNames;
};

} // end of namespace CPlusPlus

#endif // CHECKUNDEFINEDSYMBOLS_H
