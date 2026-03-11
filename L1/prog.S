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
	movq $1, %rdx
	movq $2, %rax
	cmpq %rax, %rdx
	setl %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _false_global_1
	jmp _false_global_2
	_false_global_1:
	movq $0, %rax
	retq
	_false_global_2:
	movq %rax, %rax
	retq
