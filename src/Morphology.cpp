#include "BinaryImage.h"
#include "StructuringElement.h"
#include "Morphology.h"

namespace
{
    // 用于 Dilation 膨胀
    // 把 SE 当作印章盖到输出图像上：锚点对准输入的 (input_row, input_col)，
    // SE 上参与运算的格子会在输出上留下印记
    void stamp(BinaryImage& output_image, const StructuringElement& element,
                        int input_row, int input_col)
    {
        for (int element_row = 0; element_row < element.row_count(); ++element_row)
        {
            for (int element_col = 0; element_col < element.col_count(); ++element_col)
            {
                // SE 上这一格是 0，则直接跳过
                if (!element.is_active(element_row, element_col)) continue;

                int output_row = input_row + element_row - element.anchor_row();
                int output_col = input_col + element_col - element.anchor_col();

                // 盖到图像外的部分直接丢弃
                if (!output_image.is_inside(output_row, output_col)) continue;

                output_image.write_pixel(output_row, output_col, 1);
            }
        }
    }

    // 用于 Erosion 腐蚀
    // 把 SE 锚点对准输出的 (output_row, output_col)，检查 SE 覆盖到的输入像素是否全为 1
    bool is_fully_covered(const BinaryImage& input_image, const StructuringElement& element,
                            int output_row, int output_col)
    {
        for (int element_row = 0; element_row < element.row_count(); ++element_row)
        {
            for (int element_col = 0; element_col < element.col_count(); ++element_col)
            {
                // SE 上这一格是 0，则不参与运算，直接跳过
                if (!element.is_active(element_row, element_col)) continue;

                int input_row = output_row + element_row - element.anchor_row();
                int input_col = output_col + element_col - element.anchor_col();

                // 发现一个 0 即终止
                if (input_image.read_pixel_with_fallback(input_row, input_col, 0) == 0) { return false; }
            }
        }
        return true;
    }
}

BinaryImage Dilation::apply(const BinaryImage& input_image, const StructuringElement& element) const
{
    BinaryImage output_image(input_image.row_count(), input_image.col_count());

    for (int input_row = 0; input_row < input_image.row_count(); ++input_row)
    {
        for (int input_col = 0; input_col < input_image.col_count(); ++input_col)
        {
            // 背景像素不盖章
            if (input_image.read_pixel(input_row, input_col) != 1) continue;

            stamp(output_image, element, input_row, input_col);
        }
    }
    return output_image;
}

BinaryImage Erosion::apply(const BinaryImage& input_image, const StructuringElement& element) const
{
    BinaryImage output_image(input_image.row_count(), input_image.col_count());

    for (int output_row = 0; output_row < output_image.row_count(); ++output_row)
    {
        for (int output_col = 0; output_col < output_image.col_count(); ++output_col)
        {
            // 只有 SE 完全落在前景里，这个位置才保留
            if (is_fully_covered(input_image, element, output_row, output_col))
            {
                output_image.write_pixel(output_row, output_col, 1);
            }
        }
    }
    return output_image;
}
