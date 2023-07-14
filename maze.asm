	.file	"maze.c"
	.text
	.globl	width
	.data
	.align 4
	.type	width, @object
	.size	width, 4
width:
	.long	50
	.globl	height
	.align 4
	.type	height, @object
	.size	height, 4
height:
	.long	50
	.globl	map
	.bss
	.align 8
	.type	map, @object
	.size	map, 8
map:
	.zero	8
	.section	.rodata
.LC0:
	.string	"w:h:"
	.align 8
.LC1:
	.string	"Usage: %s [-w <width>] [-h <height>]\n"
.LC2:
	.string	"Out of memory\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	jmp	.L4
.L6:
	cmpl	$104, -4(%rbp)
	je	.L3
	cmpl	$119, -4(%rbp)
	jne	.L4
	movq	optarg(%rip), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, width(%rip)
	jmp	.L4
.L3:
	movq	optarg(%rip), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, height(%rip)
	nop
.L4:
	movq	-32(%rbp), %rcx
	movl	-20(%rbp), %eax
	leaq	.LC0(%rip), %rdx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	getopt@PLT
	movl	%eax, -4(%rbp)
	cmpl	$-1, -4(%rbp)
	jne	.L6
	movl	width(%rip), %eax
	testl	%eax, %eax
	je	.L7
	movl	height(%rip), %eax
	testl	%eax, %eax
	jne	.L8
.L7:
	movq	-32(%rbp), %rax
	movq	(%rax), %rdx
	movq	stderr(%rip), %rax
	leaq	.LC1(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$1, %edi
	call	exit@PLT
.L8:
	movl	width(%rip), %eax
	addl	$1, %eax
	andl	$-2, %eax
	movl	%eax, width(%rip)
	movl	height(%rip), %eax
	addl	$1, %eax
	andl	$-2, %eax
	movl	%eax, height(%rip)
	movl	width(%rip), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L9
	movl	width(%rip), %eax
	addl	$1, %eax
	jmp	.L10
.L9:
	movl	width(%rip), %eax
.L10:
	movl	%eax, width(%rip)
	movl	height(%rip), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L11
	movl	height(%rip), %eax
	addl	$1, %eax
	jmp	.L12
.L11:
	movl	height(%rip), %eax
.L12:
	movl	%eax, height(%rip)
	movl	width(%rip), %edx
	movl	height(%rip), %eax
	imull	%edx, %eax
	cltq
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, map(%rip)
	movq	map(%rip), %rax
	testq	%rax, %rax
	jne	.L13
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$14, %edx
	movl	$1, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	fwrite@PLT
	movl	$1, %edi
	call	exit@PLT
.L13:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.globl	make_maze
	.type	make_maze, @function
make_maze:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	make_maze, .-make_maze
	.ident	"GCC: (Ubuntu 12.2.0-17ubuntu1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
