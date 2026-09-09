#include "BinaryImage.h"
#include "StructuringElement.h"
#include "Morphology.h"

/* 
创建输出图像（尺寸跟输入一样，全 0）

遍历输入的每一行 input_row：

    遍历输入的每一列 input_col：

        如果这个像素不是 1 → continue（背景不盖章）
        遍历 SE 的每一行 element_row：
            遍历 SE 的每一列 element_col：
                如果这一格不参与运算（SE 上这一格为 0） → continue
                算出落点：
                output_row = input_row + element_row - element.anchor_row()
                output_col = input_col + element_col - element.anchor_col()
                如果落点在输出图像内 → 写 1
返回输出图像
*/

BinaryImage Dilation::apply(const BinaryImage& input_image, const StructuringElement& element) const
{
    BinaryImage output_image(input_image.row_count(), input_image.col_count());

    for (int input_row = 0; input_row < input_image.row_count(); ++input_row)
    {
        for (int input_col = 0; input_col < input_image.col_count(); ++input_col)
        {
            if (input_image.read_pixel(input_row, input_col) != 1) continue;

            for (int element_row = 0; element_row < element.row_count(); ++element_row)
            {
                for (int element_col = 0; element_col < element.col_count(); ++element_col)
                {
                    if (!element.is_active(element_row, element_col)) continue;

                    int output_row = input_row + element_row - element.anchor_row();
                    int output_col = input_col + element_col - element.anchor_col();

                    if (output_image.is_inside(output_row, output_col))
                    {
                        output_image.write_pixel(output_row, output_col, 1);
                    }
                }
            }
        }
    }
    return output_image;
}

