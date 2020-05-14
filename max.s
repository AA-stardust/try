	.file	"max.c"
	.text
	.globl	a
	.data
	.align 32
	.type	a, @object
	.size	a, 44
a:
	.long	5
	.long	8
	.long	10
	.long	3
	.long	7
	.long	9
	.long	11
	.long	15
	.long	17
	.long	1
	.long	2
	.globl	len
	.align 4
	.type	len, @object
	.size	len, 4
len:
	.long	11
	.section	.rodata
.LC0:
	.string	"%d \n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x78,0x6
	.cfi_escape 0x10,0x3,0x2,0x75,0x7c
	subl	$16, %esp
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	movl	a@GOTOFF(%eax), %edx
	movl	%edx, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	.L2
.L4:
	movl	-12(%ebp), %edx
	movl	a@GOTOFF(%eax,%edx,4), %edx
	cmpl	%edx, -16(%ebp)
	jge	.L3
	movl	-12(%ebp), %edx
	movl	a@GOTOFF(%eax,%edx,4), %edx
	movl	%edx, -16(%ebp)
.L3:
	addl	$1, -12(%ebp)
.L2:
	movl	len@GOTOFF(%eax), %edx
	cmpl	%edx, -12(%ebp)
	jl	.L4
	subl	$8, %esp
	pushl	-16(%ebp)
	leal	.LC0@GOTOFF(%eax), %edx
	pushl	%edx
	movl	%eax, %ebx
	call	printf@PLT
	addl	$16, %esp
	movl	$0, %eax
	leal	-8(%ebp), %esp
	popl	%ecx
	.cfi_restore 1
	.cfi_def_cfa 1, 0
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB1:
	.cfi_startproc
	movl	(%esp), %eax
	ret
	.cfi_endproc
.LFE1:
	.ident	"GCC: (Ubuntu 8.3.0-6ubuntu1) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
