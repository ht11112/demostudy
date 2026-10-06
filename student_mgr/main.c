/*
 * main.c —— 学生成绩管理系统 v0.4（中文姓名 + 空格姓名 输入健壮版）
 *
 * v0.4 相比 v0.3 只改"输入这一层"，不动链表、不动文件存储：
 *   v0.3: scanf("%31s")  —— 遇空格就断！输入 "张 三" 只会读进 "张"
 *   v0.4: fgets + 手动去末尾换行 —— 能读进带空格的整行，也能读中文
 *
 * 三个必须掌握的知识点（本文件里都有注释标注）：
 *   知识点 1：fgets 会把末尾的 '\n' 也读进来，必须自己删掉
 *   知识点 2：删 '\n' 时不能用 sizeof(arr) 代替真实长度
 *   知识点 3：scanf 之后缓冲区里会残留 '\n'，必须"清缓冲"再 fgets
 *
 * 编译（Windows + GCC，在 VS Code 集成终端里执行）：
 *   gcc -Wall -Wextra -O2 main.c student.c -o student_mgr.exe
 * 运行：
 *   ./student_mgr.exe
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "student.h"

/* 前置声明：下面两个工具函数互相调用，先把名字告诉编译器，
 * 这样谁写在前面都不会报"找不到这个函数"。（这行也可以省略，
 * 办法是把 clear_input_buffer 整个挪到 read_line_capped 前面。） */
static void clear_input_buffer(void);
static int read_line_capped(char *buffer, int buffer_size);

/* ============================================================
 * 工具函数 1 + 2：读一整行 + 删掉末尾换行 + 丢掉超长残料
 *
 * 这一段是 v0.4 的核心，务必看懂。
 *
 * 【知识点 1】fgets 会把末尾的 '\n' 也读进来
 *   用户输入 张三<回车>，数组里实际是 '张''三''\n''\0'，
 *   不是 '张''三''\0'。这个 '\n' 会让 printf 多输出一个空行、
 *   让 strcmp 比名字失败、让存档文件每行末尾多一个空字符。
 *   所以读进来第一件事就是删掉它。
 *
 * 【知识点 2】删 '\n' 不能用 sizeof 当长度
 *   sizeof(buf) 求的是"数组总容量"（固定 32 格），
 *   strlen(buf) 求的是"实际存了几个字符"（可能只有 6 个）。
 *   用错 sizeof 会把根本没用到的那段内存也读进去。
 *
 * 【知识点 3】scanf 之后必须清缓冲区
 *   菜单用的是 scanf("%d", &choice)。用户敲 "1<回车>"，
 *   scanf 只拿走数字 1，那个回车 '\n' 还留在输入缓冲区。
 *   紧接着 fgets 一执行，它看到缓冲区里已经有东西了，
 *   立刻把那个孤零零的 '\n' 当"一整行"读走 ——
 *   结果姓名读进来是空的，程序像"跳过了一次输入"。
 *   所以每次 scanf 之后都要 clear_input_buffer()。
 *
 * 【fgets 的第三个参数 stdin】
 *   stdin 是"标准输入"的缩写，代表键盘。
 *   fgets(buf, size, stdin) = 最多读 size-1 个字符，
 *   读满或遇到换行就停，并在末尾自动补 '\0'，
 *   所以永远留了一格给结束符，不会溢出。
 *
 * 【为什么要处理"超长输入"】
 *   姓名数组只有 32 格，用户硬敲 60 个字符时，fgets 只读走前 31 个，
 *   剩下的还堆在缓冲区里，下一次读姓名会被当成新输入 ——
 *   这就是"输入串行/错位" bug。本例的处理：如果这一行没读到换行，
 *   说明被截断了，就把这一行剩下的字符全部丢干净。
 * ============================================================ */
static int read_line_capped(char *buffer, int buffer_size)
{
    int has_newline = 0;         /* 1 表示这一行完整读到了换行 */
    size_t real_length = 0;      /* 实际读进来的字符数，不含结尾 '\0' */

    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return 0;                /* 读到 EOF（比如 Ctrl+Z），当失败处理 */
    }

    real_length = strlen(buffer);            /* 用 strlen，不要用 sizeof */
    if (real_length > 0 && buffer[real_length - 1] == '\n') {
        has_newline = 1;                     /* 这一行读全了 */
        buffer[real_length - 1] = '\0';      /* 把换行改成字符串结束符 */
    }

    if (has_newline == 0) {
        clear_input_buffer();                /* 被截断，把这一行余下的丢掉 */
    }

    return 1;
}

/* ============================================================
 * 工具函数 2：清空输入缓冲区里残留的字符（被上面调用）
 *
 * 怎么清：
 *   用 getchar 一个字符一个字符地读出来丢掉，
 *   一直读到换行（说明这一行读完了）或者读到文件结束（EOF）为止。
 * ============================================================ */
static void clear_input_buffer(void)
{
    int current_char = 0;

    while ((current_char = getchar()) != '\n' && current_char != EOF) {
        /* 空循环：读出来即丢弃，什么都不用做 */
    }
}

/* ============================================================
 * 菜单打印
 * ============================================================ */
static void print_menu(void)
{
    printf("\n");
    printf("========== Student Manager v0.5 ==========\n");
    printf("  1. add student\n");
    printf("  2. list all\n");
    printf("  3. find by id\n");
    printf("  4. delete by id\n");
    printf("  5. sort by score (high to low)\n");
    printf("  6. save to file\n");
    printf("  7. load from file\n");
    printf("  8. find by name          <-- new in v0.5\n");
    printf("  9. show statistics       <-- new in v0.5\n");
    printf("  0. quit\n");
    printf("==========================================\n");
    printf("choice: ");
}

