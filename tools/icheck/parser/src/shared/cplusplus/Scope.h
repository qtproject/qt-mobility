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

#ifndef CPLUSPLUS_SCOPE_H
#define CPLUSPLUS_SCOPE_H

#include "CPlusPlusForwardDeclarations.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT Scope
{
    Scope(const Scope &other);
    void operator =(const Scope &other);

public:
    typedef Symbol **iterator;

public:
    /// Constructs an empty Scope.
    Scope(ScopedSymbol *owner = 0);

    /// Destroy this scope.
    ~Scope();

    /// Returns this scope's owner Symbol.
    ScopedSymbol *owner() const;

    /// Sets this scope's owner Symbol.
    void setOwner(ScopedSymbol *owner); // ### remove me

    /// Returns the enclosing scope.
    Scope *enclosingScope() const;

    /// Returns the eclosing namespace scope.
    Scope *enclosingNamespaceScope() const;

    /// Returns the enclosing class scope.
    Scope *enclosingClassScope() const;

    /// Returns the enclosing enum scope.
    Scope *enclosingEnumScope() const;

    /// Rerturns the enclosing function scope.
    Scope *enclosingFunctionScope() const;

    /// Rerturns the enclosing Block scope.
    Scope *enclosingBlockScope() const;

    /// Returns true if this scope's owner is a Namespace Symbol.
    bool isNamespaceScope() const;

    /// Returns true if this scope's owner is a Class Symbol.
    bool isClassScope() const;

    /// Returns true if this scope's owner is an Enum Symbol.
    bool isEnumScope() const;

    /// Returns true if this scope's owner is a Block Symbol.
    bool isBlockScope() const;

    /// Returns true if this scope's owner is a Function Symbol.
    bool isFunctionScope() const;

    /// Returns true if this scope's owner is a Prototype Symbol.
    bool isPrototypeScope() const;

    /// Returns true if this scope's owner is an ObjCClass Symbol.
    bool isObjCClassScope() const;

    /// Returns true if this scope's owner is an ObjCMethod symbol.
    bool isObjCMethodScope() const;

    /// Adds a Symbol to this Scope.
    void enterSymbol(Symbol *symbol);

    /// Returns true if this Scope is empty; otherwise returns false.
    bool isEmpty() const;

    /// Returns the number of symbols is in the scope.
    unsigned symbolCount() const;

    /// Returns the Symbol at the given position.
    Symbol *symbolAt(unsigned index) const;

    /// Returns the first Symbol in the scope.
    iterator firstSymbol() const;

    /// Returns the last Symbol in the scope.
    iterator lastSymbol() const;

    Symbol *lookat(const Name *name) const;
    Symbol *lookat(const Identifier *id) const;
    Symbol *lookat(int operatorId) const;

private:
    /// Returns the hash value for the given Symbol.
    unsigned hashValue(Symbol *symbol) const;

    /// Updates the hash table.
    void rehash();

private:
    enum { DefaultInitialSize = 11 };

    ScopedSymbol *_owner;

    Symbol **_symbols;
    int _allocatedSymbols;
    int _symbolCount;

    Symbol **_hash;
    int _hashSize;
};

} // end of namespace CPlusPlus


#endif // CPLUSPLUS_SCOPE_H
