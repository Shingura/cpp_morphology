#pragma once
#include "BinaryImage.h"
#include <vector>
#include <string>

class StructuringElement
{
    private:
        // 蒙版，复用 BinaryImage 做存储
        BinaryImage mask_;

        // 锚点行坐标
        int anchor_row_;

        // 锚点列坐标
        int anchor_col_;

    public:
        // 以二维数组构造 SE，手动传入锚点位置
        StructuringElement(const std::vector<std::vector<int>>& mask, int anchor_row, int anchor_col);

        // 获取蒙版行高
        int row_count() const;

        // 获取蒙版列宽
        int col_count() const;

        // 检查蒙版 [row][col] 处是否参与运算
        bool is_active(int row, int col) const;

        // 获取锚点行坐标
        int anchor_row() const;

        // 获取锚点列坐标
        int anchor_col() const;

        // 以字符串形式返回 SE
        std::string to_text() const;
};


// 常用蒙版，分别为正方形、圆形和十字形

// 创建正方形蒙版，可用于 dilation 1（对于偶数长 / 宽，自动向下取整）
StructuringElement rectangle(int row_count, int col_count);
// 创建圆形蒙版，可用于 dilation 2
StructuringElement disk(int radius);
// 创建十字形蒙版
StructuringElement cross(int radius); 