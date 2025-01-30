	.text
	.file	"main"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	subq	$48, %rsp
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	leaq	-56(%rbp), %rbx
	movl	$.L__unnamed_1, %esi
	movq	%rbx, %rdi
	callq	string_2@PLT
	movq	%rbx, %rdi
	callq	string_println@PLT
	leaq	-40(%rbp), %rbx
	movq	%rbx, %rdi
	callq	vectorZ0_0@PLT
	movl	$0, -20(%rbp)
	cmpl	$9999, -20(%rbp)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:
	movl	-20(%rbp), %eax
	leal	1(,%rax,2), %esi
	movq	%rbx, %rdi
	callq	vectorZ0_append@PLT
	incl	-20(%rbp)
	cmpl	$9999, -20(%rbp)
	jle	.LBB0_2
.LBB0_3:
	leaq	-16(%rsp), %rbx
	movq	%rbx, %rsp
	leaq	-40(%rbp), %r14
	movq	%r14, %rdi
	movl	$6540, %esi
	callq	vectorZ0_at@PLT
	movq	%rbx, %rdi
	movl	%eax, %esi
	callq	string_4@PLT
	movq	%rbx, %rdi
	callq	string_println@PLT
	movq	%rbx, %rdi
	callq	string_destroy@PLT
	movq	%r14, %rdi
	callq	vectorZ0_destroy@PLT
	leaq	-56(%rbp), %rdi
	callq	string_destroy@PLT
	xorl	%eax, %eax
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.globl	vectorZ0_0
	.p2align	4, 0x90
	.type	vectorZ0_0,@function
