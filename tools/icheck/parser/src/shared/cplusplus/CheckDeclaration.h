/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef CPLUSPLUS_CHECKDECLARATION_H
#define CPLUSPLUS_CHECKDECLARATION_H

#include "CPlusPlusForwardDeclarations.h"
#include "SemanticCheck.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT CheckDeclaration: public SemanticCheck
{
public:
    CheckDeclaration(Semantic *semantic);
    virtual ~CheckDeclaration();

    void check(DeclarationAST *declaration, Scope *scope, TemplateParameters *templateParameters);

protected:
    DeclarationAST *switchDeclaration(DeclarationAST *declaration);
    Scope *switchScope(Scope *scope);
    TemplateParameters *switchTemplateParameters(TemplateParameters *templateParameters);

    void checkFunctionArguments(Function *fun);

    using ASTVisitor::visit;

    unsigned locationOfDeclaratorId(DeclaratorAST *declarator) const;

    virtual bool visit(SimpleDeclarationAST *ast);
    virtual bool visit(EmptyDeclarationAST *ast);
    virtual bool visit(AccessDeclarationAST *ast);
#ifdef ICHECK_BUILD
    virtual bool visit(QPropertyDeclarationAST *ast);
    virtual bool visit(QEnumDeclarationAST *ast);
    virtual bool visit(QFlagsDeclarationAST *ast);
    virtual bool visit(QDeclareFlagsDeclarationAST *ast);
#endif
    virtual bool visit(AsmDefinitionAST *ast);
    virtual bool visit(ExceptionDeclarationAST *ast);
    virtual bool visit(FunctionDefinitionAST *ast);
    virtual bool visit(LinkageBodyAST *ast);
    virtual bool visit(LinkageSpecificationAST *ast);
    virtual bool visit(NamespaceAST *ast);
    virtual bool visit(NamespaceAliasDefinitionAST *ast);
    virtual bool visit(ParameterDeclarationAST *ast);
    virtual bool visit(TemplateDeclarationAST *ast);
    virtual bool visit(TypenameTypeParameterAST *ast);
    virtual bool visit(TemplateTypeParameterAST *ast);
    virtual bool visit(UsingAST *ast);
    virtual bool visit(UsingDirectiveAST *ast);
    virtual bool visit(MemInitializerAST *ast);

    virtual bool visit(ObjCProtocolDeclarationAST *ast);
    virtual bool visit(ObjCProtocolForwardDeclarationAST *ast);
    virtual bool visit(ObjCClassDeclarationAST *ast);
    virtual bool visit(ObjCClassForwardDeclarationAST *ast);
    virtual bool visit(ObjCMethodDeclarationAST *ast);
    virtual bool visit(ObjCVisibilityDeclarationAST *ast);
    virtual bool visit(ObjCPropertyDeclarationAST *ast);

private:
    bool checkPropertyAttribute(ObjCPropertyAttributeAST *attrAst,
                                int &flags,
                                int attr);
private:
    DeclarationAST *_declaration;
    Scope *_scope;
    TemplateParameters *_templateParameters;
    bool _checkAnonymousArguments: 1;
};

} // end of namespace CPlusPlus


#endif // CPLUSPLUS_CHECKDECLARATION_H
