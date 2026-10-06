/*
 * student.h —— 学生成绩管理系统 v0.5 的头文件（链表版）
 *
 * 版本演进：
 *   v0.2  Student list[100] 定长数组 —— 一开始就占掉 100 份空间，满了就塞不进
 *   v0.3  把"定长数组"换成"链表"：每个学生是一个节点，用多少申请多少，理论上不限个数
 *   v0.5  新增两个接口：list_find_by_name（按姓名查找）、list_stats（平均/最高/最低）
 *
 * 链表版带来一个 C 语言里非常常见的写法变化：
 *   v0.2 传参是 Student *list, int count        （数组 + 个数，两个参数要一起传）
 *   v0.3 传参是 StudentNode *head               （只要一个头指针，个数信息藏在链上）
 *
 * 头文件里只放"声明"（函数长什么样、参数是什么类型），
 * 真正的实现都在 student.c —— 这就是为什么编译时必须把两个 .c 一起给 gcc。
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>

#define MAX_NAME_LEN   32     /* 姓名缓冲区大小（含结尾 '\0'），最长 31 个字符 */
#define MAX_NAME_CHARS 31     /* 存档时允许的最大字符数 = 数组容量 - 1 */

#define DATA_FILE "students.txt"   /* 存档文件名，集中放这里，改一处即可 */

/* 链表节点：数据域 + 指针域。
 * 注意 "struct StudentNode *next;" 这一行 ——
 * 结构体内部可以放"指向自己这种类型的指针"，这叫自引用结构体。
 * 但绝不能放 "struct StudentNode next;"（会无限套娃，编译器直接报错）。 */
typedef struct StudentNode {
    int  id;                       /* 学号 */
    char name[MAX_NAME_LEN];       /* 姓名 */
    int  score;                    /* 成绩 */
    struct StudentNode *next;      /* 指向下一个学生，NULL 表示链表结束 */
} StudentNode;

/* ============================================================
 * 对外接口（v0.3 链表版）
 * ============================================================ */

/* 创建并初始化一条空链表：本项目约定"带头结点"，
 * 头结点不存学生数据，只是挂在那里方便统一处理。
 * 返回头结点指针；内存申请失败返回 NULL。 */
StudentNode *list_create(void);

/* 释放整条链表（包含头结点）。参数用二级指针，
 * 因为要把调用者的 head 置成 NULL，否则 head 变成野指针。 */
void list_destroy(StudentNode **phead);

/* 按学号升序插入一个新学生（插入时自动保持有序）。
 * 成功返回 1，内存不足返回 0。
 * 二级指针同样是"可能修改 head 本身"时（头结点后面第一个位置）才需要的。 */
int list_insert(StudentNode **phead, int id, const char *name, int score);

/* 按学号查找，返回节点指针；找不到返回 NULL */
StudentNode *list_find(StudentNode *head, int id);

/* 按学号删除，成功返回 1，没找到返回 0 */
int list_delete(StudentNode **phead, int id);

/* 按成绩从高到低排序（链表版冒泡：交换节点里的数据，不换指针，最省事） */
void list_sort_by_score(StudentNode *head);

/* 打印全部学生，返回打印了几条 */
int list_print(StudentNode *head);

/* 统计节点个数（不含头结点） */
int list_count(StudentNode *head);

/* 从文件读取并建链，返回成功读入的条数 */
int list_load(StudentNode **phead, const char *filename);

/* 保存到文件，返回成功写入的条数；失败返回 -1 */
int list_save(StudentNode *head, const char *filename);

/* ============================================================
 * v0.5 新增接口（三项增量）
 * ============================================================ */

/* 【增量 1】按姓名查找第一个同名节点，返回节点指针；找不到返回 NULL。
 * 为什么不能直接写 if (cur->name == name)？
 *   name 是 char 数组，数组名退化成首地址，"==" 比的是地址而不是文字内容。
 *   C 语言里比较字符串内容必须用 strcmp()。 */
StudentNode *list_find_by_name(StudentNode *head, const char *name);

/* 【增量 2】统计函数（一次遍历同时算完三个值）。
 * 把结果通过指针参数"带出去"，因为函数只能返回一个值。
 * 返回参与统计的人数：为 0 时三个输出参数都会被置 0，避免除零。 */
int list_stats(StudentNode *head, double *avg_out, int *max_out, int *min_out);

#endif /* STUDENT_H */
