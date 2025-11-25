.data
.balign 8
global_counter:
	.int 0
/* end data */

.data
.balign 8
result_accumulator:
	.int 0
/* end data */

.text
.globl factorial_int
factorial_int:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	pushq %rbx
	cmpl $2, %edi
	jl .Lbb2
	movl %edi, %ebx
	subl $1, %edi
	callq factorial_int
	movl %ebx, %edi
	imull %edi, %eax
	jmp .Lbb3
.Lbb2:
	movl $1, %eax
.Lbb3:
	popq %rbx
	leave
	ret
.type factorial_int, @function
.size factorial_int, .-factorial_int
/* end function factorial_int */

.text
.globl is_even_int
is_even_int:
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, %eax
	movl $2, %ecx
	cltd
	idivl %ecx
	movl %edx, %eax
	cmpl $0, %eax
	jz .Lbb6
	movl $0, %eax
	jmp .Lbb7
.Lbb6:
	movl $1, %eax
.Lbb7:
	leave
	ret
.type is_even_int, @function
.size is_even_int, .-is_even_int
/* end function is_even_int */

.text
.globl loop_logic_test_int
loop_logic_test_int:
	pushq %rbp
	movq %rsp, %rbp
	movl $0, %eax
	movl $0, %ecx
.Lbb10:
	cmpl %edi, %ecx
	jge .Lbb14
	cmpl $2, %ecx
	jz .Lbb13
	addl %ecx, %eax
	cmpl $20, %eax
	jg .Lbb14
.Lbb13:
	addl $1, %ecx
	jmp .Lbb10
.Lbb14:
	leave
	ret
.type loop_logic_test_int, @function
.size loop_logic_test_int, .-loop_logic_test_int
/* end function loop_logic_test_int */

.text
.globl math_test
math_test:
	pushq %rbp
	movq %rsp, %rbp
	movl $20, %eax
	leave
	ret
.type math_test, @function
.size math_test, .-math_test
/* end function math_test */

.text
.globl math_test_int
math_test_int:
	pushq %rbp
	movq %rsp, %rbp
	imull $2, %edi, %eax
	leave
	ret
.type math_test_int, @function
.size math_test_int, .-math_test_int
/* end function math_test_int */

.text
.globl math_test_float
math_test_float:
	pushq %rbp
	movq %rsp, %rbp
	mulsd %xmm0, %xmm0
	cvttsd2sil %xmm0, %eax
	leave
	ret
.type math_test_float, @function
.size math_test_float, .-math_test_float
/* end function math_test_float */

.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	pushq %rbx
	pushq %r12
	pushq %r13
	movl $5, global_counter(%rip)
	movl $5, %edi
	callq factorial_int
	movl %eax, %r12d
	movl %r12d, %edi
	callq is_even_int
	cmpl $0, %eax
	jnz .Lbb23
	movl $0, result_accumulator(%rip)
	jmp .Lbb24
.Lbb23:
	movl $1, result_accumulator(%rip)
.Lbb24:
	subq $16, %rsp
	movq %rsp, %r13
	movl $10, %edi
	callq loop_logic_test_int
	movl %eax, %ebx
	movl %ebx, (%r13)
	subq $16, %rsp
	movq %rsp, %r13
	movl $10, %edi
	callq math_test_int
	movl %eax, (%r13)
	subq $16, %rsp
	movq %rsp, %r13
	callq math_test
	movl %eax, %edx
	movl %edx, (%r13)
	subq $16, %rsp
	movq %rsp, %rcx
	movl result_accumulator(%rip), %eax
	addl %r12d, %eax
	addl %ebx, %eax
	addl %edx, %eax
	movl %eax, (%rcx)
	movq %rbp, %rsp
	subq $32, %rsp
	popq %r13
	popq %r12
	popq %rbx
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