/* ============================================================
 * 主流程
 * ============================================================ */
int main(void)
{
    StudentNode *head = NULL;     /* 链表头指针，一切操作的入口 */
    int choice = -1;              /* 菜单选择 */
    int is_finished = 0;          /* 0 表示继续，1 表示要退出 */

    head = list_create();
    if (head == NULL) {
        printf("[ERROR] create list failed\n");
        return 1;
    }

    printf("Student Manager v0.4 (list + file + chinese name)\n");

    while (is_finished == 0) {
        print_menu();

        /* 注意：这里如果用 scanf("%d") 失败（用户敲了字母），
         * 输入缓冲区会一直卡着，形成死循环。
         * 用 while 检查返回值、失败就清缓冲，是最省心的写法。 */
        if (scanf("%d", &choice) != 1) {
            printf("[ERROR] please enter a number\n");
            clear_input_buffer();
            continue;
        }

        /* scanf 读走数字后，回车还留在缓冲区，先清掉再往下走，
         * 否则后面 fgets 会读到一个空的换行。 */
        clear_input_buffer();

        if (choice == 1) {
            /* ---------- 添加学生 ---------- */
            int  new_id = 0;
            int  new_score = 0;
            char name_buffer[MAX_NAME_LEN];

            printf("id    : ");
            if (scanf("%d", &new_id) != 1) {
                printf("[ERROR] invalid id\n");
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();        /* 清掉 id 后面的回车 */

            printf("name  : ");
            if (read_line_capped(name_buffer, MAX_NAME_LEN) == 0) {
                printf("[ERROR] name cannot be empty\n");
                continue;
            }
            if (strlen(name_buffer) == 0) {
                printf("[ERROR] name cannot be empty\n");
                continue;
            }

            printf("score : ");
            if (scanf("%d", &new_score) != 1) {
                printf("[ERROR] invalid score\n");
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();

            if (list_insert(&head, new_id, name_buffer, new_score) == 1) {
                printf("[OK] added: id=%d name=%s score=%d\n",
                       new_id, name_buffer, new_score);
            } else {
                printf("[ERROR] insert failed (duplicate id or no memory)\n");
            }

        } else if (choice == 2) {
            /* ---------- 列出全部 ---------- */
            int total = list_print(head);
            printf("[INFO] total = %d\n", total);

        } else if (choice == 3) {
            /* ---------- 按学号查找 ---------- */
            int find_id = 0;
            StudentNode *found = NULL;

            printf("id to find: ");
            if (scanf("%d", &find_id) != 1) {
                printf("[ERROR] invalid id\n");
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();

            found = list_find(head, find_id);
            if (found != NULL) {
                printf("[FOUND] id=%d name=%s score=%d\n",
                       found->id, found->name, found->score);
            } else {
                printf("[NOT FOUND] id=%d\n", find_id);
            }

        } else if (choice == 4) {
            /* ---------- 按学号删除 ---------- */
            int delete_id = 0;

            printf("id to delete: ");
            if (scanf("%d", &delete_id) != 1) {
                printf("[ERROR] invalid id\n");
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();

            if (list_delete(&head, delete_id) == 1) {
                printf("[OK] deleted id=%d\n", delete_id);
            } else {
                printf("[NOT FOUND] id=%d\n", delete_id);
            }

        } else if (choice == 5) {
            /* ---------- 按成绩降序排序 ---------- */
            list_sort_by_score(head);
            printf("[OK] sorted by score (high to low)\n");
            list_print(head);

        } else if (choice == 6) {
            /* ---------- 存档 ---------- */
            int saved = list_save(head, DATA_FILE);

            if (saved >= 0) {
                printf("[OK] saved %d record(s) to %s\n", saved, DATA_FILE);
            } else {
                printf("[ERROR] save failed: %s\n", DATA_FILE);
            }

        } else if (choice == 7) {
            /* ---------- 读档 ---------- */
            int loaded = list_load(&head, DATA_FILE);

            if (loaded >= 0) {
                printf("[OK] loaded %d record(s) from %s\n", loaded, DATA_FILE);
                list_print(head);
            } else {
                printf("[ERROR] load failed: %s\n", DATA_FILE);
            }

        } else if (choice == 8) {
            /* ---------- v0.5 新功能：按姓名查找 ---------- */
            char name_buffer[MAX_NAME_LEN];
            StudentNode *found = NULL;

            printf("name to find: ");
            if (read_line_capped(name_buffer, MAX_NAME_LEN) == 0) {
                printf("[ERROR] name cannot be empty\n");
                continue;
            }

            found = list_find_by_name(head, name_buffer);
            if (found != NULL) {
                printf("[FOUND] id=%d name=%s score=%d\n",
                       found->id, found->name, found->score);
            } else {
                printf("[NOT FOUND] name=%s\n", name_buffer);
            }

        } else if (choice == 9) {
            /* ---------- v0.5 新功能：统计信息 ---------- */
            double avg = 0.0;
            int    max_score = 0;
            int    min_score = 0;

            /* 三个结果通过三个指针"带出来" */
            int n = list_stats(head, &avg, &max_score, &min_score);

            if (n > 0) {
                printf("[STATS] count=%d avg=%.2f max=%d min=%d\n",
                       n, avg, max_score, min_score);
            } else {
                printf("[INFO] no student yet\n");
            }

        } else if (choice == 0) {
            printf("bye\n");
            is_finished = 1;

        } else {
            printf("[ERROR] unknown choice: %d\n", choice);
        }
    }

    /* 退出前把链表内存还回去。
     * 传 &head 而不是 head，因为函数内部要把 head 置成 NULL，
     * 只传值的话改的是函数里的副本，main 里的 head 还是野指针。 */
    list_destroy(&head);

    return 0;
}
