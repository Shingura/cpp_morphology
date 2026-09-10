# cpp_morphology

用 C++ 实现的二值形态学处理管线：对二值图像做**膨胀（dilation）**和**腐蚀（erosion）**，结构元素可替换。

RoboMaster 社团培训任务二。

---

## 快速开始

```bash
# 需要 C++17
mkdir -p build

# 演示程序：打印输入图像、dilation 1、dilation 2、erosion
g++ -std=c++17 -Iinclude src/*.cpp -o build/morphology
./build/morphology

# 测试程序：四个测试用例 + 自动检查
g++ -std=c++17 -Iinclude \
    src/BinaryImage.cpp src/StructuringElement.cpp src/Morphology.cpp \
    tests/test_morphology.cpp -o build/test_morphology
./build/test_morphology
```

测试程序退出码为 0 表示全部通过，非 0 表示有失败项。

> 注意：编译测试时要**排除** `src/main.cpp`，否则会出现两个 `main` 函数。

---

## 项目结构

```
cpp_morphology/
├── include/                    # 头文件
│   ├── BinaryImage.h           # 二值图像
│   ├── StructuringElement.h    # 结构元素
│   └── Morphology.h            # 形态学算子（Dilation / Erosion）
├── src/
│   ├── BinaryImage.cpp
│   ├── StructuringElement.cpp
│   ├── Morphology.cpp
│   └── main.cpp                # 演示入口
├── tests/
│   └── test_morphology.cpp     # 测试用例 + 自动检查
└── .clangd                     # clangd 配置（让 IDE 能找到头文件）
```

---

## 核心概念

所有坐标都是 **0-based 数组下标**。图像中 `1` 表示前景，`0` 表示背景。

### 膨胀（dilation）

> 对输出位置，问：**SE 覆盖到的输入像素里，有没有一个是 1？** 有就是 1。

本项目用**盖章法**实现：准备一张空白输出图，遍历输入中每个前景像素，把 SE 当作印章、锚点对准它盖下去，被盖到的位置置 1。落到图像外的部分直接丢弃。

### 腐蚀（erosion）

> 对输出位置，问：**SE 覆盖到的输入像素，是不是全是 1？** 全是才是 1，有一个 0 就是 0。

本项目用**探针法**实现：站在每个输出位置上，回查输入中 SE 覆盖的所有格子，全部为 1 才保留。

### 边界策略

**Zero padding** —— 图像外的像素一律当作背景 0，输出尺寸与输入相同（12×12 进，12×12 出）。

这带来一个可预期的效果：腐蚀之后图像最外一圈必然全为 0（因为它们的邻域一定探到了图像外）。测试里专门有一条检查这件事。

### 锚点

SE 上"对准当前像素"的那个格子。奇数尺寸的 SE 有唯一中心锚点；偶数尺寸会向下取整（偏移半格）。

锚点**允许落在 SE 外面** —— 那等价于把整个 SE 平移一段距离，数学上是良好定义的，所以代码不做校验。

---

## 类设计

### `BinaryImage` —— 二值图像

```
BinaryImage(int row_count, int col_count, int initial_value = 0)  // 全 0（或全 1）矩阵
BinaryImage(const std::vector<std::vector<int>>& data)            // 从二维数组构造，自动推导尺寸

int  row_count() const
int  col_count() const
int  read_pixel(int row, int col) const                           // 越界抛异常
int  read_pixel_with_fallback(int row, int col, int fallback) const // 越界返回 fallback
void write_pixel(int row, int col, int value)                     // 值必须是 0 或 1
bool is_inside(int row, int col) const
std::string to_text() const                                       // 转成可打印的文本
```

**不暴露内部容器，也不提供返回引用的访问器。** 否则外面能把某一行 `resize` 成不同长度，图像就不再是矩形。外部只能通过接口一个一个访问 —— 代价是不能批量遍历，换来的是"图像不可能被搞成锯齿状"这个保证。

**禁止 `std::vector<bool>`**：它按位压缩，`operator[]` 返回的是代理对象而非 `bool&`，取不了引用。

### `StructuringElement` —— 结构元素

```
StructuringElement(const std::vector<std::vector<int>>& mask, int anchor_row, int anchor_col)

int  row_count() const
int  col_count() const
bool is_active(int row, int col) const   // 这一格参不参与运算
int  anchor_row() const
int  anchor_col() const
std::string to_text() const
```

