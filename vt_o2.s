	.file	"volatile_test.c"
	.text
	.section .rdata,"dr"
.LC0:
	.ascii "plain   : ticks=%ld\12\0"
.LC1:
	.ascii "volatile: ticks=%ld\12\0"
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	call	__main
	call	clock
	movl	$1, plain_flag(%rip)
	movl	%eax, %ebx
	call	clock
	leaq	.LC0(%rip), %rcx
	subl	%ebx, %eax
	movl	%eax, %edx
	call	printf
	call	clock
	movl	%eax, %ebx
	movl	$200000000, %eax
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L2:
	movl	$1, vol_flag(%rip)
	subl	$2, %eax
	movl	$1, vol_flag(%rip)
	jne	.L2
	call	clock
	leaq	.LC1(%rip), %rcx
	subl	%ebx, %eax
	movl	%eax, %edx
	call	printf
	xorl	%eax, %eax
	addq	$32, %rsp
	popq	%rbx
	ret
	.seh_endproc
	.globl	vol_flag
	.bss
	.align 4
vol_flag:
	.space 4
	.globl	plain_flag
	.align 4
plain_flag:
	.space 4
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (x86_64-win32-seh-rev0, Built by MinGW-Builds project) 15.2.0"
	.def	clock;	.scl	2;	.type	32;	.endef
	.def	printf;	.scl	2;	.type	32;	.endef
