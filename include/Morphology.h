#pragma once
#include "BinaryImage.h"
#include "StructuringElement.h"

class Dilation
{
    public:
        BinaryImage apply(const BinaryImage& input_image, const StructuringElement& element) const;
};