/*
 * state_machine.c —— 阶段 1 官方验收项③：有限状态机（FSM）
 *
 * 为什么嵌入式一定要学状态机？
 *   单片机程序的主循环里，最怕的就是一堆 if/else 嵌套。
 *   "待机 -> 前进 -> 遇障停车 -> 报警" 这种有明确阶段、且只允许特定跳转的逻辑，
 *   用状态机写出来一眼就能看懂，改需求也不容易改出 bug。
 *   你 STM32 项目里的 ToF 停车逻辑、BMS 的 RUN/STANDBY 切换，本质都是状态机。
 *
 * 本文件用一个"小车运动控制器"做例子，覆盖三种最常见的状态机写法：
 *   写法 1：switch + 枚举      —— 最直白，新手首选
 *   写法 2：状态转移表（表驱动）—— 加状态/加事件只改表格，不改代码
 *   写法 3：函数指针表          —— 进阶写法，每个状态的"进入动作"独立成函数
 *
 * 编译运行：
 *   gcc -Wall -Wextra -O2 state_machine.c -o state_machine.exe
 *   ./state_machine.exe
 */

#include <stdio.h>

/* ============================================================
 * 一、状态与事件的定义
 * ============================================================ */

/* 用 enum 而不是 #define 0/1/2：
 * enum 会让编译器帮你检查类型，调试时也能看到名字而不是数字。 */
typedef enum {
    ST_IDLE = 0,      /* 待机 */
    ST_RUNNING,       /* 前进 */
    ST_OBSTACLE,      /* 遇到障碍，停车 */
    ST_ERROR,         /* 出错，需要人工复位 */
    ST_COUNT          /* 哨兵：状态总数，专门用来定义数组大小 */
} CarState;

typedef enum {
    EV_START = 0,     /* 收到启动指令 */
    EV_OBSTACLE,      /* ToF 报障碍 */
    EV_CLEAR,         /* 障碍消失 */
    EV_FAULT,         /* 故障（电机过流等） */
    EV_RESET,         /* 复位 */
    EV_COUNT          /* 事件总数 */
} CarEvent;

/* 状态名数组：打印时用，调试神器。
 * 用 [ST_COUNT] 指定长度，将来加状态时数组会自动跟着变。 */
static const char *state_name[ST_COUNT] = {
    "IDLE", "RUNNING", "OBSTACLE", "ERROR"
};

static const char *event_name[EV_COUNT] = {
    "START", "OBSTACLE", "CLEAR", "FAULT", "RESET"
};

/* ============================================================
 * 二、状态机的"壳"：一个结构体装下当前状态和历史
 * ============================================================ */
typedef struct {
    CarState state;        /* 当前状态 */
    int      event_cnt;    /* 处理过多少个事件（统计用） */
    int      reject_cnt;   /* 被拒绝的非法事件个数 */
} CarFsm;

static void fsm_init(CarFsm *fsm)
{
    fsm->state = ST_IDLE;   /* 上电默认待机 */
    fsm->event_cnt = 0;
    fsm->reject_cnt = 0;
}

/* ============================================================
 * 三、写法 1：switch + 枚举（最直白）
 *
 * 返回值 1 表示"这个事件被接受了，状态可能变了"
 * 返回值 0 表示"当前状态下不认这个事件，忽略掉"
 * ------------------------------------------------------------ */
