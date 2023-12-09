#pragma once

#include "TypedTable.h"

struct DesignMatrixSource;

struct DesignSourceType : public TypedVariant<
    Case<DesignMatrixSource, MatrixSourceStr>
> {};

struct DesignMatrixSource : public TypedTable<> {};
