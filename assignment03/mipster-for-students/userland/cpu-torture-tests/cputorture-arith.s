#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Torture test for every individual instruction.  Some instructions are
# expected to generate an exception, and such instructions are marked by
# a label of the form EXNxxxx (where xxxx is a number).  Such instructions
# (and the one following it) are silently skipped over by the controlling
# program.  If the instruction does NOT cause an exception, a BREAK happens.
# Test failures are reported through BREAKs with non-0 code.  Successful
# completion of all tests ends by BREAK with code 0.
#
# Break codes:
# 0 = everything OK
# 1 = $0 has been modified
# 2 = result was not as expected
# 3 = arithmetic/memory exception expected, but not taken
#
# This source file is to be preprocessed by M4 to generate the source which
# can be fed into assembler.
#
# Additional operations recognized by assembler:
# li   = load 32-bit immediate
# la   = load address
# nop  = no operation
# move = move between registers
#
# How to build and run:
#
# m4 cputorture.sm4 > cputorture.s
# mipsel-elf-gcc cputorture.s -nostdlib -Ttext 0x1000 -o cputorture
# ./dist/Debug/Sun-Generic/mipstorture cputorture 
#
# If everything goes OK, the execution should end with exception 5, code 0
# and PC near the label SUCCESS.
#

#
# Macros used from different sources.
#

#
# Expand R(N) to $N, e.g. R(1) -> $1
#

#
# Generate unique ID.
#



#
# Code to test ALU operations with immediate operand (including shifts by
# constant amount).
# Arguments: (1:insn, 2:dst, 3:src, 4:srcop, 5:imm, 6:result) 
# Temporary regs: $23, $24
#


#
# Code to test 3-operand ALU operations.
# Arguments: (1:insn, 2:dst, 3:src1, 4:src2, 5:op1, 6:op2, 7:result)
# Temporary regs: $23, $24, $25
#


		.text
		.set	noreorder
		.set	nobopt

		.globl _start
_start:
#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Included as part of cputorture.sm4.  Tests arithmetic.
#
# Registers are cyclically used to test everything thoroughly.  $23-$25 should
# not be used by tests as macros already use them, $26-$27 should be avoided
# for compatibility with other simulators.
#

#
# Code to test {mult,div}{,u}.  The assembler macro-expands mul/div
# instructions, where it inserts extra checks (e.g. overflows).  We do NOT
# want that, and there is no way to disable this expansion in assembler, so
# the instruction is hard-coded as literal word.  Operands (op1, op2) are
# loaded into registers $2, $3 and this should be taken into account in
# instruction encoding too.
#
# Arguments: (1:insn, 2:op1, 3:op2, 4:result_hi, 5:result_lo)
# Temporary regs: $23-$25
#


#
# Code to test arithmetic overflows; immediate variant.  There is no expected
# result, as overflow is expected.
# Arguments: (1:insn, 2:src, 3:srcop, 4:imm, 5:label)
# Temporary regs: $23
#


#
# Code to test arithmetic overflows; 3-operand variant.  There is no expected
# result, as overflow is expected.
# Arguments: (1:insn, 2:src1, 3:src2, 4:srcop1, 5:srcop2, 6:label)
# Temporary regs: $23
#


		.text

		# 4 sign combinations for each of {add,sub,mult,div}{,u}.

		# Instructions with sign-extended immediate operands.

		