三个形状工厂（自由函数）：

```
rectangle(row_count, col_count)   // 全 1 矩形，锚点自动取中心
disk(radius)                      // 圆盘：到中心直线距离 <= radius，锚点自动取中心
cross(radius)                     // 十字：只有中心行和中心列
```

**为什么是组合而不是继承？** SE 确实"包含"一个二维 0/1 网格，但它**不是**一种图像 —— 图像里的 0 是"背景像素"，SE 里的 0 是"这一格不参与运算"，语义不同。用组合（`mask_` 成员）复用存储，同时保住各自的语义。

接口叫 `is_active` 而不是 `read_pixel`，就是为了提醒：这里的值回答的是"参不参与运算"，不是"是什么颜色"。

### `Dilation` / `Erosion` —— 形态学算子

```
BinaryImage apply(const BinaryImage& input_image, const StructuringElement& element) const;
```

两个类都是**无状态**的，SE 每次调用时传入、不持有。它们没有共同基类 —— 因为盖章法和探针法的算法骨架不同，现在抽象基类属于过早设计；等出现更多算子、共同点明确之后再抽不迟。

---

## 测试用例

`tests/test_morphology.cpp` 里四个用例，每个都打印说明、输入图像、结构元素（含锚点位置）和输出图像：

| 用例 | 输入 | 结构元素 | 看点 |
|---|---|---|---|
| 1 | 题目原图 | `rectangle(5, 5)` 方形，25 格 | 外扩两格，四角是 **90° 尖角** |
| 2 | **同一张原图** | `disk(2)` 圆盘，13 格 | 外扩两格，四角是**圆弧** |
| 3 | 中央 5×5 实心方块 | `rectangle(3, 3)` | 缩一圈变成 3×3 方块 |
| 4 | 题目原图 | `cross(2)` 十字，9 格 | 只沿水平/垂直生长，斜向不扩张 |

**用例 1 和 2 就是"换结构元素得到另一种结果"的答案** —— 同一张原图、同一段算子代码，只换 SE：

```
方形 SE（最后一行）：  0 0 0 0 0 0 1 1 1 1 1 0    ← 底部一整条，尖角
圆盘 SE（最后一行）：  0 0 0 0 0 0 0 0 1 0 0 0    ← 只剩一个点，圆角
```

用例 3 没有用题目原图：它的线条只有 1~2 像素宽，3×3 腐蚀会把它们全部吃掉（输出全 0 —— 这是正确结果，但不便于观察），所以改用实心方块演示。

### 自动检查（7 项）

除了逐字符比对期望输出，还有几条基于数学性质的检查，它们对任何输入都成立，比人眼可靠：

- 膨胀结果包含原图所有前景（膨胀只增不减）
- 5×5 膨胀结果包含 3×3 膨胀结果（SE 越大结果越大）
- 腐蚀结果完全包含在原图前景内（腐蚀只减不增）
- 贴边方块腐蚀后外圈消失、内部保留（验证 zero padding）

---

## 一些设计取舍

**为什么膨胀用盖章法、腐蚀用探针法？**

盖章法天然表达"有没有"（散出去就行，后到的可以覆盖）；但腐蚀问的是"是不是全部"，需要**收集**覆盖同一位置的所有信息，而盖章法遍历时一次只看一个输入像素的邻域，凑不齐。所以腐蚀换成探针法：站在输出位置上回查。

（腐蚀其实也有盖章版本 —— 遍历背景像素、把"置 0"盖出去、坐标符号取反 —— 留作后续的对拍验证。）

**为什么坐标比较用 `<` 而不是 `<=`？**

`row_count()` 返回的是**数量**，跟最大下标天生差 1。统一用 `<` 能消掉一类差一错误。

**为什么 `disk` 用平方比较而不开方？**

`sqrt` 是浮点运算，边界情况（距离正好等于半径）可能因精度误差判错，导致圆盘缺一个角。两边平方就全用整数，精确。

---

## 后续可以做

- 盖章法腐蚀，与探针法互相**对拍**验证
- 开运算 / 闭运算（腐蚀+膨胀的组合）
- 可视化（C++ 导出数据 + HTML 渲染动画）
- 支持非对称、带权重的 SE
