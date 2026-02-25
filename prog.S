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
	_call_label0:
	jmp _call_label1
	_call_label1:
	movq $3, %rax
	sarq $1, %rax
	movq %rax, %rax
	movq %rax, %rax
	salq $1, %rax
	movq %rax, %rax
	addq $1, %rax
	movq %rax, %rax
	addq $4, %rax
	movq %rax, %rdi
	movq $1, %rsi
	call allocate
	movq %rax, %rsi
	movq %rsi, %rcx
	addq $8, %rcx
	movq $3, 0(%rcx)
	movq %rsi, %rcx
	addq $16, %rcx
	movq $3, 0(%rcx)
	movq $0, %rdx
	movq $1, %rcx
	movq %rdx, %rdx
	addq $1, %rdx
	movq %rdx, %rdx
	imulq $8, %rdx
	movq %rdx, %rdx
	addq $24, %rdx
	movq %rsi, %rcx
	addq %rdx, %rcx
	movq $5, 0(%rcx)
	retq
