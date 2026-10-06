/*
 * student.c —— 学生成绩管理系统 v0.5 的实现（链表 + 文件存储 + 排序 + 姓名查找 + 统计）
 *
 * 编译方式（两个 .c 一起编译链接成 1 个 exe）：
 *   gcc -Wall -Wextra -O2 main.c student.c -o student_mgr.exe
 *
 *   ★ 必须把 main.c 和 student.c 一起写进命令里。
 *     只编译 main.c 的话，会在"链接"阶段报一长串
 *     undefined reference to `list_create' —— 因为函数实现都在这一个文件里。
 *
 * 和 v0.2 的对照表：
 *   v0.2 数组版                 v0.3 链表版
 *   ------------------------    --------------------------
 *   Student list[100]           StudentNode *head（带头结点）
 *   传 list + count 两个参数     只传 head 一个参数
 *   满了 add_student 返回 0     内存不够才返回 0（几乎不会满）
 *   排序交换数组元素             排序交换节点里的 id/name/score
 */

#include "student.h"
#include <stdlib.h>   /* malloc / free */
#include <string.h>   /* strlen / memcpy / strcmp */

/* ------------------------------------------------------------
 * 内部小工具：安全地把源字符串拷进 dest（容量 cap 字节）
 *
 * 为什么不直接用 strncpy？
 *   因为 strncpy 在源字符串长度 >= cap 时不会补 '\0'，
 *   而且 gcc 的 -Wall 会报 -Wstringop-truncation 警告。
 *   手写一段则每一步都看得见，也更容易理解"到底拷了几个字节"。
 * ------------------------------------------------------------ */
static void copy_name(char *dest, size_t cap, const char *src)
{
    size_t len = strlen(src);          /* 源串实际字符数（不含末尾 '\0'） */

    if (len > cap - 1) {               /* 太长就截断，cap-1 个字符 + 1 个 '\0' */
        len = cap - 1;
    }

    memcpy(dest, src, len);            /* 只拷 len 个字节，不带 '\0' */
    dest[len] = '\0';                  /* 自己补上结尾，这一步千万别忘 */
}

/* ------------------------------------------------------------
 * 创建带头结点的空链表
 *
 * 什么是"头结点"？
 *   就是一个不存学生数据的哨兵节点，它的 next 指向第一个真学生。
 *   好处：插入/删除时不用再分类讨论"操作的是不是第一个节点"，
 *   代码能少写一半的 if。
 * ------------------------------------------------------------ */
StudentNode *list_create(void)
{
    StudentNode *head = (StudentNode *)malloc(sizeof(StudentNode));

    if (head == NULL) {                /* malloc 可能失败，必须检查 */
        return NULL;
    }

    head->id = 0;
    head->name[0] = '\0';
    head->score = 0;
    head->next = NULL;                 /* 空链表：头结点后面什么都没有 */

    return head;
}

/* 只释放"真学生"节点，保留头结点不动。
 * 给 load 清空旧数据用 —— 头结点是链表的壳子，得留着继续用。 */
static void list_free_nodes(StudentNode *head)
{
    if (head == NULL) {
        return;
    }

    StudentNode *cur = head->next;

    while (cur != NULL) {
        StudentNode *next = cur->next;  /* 先把下一个存下来，再 free 当前 */
        free(cur);                      /* 顺序反了就 use-after-free */
        cur = next;
    }

    head->next = NULL;                  /* 壳子清空，变成空链表 */
}

void list_destroy(StudentNode **phead)
{
    if (phead == NULL || *phead == NULL) {
        return;
    }

    list_free_nodes(*phead);            /* 先干掉所有真学生 */
    free(*phead);                       /* 再干掉头结点本身 */

    *phead = NULL;                      /* 把调用者的 head 置空，防野指针 */
}

/* ------------------------------------------------------------
 * 按学号升序插入
 *
 * 为什么参数是 StudentNode **phead（二级指针）？
 *   如果新学号比所有已有学生都小，新节点要挂到"头结点后面第一个位置"，
 *   这时需要修改 head->next；而头结点本身不用换。
 *   （若采用"不带头结点"的写法，插入到最前面就要改调用者的 head，
 *    那才必须用二级指针。）
 * ------------------------------------------------------------ */
int list_insert(StudentNode **phead, int id, const char *name, int score)
{
    if (phead == NULL || *phead == NULL) {
        return 0;
    }

    StudentNode *prev = *phead;         /* prev 从头结点开始 */

    /* 边走边比：只要下一个节点的学号比新学号小，就继续往后 */
    while (prev->next != NULL && prev->next->id < id) {
        prev = prev->next;
    }

    if (prev->next != NULL && prev->next->id == id) {
        return 0;                       /* 学号已存在，拒绝重复插入 */
    }

    StudentNode *node = (StudentNode *)malloc(sizeof(StudentNode));
    if (node == NULL) {
        return 0;
    }

    node->id = id;
    copy_name(node->name, MAX_NAME_LEN, name);
    node->score = score;

    node->next = prev->next;            /* 新节点接上后半截 */
    prev->next = node;                  /* 前半截接上新节点 */
    /* 这两行的顺序不能反，反了会丢失后半截链表 */

    return 1;
}

