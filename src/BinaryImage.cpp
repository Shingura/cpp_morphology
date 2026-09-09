#include "BinaryImage.h"
#include <cstddef>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

bool BinaryImage::is_valid_matrix(const std::vector<std::vector<int>>& data)
{
    // 如果为空则非法
    if (data.empty())     { return false; }
    if (data[0].empty())  { return false; }

    size_t first_row_width = data[0].size();

    // 如果不存在非 0 / 1 元素 && 第一行列宽大于 0 && 后续行的列宽与第一行均相等，则合法
    for (const auto& row : data)
    {
        if (row.size() != first_row_width) { return false; }
        for (auto value : row)
        {
            if ( value != 0 && value != 1) { return false; }
        }
    }
    return true;
}


BinaryImage::BinaryImage(int row_count, int col_count, int initial_value)
{
    if (row_count <= 0 || col_count <= 0) { throw std::invalid_argument("行高 / 列宽必须大于 0"); }
    if (initial_value != 0 && initial_value != 1) { throw std::invalid_argument("写入值非 0 / 1"); }

    row_count_ = row_count;
    col_count_ = col_count;
    data_ = std::vector<std::vector<int>>(row_count, std::vector<int>(col_count, initial_value));
}

BinaryImage::BinaryImage(const std::vector<std::vector<int>>& data)
{
    if (!is_valid_matrix(data)) { throw std::invalid_argument("输入矩阵不合法"); }

    data_ = data;
    row_count_ = data.size();
    col_count_ = data[0].size();
}


int BinaryImage::row_count() const { return row_count_; }
int BinaryImage::col_count() const { return col_count_; }


bool BinaryImage::is_inside(int row, int col) const
{
    return row >= 0 && row < row_count()
        && col >= 0 && col < col_count();
}


int BinaryImage::read_pixel(int row, int col) const
{
    if (is_inside(row, col)) { return data_[row][col]; }

    throw std::out_of_range("像素坐标超出图像范围");
}

int BinaryImage::read_pixel_with_fallback(int row, int col, int fallback) const
{
    if (is_inside(row, col)) { return data_[row][col]; }

    return fallback;
}

void BinaryImage::write_pixel(int row, int col, int value)
{
    if (!is_inside(row, col))     { throw std::out_of_range("像素坐标超出图像范围"); }
    if (value != 0 && value != 1) { throw std::invalid_argument("写入值非 0 / 1"); }

    data_[row][col] = value;
}


std::string BinaryImage::to_text() const
{
    std::ostringstream stream;

    for (const auto& row : data_)
    {
        for (const auto& value : row)
        {
            if (&value == &row.back()) { stream << value; }      // 每行结尾不加空格
            else { stream << value << ' '; }                     // 否则要加空格
        }
        stream << '\n';
    }

    return stream.str();
}