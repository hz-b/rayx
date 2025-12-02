# Style Guide for Programming in RAYX

This document serves as a comprehensive style guide for the RAY project. It outlines the coding standards and best practices that have evolved over the course of the project's lifecycle. Adherence to these guidelines is crucial for maintaining code readability and facilitating effective collaboration among team members. Please read them attentively.

## General

Write code with collective ownership in mind; the primary audience is your teammates, not just the compiler. The KISS (Keep It Stupid Simple) principle should be applied whenever possible.

## Includes
To enhance readability, includes should be categorized as follows:

- `#include "internalHeader.h"`

- `#include <externalHeader.h>`

Internal headers are those developed within the project, while external headers pertain to dependencies integrated into the project.


## Comments

Prioritize frequent and precise commenting. Comments should be tailored to newcomers to the project, who are the most likely to require and benefit from them.

### Doxygen Comments

```c++
/**
* a desctription (don't just repeat the function name)
* @param value1 	what the parameter value1 does
* @param value2 	what the parameter value2 does
* @see related function
* @return what the function returns
*/
```

The objective is for header files to provide high-level documentation on API usage, while source files should contain more detailed documentation about implementation specifics.

## Naming Conventions

As longer names can contain more than one word, it can help to have a visual divider. In the case of Ray-UI we use "camelCase" and "PascalCase". When to use what, will be explained in the "Classes, Function and Variables" subsection. 

The upper case letter indicates a new word and thus improves readability. Every name should be able to stand alone and describe the object, function or variable. Something like "int v;" does not achieve this. This also means to avoid using abbreviations, besides the most common ones ("val" for "value" or "dx" for a distance over x).

Avoid using symbols or variable names from formulas for naming variables in the code. This makes the code a riddle to decipher, without further information.
The correct ways to do this:
 - Add a wiki entry for the formula and link to it
 - Change the variable names to its purpose or type, e.g. "hypotenuse" instead of "c"
 - Add a link to the formula, so other developers can find it (The other options are better, but at least do this)

Should the name get too long or cryptic, write a comment to clarify what you mean with the name you chose for this variable.

### Boolean

Boolean values should begin with is/can/has/etc. when possible.

### Classes, Function and Variables

Class and object names are written in "PascalCase". Functions and Variables are written in "camelCase".

e.g.:

- "ClassA"
- "functionB(int valueC, bool isD)"

### Member

Member objects and variables of a class are indicated by an "m_", e.g. : 

- m_MemberObject 
- m_memberVariable

## Const Correctness

Const correctness is the practice of using the `const` keyword to ensure that objects and variables remain immutable. Use `const` as the default. One exception: function parameters of trivial data types like `int`, `double`, etc.

1. `void f1(const std::string& s);     ` *// Pass by reference-to-`const`*
2. `void f2(const std::string* sptr);  ` *// Pass by pointer-to-`const`*
3. `void f3(std::string s);            ` *// Pass by value*

Employing const correctness from the start is advisable, as it can simplify code maintenance and improve overall code quality.

Further reading on const correctness is highly recommended and can be found [here](https://isocpp.org/wiki/faq/const-correctness).

## Pointer Usage Guidelines
Raw pointers are discouraged except when interfacing with APIs that require them. Following are alternatives to raw pointers for specific use-cases.

- **Smart Pointers**: Use `std::unique_ptr` and `std::shared_ptr` for managing dynamic memory.
- **Optionality with `std::optional`**: Utilize `std::optional` for optional parameters or return types to explicitly indicate the absence of a value.
- **Const C-Strings**: Raw C-strings (`const char*`) are acceptable only when necessary for compatibility with C APIs. Ensure they are `const` to prevent modification.
- **Optimize Strings with `std::string_view`**: For performance-critical code, use `std::string_view` to pass strings by reference without ownership or copy.