static int fsm_switch_style(CarFsm *fsm, CarEvent ev)
{
    CarState next = fsm->state;   /* 先假定状态不变 */
    int accepted = 0;             /* 单独记一个"这个事件认不认"，别用状态猜 */

    /* 【这里有个坑，务必看清】
     * 我第一版把返回值写成 `(next != fsm->state) || (ev == EV_START)`，
     * 想表达"状态变了 或 收到启动事件"，结果测试直接 FAIL。
     * 原因：EV_START 是枚举的第 0 项，它的值就是 0；
     *       而 CarEvent 的第一个枚举值也是 0（EV_START 自己）。
     *       所以这行实际变成了 "状态没变 或 事件编号是 0"，
     *       而当有人传进来 0 号事件时条件恒真，非法事件也被判成"接受"。
     * 教训：判断"事件是否被接受"应该由转移逻辑自己明确给出，
     *       不要事后用别的条件去推测 —— 推测出来的条件很容易恒真/恒假。 */
    switch (fsm->state) {
    case ST_IDLE:
        if (ev == EV_START) {
            next = ST_RUNNING;
            accepted = 1;
        } else if (ev == EV_FAULT) {
            next = ST_ERROR;
            accepted = 1;
        }
        /* 其他事件在 IDLE 下都忽略，accepted 保持 0 */
        break;

    case ST_RUNNING:
        if (ev == EV_OBSTACLE) {
            next = ST_OBSTACLE;
            accepted = 1;
        } else if (ev == EV_FAULT) {
            next = ST_ERROR;
            accepted = 1;
        } else if (ev == EV_START) {
            next = ST_IDLE;       /* 再按一次启动 = 停机 */
            accepted = 1;
        }
        break;

    case ST_OBSTACLE:
        if (ev == EV_CLEAR) {
            next = ST_RUNNING;    /* 障碍消失，自动恢复前进 */
            accepted = 1;
        } else if (ev == EV_FAULT) {
            next = ST_ERROR;
            accepted = 1;
        }
        break;

    case ST_ERROR:
        /* 错误态只认复位，别的都不理 —— 这是"安全默认拒绝"原则 */
        if (ev == EV_RESET) {
            next = ST_IDLE;
            accepted = 1;
        }
        break;

    default:
        break;
    }

    fsm->state = next;

    return accepted;
}

/* ============================================================
 * 四、写法 2：状态转移表（表驱动）
 *
 * 把"什么状态 + 什么事件 -> 去哪个状态"直接写成一张二维表。
 * 好处：逻辑改动只改表格数据，代码一行不动；
 *       审表比审 if/else 快得多，也更容易看出"有没有漏掉的组合"。
 *
 * 表的读法：trans[当前状态][事件] = 下一个状态
 * 用 -1 表示"不处理，保持原状态"。
 * ------------------------------------------------------------ */
#define KEEP (-1)

static const int trans[ST_COUNT][EV_COUNT] = {
    /*  state       START      OBSTACLE   CLEAR      FAULT      RESET   */
    /* IDLE     */ { ST_RUNNING, KEEP,     KEEP,      ST_ERROR,   KEEP   },
    /* RUNNING  */ { ST_IDLE,    ST_OBSTACLE, KEEP,   ST_ERROR,   KEEP   },
    /* OBSTACLE */ { KEEP,       KEEP,     ST_RUNNING, ST_ERROR,  KEEP   },
    /* ERROR    */ { KEEP,       KEEP,     KEEP,      KEEP,       ST_IDLE}
};

static int fsm_table_style(CarFsm *fsm, CarEvent ev)
{
    if (fsm->state >= ST_COUNT || ev >= EV_COUNT) {
        return 0;                       /* 越界保护，防止数组越界读 */
    }

    int next = trans[fsm->state][ev];

    if (next == KEEP) {
        return 0;                       /* 查表得到"不动"，说明这个事件被忽略 */
    }

    fsm->state = (CarState)next;

    return 1;
}

/* ============================================================
 * 五、写法 3：函数指针表（进阶）
 *
 * 每个状态有自己的"进入动作"函数，表里放函数地址。
 * 以后要加"进入状态时点亮 LED / 发串口日志"，就直接往对应函数里加。
 * ------------------------------------------------------------ */
typedef void (*StateAction)(void);

static void on_enter_idle(void)     { printf("      [action] stop motor, LED green\n"); }
static void on_enter_running(void)  { printf("      [action] start motor, LED blue\n");  }
static void on_enter_obstacle(void) { printf("      [action] brake motor, LED yellow\n");}
static void on_enter_error(void)    { printf("      [action] cut motor power, LED red\n");}