StudentNode *list_find(StudentNode *head, int id)
{
    if (head == NULL) {
        return NULL;
    }

    StudentNode *cur = head->next;      /* 跳过不存数据的头结点 */

    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

int list_delete(StudentNode **phead, int id)
{
    if (phead == NULL || *phead == NULL) {
        return 0;
    }

    StudentNode *prev = *phead;

    while (prev->next != NULL && prev->next->id != id) {
        prev = prev->next;
    }

    if (prev->next == NULL) {
        return 0;                       /* 走到底也没找到 */
    }

    StudentNode *victim = prev->next;
    prev->next = victim->next;          /* 跨过被删节点 */
    free(victim);                       /* 再释放它 */

    return 1;
}

/* ------------------------------------------------------------
 * 链表版冒泡排序（按成绩从高到低）
 *
 * 关键取舍：交换"节点里的数据"而不是"节点指针"。
 *   换指针要动 prev/cur/next 三根线，极易写错；
 *   换数据只是三次赋值，安全得多。
 *   代价是结构体大的时候拷贝成本高，但学生信息很小，无所谓。
 * ------------------------------------------------------------ */
void list_sort_by_score(StudentNode *head)
{
    if (head == NULL || head->next == NULL) {
        return;                         /* 0 个或 1 个学生，不用排 */
    }

    int swapped = 1;

    while (swapped) {
        swapped = 0;

        for (StudentNode *cur = head->next; cur->next != NULL; cur = cur->next) {
            if (cur->score < cur->next->score) {   /* 前面的分低就换上来 */
                int  tmp_id    = cur->id;
                char tmp_name[MAX_NAME_LEN];
                int  tmp_score = cur->score;

                copy_name(tmp_name, MAX_NAME_LEN, cur->name);

                cur->id    = cur->next->id;
                cur->score = cur->next->score;
                copy_name(cur->name, MAX_NAME_LEN, cur->next->name);

                cur->next->id    = tmp_id;
                cur->next->score = tmp_score;
                copy_name(cur->next->name, MAX_NAME_LEN, tmp_name);

                swapped = 1;
            }
        }
    }
    /* 因为只在 "严格小于" 时才交换，同分不会互换 —— 这叫稳定排序 */
}

int list_print(StudentNode *head)
{
    if (head == NULL) {
        return 0;
    }

    int n = 0;
    printf("%-6s %-14s %-6s\n", "ID", "NAME", "SCORE");

    for (StudentNode *cur = head->next; cur != NULL; cur = cur->next) {
        printf("%-6d %-14s %-6d\n", cur->id, cur->name, cur->score);
        n++;
    }

    return n;
}

int list_count(StudentNode *head)
{
    int n = 0;

    for (StudentNode *cur = (head ? head->next : NULL); cur != NULL; cur = cur->next) {
        n++;
    }

    return n;
}

/* ------------------------------------------------------------
 * v0.5 增量 1：按姓名查找
 *
 * 和 list_find（按学号）唯一的区别就是比较方式：
 *   学号是 int  -> 用 == 比
 *   姓名是字符串 -> 用 strcmp 比（返回 0 表示两个字符串完全一样）
 *
 * 注意：strcmp 是区分大小写的，"alice" 和 "Alice" 不算同一个。
 *       本项目约定精确匹配（不做模糊查找），要模糊查找得用 strstr。
 * ------------------------------------------------------------ */
StudentNode *list_find_by_name(StudentNode *head, const char *name)
{
    if (head == NULL || name == NULL) {
        return NULL;
    }

    StudentNode *cur = head->next;      /* 同样跳过不存数据的头结点 */

    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

/* ------------------------------------------------------------
 * v0.5 增量 2：统计（人数 / 平均分 / 最高分 / 最低分）
 *
 * 为什么用 double *avg_out 这种"指针参数"？
 *   C 函数的返回值只能有一个。这里要一次吐出三个数，
 *   就把三个变量的地址传进来，函数往地址里写值 —— 这叫"输出参数"。
 *   调用方写法：double avg; int mx; int mn;
 *              list_stats(head, &avg, &mx, &mn);
 *
 * 除零保护：一个人都没有时平均值是 0/0，数学上无意义，
 *   所以提前 return 0，并把三个输出参数都写成 0。
 * ------------------------------------------------------------ */
int list_stats(StudentNode *head, double *avg_out, int *max_out, int *min_out)
{
    if (head == NULL || avg_out == NULL || max_out == NULL || min_out == NULL) {
        return -1;
    }

    *avg_out = 0.0;
    *max_out = 0;
    *min_out = 0;

    int count = 0;
    long sum = 0;           /* 用 long 累加，防人多之后 int 溢出 */

    int max_score = 0;
    int min_score = 0;
    int first = 1;          /* 标记"这是第一个同学"，用来给 max/min 打初值 */

    for (StudentNode *cur = head->next; cur != NULL; cur = cur->next) {
        if (first) {
            max_score = cur->score;
            min_score = cur->score;
            first = 0;
        } else {
            if (cur->score > max_score) {
                max_score = cur->score;
            }
            if (cur->score < min_score) {
                min_score = cur->score;
            }
        }

        sum += cur->score;
        count++;
    }

    if (count == 0) {
        return 0;           /* 空链表：三个输出参数保持 0，调用方靠返回值 0 判断 */
    }

    *avg_out = (double)sum / count;   /* 先转 double 再除，否则会做整数除法丢掉小数 */
    *max_out = max_score;
    *min_out = min_score;

    return count;
}

/* ------------------------------------------------------------
 * 从文件读入建链
 *
 * 文件格式（每行一条）：
 *   1 Alice 89
 *   3 Carol 89
 * ------------------------------------------------------------ */
int list_load(StudentNode **phead, const char *filename)
{
    if (phead == NULL || *phead == NULL || filename == NULL) {
        return -1;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;                      /* 第一次运行还没有存档，属正常 */
    }

    /* 先清掉已有节点，避免"加载两次变成两份" */
    list_free_nodes(*phead);

    int loaded = 0;
    int id;
    int score;
    char name[MAX_NAME_LEN];
    char line[256];                 /* 读到的一整行原文 */

    /* ============================================================
     * 为什么不用 fscanf(fp, "%d %s %d", ...) ？——v0.4 的关键改动
     *
     * fscanf 的 %s 遇到"空格/制表符/换行"就算一个字段结束。
     * 一旦姓名里带空格（"张 三"、"王 小 明"），
     * 存档写成 "3 张 三 88"，读回来时 %s 只拿到 "张"，
     * 接着 %d 去读 "三" 又读不动 —— fscanf 返回值不足 3，
     * 循环当场中断，后面的记录全部读不进来。
     * 实测症状就是：存了 3 条，重启只回来 1 条。
     *
     * 解决办法：换成"自定义分隔符 + 逐行解析"。
     *   存档格式改成：学号|姓名|成绩   例如  3|张 三|88
     *   竖线 | 在正常姓名里不会出现，当分隔符最安全。
     * 读的时候：
     *   第 1 步 fgets 读一整行（空格也能原样保留）
     *   第 2 步 找到第 1 个 '|' 和最后一个 '|'，把一行切成三段
     *   第 3 步 中间那段（姓名）原样拷贝出来，不做任何拆分
     * ============================================================ */
    while (fgets(line, sizeof(line), fp) != NULL) {
        char *first_bar = strchr(line, '|');      /* 第 1 个竖线的位置 */
        char *last_bar = NULL;                    /* 最后一个竖线的位置 */
        char *name_start = NULL;
        int name_length = 0;

        if (first_bar == NULL) {
            continue;                             /* 没有竖线，不是合法记录，跳过 */
        }

        last_bar = strrchr(line, '|');            /* 从右往左找，取最后一个竖线 */
        if (last_bar == first_bar) {
            continue;                             /* 只有一个竖线，格式不对，跳过 */
        }

        /* ---------- 第 1 段：学号 ---------- */
        *first_bar = '\0';                        /* 先把第 1 个竖线"变成"字符串结束符，
                                                   * 这样 line 就只剩学号那一段 */
        id = atoi(line);                          /* atoi 把字符串 "3" 转成数字 3 */

        /* ---------- 第 2 段：姓名（在 first_bar+1 和 last_bar 之间） ---------- */
        name_start = first_bar + 1;               /* 跳过竖线指向姓名首字符 */
        name_length = (int)(last_bar - name_start);   /* 两个指针相减 = 中间有多少字符 */

        if (name_length <= 0) {
            continue;                             /* 姓名是空的，跳过 */
        }
        if (name_length >= MAX_NAME_LEN) {
            name_length = MAX_NAME_LEN - 1;       /* 太长就截断，留一格给 '\0' */
        }
        memcpy(name, name_start, (size_t)name_length);   /* 逐字节拷贝 */
        name[name_length] = '\0';                 /* 手工补上字符串结束符 */

        /* ---------- 第 3 段：成绩（在最后一个竖线之后） ---------- */
        score = atoi(last_bar + 1);

        if (!list_insert(phead, id, name, score)) {
            break;
        }
        loaded++;
    }

    fclose(fp);

    return loaded;
}

int list_save(StudentNode *head, const char *filename)
{
    if (head == NULL || filename == NULL) {
        return -1;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return -1;
    }

    int n = 0;

    for (StudentNode *cur = head->next; cur != NULL; cur = cur->next) {
        /* 分隔符用竖线 |，而不是空格。
         * 原因见 list_load 里的说明：姓名里可能带空格，
         * 用空格当分隔符会让"读回来"这一半逻辑彻底失效。 */
        fprintf(fp, "%d|%s|%d\n", cur->id, cur->name, cur->score);
        n++;
    }

    fclose(fp);

    return n;
}
