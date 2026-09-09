#pragma once
#include <vector>
#include <string>

/* 
    如非特殊说明，一切数值均为 0-based 数组下标，包括像素坐标、SE 元素坐标、锚点坐标、循环变量等等。 

    注意：row_count / col_count 是「数量」，与最大下标天然相差 1，比较时一律用 < ，不要用 <=。
*/

class BinaryImage
{
    private:
        // 图像矩阵的行高
        int row_count_;

        // 图像矩阵的列宽
        int col_count_;

        // 图像矩阵本身
        std::vector<std::vector<int>> data_;

        // 检查输入矩阵是否合法（空矩阵、含非 0 / 1 元素、行宽不相等）
        static bool is_valid_matrix(const std::vector<std::vector<int>>& data);

    public:
        // 构造大小为 row_count × col_count 的全 initial_value 矩阵
        BinaryImage(int row_count, int col_count, int initial_value = 0);

        // 从二维数组直接构造，自动推导尺寸
        BinaryImage(const std::vector<std::vector<int>>& data);

        // 获取行高
        int row_count() const;

        // 获取列宽
        int col_count() const;

        // 读像素（越界时抛出异常）
        int read_pixel(int row, int col) const;

        // 读像素（越界时把图像外当作 fallback）
        int read_pixel_with_fallback(int row, int col, int fallback) const;

        // 在 [row][col] 写入 value
        void write_pixel(int row, int col, int value);

        // 检测指定坐标是否位于图像内
        bool is_inside(int row, int col) const;

        // 以字符串形式返回图像
        std::string to_text() const;
};
