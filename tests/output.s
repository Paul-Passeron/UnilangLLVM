	.text
	.file	"main"
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
	je	.LBB0_2
	movl	(%rsp), %edi
	callq	print_int@PLT
.LBB0_2:
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
	callq	put_character@PLT
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	print_int, .Lfunc_end0-print_int
	.cfi_endproc

	.globl	strlen
	.p2align	4, 0x90
	.type	strlen,@function
strlen:
	.cfi_startproc
	movq	%rdi, -8(%rsp)
	movl	$0, -12(%rsp)
	.p2align	4, 0x90
.LBB1_1:
	movq	-8(%rsp), %rax
	cmpb	$0, (%rax)
	je	.LBB1_3
	incl	-12(%rsp)
	movl	-8(%rsp), %eax
	incl	%eax
	movq	%rax, -8(%rsp)
	jmp	.LBB1_1
.LBB1_3:
	movl	-12(%rsp), %eax
	retq
.Lfunc_end1:
	.size	strlen, .Lfunc_end1-strlen
	.cfi_endproc

	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$10, 4(%rsp)
	movl	$.L__unnamed_1, %edi
	callq	strlen@PLT
	movl	%eax, %edi
	callq	print_int@PLT
	movl	$10, %edi
	callq	put_character@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"Hello, my name is Paul !\n"
	.size	.L__unnamed_1, 26

	.section	".note.GNU-stack","",@progbits
