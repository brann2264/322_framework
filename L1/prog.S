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
	subq $64, %rsp
	movq $0, %rax
	movq %rax, 56(%rsp)
	movq $0, %rax
	movq %rax, 48(%rsp)
	movq $0, %rax
	movq %rax, 40(%rsp)
	movq $601, %rax
	movq %rax, 32(%rsp)
	movq 32(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	sarq $1, %rdx
	movq %rax, %rax
	imulq %rdx, %rax
	addq $2, %rax
	salq $1, %rax
	addq $1, %rax
	movq $_new_label_ciao_10_global_1_ret, -8(%rsp)
	movq %rax, %rdi
	movq $1, %rsi
	call allocate
	_new_label_ciao_10_global_1_ret:
	movq %rax, %rax
	movq %rax, 40(%rsp)
	movq 40(%rsp), %rax
	movq %rax, %rax
	addq $8, %rax
	movq 32(%rsp), %rdx
	movq %rdx, 0(%rax)
	movq 40(%rsp), %rax
	movq %rax, %rax
	addq $16, %rax
	movq 32(%rsp), %rdx
	movq %rdx, 0(%rax)
	movq 32(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	sarq $1, %rdx
	movq %rax, %rax
	imulq %rdx, %rax
	addq $2, %rax
	salq $1, %rax
	addq $1, %rax
	movq $_new_label_ciao_10_global_2_ret, -8(%rsp)
	movq %rax, %rdi
	movq $1, %rsi
	call allocate
	_new_label_ciao_10_global_2_ret:
	movq %rax, %rax
	movq %rax, 48(%rsp)
	movq 48(%rsp), %rax
	movq %rax, %rax
	addq $8, %rax
	movq 32(%rsp), %rdx
	movq %rdx, 0(%rax)
	movq 48(%rsp), %rax
	movq %rax, %rax
	addq $16, %rax
	movq 32(%rsp), %rdx
	movq %rdx, 0(%rax)
	movq 32(%rsp), %rax
	movq %rax, %rdx
	sarq $1, %rdx
	movq 32(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq %rdx, %rdx
	imulq %rax, %rdx
	addq $2, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $_new_label_ciao_10_global_3_ret, -8(%rsp)
	movq %rdx, %rdi
	movq $1, %rsi
	call allocate
	_new_label_ciao_10_global_3_ret:
	movq %rax, %rdx
	movq %rdx, 56(%rsp)
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq $8, %rdx
	movq 32(%rsp), %rsi
	movq %rsi, 0(%rdx)
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq $16, %rdx
	movq 32(%rsp), %rsi
	movq %rsi, 0(%rdx)
	movq $9, %rdx
	movq %rdx, 24(%rsp)
	movq $9, %rdx
	movq %rdx, 16(%rsp)
	movq $1, %rdx
	movq %rdx, 8(%rsp)
	_new_label_ciao_10_global_4:
	movq 8(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 24(%rsp), %rdx
	movq %rdx, %rdi
	sarq $1, %rdi
	cmpq %rdi, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_5
	addq $64, %rsp
	retq
	_new_label_ciao_10_global_5:
	movq $1, %rdx
	movq %rdx, 0(%rsp)
	_new_label_ciao_10_global_6:
	movq 0(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 16(%rsp), %rdx
	movq %rdx, %rdi
	sarq $1, %rdi
	cmpq %rdi, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_7
	movq 8(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %rdi
	sarq $1, %rdi
	movq %rsi, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq %rdi, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 8(%rsp)
	jmp _new_label_ciao_10_global_4
	_new_label_ciao_10_global_7:
	movq $_new_label_ciao_10_global_8_ret, -8(%rsp)
	movq 40(%rsp), %rax
	movq %rax, %rdi
	movq 48(%rsp), %rax
	movq %rax, %rsi
	movq 56(%rsp), %rax
	movq %rax, %rdx
	movq 8(%rsp), %rax
	movq %rax, %rcx
	movq 0(%rsp), %rax
	movq %rax, %r8
	subq $8, %rsp
	jmp _computeAndPrint
	_new_label_ciao_10_global_8_ret:
	movq 0(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %rdi
	sarq $1, %rdi
	movq %rsi, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	addq %rdi, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 0(%rsp)
	jmp _new_label_ciao_10_global_6
_computeAndPrint:
	subq $32, %rsp
	movq %rdi, %rax
	movq %rax, 8(%rsp)
	movq %rsi, %rax
	movq %rax, 24(%rsp)
	movq %rdx, %rax
	movq %rax, 0(%rsp)
	movq %rcx, %rdx
	movq %r8, %rax
	movq %rax, 16(%rsp)
	movq $_new_label_ciao_10_global_9_ret, -8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rdi
	movq %rdx, %rsi
	subq $8, %rsp
	jmp _initMatrix
	_new_label_ciao_10_global_9_ret:
	movq $_new_label_ciao_10_global_10_ret, -8(%rsp)
	movq 24(%rsp), %rax
	movq %rax, %rdi
	movq 16(%rsp), %rax
	movq %rax, %rsi
	subq $8, %rsp
	jmp _initMatrix
	_new_label_ciao_10_global_10_ret:
	movq $_new_label_ciao_10_global_11_ret, -8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rdi
	movq 24(%rsp), %rax
	movq %rax, %rsi
	movq 0(%rsp), %rax
	movq %rax, %rdx
	subq $8, %rsp
	jmp _matrixMultiplication
	_new_label_ciao_10_global_11_ret:
	movq $_new_label_ciao_10_global_12_ret, -8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rdi
	subq $8, %rsp
	jmp _totalSum
	_new_label_ciao_10_global_12_ret:
	movq %rax, %rax
	movq $_new_label_ciao_10_global_13_ret, -8(%rsp)
	movq %rax, %rdi
	call print
	_new_label_ciao_10_global_13_ret:
	movq $_new_label_ciao_10_global_14_ret, -8(%rsp)
	movq 24(%rsp), %rax
	movq %rax, %rdi
	subq $8, %rsp
	jmp _totalSum
	_new_label_ciao_10_global_14_ret:
	movq %rax, %rax
	movq $_new_label_ciao_10_global_15_ret, -8(%rsp)
	movq %rax, %rdi
	call print
	_new_label_ciao_10_global_15_ret:
	movq $_new_label_ciao_10_global_16_ret, -8(%rsp)
	movq 0(%rsp), %rax
	movq %rax, %rdi
	subq $8, %rsp
	jmp _totalSum
	_new_label_ciao_10_global_16_ret:
	movq %rax, %rax
	movq $_new_label_ciao_10_global_17_ret, -8(%rsp)
	movq %rax, %rdi
	call print
	_new_label_ciao_10_global_17_ret:
	addq $32, %rsp
	retq
_initMatrix:
	subq $40, %rsp
	movq %rdi, %rdx
	movq %rdx, 32(%rsp)
	movq %rsi, %rsi
	movq $8, %rdi
	imulq $0, %rdi
	addq $8, %rdi
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rdi, %rdx
	movq 0(%rdx), %rdi
	movq $8, %r8
	imulq $1, %r8
	addq $8, %r8
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r8, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, 16(%rsp)
	movq $1, %rdx
	movq %rdx, 0(%rsp)
	_new_label_ciao_10_global_18:
	movq 0(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq %rdi, %r9
	sarq $1, %r9
	cmpq %r9, %r8
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq $1, %r8
	je _new_label_ciao_10_global_19
	addq $40, %rsp
	retq
	_new_label_ciao_10_global_19:
	movq $1, %rdx
	movq %rdx, 8(%rsp)
	_new_label_ciao_10_global_20:
	movq 8(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq 16(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq %r9, %r8
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq $1, %r8
	je _new_label_ciao_10_global_21
	movq 0(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq $3, %r9
	sarq $1, %r9
	movq %r8, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	addq %r9, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 0(%rsp)
	jmp _new_label_ciao_10_global_18
	_new_label_ciao_10_global_21:
	movq %rsi, %r8
	sarq $1, %r8
	movq 0(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	movq %r8, %r8
	addq %r9, %r8
	salq $1, %r8
	addq $1, %r8
	movq %r8, %r8
	sarq $1, %r8
	movq 8(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	movq %r8, %r8
	addq %r9, %r8
	salq $1, %r8
	addq $1, %r8
	movq 0(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	movq 8(%rsp), %rdx
	movq %rdx, %r11
	sarq $1, %r11
	movq $0, %r9
	movq 32(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_22
	movq $8, %r9
	imulq $0, %r9
	addq $8, %r9
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %r10, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_23
	_new_label_ciao_10_global_22:
	movq $_new_label_ciao_10_global_24_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_24_ret:
	_new_label_ciao_10_global_25:
	movq $24, %r9
	movq $1, %rcx
	movq $0, %rdx
	movq %rdx, 24(%rsp)
	movq %rcx, %rdx
	imulq %r11, %rdx
	movq 24(%rsp), %r11
	addq %rdx, %r11
	movq %r11, 24(%rsp)
	movq $16, %r11
	movq 32(%rsp), %rdx
	addq %rdx, %r11
	movq 0(%r11), %rdx
	sarq $1, %rdx
	imulq %rdx, %rcx
	movq %rcx, %r11
	imulq %r10, %r11
	movq 24(%rsp), %rdx
	addq %r11, %rdx
	movq %rdx, 24(%rsp)
	movq $8, %rdx
	movq 32(%rsp), %r10
	addq %r10, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	imulq %rdx, %rcx
	movq 24(%rsp), %rdx
	imulq $8, %rdx
	movq %rdx, 24(%rsp)
	movq 24(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq %rdx, %rdx
	movq 32(%rsp), %r9
	addq %r9, %rdx
	movq %r8, 0(%rdx)
	movq 8(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq $3, %r9
	sarq $1, %r9
	movq %r8, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq %r9, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 8(%rsp)
	jmp _new_label_ciao_10_global_20
	_new_label_ciao_10_global_23:
	movq $8, %r9
	imulq $1, %r9
	addq $8, %r9
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %r11, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %r11
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_26
	jmp _new_label_ciao_10_global_22
	_new_label_ciao_10_global_26:
	jmp _new_label_ciao_10_global_25
_matrixMultiplication:
	subq $80, %rsp
	movq %rdi, %rdi
	movq %rdi, 72(%rsp)
	movq %rsi, %rsi
	movq %rsi, 64(%rsp)
	movq %rdx, %rdx
	movq %rdx, 56(%rsp)
	movq $8, %rsi
	imulq $0, %rsi
	addq $8, %rsi
	movq 72(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, 40(%rsp)
	movq $8, %rsi
	imulq $1, %rsi
	addq $8, %rsi
	movq 72(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, 32(%rsp)
	movq $8, %rsi
	imulq $0, %rsi
	addq $8, %rsi
	movq 64(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %rdi
	movq $8, %rsi
	imulq $1, %rsi
	addq $8, %rsi
	movq 64(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, 24(%rsp)
	movq $8, %rsi
	imulq $0, %rsi
	addq $8, %rsi
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %r10
	movq $8, %rsi
	imulq $1, %rsi
	addq $8, %rsi
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %r9
	movq 32(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq %rdi, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	sete %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_27
	_new_label_ciao_10_global_28:
	addq $80, %rsp
	retq
	_new_label_ciao_10_global_27:
	movq %r10, %rsi
	sarq $1, %rsi
	movq 40(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	sete %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_29
	jmp _new_label_ciao_10_global_28
	_new_label_ciao_10_global_29:
	movq %r9, %rsi
	sarq $1, %rsi
	movq 24(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	sete %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_30
	jmp _new_label_ciao_10_global_28
	_new_label_ciao_10_global_30:
	movq $1, %rdx
	movq %rdx, 0(%rsp)
	_new_label_ciao_10_global_31:
	movq 0(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 40(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_32
	movq $1, %rdx
	movq %rdx, 8(%rsp)
	_new_label_ciao_10_global_33:
	movq 8(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 40(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_34
	jmp _new_label_ciao_10_global_28
	_new_label_ciao_10_global_34:
	movq $1, %rdx
	movq %rdx, 48(%rsp)
	_new_label_ciao_10_global_35:
	movq 48(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 24(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_36
	movq 8(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %r8
	sarq $1, %r8
	movq %rsi, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq %r8, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 8(%rsp)
	jmp _new_label_ciao_10_global_33
	_new_label_ciao_10_global_32:
	movq $1, %rdi
	_new_label_ciao_10_global_37:
	movq %rdi, %rsi
	sarq $1, %rsi
	movq 24(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_38
	movq 0(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %r8
	sarq $1, %r8
	movq %rsi, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	addq %r8, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 0(%rsp)
	jmp _new_label_ciao_10_global_31
	_new_label_ciao_10_global_38:
	movq $1, %r9
	_new_label_ciao_10_global_39:
	movq %r9, %rsi
	sarq $1, %rsi
	movq 32(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_40
	movq %rdi, %rsi
	sarq $1, %rsi
	movq $3, %r8
	sarq $1, %r8
	movq %rsi, %rdi
	addq %r8, %rdi
	salq $1, %rdi
	addq $1, %rdi
	jmp _new_label_ciao_10_global_37
	_new_label_ciao_10_global_40:
	movq 0(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq %rdi, %rsi
	sarq $1, %rsi
	movq $0, %r10
	movq 56(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_41
	movq $8, %r10
	imulq $0, %r10
	addq $8, %r10
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %r10, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %r8, %r10
	salq $1, %r10
	addq $1, %r10
	cmpq %rdx, %r8
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_42
	_new_label_ciao_10_global_41:
	movq $_new_label_ciao_10_global_43_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_43_ret:
	_new_label_ciao_10_global_44:
	movq $24, %r11
	movq $1, %rcx
	movq $0, %r10
	movq %rcx, %rdx
	imulq %rsi, %rdx
	addq %rdx, %r10
	movq $16, %rsi
	movq 56(%rsp), %rdx
	addq %rdx, %rsi
	movq 0(%rsi), %rdx
	sarq $1, %rdx
	imulq %rdx, %rcx
	movq %rcx, %rdx
	imulq %r8, %rdx
	addq %rdx, %r10
	movq $8, %rsi
	movq 56(%rsp), %rdx
	addq %rdx, %rsi
	movq 0(%rsi), %rdx
	sarq $1, %rdx
	imulq %rdx, %rcx
	imulq $8, %r10
	movq %r10, %rdx
	addq %r11, %rdx
	movq %rdx, %rsi
	movq 56(%rsp), %rdx
	addq %rdx, %rsi
	movq $1, 0(%rsi)
	movq %r9, %rsi
	sarq $1, %rsi
	movq $3, %r8
	sarq $1, %r8
	movq %rsi, %r9
	addq %r8, %r9
	salq $1, %r9
	addq $1, %r9
	jmp _new_label_ciao_10_global_39
	_new_label_ciao_10_global_42:
	movq $8, %r10
	imulq $1, %r10
	addq $8, %r10
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %r10, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %rsi, %r10
	salq $1, %r10
	addq $1, %r10
	cmpq %rdx, %rsi
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_45
	jmp _new_label_ciao_10_global_41
	_new_label_ciao_10_global_45:
	jmp _new_label_ciao_10_global_44
	_new_label_ciao_10_global_36:
	movq $1, %rdx
	movq %rdx, 16(%rsp)
	_new_label_ciao_10_global_46:
	movq 16(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 32(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	cmpq %r8, %rsi
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	cmpq $1, %rsi
	je _new_label_ciao_10_global_47
	movq 48(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %r8
	sarq $1, %r8
	movq %rsi, %rdx
	movq %rdx, 48(%rsp)
	movq 48(%rsp), %rdx
	addq %r8, %rdx
	movq %rdx, 48(%rsp)
	movq 48(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 48(%rsp)
	movq 48(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 48(%rsp)
	jmp _new_label_ciao_10_global_35
	_new_label_ciao_10_global_47:
	movq 8(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 16(%rsp), %rdx
	movq %rdx, %rdi
	sarq $1, %rdi
	movq $0, %r8
	movq 72(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_48
	movq $8, %r8
	imulq $0, %r8
	addq $8, %r8
	movq 72(%rsp), %rdx
	movq %rdx, %rdx
	addq %r8, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %rsi, %r8
	salq $1, %r8
	addq $1, %r8
	cmpq %rdx, %rsi
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_49
	_new_label_ciao_10_global_48:
	movq $_new_label_ciao_10_global_50_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_50_ret:
	_new_label_ciao_10_global_51:
	movq $24, %r9
	movq $1, %r10
	movq $0, %r8
	movq %r10, %rdx
	imulq %rdi, %rdx
	addq %rdx, %r8
	movq $16, %rdi
	movq 72(%rsp), %rdx
	addq %rdx, %rdi
	movq 0(%rdi), %rdx
	sarq $1, %rdx
	imulq %rdx, %r10
	movq %r10, %rdx
	imulq %rsi, %rdx
	addq %rdx, %r8
	movq $8, %rdx
	movq 72(%rsp), %rsi
	addq %rsi, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	imulq %rdx, %r10
	imulq $8, %r8
	movq %r8, %rdx
	addq %r9, %rdx
	movq %rdx, %rsi
	movq 72(%rsp), %rdx
	addq %rdx, %rsi
	movq 0(%rsi), %rsi
	movq 16(%rsp), %rdx
	movq %rdx, %rdi
	sarq $1, %rdi
	movq 48(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq $0, %r9
	movq 64(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_52
	movq $8, %r9
	imulq $0, %r9
	addq $8, %r9
	movq 64(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %rdi, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %rdi
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_53
	_new_label_ciao_10_global_52:
	movq $_new_label_ciao_10_global_54_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_54_ret:
	_new_label_ciao_10_global_55:
	movq $24, %r10
	movq $1, %r11
	movq $0, %r9
	movq %r11, %rdx
	imulq %r8, %rdx
	addq %rdx, %r9
	movq $16, %r8
	movq 64(%rsp), %rdx
	addq %rdx, %r8
	movq 0(%r8), %rdx
	sarq $1, %rdx
	imulq %rdx, %r11
	movq %r11, %rdx
	imulq %rdi, %rdx
	addq %rdx, %r9
	movq $8, %rdi
	movq 64(%rsp), %rdx
	addq %rdx, %rdi
	movq 0(%rdi), %rdx
	sarq $1, %rdx
	imulq %rdx, %r11
	imulq $8, %r9
	movq %r9, %rdx
	addq %r10, %rdx
	movq %rdx, %rdi
	movq 64(%rsp), %rdx
	addq %rdx, %rdi
	movq 0(%rdi), %rdx
	movq %rsi, %rsi
	sarq $1, %rsi
	movq %rdx, %r8
	sarq $1, %r8
	movq %rsi, %rdi
	imulq %r8, %rdi
	salq $1, %rdi
	addq $1, %rdi
	movq 8(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 48(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq $0, %r9
	movq 56(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_56
	movq $8, %r9
	imulq $0, %r9
	addq $8, %r9
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %rsi, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %rsi
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_57
	_new_label_ciao_10_global_56:
	movq $_new_label_ciao_10_global_58_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_58_ret:
	_new_label_ciao_10_global_59:
	movq $24, %r10
	movq $1, %r11
	movq $0, %r9
	movq %r11, %rdx
	imulq %r8, %rdx
	addq %rdx, %r9
	movq $16, %r8
	movq 56(%rsp), %rdx
	addq %rdx, %r8
	movq 0(%r8), %rdx
	sarq $1, %rdx
	imulq %rdx, %r11
	movq %r11, %rdx
	imulq %rsi, %rdx
	addq %rdx, %r9
	movq $8, %rdx
	movq 56(%rsp), %rsi
	addq %rsi, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	imulq %rdx, %r11
	imulq $8, %r9
	movq %r9, %rdx
	addq %r10, %rdx
	movq %rdx, %rsi
	movq 56(%rsp), %rdx
	addq %rdx, %rsi
	movq 0(%rsi), %rsi
	movq %rsi, %rsi
	sarq $1, %rsi
	movq $9, %r8
	sarq $1, %r8
	movq %rsi, %rsi
	imulq %r8, %rsi
	salq $1, %rsi
	addq $1, %rsi
	movq %rsi, %rsi
	sarq $1, %rsi
	movq %rdi, %r8
	sarq $1, %r8
	movq %rsi, %rsi
	addq %r8, %rsi
	salq $1, %rsi
	addq $1, %rsi
	movq 8(%rsp), %rdx
	movq %rdx, %rdi
	sarq $1, %rdi
	movq 48(%rsp), %rdx
	movq %rdx, %r8
	sarq $1, %r8
	movq $0, %r9
	movq 56(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_60
	movq $8, %r9
	imulq $0, %r9
	addq $8, %r9
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %rdi, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %rdi
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_61
	_new_label_ciao_10_global_60:
	movq $_new_label_ciao_10_global_62_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_62_ret:
	_new_label_ciao_10_global_63:
	movq $24, %r10
	movq $1, %r11
	movq $0, %r9
	movq %r11, %rdx
	imulq %r8, %rdx
	addq %rdx, %r9
	movq $16, %rdx
	movq 56(%rsp), %r8
	addq %r8, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	imulq %rdx, %r11
	movq %r11, %rdx
	imulq %rdi, %rdx
	addq %rdx, %r9
	movq $8, %rdx
	movq 56(%rsp), %rdi
	addq %rdi, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	imulq %rdx, %r11
	imulq $8, %r9
	movq %r9, %rdx
	addq %r10, %rdx
	movq %rdx, %rdx
	movq 56(%rsp), %rdi
	addq %rdi, %rdx
	movq %rsi, 0(%rdx)
	movq 16(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %r8
	sarq $1, %r8
	movq %rsi, %rdx
	movq %rdx, 16(%rsp)
	movq 16(%rsp), %rdx
	addq %r8, %rdx
	movq %rdx, 16(%rsp)
	movq 16(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 16(%rsp)
	movq 16(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 16(%rsp)
	jmp _new_label_ciao_10_global_46
	_new_label_ciao_10_global_49:
	movq $8, %r8
	imulq $1, %r8
	addq $8, %r8
	movq 72(%rsp), %rdx
	movq %rdx, %rdx
	addq %r8, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %rdi, %r8
	salq $1, %r8
	addq $1, %r8
	cmpq %rdx, %rdi
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_64
	jmp _new_label_ciao_10_global_48
	_new_label_ciao_10_global_64:
	jmp _new_label_ciao_10_global_51
	_new_label_ciao_10_global_53:
	movq $8, %r9
	imulq $1, %r9
	addq $8, %r9
	movq 64(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %r8, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %r8
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_65
	jmp _new_label_ciao_10_global_52
	_new_label_ciao_10_global_65:
	jmp _new_label_ciao_10_global_55
	_new_label_ciao_10_global_57:
	movq $8, %r9
	imulq $1, %r9
	addq $8, %r9
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %r8, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %r8
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_66
	jmp _new_label_ciao_10_global_56
	_new_label_ciao_10_global_66:
	jmp _new_label_ciao_10_global_59
	_new_label_ciao_10_global_61:
	movq $8, %r9
	imulq $1, %r9
	addq $8, %r9
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	movq %r8, %r9
	salq $1, %r9
	addq $1, %r9
	cmpq %rdx, %r8
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _new_label_ciao_10_global_67
	jmp _new_label_ciao_10_global_60
	_new_label_ciao_10_global_67:
	jmp _new_label_ciao_10_global_63
_totalSum:
	subq $24, %rsp
	movq %rdi, %rdx
	movq $8, %rsi
	imulq $0, %rsi
	addq $8, %rsi
	movq %rdx, %rax
	addq %rsi, %rax
	movq 0(%rax), %rax
	movq %rax, 0(%rsp)
	movq $8, %rsi
	imulq $1, %rsi
	addq $8, %rsi
	movq %rdx, %rax
	addq %rsi, %rax
	movq 0(%rax), %rdi
	movq $1, %rsi
	movq $1, %r8
	_new_label_ciao_10_global_68:
	movq %rsi, %r10
	sarq $1, %r10
	movq 0(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	cmpq %rax, %r10
	setl %al
	movzbq %al, %rax
	salq $1, %rax
	addq $1, %rax
	movq %rax, %r10
	sarq $1, %r10
	cmpq $1, %r10
	je _new_label_ciao_10_global_69
	movq %r8, %rax
	addq $24, %rsp
	retq
	_new_label_ciao_10_global_69:
	movq $1, %r9
	_new_label_ciao_10_global_70:
	movq %r9, %r10
	sarq $1, %r10
	movq %rdi, %rax
	sarq $1, %rax
	cmpq %rax, %r10
	setl %al
	movzbq %al, %rax
	salq $1, %rax
	addq $1, %rax
	movq %rax, %r10
	sarq $1, %r10
	cmpq $1, %r10
	je _new_label_ciao_10_global_71
	movq %rsi, %r10
	sarq $1, %r10
	movq $3, %rax
	sarq $1, %rax
	movq %r10, %rsi
	addq %rax, %rsi
	salq $1, %rsi
	addq $1, %rsi
	jmp _new_label_ciao_10_global_68
	_new_label_ciao_10_global_71:
	movq %rsi, %r11
	sarq $1, %r11
	movq %r9, %rcx
	sarq $1, %rcx
	movq $0, %r10
	cmpq $0, %rdx
	sete %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _new_label_ciao_10_global_72
	movq $8, %rax
	imulq $0, %rax
	addq $8, %rax
	movq %rdx, %r10
	addq %rax, %r10
	movq 0(%r10), %rax
	sarq $1, %rax
	movq %r11, %r10
	salq $1, %r10
	addq $1, %r10
	cmpq %rax, %r11
	setl %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _new_label_ciao_10_global_73
	_new_label_ciao_10_global_72:
	movq $_new_label_ciao_10_global_74_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_new_label_ciao_10_global_74_ret:
	_new_label_ciao_10_global_75:
	movq $24, %r10
	movq $1, %rax
	movq %rax, 16(%rsp)
	movq $0, %rax
	movq %rax, 8(%rsp)
	movq 16(%rsp), %rax
	movq %rax, %rax
	imulq %rcx, %rax
	movq 8(%rsp), %rcx
	addq %rax, %rcx
	movq %rcx, 8(%rsp)
	movq $16, %rax
	addq %rdx, %rax
	movq 0(%rax), %rcx
	sarq $1, %rcx
	movq 16(%rsp), %rax
	imulq %rcx, %rax
	movq %rax, 16(%rsp)
	movq 16(%rsp), %rax
	movq %rax, %rcx
	imulq %r11, %rcx
	movq 8(%rsp), %rax
	addq %rcx, %rax
	movq %rax, 8(%rsp)
	movq $8, %rax
	addq %rdx, %rax
	movq 0(%rax), %r11
	sarq $1, %r11
	movq 16(%rsp), %rax
	imulq %r11, %rax
	movq %rax, 16(%rsp)
	movq 8(%rsp), %rax
	imulq $8, %rax
	movq %rax, 8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rax
	addq %r10, %rax
	movq %rax, %rax
	addq %rdx, %rax
	movq 0(%rax), %rax
	movq %r8, %r10
	sarq $1, %r10
	movq %rax, %rax
	sarq $1, %rax
	movq %r10, %r8
	addq %rax, %r8
	salq $1, %r8
	addq $1, %r8
	movq %r9, %r10
	sarq $1, %r10
	movq $3, %rax
	sarq $1, %rax
	movq %r10, %r9
	addq %rax, %r9
	salq $1, %r9
	addq $1, %r9
	jmp _new_label_ciao_10_global_70
	_new_label_ciao_10_global_73:
	movq $8, %rax
	imulq $1, %rax
	addq $8, %rax
	movq %rdx, %r10
	addq %rax, %r10
	movq 0(%r10), %rax
	sarq $1, %rax
	movq %rcx, %r10
	salq $1, %r10
	addq $1, %r10
	cmpq %rax, %rcx
	setl %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _new_label_ciao_10_global_76
	jmp _new_label_ciao_10_global_72
	_new_label_ciao_10_global_76:
	jmp _new_label_ciao_10_global_75