t_alui_addi_1:
		li		$3, 0x12345678			# 1st source operand
		li		$23, 0x12349679		# expected result
		move	$24, $3		# back up src
		li		$2, 0			# dst := 0
		addi		$0, $3, 0x4001	# try to change r0
		bne		$0, $2, B1	# taken if r0 or $2 modified
		addi		$2, $3, 0x4001		# dst := src (addi) imm
		bne		$2, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$3, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_2:
		li		$5, 0x12345678			# 1st source operand
		li		$23, 0x12345677		# expected result
		move	$24, $5		# back up src
		li		$4, 0			# dst := 0
		addi		$0, $5, 0xFFFF	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		addi		$4, $5, 0xFFFF		# dst := src (addi) imm
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_3:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		addi		$0, $7, 0x0001	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addi		$6, $7, 0x0001		# dst := src (addi) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_4:
		li		$9, 0xFFFFFFF1			# 1st source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		addi		$0, $9, 0xFFFF	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		addi		$8, $9, 0xFFFF		# dst := src (addi) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alui_addiu_5:
		li		$11, 0x12345678			# 1st source operand
		li		$23, 0x12349679		# expected result
		move	$24, $11		# back up src
		li		$10, 0			# dst := 0
		addiu		$0, $11, 0x4001	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		addiu		$10, $11, 0x4001		# dst := src (addiu) imm
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addiu_6:
		li		$13, 0x12345678			# 1st source operand
		li		$23, 0x12345677		# expected result
		move	$24, $13		# back up src
		li		$12, 0			# dst := 0
		addiu		$0, $13, 0xFFFF	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		addiu		$12, $13, 0xFFFF		# dst := src (addiu) imm
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addiu_7:
		li		$15, 0xFFFFFFF0			# 1st source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $15		# back up src
		li		$14, 0			# dst := 0
		addiu		$0, $15, 0x0001	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		addiu		$14, $15, 0x0001		# dst := src (addiu) imm
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addiu_8:
		li		$17, 0xFFFFFFF1			# 1st source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $17		# back up src
		li		$16, 0			# dst := 0
		addiu		$0, $17, 0xFFFF	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		addiu		$16, $17, 0xFFFF		# dst := src (addiu) imm
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		# 3-register arithmetic instructions.

		
t_alu3_add_9:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x723EE679		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_10:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345676		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_11:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_12:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_addu_13:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x723EE679		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_addu_14:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345676		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_addu_15:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_addu_16:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_sub_17:
		li		$7, 0x723EE679			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_18:
		li		$7, 0x12345676			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_19:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_20:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_subu_21:
		li		$7, 0x723EE679			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_subu_22:
		li		$7, 0x12345676			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_subu_23:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_subu_24:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		# Multiplication and division.  Due to assembler's expansion of these
		# instructions, we hard-code the instruction with $2,$3 as operands.

		# mult
		
t_muldiv_25:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x06D460B5		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0xE387D678		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_26:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0xFFFFFFFE		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0xFFFFFFFF		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0xDB975310		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_27:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0xD0D26625		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x449CF679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_28:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x80000001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x3EE08CC3		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x023EE679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# multu
		
t_muldiv_29:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x06D460B5		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0xE387D678		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_30:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0xFFFFFFFE		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x12345677		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0xDB975310		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_31:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x30DCF626		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x449CF679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_32:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x80000001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x411F733D		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x023EE679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# div. expected result is remainder,quotient
		
t_muldiv_33:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0x02000021		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x000A89D1		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x00000030		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_34:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0xFFFFFFE3		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x00000001		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0xFCB03000		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_35:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x200A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0xE25E967C 		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0xFFFFFFFD		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_36:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0xFFFFFFF7		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0xFFFFFFFF		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x0DF902D6		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# divu. expected result is remainder,quotient
		
t_muldiv_37:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0x02000021		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x000A89D1		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x00000030		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

  	
t_muldiv_38:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0xFFFFFFE3		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x600A9001		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x00000000		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_39:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x200A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x0214A675		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x00000004		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_40:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0xFFFFFFF7		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
    nop
		li		$24, 0x823EE679		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
    nop
		li		$25, 0x00000000		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
    nop
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# Test that add/sub (do not) trigger overflow in right circumstances
		# and that unsigned variants give correct results.  Note that sub
		# with immediate argument does not exist.

		
t_alui_addi_41:
		li		$3, 0x7FFFFFFE			# 1st source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $3		# back up src
		li		$2, 0			# dst := 0
		addi		$0, $3, 0x0001	# try to change r0
		bne		$0, $2, B1	# taken if r0 or $2 modified
		addi		$2, $3, 0x0001		# dst := src (addi) imm
		bne		$2, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$3, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_ovfi_addi_42:
		.globl	EXN01
		li		$6, 0x7FFFFFFF			# source operand
EXN01:		addi		$23, $6, 0x0001	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alui_addiu_43:
		li		$7, 0x7FFFFFFF			# 1st source operand
		li		$23, 0x80000000		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		addiu		$0, $7, 0x0001	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addiu		$6, $7, 0x0001		# dst := src (addiu) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_44:
		li		$5, 0x80000000			# 1st source operand
		li		$23, 0x80000001		# expected result
		move	$24, $5		# back up src
		li		$4, 0			# dst := 0
		addi		$0, $5, 0x0001	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		addi		$4, $5, 0x0001		# dst := src (addi) imm
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_ovfi_addi_45:
		.globl	EXN02
		li		$7, 0x80000000			# source operand
EXN02:		addi		$23, $7, 0xFFFF	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alui_addiu_46:
		li		$9, 0x80000000			# 1st source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		addiu		$0, $9, 0xFFFF	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		addiu		$8, $9, 0xFFFF		# dst := src (addiu) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alu3_add_47:
		li		$11, 0x7FFFFFFE			# 1st source operand
		li		$12, 0x00000001			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $11		# back up src1
		move	$25, $12		# back up src2
		li		$10, 0			# dst := 0
		add		$0, $11, $12	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		add		$10, $11, $12		# dst := src1 (add) src2
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# src1 modified
		nop						# branch slot
		bne		$12, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_add_48:
		.globl	EXN05
		li		$13, 0x7FFFFFFF			# 1st source operand
		li		$14, 0x00000001			# 2nd source operand
