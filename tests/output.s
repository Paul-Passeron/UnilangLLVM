	.text
	.file	"main"
	.globl	strlen
	.p2align	4, 0x90
	.type	strlen,@function
strlen:
	.cfi_startproc
	movq	%rdi, -8(%rsp)
	movl	$0, -12(%rsp)
	.p2align	4, 0x90
.LBB0_1:
	movq	-8(%rsp), %rax
	movslq	-12(%rsp), %rcx
	cmpb	$0, (%rax,%rcx)
	je	.LBB0_3
	incl	-12(%rsp)
	jmp	.LBB0_1
.LBB0_3:
	movl	-12(%rsp), %eax
	retq
.Lfunc_end0:
	.size	strlen, .Lfunc_end0-strlen
	.cfi_endproc

	.globl	intlen
	.p2align	4, 0x90
	.type	intlen,@function
intlen:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	%edi, %eax
	movl	%edi, 20(%rsp)
	movl	%esi, 16(%rsp)
	cltd
	idivl	%esi
	movl	%eax, 12(%rsp)
	testl	%eax, %eax
	je	.LBB1_2
	movl	12(%rsp), %edi
	movl	16(%rsp), %esi
	callq	intlen@PLT
	incl	%eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.LBB1_2:
	.cfi_def_cfa_offset 32
	movl	$1, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	intlen, .Lfunc_end1-intlen
	.cfi_endproc

	.globl	print
	.p2align	4, 0x90
	.type	print,@function
print:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	%rdi, 8(%rsp)
	callq	strlen@PLT
	movl	$1, %edi
	movl	$1, %esi
	movq	%rbx, %rdx
	movl	%eax, %ecx
	xorl	%eax, %eax
	callq	syscall@PLT
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	print, .Lfunc_end2-print
	.cfi_endproc

	.globl	printc
	.p2align	4, 0x90
	.type	printc,@function
printc:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movb	%dil, 7(%rsp)
	leaq	7(%rsp), %rdx
	movl	$1, %edi
	movl	$1, %esi
	movl	$1, %ecx
	xorl	%eax, %eax
	callq	syscall@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end3:
	.size	printc, .Lfunc_end3-printc
	.cfi_endproc

	.globl	print_int
	.p2align	4, 0x90
	.type	print_int,@function
print_int:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, 4(%rsp)
	movslq	%edi, %rax
	imulq	$1717986919, %rax, %rax
	movq	%rax, %rcx
	shrq	$63, %rcx
	sarq	$34, %rax
	addl	%ecx, %eax
	movl	%eax, (%rsp)
	je	.LBB4_2
	movl	(%rsp), %edi
	callq	print_int@PLT
.LBB4_2:
	movslq	4(%rsp), %rdi
	imulq	$1717986919, %rdi, %rax
	movq	%rax, %rcx
	shrq	$63, %rcx
	sarq	$34, %rax
	addl	%ecx, %eax
	addl	%eax, %eax
	leal	(%rax,%rax,4), %eax
	subl	%eax, %edi
	addl	$48, %edi
	callq	printc@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end4:
	.size	print_int, .Lfunc_end4-print_int
	.cfi_endproc

	.globl	nl
	.p2align	4, 0x90
	.type	nl,@function
nl:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$10, %edi
	callq	printc@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end5:
	.size	nl, .Lfunc_end5-nl
	.cfi_endproc

	.globl	print_iob_message
	.p2align	4, 0x90
	.type	print_iob_message,@function
print_iob_message:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, 4(%rsp)
	movl	%esi, (%rsp)
	movl	$.L__unnamed_1, %edi
	callq	print@PLT
	movl	4(%rsp), %edi
	callq	print_int@PLT
	movl	$.L__unnamed_2, %edi
	callq	print@PLT
	movl	(%rsp), %edi
	callq	print_int@PLT
	movl	$10, %edi
	callq	printc@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end6:
	.size	print_iob_message, .Lfunc_end6-print_iob_message
	.cfi_endproc

	.globl	string_0
	.p2align	4, 0x90
	.type	string_0,@function
string_0:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	%rsi, 8(%rsp)
	movq	%rsi, %rdi
	callq	strlen@PLT
	movl	%eax, 8(%rbx)
	movl	%eax, %edi
	callq	malloc@PLT
	movq	%rax, (%rbx)
	movl	$0, 4(%rsp)
	.p2align	4, 0x90
.LBB7_1:
	movl	4(%rsp), %eax
	cmpl	8(%rbx), %eax
	jge	.LBB7_3
	movq	8(%rsp), %rax
	movslq	4(%rsp), %rcx
	movzbl	(%rax,%rcx), %eax
	movq	(%rbx), %rdx
	movb	%al, (%rdx,%rcx)
	incl	4(%rsp)
	jmp	.LBB7_1
