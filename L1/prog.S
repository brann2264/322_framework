.text
	.globl go
go:
	pushq %rbx
	pushq %rbp
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	call _main
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbp
	popq %rbx
	retq
_main:
	retq
_h:
	movq $209, %rdx
	retq
_e:
	movq $203, %rdx
	retq
_l:
	movq $217, %rdx
	retq
_o:
	movq $223, %rdx
	retq
_w:
	movq $239, %rdx
	retq
_space:
	movq $65, %rdx
	retq
_r:
	movq $229, %rdx
	retq
_d:
	movq $201, %rdx
	retq
