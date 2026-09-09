#include "BinaryImage.h"
#include "StructuringElement.h"
#include "Morphology.h"
#include <iostream>

int main()
{
    BinaryImage image({
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    
    std::cout << image.to_text();

    Dilation dilation;
    std::cout << dilation.apply(image, rectangle(3, 3)).to_text() << "\n";

    return 0;
}
