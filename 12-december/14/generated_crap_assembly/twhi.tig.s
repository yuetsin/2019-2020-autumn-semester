.text
.globl tigermain
.type tigermain, @function

tigermain:
pushq %rbp
movq %rsp, %rbp
subq $96, %rsp
L8:
movq -96(%rbp), %r10
movq %r10, -16(%rbp)
L1:
movq -8(%rbp), %r12
movq %r12, -88(%rbp)
movq -80(%rbp), %r13
movq -88(%rbp), %r10
cmpq %r13, %r10
jge L2
L3:
jmp L7
L2:
movq -8(%rbp), %r12
movq %r12, -72(%rbp)
callq printi
movq -8(%rbp), %r12
movq %r12, -64(%rbp)
callq printi
movq -8(%rbp), %r12
movq %r12, -56(%rbp)
movq -40(%rbp), %r13
movq -48(%rbp), %r13
subq %r13, %r12
movq %r12, -48(%rbp)
movq -48(%rbp), %r10
movq %r10, -8(%rbp)
movq -8(%rbp), %r12
movq %r12, -32(%rbp)
movq -24(%rbp), %r10
movq -32(%rbp), %r10
cmpq %r10, %r10
je L4
L5:
jmp L1
L4:
jmp L3
L7:
addq $96, %rsp
ret


.size tigermain, .-tigermain
.section .rodata