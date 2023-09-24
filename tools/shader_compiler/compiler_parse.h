#pragma once

#include "sjson_parser.h"
#include "compiler_structures.h"

void deserialize(const sjson::sjson_object& sjson, FShaderObject& value);

void deserialize(const sjson::sjson_object& sjson, FShaderSource& value);

void deserialize(const sjson::sjson_object& sjson, FShaderDescription& value);

void deserialize(const sjson::sjson_object& sjson, FShaderPermutation& value);

void deserialize(const sjson::sjson_object& sjson, FShaderPermutationGenerator& value);

void deserialize(const sjson::sjson_object& sjson, FShaderCompile& value);

void deserialize(const sjson::sjson_object& sjson, FShaderLibrary& value);