/* 注意：数组下标必须按 CarState 的顺序一一对应，
 * 顺序错了就会出现"进 IDLE 却点亮红灯"这种查不出来的 bug。 */
static const StateAction enter_action[ST_COUNT] = {
    on_enter_idle,      /* [ST_IDLE]     */
    on_enter_running,   /* [ST_RUNNING]  */
    on_enter_obstacle,  /* [ST_OBSTACLE] */
    on_enter_error      /* [ST_ERROR]    */
};

static int fsm_funcptr_style(CarFsm *fsm, CarEvent ev)
{
    CarState old = fsm->state;
    int accepted = fsm_table_style(fsm, ev);   /* 复用表驱动做转移判断 */

    if (accepted && fsm->state != old) {
        enter_action[fsm->state]();            /* 状态真变了，才执行进入动作 */
    }

    return accepted;
}

/* ============================================================
 * 六、测试用例
 * ============================================================ */

/* 用同一串事件序列跑三种写法，检查最终状态是否完全一致 */
static void cross_check(void)
{
    CarEvent script[] = {
        EV_START, EV_OBSTACLE, EV_CLEAR,
        EV_START, EV_FAULT, EV_RESET,
        EV_FAULT, EV_RESET, EV_OBSTACLE, EV_CLEAR
    };
    int n = (int)(sizeof(script) / sizeof(script[0]));

    CarFsm a;
    CarFsm b;
    CarFsm c;

    fsm_init(&a);
    fsm_init(&b);
    fsm_init(&c);

    int mismatch = 0;

    for (int i = 0; i < n; i++) {
        fsm_switch_style(&a, script[i]);
        fsm_table_style(&b, script[i]);
        fsm_funcptr_style(&c, script[i]);

        if (a.state != b.state || b.state != c.state) {
            printf("MISMATCH at step %d: switch=%s table=%s funcptr=%s\n",
                   i, state_name[a.state], state_name[b.state], state_name[c.state]);
            mismatch = 1;
        }
    }

    printf("cross check: switch / table / funcptr final state = %s / %s / %s\n",
           state_name[a.state], state_name[b.state], state_name[c.state]);
    printf("cross check result: %s\n", mismatch ? "FAIL" : "PASS");
}

/* 检查非法事件确实被忽略（安全性测试） */
static void illegal_event_test(void)
{
    CarFsm fsm;

    fsm_init(&fsm);                        /* IDLE */

    int ok1 = (fsm_switch_style(&fsm, EV_CLEAR) == 0);      /* IDLE 下 CLEAR 无意义 */
    int ok2 = (fsm.state == ST_IDLE);                       /* 状态应原地不动 */

    fsm.state = ST_ERROR;
    int ok3 = (fsm_switch_style(&fsm, EV_START) == 0);      /* ERROR 下不许直接启动 */
    int ok4 = (fsm.state == ST_ERROR);

    printf("illegal event test: %s\n",
           (ok1 && ok2 && ok3 && ok4) ? "PASS" : "FAIL");
}

/* 手动逐事件走一遍，把过程打印出来看清楚 */
static void walkthrough(void)
{
    CarFsm fsm;
    CarEvent script[] = { EV_START, EV_OBSTACLE, EV_CLEAR, EV_FAULT, EV_RESET };
    int n = (int)(sizeof(script) / sizeof(script[0]));

    fsm_init(&fsm);

    printf("start state: %s\n", state_name[fsm.state]);

    for (int i = 0; i < n; i++) {
        CarState old = fsm.state;
        int accepted = fsm_funcptr_style(&fsm, script[i]);

        printf("event %-8s : %s -> %s   (%s)\n",
               event_name[script[i]],
               state_name[old],
               state_name[fsm.state],
               accepted ? "accepted" : "ignored");
    }
}

int main(void)
{
    printf("=== 1) state transition walkthrough ===\n");
    walkthrough();

    printf("\n=== 2) cross check: three implementations ===\n");
    cross_check();

    printf("\n=== 3) illegal event handling ===\n");
    illegal_event_test();

    printf("\nSUMMARY: done\n");

    return 0;
}
