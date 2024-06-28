# Copyright © 2023~ UNKNOWN STRYKER. All Rights Reserved.
# This code is a part of Project Frogman, and the part of Frogman Engine memcpy implementation.
# WARNING: This code has been assembled but has not been tested on any machine.

# x86-64 AT&T assembly register look-up table
# https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf 
# qword  dword  word   byte
# %rax   %eax   %ax    %al
# %rcx   %ecx   %cx    %cl
# %rdx   %edx   %dx    %dl
# %rbx   %ebx   %bx    %bl
# %rsi   %esi   %si    %sil
# %rdi   %edi   %di    %dil
# %rsp   %esp   %sp    %spl
# %rbp   %ebp   %bp    %bpl
# %r8    %r8d   %r8w   %r8b
# %r9    %r9d   %r9w   %r9b
# %r10   %r10d  %r10w  %r10b
# %r11   %r11d  %r11w  %r11b
# %r12   %r12d  %r12w  %r12b
# %r13   %r13d  %r13w  %r13b
# %r14   %r14d  %r14w  %r14b
# %r15   %r15d  %r15w  %r15b

# References used to implement void __x86_64_aligned_memcpy(void* dest_p, const void* source_p, size_t bytes_to_copy_p):
# https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#techs=AVX_ALL&text=_mm256_store_si256&ig_expand=6491,6548,6491

.text # Marks the beginning of the code section where executable instructions are defined.
.global __x86_64_aligned_memcpy # Declares the name as a global symbol, making it accessible to other parts of the program.

# X86-64 calling convention parameter order:
# first: %rdi
# second: %rsi
# third: %rdx
# fourth: %rcx
# fifth: %r8
# sixth: %r9

# void* dest_p: %rdi
# const void* source_p: %rsi
# size_t bytes_to_copy_p: %rdx 
__x86_64_aligned_memcpy:
    # Note: jge is for signed, and jae is for unsigned.

    cmpq %rdx, 32 # Compare the qword integer 32 with %rdx.
    jae copy_256bits_using_avx # Jump to copy_256bits_using_avx label if %rdx is greater or equal to 32.
 
    cmpq %rdx, 16 # Compare the qword integer 16 with %rdx.
    jae copy_128bits_using_sse2 # Jump to copy_128bits_using_sse2 label if %rdx is greater or equal to 16.

    cmpq %rdx, 8 # Compare the qword integer 8 with %rdx.
    jae copy_64bits # Jump to copy_64bits label if %rdx is greater or equal to 8.

    cmpq %rdx, 4 # Compare the qword integer 4 with %rdx.
    jae copy_32bits # Jump to copy_32bits label if %rdx is greater or equal to 4.

    cmpq %rdx, 2 # Compare the qword integer 2 with %rdx.
    jae copy_16bits # Jump to copy_16bits label if %rdx is greater or equal to 2.

    cmpq %rdx, 1 # Compare the qword integer 1 with %rdx.
    jae copy_8bits # Jump to copy_8bits label if %rdx is greater or equal to 1.

    ret




copy_256bits_using_avx:
    vmovdqa (%rsi), %ymm0 # Dereference %rsi and load the data into AVX 256-bit %ymm vector register.
    vmovdqa %ymm0, (%rdi) # Store the loaded 256 bits data into the dereferenced %rdi.
    addq 32, %rdi # Increment the void* by 32 bytes
    addq 32, %rsi # Increment the void* by 32 bytes
    subq 32, %rdx # Decrement the size to copy by 32 bytes
    jmp __x86_64_aligned_memcpy



copy_128bits_using_sse2:
    movdqa (%rsi), %xmm0 # Dereference %rsi and load the data into SSE2 128-bit %xmm vector register.
    vmovdqa %xmm0, (%rdi) # Store the loaded 128 bits data into the dereferenced %rdi.
    addq 16, %rdi # Increment the void* by 16 bytes
    addq 16, %rsi # Increment the void* by 16 bytes
    subq 16, %rdx # Decrement the size to copy by 16 bytes
    jmp __x86_64_aligned_memcpy



# qword refers to an 64-bit value (suffix q).
copy_64bits:
    movq (%rsi), %rax # Dereference %rsi and load the data into qword %rax register.
    movq %rax, (%rdi) # Store the loaded 64 bits data into the dereferenced %rdi.
    addq 8, %rdi # Increment the void* by 8 bytes
    addq 8, %rsi # Increment the void* by 8 bytes
    subq 8, %rdx # Decrement the size to copy by 8 bytes
    jmp __x86_64_aligned_memcpy



# dword refers to a 32-bit integer (suffix l)
copy_32bits:
    movl (%rsi), %eax # Dereference %rsi and load the data into dword %eax register.
    movl %eax, (%rdi) # Store the loaded 32 bits data into the dereferenced %rdi.
    addq 4, %rdi # Increment the void* by 4 bytes
    addq 4, %rsi # Increment the void* by 4 bytes
    subq 4, %rdx # Decrement the size to copy by 4 bytes
    jmp __x86_64_aligned_memcpy



# word refers to a 16-bit integer (suffix w)
copy_16bits:
    movw (%rsi), %ax # Dereference %rsi and load the data into word %ax register.
    movw %ax, (%rdi) # Store the loaded 16 bits data into the dereferenced %rdi.
    addq 2, %rdi # Increment the void* by 2 bytes
    addq 2, %rsi # Increment the void* by 2 bytes
    subq 2, %rdx # Decrement the size to copy by 2 bytes
    jmp __x86_64_aligned_memcpy



# byte refers to a 8-bit integer (suffix b)
copy_8bits:
    movb (%rsi), %al # Dereference %rsi and load the data into byte %al register.
    movb %al, (%rdi) # Store the loaded 8 bits data into the dereferenced %rdi.
    addq 1, %rdi # Increment the void* by a byte
    addq 1, %rsi # Increment the void* by a byte
    subq 1, %rdx # Decrement the size to copy by a byte
    jmp __x86_64_aligned_memcpy