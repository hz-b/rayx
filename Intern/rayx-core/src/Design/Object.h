#pragma once

#include <variant>
#include <memory>

#include "SurfaceElement.h"
#include "Source.h"

struct Element;
struct Source;
struct Tree;
using TreePtr = std::shared_ptr<Tree>;

struct Object {
    std::varant<Element, Source, Tree> value;
};