vectorZ0_0:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movabsq	$68719476736, %rax
	movq	%rax, 8(%rdi)
	movl	$64, %edi
	callq	malloc@PLT
	movq	%rax, (%rbx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	vectorZ0_0, .Lfunc_end1-vectorZ0_0
	.cfi_endproc

	.globl	vectorZ0_at
	.p2align	4, 0x90
	.type	vectorZ0_at,@function
vectorZ0_at:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%esi, -44(%rbp)
	cmpl	8(%rdi), %esi
	jge	.LBB2_2
	movq	(%rdi), %rax
	movslq	-44(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB2_2:
	.cfi_def_cfa %rbp, 16
	leaq	-16(%rsp), %r15
	movq	%r15, %rsp
	movl	$.L__unnamed_2, %esi
	movq	%rdi, %rbx
	movq	%r15, %rdi
	movq	%r15, -56(%rbp)
	callq	string_2@PLT
	movl	-44(%rbp), %esi
	movq	%rsp, %r14
	leaq	-16(%r14), %rdi
	movq	%rdi, -64(%rbp)
	movq	%rdi, %rsp
	callq	string_4@PLT
	movl	-4(%r14), %ecx
	movl	-8(%r14), %edx
	movq	-16(%r14), %rsi
	movq	%r15, %rdi
	callq	string_op_add@PLT
	movq	%rsp, %rsi
	leaq	-16(%rsi), %r15
	movq	%r15, %rsp
	movl	%ecx, -4(%rsi)
	movl	%edx, -8(%rsi)
	movq	%rax, -16(%rsi)
	movq	%rsp, %r14
	leaq	-16(%r14), %r12
	movq	%r12, %rsp
	movl	$.L__unnamed_3, %esi
	movq	%r12, %rdi
	callq	string_2@PLT
	movl	-4(%r14), %ecx
	movl	-8(%r14), %edx
	movq	-16(%r14), %rsi
	movq	%r15, %rdi
	callq	string_op_add@PLT
	movq	%rsp, %rsi
	leaq	-16(%rsi), %r13
	movq	%r13, %rsp
	movl	%ecx, -4(%rsi)
	movl	%edx, -8(%rsi)
	movq	%rax, -16(%rsi)
	movl	8(%rbx), %esi
	decl	%esi
	movq	%rsp, %rbx
	leaq	-16(%rbx), %r14
	movq	%r14, %rsp
	movq	%r14, %rdi
	callq	string_4@PLT
	movl	-4(%rbx), %ecx
	movl	-8(%rbx), %edx
	movq	-16(%rbx), %rsi
	movq	%r13, %rdi
	callq	string_op_add@PLT
	movq	%rsp, %rsi
	leaq	-16(%rsi), %rbx
	movq	%rbx, %rsp
	movl	%ecx, -4(%rsi)
	movl	%edx, -8(%rsi)
	movq	%rax, -16(%rsi)
	movq	%rbx, %rdi
	callq	string_println@PLT
	movl	$1, %edi
	callq	exit@PLT
	movq	%rbx, %rdi
	callq	string_destroy@PLT
	movq	%r14, %rdi
	callq	string_destroy@PLT
	movq	%r13, %rdi
	callq	string_destroy@PLT
	movq	%r12, %rdi
	callq	string_destroy@PLT
	movq	%r15, %rdi
	callq	string_destroy@PLT
	movq	-64(%rbp), %rdi
	callq	string_destroy@PLT
	movq	-56(%rbp), %rdi
	callq	string_destroy@PLT
.Lfunc_end2:
	.size	vectorZ0_at, .Lfunc_end2-vectorZ0_at
	.cfi_endproc

	.globl	vectorZ0_append
	.p2align	4, 0x90
	.type	vectorZ0_append,@function
vectorZ0_append:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	subq	$16, %rsp
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	movq	%rdi, %rbx
	movl	%esi, -20(%rbp)
	movl	8(%rdi), %eax
	incl	%eax
	cmpl	12(%rdi), %eax
	jl	.LBB3_5
	shll	12(%rbx)
	movq	%rsp, %rax
	leaq	-16(%rax), %r14
	movq	%r14, %rsp
	movq	(%rbx), %rcx
	movq	%rcx, -16(%rax)
	movslq	12(%rbx), %rdi
	shlq	$2, %rdi
	callq	malloc@PLT
	movq	%rax, (%rbx)
	movq	%rsp, %rcx
	leaq	-16(%rcx), %rax
	movq	%rax, %rsp
	movl	$0, -16(%rcx)
	.p2align	4, 0x90
.LBB3_2:
	movl	(%rax), %ecx
	cmpl	8(%rbx), %ecx
	jge	.LBB3_4
	movq	(%r14), %rcx
	movslq	(%rax), %rdx
	movl	(%rcx,%rdx,4), %ecx
	movq	(%rbx), %rsi
	movl	%ecx, (%rsi,%rdx,4)
	incl	(%rax)
	jmp	.LBB3_2
.LBB3_4:
	movq	(%r14), %rdi
	callq	free@PLT
.LBB3_5:
	movl	-20(%rbp), %eax
	movq	(%rbx), %rcx
	movslq	8(%rbx), %rdx
	movl	%eax, (%rcx,%rdx,4)
	incl	8(%rbx)
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end3:
	.size	vectorZ0_append, .Lfunc_end3-vectorZ0_append
	.cfi_endproc

	.globl	vectorZ0_destroy
	.p2align	4, 0x90
	.type	vectorZ0_destroy,@function
vectorZ0_destroy:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	(%rdi), %rdi
	callq	free@PLT
	xorps	%xmm0, %xmm0
	movups	%xmm0, (%rbx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end4:
	.size	vectorZ0_destroy, .Lfunc_end4-vectorZ0_destroy
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"Hello, World !"
	.size	.L__unnamed_1, 15

	.type	.L__unnamed_2,@object
.L__unnamed_2:
	.asciz	"Could not access element at index "
	.size	.L__unnamed_2, 35

	.type	.L__unnamed_3,@object
.L__unnamed_3:
	.asciz	" out of "
	.size	.L__unnamed_3, 9

	.section	".note.GNU-stack","",@progbits
