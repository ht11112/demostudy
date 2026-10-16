# ================================================================
# Makefile —— 学生成绩管理系统（多文件项目的标准构建脚本）
#
# 为什么需要 Makefile？
#   手写 `gcc -Wall -Wextra -O2 main.c student.c -o student_mgr.exe` 每次都要敲一遍，
#   文件一多（STM32 工程几十个 .c）就彻底不可维护了。
#   make 的核心逻辑就一句话：
#       "目标文件 比 依赖文件 旧（或不存在），才重新构建"
#   —— 这就是它比"每次都全量编译"快的原因。
#
# ================================================================
# 【Windows 用户必读】
#
#   MinGW 自带的 make 命令叫 mingw32-make（不是 make），所以：
#
#     mingw32-make            # 增量构建，产出 student_mgr.exe
#     mingw32-make run        # 构建 + 运行
#     mingw32-make clean      # 清理所有中间产物
#
#   嫌麻烦可以起个别名（在本文件里加一行 Makefile 是做不到的，
#   需要在 shell 里敲）：  alias make=mingw32-make
#
# ================================================================
# 【Makefile 语法要点 —— 只有三样东西要记】
#
#   1) 变量赋值       CC = gcc
#   2) 规则           target: prerequisites
#                         <TAB>命令        <- 命令前必须是 TAB，不能是空格！
#   3) 自动变量       $@ = 目标名   $< = 第一个依赖   $^ = 所有依赖
#
#   最常见的报错 "missing separator"，99% 是因为命令缩进用了空格而非 TAB。
# ================================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGET  = student_mgr.exe
OBJS    = main.o student.o

# 第一个规则是默认规则：直接敲 mingw32-make 时执行它
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@
	@echo "=== build done: $(TARGET) ==="

# 每个 .o 由对应的 .c 编译而来
# $@ = main.o   $< = main.c
main.o: main.c student.h
	$(CC) $(CFLAGS) -c $< -o $@

student.o: student.c student.h
	$(CC) $(CFLAGS) -c $< -o $@

# .PHONY 声明"假目标"：这些名字不是真实文件，每次都执行
.PHONY: run clean rebuild

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "=== cleaned ==="

rebuild: clean $(TARGET)

# ================================================================
# 【增量构建怎么验证？】动手做一遍就懂了
#
#   1) mingw32-make        -> 编译两个 .c，产出两个 .o + 一个 exe
#   2) 立刻再敲一次         -> 输出 "nothing to be done"，因为没文件变新
#   3) 用记事本改一下 student.c 存盘（哪怕只加个空格）
#   4) 再敲 mingw32-make    -> 只重新编译 student.o，main.o 复用！
#
# ================================================================
# 【对照：不用 Makefile 时在 VS Code 里怎么写】
#
#   .vscode/tasks.json 的 args 必须显式列出所有源文件：
#       "args": ["-fdiagnostics-color=always", "-g", "${fileDirname}/*.c",
#                "-o", "${fileDirname}/${fileBasenameNoExtension}.exe"]
#   只写 "${file}" 的话，多文件项目会在链接阶段报
#   undefined reference to ... （因为只编译了当前那一个文件）
# ================================================================
