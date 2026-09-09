#include "BinaryImage.h"
#include "StructuringElement.h"
#include "Morphology.h"
#include <iostream>
#include <string>

namespace
{
    int passed_count = 0;
    int failed_count = 0;

    void check(bool condition, const std::string& description)
    {
        if (condition)
        {
            ++passed_count;
            std::cout << "[通过] " << description << "\n";
        }
        else
        {
            ++failed_count;
            std::cout << "[失败] " << description << "\n";
        }
    }

    // 题目给出的 12x12 输入图像
    BinaryImage build_input_image()
    {
        return BinaryImage({
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
    }

    // 检查 bigger 是否包含了 smaller 里所有的 1（要求两张图尺寸相同）
    bool contains_all_ones(const BinaryImage& bigger, const BinaryImage& smaller)
    {
        if (bigger.row_count() != smaller.row_count() || bigger.col_count() != smaller.col_count())
        {
            return false;
        }

        for (int row = 0; row < smaller.row_count(); ++row)
        {
            for (int col = 0; col < smaller.col_count(); ++col)
            {
                if (smaller.read_pixel(row, col) == 1 && bigger.read_pixel(row, col) != 1)
                {
                    return false;
                }
            }
        }
        return true;
    }
}

int main()
{
    const BinaryImage input_image = build_input_image();
    const Dilation dilation;

    // 测试 1：3x3 膨胀必须等于题目给出的期望输出
    const std::string expected_text =
        "0 0 0 0 1 1 1 1 0 0 0 0\n"
        "0 0 0 1 1 1 1 1 0 0 0 0\n"
        "0 0 0 1 1 1 1 1 0 0 0 0\n"
        "0 0 0 1 1 1 1 1 0 0 0 0\n"
        "0 0 0 0 1 1 1 1 1 0 0 0\n"
        "0 0 0 0 1 1 1 1 1 0 0 0\n"
        "0 0 0 0 1 1 1 1 1 1 0 0\n"
        "0 0 0 0 1 1 1 1 1 1 0 0\n"
        "0 0 0 0 0 0 1 1 1 1 0 0\n"
        "0 0 0 0 0 0 1 1 1 1 0 0\n"
        "0 0 0 0 0 0 0 1 1 1 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n";

    const BinaryImage dilated_by_3 = dilation.apply(input_image, rectangle(3, 3));
    check(dilated_by_3.to_text() == expected_text, "3x3 膨胀结果与题目期望输出逐字符一致");

    // 测试 2：膨胀只会让前景变多，不会吃掉原有的前景像素
    const BinaryImage dilated_by_5 = dilation.apply(input_image, rectangle(5, 5));
    check(contains_all_ones(dilated_by_5, input_image), "5x5 膨胀保留了输入中所有前景像素");

    // 测试 3：结构元素越大，膨胀结果越大（5x5 的 SE 完全包含 3x3 的 SE）
    check(contains_all_ones(dilated_by_5, dilated_by_3), "5x5 膨胀结果包含 3x3 膨胀结果");

    std::cout << "\n通过 " << passed_count << " 项，失败 " << failed_count << " 项\n";
    return failed_count == 0 ? 0 : 1;
}
