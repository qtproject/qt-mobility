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

#ifndef CPLUSPLUS_RESOLVEEXPRESSION_H
#define CPLUSPLUS_RESOLVEEXPRESSION_H

#include "LookupContext.h"

#include <ASTVisitor.h>
#include <Semantic.h>
#include <FullySpecifiedType.h>

namespace CPlusPlus {

class CPLUSPLUS_EXPORT ResolveExpression: protected ASTVisitor
{
public:
    ResolveExpression(const LookupContext &context);
    virtual ~ResolveExpression();

    QList<LookupItem> operator()(ExpressionAST *ast);

    QList<LookupItem> resolveMemberExpression(const QList<LookupItem> &baseResults,
                                          unsigned accessOp,
                                          const Name *memberName,
                                          bool *replacedDotOperator = 0) const;

    QList<LookupItem> resolveBaseExpression(const QList<LookupItem> &baseResults,
                                        int accessOp,
                                        bool *replacedDotOperator = 0) const;

    QList<LookupItem> resolveMember(const Name *memberName, Class *klass,
                                    const Name *className = 0) const;

    QList<LookupItem> resolveMember(const Name *memberName, ObjCClass *klass) const;

protected:
    QList<LookupItem> switchResults(const QList<LookupItem> &symbols);

    void addResult(const FullySpecifiedType &ty, Symbol *symbol = 0);
    void addResult(const LookupItem &result);
    void addResults(const QList<LookupItem> &results);

    bool maybeValidPrototype(Function *funTy, unsigned actualArgumentCount) const;

    using ASTVisitor::visit;

    virtual bool visit(BinaryExpressionAST *ast);
    virtual bool visit(CastExpressionAST *ast);
    virtual bool visit(ConditionAST *ast);
    virtual bool visit(ConditionalExpressionAST *ast);
    virtual bool visit(CppCastExpressionAST *ast);
    virtual bool visit(DeleteExpressionAST *ast);
    virtual bool visit(ArrayInitializerAST *ast);
    virtual bool visit(NewExpressionAST *ast);
    virtual bool visit(TypeidExpressionAST *ast);
    virtual bool visit(TypenameCallExpressionAST *ast);
    virtual bool visit(TypeConstructorCallAST *ast);
    virtual bool visit(PostfixExpressionAST *ast);
    virtual bool visit(SizeofExpressionAST *ast);
    virtual bool visit(NumericLiteralAST *ast);
    virtual bool visit(BoolLiteralAST *ast);
    virtual bool visit(ThisExpressionAST *ast);
    virtual bool visit(NestedExpressionAST *ast);
    virtual bool visit(StringLiteralAST *ast);
    virtual bool visit(ThrowExpressionAST *ast);
    virtual bool visit(TypeIdAST *ast);
    virtual bool visit(UnaryExpressionAST *ast);
    virtual bool visit(CompoundLiteralAST *ast);

    //names
    virtual bool visit(QualifiedNameAST *ast);
    virtual bool visit(OperatorFunctionIdAST *ast);
    virtual bool visit(ConversionFunctionIdAST *ast);
    virtual bool visit(SimpleNameAST *ast);
    virtual bool visit(DestructorNameAST *ast);
    virtual bool visit(TemplateIdAST *ast);

    // postfix expressions
    virtual bool visit(CallAST *ast);
    virtual bool visit(ArrayAccessAST *ast);
    virtual bool visit(PostIncrDecrAST *ast);
    virtual bool visit(MemberAccessAST *ast);

    // Objective-C expressions
    virtual bool visit(ObjCMessageExpressionAST *ast);

    QList<Scope *> visibleScopes(const LookupItem &result) const;

private:
    LookupContext _context;
    Semantic sem;
    QList<LookupItem> _results;
    Symbol *_declSymbol;
};

class CPLUSPLUS_EXPORT ResolveClass
{
public:
    ResolveClass();

    QList<Symbol *> operator()(const Name *name,
                               const LookupItem &p,
                               const LookupContext &context);

private:
    QList<Symbol *> resolveClass(const Name *name,
                                 const LookupItem &p,
                                 const LookupContext &context);

private:
    QList<LookupItem> _blackList;
};

class CPLUSPLUS_EXPORT ResolveObjCClass
{
public:
    ResolveObjCClass();

    QList<Symbol *> operator()(const Name *name,
                               const LookupItem &p,
                               const LookupContext &context);
};


} // end of namespace CPlusPlus

#endif // CPLUSPLUS_RESOLVEEXPRESSION_H
