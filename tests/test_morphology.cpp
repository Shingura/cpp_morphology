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
    BinaryImage build_sample_image()
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

    // 12x12 图像，正中间放一个 5x5 的实心方块（第 3~7 行、第 3~7 列）
    BinaryImage build_block_image()
    {
        return BinaryImage({
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    }

    // 12x12 图像，左上角贴边放一个 5x5 的实心方块（第 0~4 行、第 0~4 列）
    BinaryImage build_corner_image()
    {
        return BinaryImage({
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
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

    // 打印一个完整用例：标题、说明、输入图像、结构元素、输出图像
    void show_case(const std::string& title, const std::string& explanation,
                   const BinaryImage& input_image, const StructuringElement& element,
                   const std::string& output_label, const BinaryImage& output_image)
    {
        std::cout << "\n============================================================\n";
        std::cout << title << "\n";
        std::cout << "============================================================\n";
        std::cout << explanation << "\n\n";
        std::cout << "【输入图像】\n" << input_image.to_text() << "\n";
        std::cout << "【结构元素】（锚点在第 " << element.anchor_row()
                  << " 行第 " << element.anchor_col() << " 列）\n" << element.to_text() << "\n";
        std::cout << "【" << output_label << "】\n" << output_image.to_text() << "\n";
    }
}

int main()
{
    const BinaryImage sample_image = build_sample_image();
    const BinaryImage block_image = build_block_image();
    const BinaryImage corner_image = build_corner_image();
    const Dilation dilation;
    const Erosion erosion;

    // ---------------------------------------------------------------
    // 用例 1：dilation 1 —— 5x5 方形结构元素膨胀
    // ---------------------------------------------------------------
    const StructuringElement square_5x5 = rectangle(5, 5);
    const BinaryImage dilation_1 = dilation.apply(sample_image, square_5x5);
    show_case("用例 1：dilation 1 —— 5x5 方形结构元素膨胀",
              "题目要求的基本操作。用 5x5 的全 1 方形结构元素（25 格全部参与运算）膨胀。\n"
              "做法：把结构元素的锚点（5x5 的中心，第 2 行第 2 列）对准每一个前景像素，\n"
              "把结构元素当作印章盖下去，被盖到的输出位置一律置 1。\n"
              "效果：前景整体向外扩张两格，轮廓拐角保持 90 度尖角 —— 因为结构元素本身是方的。\n"
              "边界：图像外一律当作背景 0（zero padding），输出尺寸与输入相同，12x12 进 12x12 出。",
              sample_image, square_5x5, "输出图像 dilation 1", dilation_1);

    // ---------------------------------------------------------------
    // 用例 2：dilation 2 —— 5x5 圆形（内切圆）结构元素膨胀
    // ---------------------------------------------------------------
    const StructuringElement disk_5x5 = disk(2);
    const BinaryImage dilation_2 = dilation.apply(sample_image, disk_5x5);
    show_case("用例 2：dilation 2 —— 5x5 圆形结构元素膨胀（内切圆）",
              "这是任务要求 4 的答案：输入图像和膨胀算法一行都没有改，只把结构元素换成了\n"
              "5x5 的圆盘 —— 到中心直线距离不超过 2 的格子才参与运算，共 13 格，四角被切掉。\n"
              "效果：前景同样外扩两格，但轮廓拐角变成了圆弧，不再有 90 度尖角。\n"
              "对比用例 1 可以看出：膨胀出来的形状完全由结构元素决定，算子代码对形状没有任何假设，\n"
              "这正是「结构元素可替换」这个设计要求的意义所在。",
              sample_image, disk_5x5, "输出图像 dilation 2", dilation_2);

    // ---------------------------------------------------------------
    // 用例 3：erosion —— 3x3 方形结构元素腐蚀
    // ---------------------------------------------------------------
    const StructuringElement square_3x3 = rectangle(3, 3);
    const BinaryImage eroded_block = erosion.apply(block_image, square_3x3);
    show_case("用例 3：erosion —— 3x3 方形结构元素腐蚀",
              "腐蚀是膨胀的反向操作：不再问「有没有一个是 1」，而是问「是不是全部都是 1」。\n"
              "做法：站在每一个输出位置上，把结构元素锚点对准它，回查输入中对应的所有格子，\n"
              "全部为 1 才保留 1，只要有一个是 0 就变成 0。越界的格子当作 0。\n"
              "效果：前景整体收缩，四周各缩掉一圈 —— 腐蚀会让图像「瘦一圈」。\n"
              "这里没有用题目给的原始图像：它的线条只有 1~2 像素宽，3x3 腐蚀会把它完全吃掉、\n"
              "输出全 0（这是正确结果，但不便于观察）。所以改用中央的 5x5 实心方块演示，\n"
              "可以看到它稳定地缩成了 3x3 方块。",
              block_image, square_3x3, "输出图像 erosion", eroded_block);

    // ---------------------------------------------------------------
    // 用例 4：特殊情况 —— 十字形结构元素膨胀
    // ---------------------------------------------------------------
    const StructuringElement cross_5x5 = cross(2);
    const BinaryImage dilation_cross = dilation.apply(sample_image, cross_5x5);
    show_case("用例 4：特殊情况 —— 十字形结构元素膨胀",
              "换一个不规则的结构元素看看：5x5 的十字形，只有中心行和中心列参与运算，共 9 格，\n"
              "四个角完全不参与。\n"
              "效果：前景只沿着水平和垂直方向生长，斜向一格都不扩张 —— 因为结构元素的四角\n"
              "从来不落点，斜方向上永远盖不到任何东西。\n"
              "这个例子最直观地说明：膨胀不是简单地「把图形放大」，\n"
              "而是「用结构元素的形状去扫过图形的每一个前景点」，结果长什么样，全看 SE 长什么样。",
              sample_image, cross_5x5, "输出图像 dilation（十字）", dilation_cross);

    // ---------------------------------------------------------------
    // 自动检查
    // ---------------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "自动检查\n";
    std::cout << "============================================================\n";

    // 检查 1：3x3 膨胀必须等于题目给出的期望输出
    const std::string dilation_3_expected =
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

    const BinaryImage dilation_3 = dilation.apply(sample_image, rectangle(3, 3));
    check(dilation_3.to_text() == dilation_3_expected, "3x3 膨胀结果与题目给出的期望输出逐字符一致");

    // 检查 2：膨胀只会让前景变多，不会吃掉原有的前景
    check(contains_all_ones(dilation_1, sample_image), "5x5 膨胀保留了输入中所有前景像素");

    // 检查 3：结构元素越大，膨胀结果越大（5x5 的 SE 完全包含 3x3 的 SE）
    check(contains_all_ones(dilation_1, dilation_3), "5x5 膨胀结果包含 3x3 膨胀结果");

    // 检查 4：5x5 实心方块经 3x3 腐蚀后只剩中心 3x3 方块
    const std::string eroded_block_expected =
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 1 1 1 0 0 0 0 0\n"
        "0 0 0 0 1 1 1 0 0 0 0 0\n"
        "0 0 0 0 1 1 1 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0 0 0 0 0\n";

    check(eroded_block.to_text() == eroded_block_expected, "5x5 实心方块经 3x3 腐蚀后只剩中心 3x3 方块");

    // 检查 5：腐蚀只会让前景变少，不会凭空造出前景
    check(contains_all_ones(block_image, eroded_block), "腐蚀结果完全包含在原图的前景内");

    // 检查 6、7：贴着左上角的方块，用来验证「图像外当作背景 0」这条边界策略
    const BinaryImage eroded_corner = erosion.apply(corner_image, rectangle(3, 3));
    check(eroded_corner.read_pixel(0, 0) == 0, "zero padding：贴边方块的外圈被腐蚀掉");
    check(eroded_corner.read_pixel(1, 1) == 1, "zero padding：贴边方块的内部仍然保留");

    std::cout << "\n通过 " << passed_count << " 项，失败 " << failed_count << " 项\n";
    return failed_count == 0 ? 0 : 1;
}
