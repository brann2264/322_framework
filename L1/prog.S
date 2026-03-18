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
	_NNNNNNNNNNNNNNNNNN_global_1:
	jmp _NNNNNNNNNNNNNNNNNN_global_2
	_NNNNNNNNNNNNNNNNNN_global_3:
	addq $64, %rsp
	retq
	_NNNNNNNNNNNNNNNNNN_global_4:
	movq 40(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %rdi
	sarq $1, %rdi
	movq %rsi, %rdx
	movq %rdx, 40(%rsp)
	movq 40(%rsp), %rdx
	addq %rdi, %rdx
	movq %rdx, 40(%rsp)
	movq 40(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 40(%rsp)
	movq 40(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 40(%rsp)
	jmp _NNNNNNNNNNNNNNNNNN_global_5
	_NNNNNNNNNNNNNNNNNN_global_5:
	movq 40(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq 32(%rsp), %rdx
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
	je _NNNNNNNNNNNNNNNNNN_global_6
	jmp _NNNNNNNNNNNNNNNNNN_global_3
	_NNNNNNNNNNNNNNNNNN_global_6:
	movq $1, %rdx
	movq %rdx, 16(%rsp)
	jmp _NNNNNNNNNNNNNNNNNN_global_7
	_NNNNNNNNNNNNNNNNNN_global_7:
	movq 16(%rsp), %rdx
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
	je _NNNNNNNNNNNNNNNNNN_global_8
	jmp _NNNNNNNNNNNNNNNNNN_global_4
	_NNNNNNNNNNNNNNNNNN_global_8:
	movq $_NNNNNNNNNNNNNNNNNN_global_9_ret, -8(%rsp)
	movq 0(%rsp), %rax
	movq %rax, %rdi
	movq 48(%rsp), %rax
	movq %rax, %rsi
	movq 56(%rsp), %rax
	movq %rax, %rdx
	movq 40(%rsp), %rax
	movq %rax, %rcx
	movq 16(%rsp), %rax
	movq %rax, %r8
	subq $8, %rsp
	jmp _computeAndPrint
	_NNNNNNNNNNNNNNNNNN_global_9_ret:
	movq 16(%rsp), %rdx
	movq %rdx, %rsi
	sarq $1, %rsi
	movq $3, %rdi
	sarq $1, %rdi
	movq %rsi, %rdx
	movq %rdx, 16(%rsp)
	movq 16(%rsp), %rdx
	addq %rdi, %rdx
	movq %rdx, 16(%rsp)
	movq 16(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 16(%rsp)
	movq 16(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 16(%rsp)
	jmp _NNNNNNNNNNNNNNNNNN_global_7
	_NNNNNNNNNNNNNNNNNN_global_2:
	movq $0, %rax
	movq %rax, 56(%rsp)
	movq $0, %rax
	movq %rax, 48(%rsp)
	movq $0, %rax
	movq %rax, 0(%rsp)
	movq $601, %rax
	movq %rax, 8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq %rax, %rdx
	movq 8(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	imulq %rax, %rdx
	salq $1, %rdx
	addq $1, %rdx
	addq $6, %rdx
	movq $_NNNNNNNNNNNNNNNNNN_global_10_ret, -8(%rsp)
	movq %rdx, %rdi
	movq $1, %rsi
	call allocate
	_NNNNNNNNNNNNNNNNNN_global_10_ret:
	movq %rax, %rax
	movq %rax, 0(%rsp)
	movq 0(%rsp), %rax
	movq %rax, %rax
	addq $8, %rax
	movq $5, 0(%rax)
	movq 0(%rsp), %rax
	movq %rax, %rdx
	addq $16, %rdx
	movq 8(%rsp), %rax
	movq %rax, 0(%rdx)
	movq 0(%rsp), %rax
	movq %rax, %rax
	addq $24, %rax
	movq 8(%rsp), %rdx
	movq %rdx, 0(%rax)
	movq 8(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq %rax, %rdx
	movq 8(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	imulq %rax, %rdx
	salq $1, %rdx
	addq $1, %rdx
	addq $6, %rdx
	movq $_NNNNNNNNNNNNNNNNNN_global_11_ret, -8(%rsp)
	movq %rdx, %rdi
	movq $1, %rsi
	call allocate
	_NNNNNNNNNNNNNNNNNN_global_11_ret:
	movq %rax, %rax
	movq %rax, 48(%rsp)
	movq 48(%rsp), %rax
	movq %rax, %rax
	addq $8, %rax
	movq $5, 0(%rax)
	movq 48(%rsp), %rax
	movq %rax, %rdx
	addq $16, %rdx
	movq 8(%rsp), %rax
	movq %rax, 0(%rdx)
	movq 48(%rsp), %rax
	movq %rax, %rax
	addq $24, %rax
	movq 8(%rsp), %rdx
	movq %rdx, 0(%rax)
	movq 8(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq %rax, %rdx
	movq 8(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	imulq %rax, %rdx
	salq $1, %rdx
	addq $1, %rdx
	addq $6, %rdx
	movq $_NNNNNNNNNNNNNNNNNN_global_12_ret, -8(%rsp)
	movq %rdx, %rdi
	movq $1, %rsi
	call allocate
	_NNNNNNNNNNNNNNNNNN_global_12_ret:
	movq %rax, %rdx
	movq %rdx, 56(%rsp)
	movq 56(%rsp), %rdx
	movq %rdx, %rdx
	addq $8, %rdx
	movq $5, 0(%rdx)
	movq 56(%rsp), %rdx
	movq %rdx, %rsi
	addq $16, %rsi
	movq 8(%rsp), %rdx
	movq %rdx, 0(%rsi)
	movq 56(%rsp), %rdx
	movq %rdx, %rsi
	addq $24, %rsi
	movq 8(%rsp), %rdx
	movq %rdx, 0(%rsi)
	movq $9, %rdx
	movq %rdx, 32(%rsp)
	movq $9, %rdx
	movq %rdx, 24(%rsp)
	movq $1, %rdx
	movq %rdx, 40(%rsp)
	jmp _NNNNNNNNNNNNNNNNNN_global_5
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
	_NNNNNNNNNNNNNNNNNN_global_13:
	jmp _NNNNNNNNNNNNNNNNNN_global_14
	_NNNNNNNNNNNNNNNNNN_global_14:
	movq $_NNNNNNNNNNNNNNNNNN_global_15_ret, -8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rdi
	movq %rdx, %rsi
	subq $8, %rsp
	jmp _initMatrix
	_NNNNNNNNNNNNNNNNNN_global_15_ret:
	movq $_NNNNNNNNNNNNNNNNNN_global_16_ret, -8(%rsp)
	movq 24(%rsp), %rax
	movq %rax, %rdi
	movq 16(%rsp), %rax
	movq %rax, %rsi
	subq $8, %rsp
	jmp _initMatrix
	_NNNNNNNNNNNNNNNNNN_global_16_ret:
	movq $_NNNNNNNNNNNNNNNNNN_global_17_ret, -8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rdi
	movq 24(%rsp), %rax
	movq %rax, %rsi
	movq 0(%rsp), %rax
	movq %rax, %rdx
	subq $8, %rsp
	jmp _matrixMultiplication
	_NNNNNNNNNNNNNNNNNN_global_17_ret:
	movq $_NNNNNNNNNNNNNNNNNN_global_18_ret, -8(%rsp)
	movq 8(%rsp), %rax
	movq %rax, %rdi
	subq $8, %rsp
	jmp _totalSum
	_NNNNNNNNNNNNNNNNNN_global_18_ret:
	movq %rax, %rax
	movq $_NNNNNNNNNNNNNNNNNN_global_19_ret, -8(%rsp)
	movq %rax, %rdi
	call print
	_NNNNNNNNNNNNNNNNNN_global_19_ret:
	movq $_NNNNNNNNNNNNNNNNNN_global_20_ret, -8(%rsp)
	movq 24(%rsp), %rax
	movq %rax, %rdi
	subq $8, %rsp
	jmp _totalSum
	_NNNNNNNNNNNNNNNNNN_global_20_ret:
	movq %rax, %rax
	movq $_NNNNNNNNNNNNNNNNNN_global_21_ret, -8(%rsp)
	movq %rax, %rdi
	call print
	_NNNNNNNNNNNNNNNNNN_global_21_ret:
	movq $_NNNNNNNNNNNNNNNNNN_global_22_ret, -8(%rsp)
	movq 0(%rsp), %rax
	movq %rax, %rdi
	subq $8, %rsp
	jmp _totalSum
	_NNNNNNNNNNNNNNNNNN_global_22_ret:
	movq %rax, %rax
	movq $_NNNNNNNNNNNNNNNNNN_global_23_ret, -8(%rsp)
	movq %rax, %rdi
	call print
	_NNNNNNNNNNNNNNNNNN_global_23_ret:
	addq $32, %rsp
	retq
_initMatrix:
	subq $24, %rsp
	movq %rdi, %rdx
	movq %rdx, 16(%rsp)
	movq %rsi, %rsi
	_NNNNNNNNNNNNNNNNNN_global_24:
	jmp _NNNNNNNNNNNNNNNNNN_global_25
	_NNNNNNNNNNNNNNNNNN_global_26:
	addq $24, %rsp
	retq
	_NNNNNNNNNNNNNNNNNN_global_27:
	movq %r8, %r11
	sarq $1, %r11
	movq $3, %r10
	sarq $1, %r10
	movq %r11, %r8
	addq %r10, %r8
	salq $1, %r8
	addq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_28
	_NNNNNNNNNNNNNNNNNN_global_28:
	movq %r8, %r11
	sarq $1, %r11
	movq 0(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r11
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r11
	sarq $1, %r11
	cmpq $1, %r11
	je _NNNNNNNNNNNNNNNNNN_global_29
	jmp _NNNNNNNNNNNNNNNNNN_global_26
	_NNNNNNNNNNNNNNNNNN_global_29:
	movq $1, %r9
	jmp _NNNNNNNNNNNNNNNNNN_global_30
	_NNNNNNNNNNNNNNNNNN_global_30:
	movq %r9, %r11
	sarq $1, %r11
	movq %rdi, %r10
	sarq $1, %r10
	cmpq %r10, %r11
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r11
	sarq $1, %r11
	cmpq $1, %r11
	je _NNNNNNNNNNNNNNNNNN_global_31
	jmp _NNNNNNNNNNNNNNNNNN_global_27
	_NNNNNNNNNNNNNNNNNN_global_31:
	movq %rsi, %r11
	sarq $1, %r11
	movq %r8, %r10
	sarq $1, %r10
	movq %r11, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq %r10, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	movq %rdx, %r11
	sarq $1, %r11
	movq %r9, %r10
	sarq $1, %r10
	movq %r11, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq %r10, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 8(%rsp)
	movq %r8, %r11
	sarq $1, %r11
	movq %r9, %r10
	sarq $1, %r10
	movq $0, %rdx
	movq 16(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_32
	jmp _NNNNNNNNNNNNNNNNNN_global_33
	_NNNNNNNNNNNNNNNNNN_global_33:
	movq %r11, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 16(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r11
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_34
	jmp _NNNNNNNNNNNNNNNNNN_global_32
	_NNNNNNNNNNNNNNNNNN_global_34:
	movq %r10, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 16(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_35
	jmp _NNNNNNNNNNNNNNNNNN_global_32
	_NNNNNNNNNNNNNNNNNN_global_35:
	jmp _NNNNNNNNNNNNNNNNNN_global_36
	_NNNNNNNNNNNNNNNNNN_global_36:
	movq 16(%rsp), %rdx
	movq %rdx, %rdx
	addq $24, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %rcx
	sarq $1, %rcx
	movq $1, %rdx
	imulq %rcx, %rdx
	movq %r11, %r11
	imulq %rdx, %r11
	movq $0, %rdx
	addq %r11, %rdx
	addq %r10, %rdx
	movq %rdx, %r11
	imulq $8, %r11
	addq $32, %r11
	movq 16(%rsp), %rdx
	movq %rdx, %r10
	addq %r11, %r10
	movq 8(%rsp), %rdx
	movq %rdx, 0(%r10)
	movq %r9, %r11
	sarq $1, %r11
	movq $3, %r10
	sarq $1, %r10
	movq %r11, %r9
	addq %r10, %r9
	salq $1, %r9
	addq $1, %r9
	jmp _NNNNNNNNNNNNNNNNNN_global_30
	_NNNNNNNNNNNNNNNNNN_global_32:
	movq $_NNNNNNNNNNNNNNNNNN_global_37_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_37_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_36
	_NNNNNNNNNNNNNNNNNN_global_25:
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rdi
	addq $16, %rdi
	movq 16(%rsp), %rdx
	movq %rdx, %rdx
	addq %rdi, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, 0(%rsp)
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rdi
	addq $16, %rdi
	movq 16(%rsp), %rdx
	movq %rdx, %rdx
	addq %rdi, %rdx
	movq 0(%rdx), %rdi
	movq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_28
_matrixMultiplication:
	subq $48, %rsp
	movq %rdi, %rdi
	movq %rsi, %rsi
	movq %rsi, 40(%rsp)
	movq %rdx, %rdx
	movq %rdx, 32(%rsp)
	_NNNNNNNNNNNNNNNNNN_global_38:
	jmp _NNNNNNNNNNNNNNNNNN_global_39
	_NNNNNNNNNNNNNNNNNN_global_40:
	addq $48, %rsp
	retq
	_NNNNNNNNNNNNNNNNNN_global_41:
	movq %rsi, %r9
	sarq $1, %r9
	movq $3, %r10
	sarq $1, %r10
	movq %r9, %rsi
	addq %r10, %rsi
	salq $1, %rsi
	addq $1, %rsi
	jmp _NNNNNNNNNNNNNNNNNN_global_42
	_NNNNNNNNNNNNNNNNNN_global_42:
	movq %rsi, %r9
	sarq $1, %r9
	movq 24(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_43
	jmp _NNNNNNNNNNNNNNNNNN_global_40
	_NNNNNNNNNNNNNNNNNN_global_43:
	movq $1, %rdx
	movq %rdx, 8(%rsp)
	jmp _NNNNNNNNNNNNNNNNNN_global_44
	_NNNNNNNNNNNNNNNNNN_global_44:
	movq 8(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	movq 16(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_45
	jmp _NNNNNNNNNNNNNNNNNN_global_41
	_NNNNNNNNNNNNNNNNNN_global_45:
	movq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_46
	_NNNNNNNNNNNNNNNNNN_global_46:
	movq %r8, %r9
	sarq $1, %r9
	movq 0(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_47
	jmp _NNNNNNNNNNNNNNNNNN_global_48
	_NNNNNNNNNNNNNNNNNN_global_47:
	movq %rsi, %r10
	sarq $1, %r10
	movq %r8, %r9
	sarq $1, %r9
	movq $0, %rdx
	cmpq $0, %rdi
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_49
	jmp _NNNNNNNNNNNNNNNNNN_global_50
	_NNNNNNNNNNNNNNNNNN_global_50:
	movq %r10, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %r11
	addq $16, %r11
	movq %rdi, %rdx
	addq %r11, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_51
	jmp _NNNNNNNNNNNNNNNNNN_global_49
	_NNNNNNNNNNNNNNNNNN_global_51:
	movq %r9, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %r11
	addq $16, %r11
	movq %rdi, %rdx
	addq %r11, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r9
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_52
	jmp _NNNNNNNNNNNNNNNNNN_global_49
	_NNNNNNNNNNNNNNNNNN_global_52:
	jmp _NNNNNNNNNNNNNNNNNN_global_53
	_NNNNNNNNNNNNNNNNNN_global_53:
	movq %rdi, %rdx
	addq $24, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %rdx
	sarq $1, %rdx
	movq $1, %r11
	imulq %rdx, %r11
	movq %r10, %r10
	imulq %r11, %r10
	movq $0, %rdx
	addq %r10, %rdx
	addq %r9, %rdx
	movq %rdx, %r9
	imulq $8, %r9
	addq $32, %r9
	movq %rdi, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %r9
	movq %r8, %r10
	sarq $1, %r10
	movq 8(%rsp), %rdx
	movq %rdx, %r11
	sarq $1, %r11
	movq $0, %rdx
	movq 40(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_54
	jmp _NNNNNNNNNNNNNNNNNN_global_55
	_NNNNNNNNNNNNNNNNNN_global_55:
	movq %r10, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 40(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_56
	jmp _NNNNNNNNNNNNNNNNNN_global_54
	_NNNNNNNNNNNNNNNNNN_global_56:
	movq %r11, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 40(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r11
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_57
	jmp _NNNNNNNNNNNNNNNNNN_global_54
	_NNNNNNNNNNNNNNNNNN_global_57:
	jmp _NNNNNNNNNNNNNNNNNN_global_58
	_NNNNNNNNNNNNNNNNNN_global_58:
	movq %r9, %r9
	sarq $1, %r9
	movq 40(%rsp), %rdx
	movq %rdx, %rdx
	addq $24, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %rcx
	sarq $1, %rcx
	movq $1, %rdx
	imulq %rcx, %rdx
	movq %r10, %r10
	imulq %rdx, %r10
	movq $0, %rdx
	addq %r10, %rdx
	addq %r11, %rdx
	movq %rdx, %r10
	imulq $8, %r10
	addq $32, %r10
	movq 40(%rsp), %rdx
	movq %rdx, %rdx
	addq %r10, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	movq %r9, %r11
	imulq %r10, %r11
	salq $1, %r11
	addq $1, %r11
	movq %rsi, %r10
	sarq $1, %r10
	movq 8(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	movq $0, %rdx
	movq 32(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_59
	jmp _NNNNNNNNNNNNNNNNNN_global_60
	_NNNNNNNNNNNNNNNNNN_global_60:
	movq %r10, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_61
	jmp _NNNNNNNNNNNNNNNNNN_global_59
	_NNNNNNNNNNNNNNNNNN_global_61:
	movq %r9, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r9
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_62
	jmp _NNNNNNNNNNNNNNNNNN_global_59
	_NNNNNNNNNNNNNNNNNN_global_62:
	jmp _NNNNNNNNNNNNNNNNNN_global_63
	_NNNNNNNNNNNNNNNNNN_global_63:
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq $24, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %rdx
	sarq $1, %rdx
	movq $1, %rcx
	imulq %rdx, %rcx
	movq %r10, %rdx
	imulq %rcx, %rdx
	movq $0, %r10
	addq %rdx, %r10
	addq %r9, %r10
	movq %r10, %r9
	imulq $8, %r9
	addq $32, %r9
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq 0(%rdx), %r9
	movq %r9, %r9
	sarq $1, %r9
	movq $9, %r10
	sarq $1, %r10
	movq %r9, %r9
	imulq %r10, %r9
	salq $1, %r9
	addq $1, %r9
	movq %r9, %r9
	sarq $1, %r9
	movq %r11, %r10
	sarq $1, %r10
	movq %r9, %r9
	addq %r10, %r9
	salq $1, %r9
	addq $1, %r9
	movq %rsi, %r11
	sarq $1, %r11
	movq 8(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	movq $0, %rdx
	movq 32(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_64
	jmp _NNNNNNNNNNNNNNNNNN_global_65
	_NNNNNNNNNNNNNNNNNN_global_65:
	movq %r11, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r11
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_66
	jmp _NNNNNNNNNNNNNNNNNN_global_64
	_NNNNNNNNNNNNNNNNNN_global_66:
	movq %r10, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_67
	jmp _NNNNNNNNNNNNNNNNNN_global_64
	_NNNNNNNNNNNNNNNNNN_global_67:
	jmp _NNNNNNNNNNNNNNNNNN_global_68
	_NNNNNNNNNNNNNNNNNN_global_68:
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq $24, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %rcx
	sarq $1, %rcx
	movq $1, %rdx
	imulq %rcx, %rdx
	movq %r11, %r11
	imulq %rdx, %r11
	movq $0, %rdx
	addq %r11, %rdx
	addq %r10, %rdx
	movq %rdx, %r10
	imulq $8, %r10
	addq $32, %r10
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r10, %rdx
	movq %r9, 0(%rdx)
	movq %r8, %r9
	sarq $1, %r9
	movq $3, %r10
	sarq $1, %r10
	movq %r9, %r8
	addq %r10, %r8
	salq $1, %r8
	addq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_46
	_NNNNNNNNNNNNNNNNNN_global_48:
	movq 8(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	movq $3, %r10
	sarq $1, %r10
	movq %r9, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq %r10, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	salq $1, %rdx
	movq %rdx, 8(%rsp)
	movq 8(%rsp), %rdx
	addq $1, %rdx
	movq %rdx, 8(%rsp)
	jmp _NNNNNNNNNNNNNNNNNN_global_44
	_NNNNNNNNNNNNNNNNNN_global_64:
	movq $_NNNNNNNNNNNNNNNNNN_global_69_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_69_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_68
	_NNNNNNNNNNNNNNNNNN_global_59:
	movq $_NNNNNNNNNNNNNNNNNN_global_70_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_70_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_63
	_NNNNNNNNNNNNNNNNNN_global_54:
	movq $_NNNNNNNNNNNNNNNNNN_global_71_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_71_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_58
	_NNNNNNNNNNNNNNNNNN_global_49:
	movq $_NNNNNNNNNNNNNNNNNN_global_72_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_72_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_53
	_NNNNNNNNNNNNNNNNNN_global_73:
	movq $1, %rsi
	jmp _NNNNNNNNNNNNNNNNNN_global_42
	_NNNNNNNNNNNNNNNNNN_global_74:
	movq %rsi, %r9
	sarq $1, %r9
	movq $3, %r10
	sarq $1, %r10
	movq %r9, %rsi
	addq %r10, %rsi
	salq $1, %rsi
	addq $1, %rsi
	jmp _NNNNNNNNNNNNNNNNNN_global_75
	_NNNNNNNNNNNNNNNNNN_global_75:
	movq %rsi, %r9
	sarq $1, %r9
	movq 24(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_76
	jmp _NNNNNNNNNNNNNNNNNN_global_73
	_NNNNNNNNNNNNNNNNNN_global_76:
	movq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_77
	_NNNNNNNNNNNNNNNNNN_global_77:
	movq %r8, %r9
	sarq $1, %r9
	movq 16(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_78
	jmp _NNNNNNNNNNNNNNNNNN_global_74
	_NNNNNNNNNNNNNNNNNN_global_78:
	movq $1, %r11
	jmp _NNNNNNNNNNNNNNNNNN_global_79
	_NNNNNNNNNNNNNNNNNN_global_79:
	movq %r11, %r9
	sarq $1, %r9
	movq 0(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	setl %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_80
	jmp _NNNNNNNNNNNNNNNNNN_global_81
	_NNNNNNNNNNNNNNNNNN_global_80:
	movq %rsi, %r10
	sarq $1, %r10
	movq %r8, %r9
	sarq $1, %r9
	movq $0, %rdx
	movq 32(%rsp), %rdx
	cmpq $0, %rdx
	sete %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_82
	jmp _NNNNNNNNNNNNNNNNNN_global_83
	_NNNNNNNNNNNNNNNNNN_global_83:
	movq %r10, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r10
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_84
	jmp _NNNNNNNNNNNNNNNNNN_global_82
	_NNNNNNNNNNNNNNNNNN_global_84:
	movq %r9, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rcx
	addq $16, %rcx
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rcx, %rdx
	movq 0(%rdx), %rdx
	sarq $1, %rdx
	cmpq %rdx, %r9
	setl %dl
	movzbq %dl, %rdx
	cmpq $1, %rdx
	je _NNNNNNNNNNNNNNNNNN_global_85
	jmp _NNNNNNNNNNNNNNNNNN_global_82
	_NNNNNNNNNNNNNNNNNN_global_85:
	jmp _NNNNNNNNNNNNNNNNNN_global_86
	_NNNNNNNNNNNNNNNNNN_global_86:
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq $24, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %rcx
	sarq $1, %rcx
	movq $1, %rdx
	imulq %rcx, %rdx
	movq %r10, %r10
	imulq %rdx, %r10
	movq $0, %rdx
	addq %r10, %rdx
	addq %r9, %rdx
	movq %rdx, %r9
	imulq $8, %r9
	addq $32, %r9
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r9, %rdx
	movq $1, 0(%rdx)
	movq %r11, %r9
	sarq $1, %r9
	movq $3, %r10
	sarq $1, %r10
	movq %r9, %r11
	addq %r10, %r11
	salq $1, %r11
	addq $1, %r11
	jmp _NNNNNNNNNNNNNNNNNN_global_79
	_NNNNNNNNNNNNNNNNNN_global_81:
	movq %r8, %r9
	sarq $1, %r9
	movq $3, %r10
	sarq $1, %r10
	movq %r9, %r8
	addq %r10, %r8
	salq $1, %r8
	addq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_77
	_NNNNNNNNNNNNNNNNNN_global_82:
	movq $_NNNNNNNNNNNNNNNNNN_global_87_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_87_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_86
	_NNNNNNNNNNNNNNNNNN_global_88:
	movq $1, %rsi
	jmp _NNNNNNNNNNNNNNNNNN_global_75
	_NNNNNNNNNNNNNNNNNN_global_89:
	movq %r8, %r9
	sarq $1, %r9
	movq 16(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	sete %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_88
	jmp _NNNNNNNNNNNNNNNNNN_global_40
	_NNNNNNNNNNNNNNNNNN_global_90:
	movq %rsi, %r9
	sarq $1, %r9
	movq 24(%rsp), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	sete %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_89
	jmp _NNNNNNNNNNNNNNNNNN_global_40
	_NNNNNNNNNNNNNNNNNN_global_39:
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rdx
	addq $16, %rdx
	movq %rdi, %rsi
	addq %rdx, %rsi
	movq 0(%rsi), %rdx
	movq %rdx, 24(%rsp)
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rsi
	addq $16, %rsi
	movq 40(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, 16(%rsp)
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %rsi
	addq $16, %rsi
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %rsi, %rdx
	movq 0(%rdx), %rsi
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %r8
	addq $16, %r8
	movq 32(%rsp), %rdx
	movq %rdx, %rdx
	addq %r8, %rdx
	movq 0(%rdx), %r8
	movq $1, %rdx
	imulq $8, %rdx
	movq %rdx, %rdx
	addq $16, %rdx
	movq %rdi, %r9
	addq %rdx, %r9
	movq 0(%r9), %rdx
	movq %rdx, 0(%rsp)
	movq 0(%rsp), %rdx
	movq %rdx, %r9
	sarq $1, %r9
	movq $0, %rdx
	imulq $8, %rdx
	movq %rdx, %r10
	addq $16, %r10
	movq 40(%rsp), %rdx
	movq %rdx, %rdx
	addq %r10, %rdx
	movq 0(%rdx), %rdx
	movq %rdx, %r10
	sarq $1, %r10
	cmpq %r10, %r9
	sete %dl
	movzbq %dl, %rdx
	salq $1, %rdx
	addq $1, %rdx
	movq %rdx, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_90
	jmp _NNNNNNNNNNNNNNNNNN_global_40
_totalSum:
	subq $8, %rsp
	movq %rdi, %rdx
	_NNNNNNNNNNNNNNNNNN_global_91:
	jmp _NNNNNNNNNNNNNNNNNN_global_92
	_NNNNNNNNNNNNNNNNNN_global_93:
	movq %r10, %rax
	addq $8, %rsp
	retq
	_NNNNNNNNNNNNNNNNNN_global_94:
	movq %rdi, %r9
	sarq $1, %r9
	movq $3, %rax
	sarq $1, %rax
	movq %r9, %rdi
	addq %rax, %rdi
	salq $1, %rdi
	addq $1, %rdi
	jmp _NNNNNNNNNNNNNNNNNN_global_95
	_NNNNNNNNNNNNNNNNNN_global_95:
	movq %rdi, %r9
	sarq $1, %r9
	movq 0(%rsp), %rax
	movq %rax, %rax
	sarq $1, %rax
	cmpq %rax, %r9
	setl %al
	movzbq %al, %rax
	salq $1, %rax
	addq $1, %rax
	movq %rax, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_96
	jmp _NNNNNNNNNNNNNNNNNN_global_93
	_NNNNNNNNNNNNNNNNNN_global_96:
	movq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_97
	_NNNNNNNNNNNNNNNNNN_global_97:
	movq %r8, %r9
	sarq $1, %r9
	movq %rsi, %rax
	sarq $1, %rax
	cmpq %rax, %r9
	setl %al
	movzbq %al, %rax
	salq $1, %rax
	addq $1, %rax
	movq %rax, %r9
	sarq $1, %r9
	cmpq $1, %r9
	je _NNNNNNNNNNNNNNNNNN_global_98
	jmp _NNNNNNNNNNNNNNNNNN_global_94
	_NNNNNNNNNNNNNNNNNN_global_98:
	movq %rdi, %rcx
	sarq $1, %rcx
	movq %r8, %r11
	sarq $1, %r11
	movq $0, %rax
	cmpq $0, %rdx
	sete %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _NNNNNNNNNNNNNNNNNN_global_99
	jmp _NNNNNNNNNNNNNNNNNN_global_100
	_NNNNNNNNNNNNNNNNNN_global_100:
	movq %rcx, %rax
	salq $1, %rax
	addq $1, %rax
	movq $0, %rax
	imulq $8, %rax
	movq %rax, %r9
	addq $16, %r9
	movq %rdx, %rax
	addq %r9, %rax
	movq 0(%rax), %rax
	sarq $1, %rax
	cmpq %rax, %rcx
	setl %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _NNNNNNNNNNNNNNNNNN_global_101
	jmp _NNNNNNNNNNNNNNNNNN_global_99
	_NNNNNNNNNNNNNNNNNN_global_101:
	movq %r11, %rax
	salq $1, %rax
	addq $1, %rax
	movq $1, %rax
	imulq $8, %rax
	movq %rax, %r9
	addq $16, %r9
	movq %rdx, %rax
	addq %r9, %rax
	movq 0(%rax), %rax
	sarq $1, %rax
	cmpq %rax, %r11
	setl %al
	movzbq %al, %rax
	cmpq $1, %rax
	je _NNNNNNNNNNNNNNNNNN_global_102
	jmp _NNNNNNNNNNNNNNNNNN_global_99
	_NNNNNNNNNNNNNNNNNN_global_102:
	jmp _NNNNNNNNNNNNNNNNNN_global_103
	_NNNNNNNNNNNNNNNNNN_global_103:
	movq %r10, %r9
	sarq $1, %r9
	movq %rdx, %rax
	addq $24, %rax
	movq 0(%rax), %rax
	movq %rax, %r10
	sarq $1, %r10
	movq $1, %rax
	imulq %r10, %rax
	movq %rcx, %r10
	imulq %rax, %r10
	movq $0, %rax
	addq %r10, %rax
	addq %r11, %rax
	movq %rax, %r10
	imulq $8, %r10
	addq $32, %r10
	movq %rdx, %rax
	addq %r10, %rax
	movq 0(%rax), %rax
	movq %rax, %rax
	sarq $1, %rax
	movq %r9, %r10
	addq %rax, %r10
	salq $1, %r10
	addq $1, %r10
	movq %r8, %r9
	sarq $1, %r9
	movq $3, %rax
	sarq $1, %rax
	movq %r9, %r8
	addq %rax, %r8
	salq $1, %r8
	addq $1, %r8
	jmp _NNNNNNNNNNNNNNNNNN_global_97
	_NNNNNNNNNNNNNNNNNN_global_99:
	movq $_NNNNNNNNNNNNNNNNNN_global_104_ret, -8(%rsp)
	movq $0, %rdi
	call array_tensor_error_null
	_NNNNNNNNNNNNNNNNNN_global_104_ret:
	jmp _NNNNNNNNNNNNNNNNNN_global_103
	_NNNNNNNNNNNNNNNNNN_global_92:
	movq $0, %rax
	imulq $8, %rax
	movq %rax, %rsi
	addq $16, %rsi
	movq %rdx, %rax
	addq %rsi, %rax
	movq 0(%rax), %rax
	movq %rax, 0(%rsp)
	movq $1, %rax
	imulq $8, %rax
	movq %rax, %rsi
	addq $16, %rsi
	movq %rdx, %rax
	addq %rsi, %rax
	movq 0(%rax), %rsi
	movq $1, %rdi
	movq $1, %r10
	jmp _NNNNNNNNNNNNNNNNNN_global_95
