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

#ifndef CPLUSPLUS_SEMANTIC_H
#define CPLUSPLUS_SEMANTIC_H

#include "CPlusPlusForwardDeclarations.h"
#include "ASTfwd.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT Semantic
{
    Semantic(const Semantic &other);
    void operator =(const Semantic &other);

public:
    Semantic(TranslationUnit *translationUnit);
    virtual ~Semantic();

    TranslationUnit *translationUnit() const;
    Control *control() const;

    FullySpecifiedType check(SpecifierListAST *specifier, Scope *scope);

    FullySpecifiedType check(DeclaratorAST *declarator, const FullySpecifiedType &type,
                             Scope *scope, const Name **name = 0); // ### ugly

    FullySpecifiedType check(PtrOperatorListAST *ptrOperators, const FullySpecifiedType &type,
                             Scope *scope);

    FullySpecifiedType check(ObjCMethodPrototypeAST *methodPrototype, Scope *scope);

    FullySpecifiedType check(ExpressionAST *expression, Scope *scope);

    void check(DeclarationAST *declaration, Scope *scope, TemplateParameters *templateParameters = 0);

    void check(StatementAST *statement, Scope *scope);

    const Name *check(NameAST *name, Scope *scope);

    const Name *check(NestedNameSpecifierListAST *name, Scope *scope);

    const Name *check(ObjCSelectorAST *args, Scope *scope);
    FullySpecifiedType check(ObjCTypeNameAST *typeName, Scope *scope);

    void check(ObjCMessageArgumentDeclarationAST *arg, Scope *scope);

    bool skipFunctionBodies() const;
    void setSkipFunctionBodies(bool skipFunctionBodies);

    int currentVisibility() const;
    int switchVisibility(int visibility);

    int currentObjCVisibility() const;
    int switchObjCVisibility(int visibility);

    int currentMethodKey() const;
    int switchMethodKey(int methodKey);

    int visibilityForClassKey(int tokenKind) const;
    int visibilityForAccessSpecifier(int tokenKind) const;
    int visibilityForObjCAccessSpecifier(int tokenKind) const;
    bool isObjCClassMethod(int tokenKind) const;

private:
    class Data;
    friend class Data;
    Data *d;
};

} // end of namespace CPlusPlus


#endif // CPLUSPLUS_SEMANTIC_H