.LBB7_3:
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end7:
	.size	string_0, .Lfunc_end7-string_0
	.cfi_endproc

	.globl	string_1
	.p2align	4, 0x90
	.type	string_1,@function
string_1:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movb	%sil, 15(%rsp)
	movl	$1, 8(%rdi)
	movl	$1, %edi
	callq	malloc@PLT
	movq	%rax, (%rbx)
	movzbl	15(%rsp), %ecx
	movb	%cl, (%rax)
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end8:
	.size	string_1, .Lfunc_end8-string_1
	.cfi_endproc

	.globl	string_print
	.p2align	4, 0x90
	.type	string_print,@function
string_print:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	(%rdi), %rdx
	movl	8(%rdi), %ecx
	movl	$1, %edi
	movl	$1, %esi
	xorl	%eax, %eax
	callq	syscall@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end9:
	.size	string_print, .Lfunc_end9-string_print
	.cfi_endproc

	.globl	string_println
	.p2align	4, 0x90
	.type	string_println,@function
string_println:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	callq	string_print@PLT
	movl	$10, %edi
	callq	printc@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end10:
	.size	string_println, .Lfunc_end10-string_println
	.cfi_endproc

	.globl	string_len
	.p2align	4, 0x90
	.type	string_len,@function
string_len:
	.cfi_startproc
	movl	8(%rdi), %eax
	retq
.Lfunc_end11:
	.size	string_len, .Lfunc_end11-string_len
	.cfi_endproc

	.globl	string_at
	.p2align	4, 0x90
	.type	string_at,@function
string_at:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%esi, 4(%rsp)
	testl	%esi, %esi
	js	.LBB12_2
	cmpl	8(%rdi), %esi
	jge	.LBB12_2
	movq	(%rdi), %rax
	movslq	4(%rsp), %rcx
	movzbl	(%rax,%rcx), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.LBB12_2:
	.cfi_def_cfa_offset 16
	movl	4(%rsp), %eax
	movl	8(%rdi), %esi
	movl	%eax, %edi
	callq	print_iob_message@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end12:
	.size	string_at, .Lfunc_end12-string_at
	.cfi_endproc

	.globl	string_foo
	.p2align	4, 0x90
	.type	string_foo,@function
string_foo:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%esi, 4(%rsp)
	movl	$.L__unnamed_3, %edi
	callq	print@PLT
	movl	4(%rsp), %eax
	movl	(%rax), %edi
	callq	print_int@PLT
	callq	nl@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end13:
	.size	string_foo, .Lfunc_end13-string_foo
	.cfi_endproc

	.globl	string_append
	.p2align	4, 0x90
	.type	string_append,@function
string_append:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$40, %rsp
	.cfi_def_cfa_offset 64
	.cfi_offset %rbx, -24
	.cfi_offset %r14, -16
	movq	%rdi, %rbx
	movl	%edx, 32(%rsp)
	movq	%rsi, 24(%rsp)
	movq	(%rdi), %rax
	movq	%rax, 16(%rsp)
	movl	8(%rdi), %eax
	movl	%eax, 8(%rsp)
	leaq	24(%rsp), %rdi
	callq	string_len@PLT
	movl	%eax, 12(%rsp)
	addl	8(%rsp), %eax
	movl	%eax, 8(%rbx)
	movq	%rax, %rdi
	callq	malloc@PLT
	movq	%rax, (%rbx)
	movl	$0, 4(%rsp)
	.p2align	4, 0x90
.LBB14_1:
	movl	4(%rsp), %eax
	cmpl	8(%rsp), %eax
	jge	.LBB14_3
	movq	16(%rsp), %rax
	movslq	4(%rsp), %rcx
	movzbl	(%rax,%rcx), %eax
	movq	(%rbx), %rdx
	movb	%al, (%rdx,%rcx)
	incl	4(%rsp)
	jmp	.LBB14_1
.LBB14_3:
	movl	$0, 4(%rsp)
	leaq	24(%rsp), %r14
	.p2align	4, 0x90
.LBB14_4:
	movl	4(%rsp), %eax
	cmpl	12(%rsp), %eax
	jge	.LBB14_6
	movl	4(%rsp), %esi
	movq	%r14, %rdi
	callq	string_at@PLT
	movq	(%rbx), %rcx
	movl	8(%rsp), %edx
	addl	4(%rsp), %edx
	movslq	%edx, %rdx
	movb	%al, (%rcx,%rdx)
	incl	4(%rsp)
	jmp	.LBB14_4
.LBB14_6:
	movq	16(%rsp), %rdi
	callq	free@PLT
	addq	$40, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end14:
	.size	string_append, .Lfunc_end14-string_append
	.cfi_endproc

	.globl	string_join
	.p2align	4, 0x90
	.type	string_join,@function
