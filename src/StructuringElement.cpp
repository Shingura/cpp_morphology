#include "StructuringElement.h"
#include "BinaryImage.h"
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>


StructuringElement::StructuringElement(const std::vector<std::vector<int>>& mask, int anchor_row, int anchor_col)
    : mask_(mask), anchor_row_(anchor_row), anchor_col_(anchor_col)
{
}

int StructuringElement::row_count() const { return mask_.row_count(); }
int StructuringElement::col_count() const { return mask_.col_count(); }

bool StructuringElement::is_active(int row, int col) const
{
    return mask_.read_pixel(row, col) == 1;
}

int StructuringElement::anchor_row() const { return anchor_row_; }
int StructuringElement::anchor_col() const { return anchor_col_; }

std::string StructuringElement::to_text() const
{
    std::ostringstream stream;

    for (int row = 0; row < row_count(); ++row)
    {
        for (int col = 0; col < col_count(); ++col)
        {
            stream << mask_.read_pixel(row, col);
            if (col + 1 < col_count()) { stream << ' '; }         // 每行结尾不加空格
        }
        stream << '\n';
    }

    return stream.str();
}


StructuringElement rectangle(int row_count, int col_count)
{
    if (row_count <= 0 || col_count <= 0)
    {
        throw std::invalid_argument("行高 / 列宽必须大于 0");
    }

    std::vector<std::vector<int>> mask(row_count, std::vector<int>(col_count, 1));
    return StructuringElement(mask, row_count / 2, col_count / 2);
}

StructuringElement disk(int radius)
{
    if (radius < 0) { throw std::invalid_argument("半径不能为负数"); }

    int size = 2 * radius + 1;
    std::vector<std::vector<int>> mask(size, std::vector<int>(size, 0));

    for (int row = 0; row < size; ++row)
    {
        for (int col = 0; col < size; ++col)
        {
            // 比较平方，避免开方引入浮点误差
            int row_offset = row - radius;
            int col_offset = col - radius;
            if (row_offset * row_offset + col_offset * col_offset <= radius * radius)
            {
                mask[row][col] = 1;
            }
        }
    }

    return StructuringElement(mask, radius, radius);
}

StructuringElement cross(int radius)
{
    if (radius < 0) { throw std::invalid_argument("半径不能为负数"); }

    int size = 2 * radius + 1;
    std::vector<std::vector<int>> mask(size, std::vector<int>(size, 0));

    for (int index = 0; index < size; ++index)
    {
        mask[radius][index] = 1;    // 中心行
        mask[index][radius] = 1;    // 中心列
    }

    return StructuringElement(mask, radius, radius);
}
