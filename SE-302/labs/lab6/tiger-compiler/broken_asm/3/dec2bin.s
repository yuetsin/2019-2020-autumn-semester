.text
.globl dec2bin
.type dec2bin, @function

dec2bin:
pushq %rbp
movq %rsp, %rbp
subq $96, %rsp
L21:
movq 8(%rbp), %r12
movq %r12, -96(%rbp)
movq -88(%rbp), %r10
movq -96(%rbp), %r13
cmpq %r10, %r13
jg L1
L2:
jmp L20
L1:
movq 8(%rbp), %r11
movq %r11, -80(%rbp)
movq -80(%rbp), %r13
movq %rax, %r13
movq -72(%rbp), %r10
idivq %r10
movq %r12, %rax
movq %r12, -64(%rbp)
movq -64(%rbp), %r13
movq %r13, %rdi
callq dec2bin
movq 8(%rbp), %r11
movq %r11, -56(%rbp)
movq 8(%rbp), %r12
movq %r12, -48(%rbp)
movq -48(%rbp), %r10
movq %rax, %r10
movq -40(%rbp), %r13
idivq %r13
movq %r11, %rax
movq %r11, -32(%rbp)
movq -32(%rbp), %r10
movq %rax, %r10
movq -24(%rbp), %r13
imulq %r13
movq %r12, %rax
movq %r12, -16(%rbp)
movq -8(%rbp), %r10
movq -16(%rbp), %r10
subq %r10, %r11
movq %r11, -8(%rbp)
movq -8(%rbp), %r13
movq %r13, %rdi
callq printi
jmp L2
L20:
addq $96, %rsp
popq %rbp
ret


.size dec2bin, .-dec2bin
.globl try
.type try, @function

try:
pushq %rbp
movq %rsp, %rbp
subq $136, %rsp
L23:
movq -136(%rbp), %r10
movq %r10, %rdi
callq printi
movq -128(%rbp), %r13
movq %r13, %rdi
callq printi
movq L5(%rip), %r12
movq %r12, -120(%rbp)
movq -120(%rbp), %r10
movq %r10, %rdi
callq print
movq -112(%rbp), %r13
movq %r13, %rdi
callq dec2bin
movq L7(%rip), %r11
movq %r11, -104(%rbp)
movq -104(%rbp), %r10
movq %r10, %rdi
callq print
movq -96(%rbp), %r13
movq %r13, %rdi
callq printi
movq L9(%rip), %r12
movq %r12, -88(%rbp)
movq -88(%rbp), %r10
movq %r10, %rdi
callq print
movq -80(%rbp), %r13
movq %r13, %rdi
callq dec2bin
movq L11(%rip), %r11
movq %r11, -72(%rbp)
movq -72(%rbp), %r10
movq %r10, %rdi
callq print
movq -64(%rbp), %r13
movq %r13, %rdi
callq printi
movq L13(%rip), %r12
movq %r12, -56(%rbp)
movq -56(%rbp), %r10
movq %r10, %rdi
callq print
movq -48(%rbp), %r13
movq %r13, %rdi
callq dec2bin
movq L15(%rip), %r11
movq %r11, -40(%rbp)
movq -40(%rbp), %r10
movq %r10, %rdi
callq print
movq -32(%rbp), %r13
movq %r13, %rdi
callq printi
movq L17(%rip), %r12
movq %r12, -24(%rbp)
movq -24(%rbp), %r10
movq %r10, %rdi
callq print
movq -16(%rbp), %r13
movq %r13, %rdi
callq dec2bin
movq L19(%rip), %r11
movq %r11, -8(%rbp)
movq -8(%rbp), %r10
movq %r10, %rdi
callq print
jmp L22
L22:
addq $136, %rsp
popq %rbp
ret


.size try, .-try
.globl tigermain
.type tigermain, @function

tigermain:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
L25:
callq try
jmp L24
L24:
addq $0, %rsp
popq %rbp
ret


.size tigermain, .-tigermain
.section .rodata
.globl L4
.type L4, @object
.size L4, 4
L4:
.string "\t->\t"
.globl L5
.type L5, @object
.size L5, 4
L5:
.string "\t->\t"
.globl L6
.type L6, @object
.size L6, 1
L6:
.string "\n"
.globl L7
.type L7, @object
.size L7, 1
L7:
.string "\n"
.globl L8
.type L8, @object
.size L8, 4
L8:
.string "\t->\t"
.globl L9
.type L9, @object
.size L9, 4
L9:
.string "\t->\t"
.globl L10
.type L10, @object
.size L10, 1
L10:
.string "\n"
.globl L11
.type L11, @object
.size L11, 1
L11:
.string "\n"
.globl L12
.type L12, @object
.size L12, 4
L12:
.string "\t->\t"
.globl L13
.type L13, @object
.size L13, 4
L13:
.string "\t->\t"
.globl L14
.type L14, @object
.size L14, 1
L14:
.string "\n"
.globl L15
.type L15, @object
.size L15, 1
L15:
.string "\n"
.globl L16
.type L16, @object
.size L16, 4
L16:
.string "\t->\t"
.globl L17
.type L17, @object
.size L17, 4
L17:
.string "\t->\t"
.globl L18
.type L18, @object
.size L18, 1
L18:
.string "\n"
.globl L19
.type L19, @object
.size L19, 1
L19:
.string "\n"