string_join:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$40, %rsp
	.cfi_def_cfa_offset 64
	.cfi_offset %rbx, -24
	.cfi_offset %r14, -16
	movq	%rdi, %rbx
	movl	%edx, 16(%rsp)
	movq	%rsi, 8(%rsp)
	movb	%cl, 7(%rsp)
	leaq	24(%rsp), %r14
	movq	%r14, %rdi
	movl	%ecx, %esi
	callq	string_1@PLT
	movq	24(%rsp), %rsi
	movl	32(%rsp), %edx
	movq	%rbx, %rdi
	callq	string_append@PLT
	movq	%r14, %rdi
	callq	string_destroy@PLT
	movl	16(%rsp), %edx
	movq	8(%rsp), %rsi
	movq	%rbx, %rdi
	callq	string_append@PLT
	movq	%r14, %rdi
	callq	string_destroy@PLT
	addq	$40, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end15:
	.size	string_join, .Lfunc_end15-string_join
	.cfi_endproc

	.globl	string_destroy
	.p2align	4, 0x90
	.type	string_destroy,@function
string_destroy:
	.cfi_startproc
	cmpl	$0, 8(%rdi)
	je	.LBB16_1
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movl	$0, 8(%rdi)
	movq	(%rdi), %rax
	movq	%rdi, %rbx
	movq	%rax, %rdi
	callq	free@PLT
	movq	$0, (%rbx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB16_1:
	.cfi_restore %rbx
	retq
.Lfunc_end16:
	.size	string_destroy, .Lfunc_end16-string_destroy
	.cfi_endproc

	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r12
	.cfi_def_cfa_offset 32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	subq	$120, %rsp
	.cfi_def_cfa_offset 160
	.cfi_offset %rbx, -40
	.cfi_offset %r12, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
	leaq	104(%rsp), %rbx
	movl	$.L__unnamed_4, %esi
	movq	%rbx, %rdi
	callq	string_0@PLT
	leaq	88(%rsp), %r14
	movl	$.L__unnamed_5, %esi
	movq	%r14, %rdi
	callq	string_0@PLT
	leaq	56(%rsp), %rdi
	movl	$.L__unnamed_6, %esi
	callq	string_0@PLT
	movl	64(%rsp), %eax
	movq	56(%rsp), %rcx
	movq	%rcx, 72(%rsp)
	movl	%eax, 80(%rsp)
	leaq	72(%rsp), %r15
	movq	%r15, %rdi
	callq	string_println@PLT
	movq	%r15, %rdi
	callq	string_destroy@PLT
	leaq	40(%rsp), %r15
	movl	$.L__unnamed_7, %esi
	movq	%r15, %rdi
	callq	string_0@PLT
	movl	48(%rsp), %eax
	movq	40(%rsp), %rcx
	movq	%rcx, 24(%rsp)
	movl	%eax, 32(%rsp)
	leaq	24(%rsp), %rdi
	callq	string_println@PLT
	movl	96(%rsp), %edx
	movq	88(%rsp), %rsi
	movq	%rbx, %rdi
	callq	string_append@PLT
	leaq	8(%rsp), %r12
	movl	$.L__unnamed_8, %esi
	movq	%r12, %rdi
	callq	string_0@PLT
	movl	16(%rsp), %edx
	movq	8(%rsp), %rsi
	movq	%rbx, %rdi
	callq	string_append@PLT
	movq	%rbx, %rdi
	callq	string_println@PLT
	movq	%r12, %rdi
	callq	string_destroy@PLT
	movq	%r15, %rdi
	callq	string_destroy@PLT
	movq	%r14, %rdi
	callq	string_destroy@PLT
	movq	%rbx, %rdi
	callq	string_destroy@PLT
	xorl	%eax, %eax
	addq	$120, %rsp
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end17:
	.size	main, .Lfunc_end17-main
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"string::at: index out of bounds "
	.size	.L__unnamed_1, 33

	.type	.L__unnamed_2,@object
.L__unnamed_2:
	.asciz	" of "
	.size	.L__unnamed_2, 5

	.type	.L__unnamed_3,@object
.L__unnamed_3:
	.asciz	"fooo: "
	.size	.L__unnamed_3, 7

	.type	.L__unnamed_4,@object
.L__unnamed_4:
	.asciz	"Hello, "
	.size	.L__unnamed_4, 8

	.type	.L__unnamed_5,@object
.L__unnamed_5:
	.asciz	"World !"
	.size	.L__unnamed_5, 8

	.type	.L__unnamed_6,@object
.L__unnamed_6:
	.asciz	"I leak ???"
	.size	.L__unnamed_6, 11

	.type	.L__unnamed_7,@object
.L__unnamed_7:
	.asciz	"I leak no more..."
	.size	.L__unnamed_7, 18

	.type	.L__unnamed_8,@object
.L__unnamed_8:
	.asciz	" whattt"
	.size	.L__unnamed_8, 8

	.section	".note.GNU-stack","",@progbits