EXN05:		add		$23, $13, $14	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_addu_49:
		li		$14, 0x7FFFFFFF			# 1st source operand
		li		$15, 0x00000001			# 2nd source operand
		li		$23, 0x80000000		# expected result
		move	$24, $14		# back up src1
		move	$25, $15		# back up src2
		li		$13, 0			# dst := 0
		addu		$0, $14, $15	# try to change r0
		bne		$0, $13, B1	# taken if r0 or $13 modified
		addu		$13, $14, $15		# dst := src1 (addu) src2
		bne		$13, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$14, $24, B2	# src1 modified
		nop						# branch slot
		bne		$15, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_50:
		li		$17, 0x80000000			# 1st source operand
		li		$18, 0x00000001			# 2nd source operand
		li		$23, 0x80000001		# expected result
		move	$24, $17		# back up src1
		move	$25, $18		# back up src2
		li		$16, 0			# dst := 0
		add		$0, $17, $18	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		add		$16, $17, $18		# dst := src1 (add) src2
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# src1 modified
		nop						# branch slot
		bne		$18, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_add_51:
		.globl	EXN06
		li		$19, 0x80000000			# 1st source operand
		li		$20, 0xFFFFFFFF			# 2nd source operand
EXN06:		add		$23, $19, $20	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_addu_52:
		li		$28, 0x80000000			# 1st source operand
		li		$29, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $28		# back up src1
		move	$25, $29		# back up src2
		li		$22, 0			# dst := 0
		addu		$0, $28, $29	# try to change r0
		bne		$0, $22, B1	# taken if r0 or $22 modified
		addu		$22, $28, $29		# dst := src1 (addu) src2
		bne		$22, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$28, $24, B2	# src1 modified
		nop						# branch slot
		bne		$29, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_sub_53:
		li		$31, 0x7FFFFFFE			# 1st source operand
		li		$2, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $31		# back up src1
		move	$25, $2		# back up src2
		li		$30, 0			# dst := 0
		sub		$0, $31, $2	# try to change r0
		bne		$0, $30, B1	# taken if r0 or $30 modified
		sub		$30, $31, $2		# dst := src1 (sub) src2
		bne		$30, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$31, $24, B2	# src1 modified
		nop						# branch slot
		bne		$2, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_sub_54:
		.globl	EXN07
		li		$3, 0x7FFFFFFF			# 1st source operand
		li		$4, 0xFFFFFFFF			# 2nd source operand
EXN07:		sub		$23, $3, $4	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_subu_55:
		li		$7, 0x7FFFFFFF			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x80000000		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_56:
		li		$10, 0xFFFFFFFF			# 1st source operand
		li		$11, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFFE		# expected result
		move	$24, $10		# back up src1
		move	$25, $11		# back up src2
		li		$9, 0			# dst := 0
		sub		$0, $10, $11	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		sub		$9, $10, $11		# dst := src1 (sub) src2
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# src1 modified
		nop						# branch slot
		bne		$11, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_sub_57:
		.globl	EXN08
		li		$12, 0x80000000			# 1st source operand
		li		$13, 0x00000001			# 2nd source operand
EXN08:		sub		$23, $12, $13	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_subu_58:
		li		$16, 0x80000000			# 1st source operand
		li		$17, 0x00000001			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $16		# back up src1
		move	$25, $17		# back up src2
		li		$15, 0			# dst := 0
		subu		$0, $16, $17	# try to change r0
		bne		$0, $15, B1	# taken if r0 or $15 modified
		subu		$15, $16, $17		# dst := src1 (subu) src2
		bne		$15, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$16, $24, B2	# src1 modified
		nop						# branch slot
		bne		$17, $25, B2	# operand2 modified
		nop						# branch slot


		# Explicit m{t,f}{hi,lo} tests.

t_hilo:
.globl	t_hilo
		xor		$18, $18, $18
		li		$30, 0x12345678
		mthi	$30
		li		$31, 0x9ABCDEF0
		mtlo	$31
		mfhi	$0					# check for $0 modif
		bne		$0, $18, B1
		mfhi	$2
		bne		$2, $30, B2			# branch if read hi != stored
		mflo	$0					# check for $0 modif
		bne		$0, $18, B1
		mflo	$2					# delay slot
		bne		$2, $31, B2			# branch if read lo != stored
		nop							# delay slot
			# branch tests
		.globl	SUCCESS
SUCCESS:
		break 0				# mark successful completion

		# Failure labels
		.globl	FAILURE
FAILURE:
B1:		break	1
B2:		break	2
B3:		break	3
