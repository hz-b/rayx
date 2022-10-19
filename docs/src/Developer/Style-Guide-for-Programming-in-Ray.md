# Style Guide

Here you can find a collection of rules for our code base, which developed over the projects lifetime. There are here to make it easier for others on the team, to read your code. Please read them carefully.

## General

Keep collective ownership of the code in mind, when writing it (write code to be read by your team mates, not the compiler). 

So keep it stupid simple. In most places it is not necessary or sensible to optimise the runtime of code to the last CPU cycle. Often the compiler does it better than the programmer anyways.

## Includes
As an easy step towards readability we divide includes in the following way: 

`#include "internalHeader.h"`

`#include <externalHeader.h>`

Where internal headers are the ones we wrote ourselves and external headers are from dependencies we included into our project.


## Comments

Write comments often and as precise as possible. Comments should contain what you would say to a newcomer to the project, because that is most likely the person that reads/needs the comments.

### Doxygen

```c++
/**
* a desctription (should be helpful)
* @param value1 	what the parameter value1 does
* @param value2 	what the parameter value2 does
* @see related function
* @return what the function returns
*/
```

This is how a comment before a function should look. It contains a brief description of it and of the parameters and return values of it. It also points to related functions, if there are any.

## Naming scheme

As longer names can contain more than one word, it can help to have a visual divider. In the case of Ray-UI we use "camelCase" and "PascalCase". When to use what, will be in the next section. 

The upper case letter indicates a new word and thus improves readability. Every name should be able to stand alone and describe the object, function or variable. Something like "int v;" does not achieve this. This also means only using common abbreviations, like "val" for "value" or "dx" for a distance over x.

Avoid using symbols or variable names from formulas for naming variables in the code. This makes the code a riddle to decipher, without further information.
The correct ways to do this:
 - Add a wiki entry for the formula and link to it
 - Change the variable names to its purpose or type, e.g. "hypotenuse" instead of "c"
 - Add a link to the formula, so other developers can find it (The other options are better, but at least do this)

Should the name get too long or cryptic, write a comment to clarify what you mean with the name you chose for this variable.

### Boolean

All Boolean values should begin with is/can/has/etc. when possible.

## Classes, Function and Variables

Class and object names are written in "PascalCase". Functions and Variables are written in "camelCase".

e.g.:

- "ClassA"
- "functionB(int valueC, bool isD)"

## Member

Member objects and variables of a class are indicated by an "m_", e.g. : 

- m_MemberObject 
- m_memberVariable

## Static

A static variable or object is indicated by an "s_", e.g. :

- s_StaticObject
- s_staticVariable



## Const Correctness

Const correctness means using the keyword `const`, to avoid const objects from getting mutated.

1. `void f1(const std::string& s);     ` *// Pass by reference-to-`const`*
2. `void f2(const std::string* sptr);  ` *// Pass by pointer-to-`const`*
3. `void f3(std::string s);            ` *// Pass by value*

In the first case a reference is passed to the function with the keyword const. This means the function won't change the value of the object or variable. The same can be done for pointers. Another option is to pass by value. This creates a copy of an object or variable, so the original isn't changed even if the function changes the copy. 

Add const keywords as early as possible. Const correctness can create a snow ball effect and is easier implemented from the beginning. 

Further reading can and should be done here: https://isocpp.org/wiki/faq/const-correctness