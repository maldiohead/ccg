/* C Code Generator
 *
 * Copyright (C) 2012, Antoine Balestrat <merkil@savhon.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "ccg.h"

void addVariableToList(Variable *variable, VariableList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(*list));
        (*list)->variable = variable;
        (*list)->next = NULL;
    }

    else
    {
        VariableList *v;

        for(v = *list; v->next; v = v->next);
        v->next = xmalloc(sizeof(v->next));
        v->next->variable = variable;
        v->next->next = NULL;
    }
}

Variable *makeVariable(VariableList *scope, VariableType type)
{
    Variable *ret = xmalloc(sizeof(Variable));

    if(type != _randomvartype)
    {
        ret->type = type;

        if(type == _integer)
            makeInteger(ret, scope);
        else
            makePointer(ret, scope);
    }

    else
    {
        if((ret->type = rand() % _vartypemax) == _integer)
            makeInteger(ret, scope);
        else if(ret->type == _pointer)
            makePointer(ret, scope);
    }

    return ret;
}

void printVariableDecl(Variable *var)
{
    if(var->type == _integer)
        printIntegerDecl(var);
    else
        printPointerDecl(var);
}

void printVariableType(Variable *var)
{
    printf("%s", inttype2str[ultimateType(var)]);

    if(var->type == _pointer)
    {
        size_t n = pointerDepth(var);

        for(size_t i = 0; i < n; ++i)
            putchar('*');
    }
}

void copyVariableList(VariableList *src, VariableList **dest)
{
    VariableList *v;

    foreach(v, src)
    {
        Variable *var = xmalloc(sizeof(*var));

        /* OK. We make a copy of the variable, but no need to duplicate its name */
        memcpy(var, v->variable, sizeof(*var));

        /* Then we add it to the destination list. So much allocated memory without a single free... FIXME. */
        addVariableToList(var, dest);
    }
}

size_t numVariablesInScope(VariableList *scope)
{
    size_t i = 0;
    VariableList *v;

    foreach(v, scope) ++i;

    return i;
}

/* Pick a variable from the `scope' which is of type `type' */
Variable *selectVariable(VariableList *scope, VariableType type)
{
    Variable *ret;
    VariableList *v;
    size_t n = numVariablesInScope(scope);

    do
    {
        size_t i = 0, t = rand() % n + 1;

        foreach(v, scope)
        {
            if(++i == t)
                ret = v->variable;
        }
    } while((type == _randomvartype ? 0 : type != ret->type));

    return ret;
}

void makeGlobalVariables(void)
{
    size_t i;

    program.numglobalvars = rand() % 10 + 1;

    for(i = 0; i < program.numglobalvars; ++i)
        addVariableToList(makeVariable(program.globalvars, _integer), &program.globalvars);
}