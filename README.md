# demostudy — 嵌入式 C 语言 / 数据结构 练习仓库

> 一年成长为嵌入式工程师学习计划（2026-08-10 启动）的**阶段 1：C 语言与数据结构**每日手写练习。
> 每天一个最小实验或一道 LeetCode 题，GCC 编译验证，git 提交 push 形成完整学习轨迹。

## 当前进度

- 阶段 1（C / 数据结构）进行中：**第 4 周 · 二叉树专题 + BST 入门**（2026-08-10 起，预计 13 周）
- 已覆盖：指针 → 内存 → 结构体 → 链表 → 栈 / 队列 → 树（遍历 / 递归 / BFS）
- 主线预告：BST → 哈希表 → 阶段 2（STM32 + RTOS）

## 知识点进度表（按提交时间）

| 时间 | 主题 | 代表文件 |
|---|---|---|
| 08-10 ~ 08-11 | C 入门、指针概念（sizeof / 地址步长） | hello.c, pointer/addr_test.c |
| 08-12 | 内存四区（栈/堆/静态区）、函数调用栈 | memory_zones.c |
| 08-13 | struct 内存对齐、sizeof/offsetof、结构体+指针 | struct_layout.c, node_demo.c |
| 08-14 ~ 08-15 | malloc/free 堆内存实验 | malloctest.c |
| 08-17 ~ 08-19 | **链表补齐**：反转链表、动态单链表、带头结点双向链表 | reverse_list.c, dlist_single.c, listnode_test.c, linked_list/basic.c |
| 08-20 | const 关键字实验 | const_test.c |
| 08-21 ~ 08-22 | 数组栈/队列实现、括号匹配 | stack_array.c, queue_array.c, valid_parentheses.c |
| 08-24 ~ 08-25 | 二叉树遍历（前/中/后/层序）、用队列实现栈 | traverse.c, my_queue_to_stack.c |
| 08-26 | 递归入门、二叉树最大深度 | factorial.c, fibonacci.c, max_depth.c |
| 08-27 | 翻转二叉树、节点统计（count/leaves/height） | invert_tree.c, btree_statcs.c |
| 08-28 ~ 08-29 | 相同树、层序遍历（BFS + 队列） | same_tree.c, level_order.c |
| 08-31 | 层序遍历闭卷默写、102 按层切分 | blind_level_order.c, level_order_102.c |
| 09-02 ~ 09-03 | 最小深度、对称二叉树、BST 插入/验证（推进中） | min_depth_111.c, symmetric_101.c |

## LeetCode 练习（C 语言）

| # | 题目 | 状态 |
|---|---|---|
| 20 | 有效的括号 | ✅ |
| 104 | 二叉树的最大深度 | ✅ |
| 226 | 翻转二叉树 | ✅ |
| 100 | 相同的树 | ✅ |
| 102 | 二叉树的层序遍历 | ✅ 力扣 AC |
| 111 | 二叉树的最小深度 | ✅ 本地通过 |
| 206 | 反转链表 | ✅ |
| 225 | 用队列实现栈 | ✅ |
| 977 | 有序数组的平方（双指针） | ✅ |
| 101 | 对称二叉树 | 🔄 进行中 |
| 701 / 98 | BST 插入 / 验证 | 🔄 进行中 |

## 编译运行约定

```bash
gcc xxx.c -o xxx.exe && ./xxx.exe     # Windows + GCC（VS Code 集成终端）
```

- 代码内注释与 printf 输出统一用英文/ASCII 标签（规避 Windows 控制台 GBK 乱码）
- 每天产出独立提交并 push，提交信息标注日期与主题

## 目录结构

- 根目录：按天/按题的 `.c` 源文件 + 编译产物 `.exe`
- `pointer/`、`linked_list/`：分主题目录；`.exe` 已被 .gitignore 忽略
