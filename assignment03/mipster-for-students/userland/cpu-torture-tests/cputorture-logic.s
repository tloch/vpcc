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
# Included as part of cputorture.sm4.  Tests logic operations.
#
# Registers are cyclically used to test everything thoroughly.  $23-$25 should
# not be used by tests as macros already use them, $26-$27 should be avoided
# for compatibility with other simulators.
#

		.text

		# Bit operations with zero-extended immediate.

		
t_alui_andi_1:
		li		$22, 0x12345678			# 1st source operand
		li		$23, 0x00005678		# expected result
		move	$24, $22		# back up src
		li		$21, 0			# dst := 0
		andi		$0, $22, 0xFFFF	# try to change r0
		bne		$0, $21, B1	# taken if r0 or $21 modified
		andi		$21, $22, 0xFFFF		# dst := src (andi) imm
		bne		$21, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$22, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_ori_2:
		li		$29, 0x12345678			# 1st source operand
		li		$23, 0x1234F679		# expected result
		move	$24, $29		# back up src
		li		$28, 0			# dst := 0
		ori		$0, $29, 0xA001	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		ori		$28, $29, 0xA001		# dst := src (ori) imm
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_xori_3:
		li		$31, 0x12345678			# 1st source operand
		li		$23, 0x1234A987		# expected result
		move	$24, $31		# back up src
		li		$30, 0			# dst := 0
		xori		$0, $31, 0xFFFF	# try to change r0
		bne		$0, $30, B1	# taken if r0 or $30 modified
		xori		$30, $31, 0xFFFF		# dst := src (xori) imm
		bne		$30, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$31, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		# Bit operations on two registers.

		
t_alu3_and_4:
		li		$9, 0x12345678			# 1st source operand
		li		$10, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $9		# back up src1
		move	$25, $10		# back up src2
		li		$8, 0			# dst := 0
		and		$0, $9, $10	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		and		$8, $9, $10		# dst := src1 (and) src2
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# src1 modified
		nop						# branch slot
		bne		$10, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_or_5:
		li		$12, 0x12345678			# 1st source operand
		li		$13, 0xA001100A			# 2nd source operand
		li		$23, 0xB235567A		# expected result
		move	$24, $12		# back up src1
		move	$25, $13		# back up src2
		li		$11, 0			# dst := 0
		or		$0, $12, $13	# try to change r0
		bne		$0, $11, B1	# taken if r0 or $11 modified
		or		$11, $12, $13		# dst := src1 (or) src2
		bne		$11, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$12, $24, B2	# src1 modified
		nop						# branch slot
		bne		$13, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_xor_6:
		li		$15, 0x12345678			# 1st source operand
		li		$16, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xEDCBA987		# expected result
		move	$24, $15		# back up src1
		move	$25, $16		# back up src2
		li		$14, 0			# dst := 0
		xor		$0, $15, $16	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		xor		$14, $15, $16		# dst := src1 (xor) src2
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# src1 modified
		nop						# branch slot
		bne		$16, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_nor_7:
		li		$18, 0x12345678			# 1st source operand
		li		$19, 0x11111111			# 2nd source operand
		li		$23, 0xECCAA886		# expected result
		move	$24, $18		# back up src1
		move	$25, $19		# back up src2
		li		$17, 0			# dst := 0
		nor		$0, $18, $19	# try to change r0
		bne		$0, $17, B1	# taken if r0 or $17 modified
		nor		$17, $18, $19		# dst := src1 (nor) src2
		bne		$17, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$18, $24, B2	# src1 modified
		nop						# branch slot
		bne		$19, $25, B2	# operand2 modified
		nop						# branch slot


		# Shifts with immediate count.

		
t_alui_sll_8:
		li		$3, 0x40000000			# 1st source operand
		li		$23, 0x80000000		# expected result
		move	$24, $3		# back up src
		li		$2, 0			# dst := 0
		sll		$0, $3, 0x01	# try to change r0
		bne		$0, $2, B1	# taken if r0 or $2 modified
		sll		$2, $3, 0x01		# dst := src (sll) imm
		bne		$2, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$3, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sll_9:
		li		$5, 0x80000000			# 1st source operand
		li		$23, 0x00000000		# expected result
		move	$24, $5		# back up src
		li		$4, 0			# dst := 0
		sll		$0, $5, 0x01	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		sll		$4, $5, 0x01		# dst := src (sll) imm
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sll_10:
		li		$7, 0x00000002			# 1st source operand
		li		$23, 0x00000020		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		sll		$0, $7, 0x04	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sll		$6, $7, 0x04		# dst := src (sll) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alui_srl_11:
		li		$9, 0x40000000			# 1st source operand
		li		$23, 0x04000000		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		srl		$0, $9, 0x04	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		srl		$8, $9, 0x04		# dst := src (srl) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_srl_12:
		li		$11, 0x00000002			# 1st source operand
		li		$23, 0x00000001		# expected result
		move	$24, $11		# back up src
		li		$10, 0			# dst := 0
		srl		$0, $11, 0x01	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		srl		$10, $11, 0x01		# dst := src (srl) imm
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_srl_13:
		li		$13, 0x00000001			# 1st source operand
		li		$23, 0x00000000		# expected result
		move	$24, $13		# back up src
		li		$12, 0			# dst := 0
		srl		$0, $13, 0x01	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		srl		$12, $13, 0x01		# dst := src (srl) imm
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alui_sra_14:
		li		$15, 0x40000000			# 1st source operand
		li		$23, 0x04000000		# expected result
		move	$24, $15		# back up src
		li		$14, 0			# dst := 0
		sra		$0, $15, 0x04	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		sra		$14, $15, 0x04		# dst := src (sra) imm
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sra_15:
		li		$17, 0x80000000			# 1st source operand
		li		$23, 0xC0000000		# expected result
		move	$24, $17		# back up src
		li		$16, 0			# dst := 0
		sra		$0, $17, 0x01	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		sra		$16, $17, 0x01		# dst := src (sra) imm
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sra_16:
		li		$19, 0xF0000000			# 1st source operand
		li		$23, 0xF8000000		# expected result
		move	$24, $19		# back up src
		li		$18, 0			# dst := 0
		sra		$0, $19, 0x01	# try to change r0
		bne		$0, $18, B1	# taken if r0 or $18 modified
		sra		$18, $19, 0x01		# dst := src (sra) imm
		bne		$18, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$19, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		# Shifts with variable count (in register).

		
t_alu3_sllv_17:
		li		$21, 0x40000000			# 1st source operand
		li		$22, 0x01			# 2nd source operand
		li		$23, 0x80000000		# expected result
		move	$24, $21		# back up src1
		move	$25, $22		# back up src2
		li		$20, 0			# dst := 0
		sllv		$0, $21, $22	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		sllv		$20, $21, $22		# dst := src1 (sllv) src2
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# src1 modified
		nop						# branch slot
		bne		$22, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sllv_18:
		li		$29, 0x80000000			# 1st source operand
		li		$30, 0x01			# 2nd source operand
		li		$23, 0x00000000		# expected result
		move	$24, $29		# back up src1
		move	$25, $30		# back up src2
		li		$28, 0			# dst := 0
		sllv		$0, $29, $30	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		sllv		$28, $29, $30		# dst := src1 (sllv) src2
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# src1 modified
		nop						# branch slot
		bne		$30, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sllv_19:
		li		$2, 0x00000002			# 1st source operand
		li		$3, 0x04			# 2nd source operand
		li		$23, 0x00000020		# expected result
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		li		$31, 0			# dst := 0
		sllv		$0, $2, $3	# try to change r0
		bne		$0, $31, B1	# taken if r0 or $31 modified
		sllv		$31, $2, $3		# dst := src1 (sllv) src2
		bne		$31, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$2, $24, B2	# src1 modified
		nop						# branch slot
		bne		$3, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sllv_20:
		li		$5, 0x00000002			# 1st source operand
		li		$6, 0xC4			# 2nd source operand
		li		$23, 0x00000020		# expected result
		move	$24, $5		# back up src1
		move	$25, $6		# back up src2
		li		$4, 0			# dst := 0
		sllv		$0, $5, $6	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		sllv		$4, $5, $6		# dst := src1 (sllv) src2
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# src1 modified
		nop						# branch slot
		bne		$6, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_srlv_21:
		li		$8, 0x40000000			# 1st source operand
		li		$9, 0xC4			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $8		# back up src1
		move	$25, $9		# back up src2
		li		$7, 0			# dst := 0
		srlv		$0, $8, $9	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		srlv		$7, $8, $9		# dst := src1 (srlv) src2
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# src1 modified
		nop						# branch slot
		bne		$9, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srlv_22:
		li		$11, 0x40000000			# 1st source operand
		li		$12, 0x04			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $11		# back up src1
		move	$25, $12		# back up src2
		li		$10, 0			# dst := 0
		srlv		$0, $11, $12	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		srlv		$10, $11, $12		# dst := src1 (srlv) src2
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# src1 modified
		nop						# branch slot
		bne		$12, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srlv_23:
		li		$14, 0x00000002			# 1st source operand
		li		$15, 0x01			# 2nd source operand
		li		$23, 0x00000001		# expected result
		move	$24, $14		# back up src1
		move	$25, $15		# back up src2
		li		$13, 0			# dst := 0
		srlv		$0, $14, $15	# try to change r0
		bne		$0, $13, B1	# taken if r0 or $13 modified
		srlv		$13, $14, $15		# dst := src1 (srlv) src2
		bne		$13, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$14, $24, B2	# src1 modified
		nop						# branch slot
		bne		$15, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srlv_24:
		li		$17, 0x00000001			# 1st source operand
		li		$18, 0x01			# 2nd source operand
		li		$23, 0x00000000		# expected result
		move	$24, $17		# back up src1
		move	$25, $18		# back up src2
		li		$16, 0			# dst := 0
		srlv		$0, $17, $18	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		srlv		$16, $17, $18		# dst := src1 (srlv) src2
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# src1 modified
		nop						# branch slot
		bne		$18, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_srav_25:
		li		$20, 0x40000000			# 1st source operand
		li		$21, 0xC4			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $20		# back up src1
		move	$25, $21		# back up src2
		li		$19, 0			# dst := 0
		srav		$0, $20, $21	# try to change r0
		bne		$0, $19, B1	# taken if r0 or $19 modified
		srav		$19, $20, $21		# dst := src1 (srav) src2
		bne		$19, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$20, $24, B2	# src1 modified
		nop						# branch slot
		bne		$21, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srav_26:
		li		$28, 0x40000000			# 1st source operand
		li		$29, 0x04			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $28		# back up src1
		move	$25, $29		# back up src2
		li		$22, 0			# dst := 0
		srav		$0, $28, $29	# try to change r0
		bne		$0, $22, B1	# taken if r0 or $22 modified
		srav		$22, $28, $29		# dst := src1 (srav) src2
		bne		$22, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$28, $24, B2	# src1 modified
		nop						# branch slot
		bne		$29, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srav_27:
		li		$31, 0x80000000			# 1st source operand
		li		$2, 0x01			# 2nd source operand
		li		$23, 0xC0000000		# expected result
		move	$24, $31		# back up src1
		move	$25, $2		# back up src2
		li		$30, 0			# dst := 0
		srav		$0, $31, $2	# try to change r0
		bne		$0, $30, B1	# taken if r0 or $30 modified
		srav		$30, $31, $2		# dst := src1 (srav) src2
		bne		$30, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$31, $24, B2	# src1 modified
		nop						# branch slot
		bne		$2, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srav_28:
		li		$4, 0xF0000000			# 1st source operand
		li		$5, 0x01			# 2nd source operand
		li		$23, 0xF8000000		# expected result
		move	$24, $4		# back up src1
		move	$25, $5		# back up src2
		li		$3, 0			# dst := 0
		srav		$0, $4, $5	# try to change r0
		bne		$0, $3, B1	# taken if r0 or $3 modified
		srav		$3, $4, $5		# dst := src1 (srav) src2
		bne		$3, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$4, $24, B2	# src1 modified
		nop						# branch slot
		bne		$5, $25, B2	# operand2 modified
		nop						# branch slot


		# Comparison predicates with sign-extended immediate operand.
		
		
t_alui_slti_29:
		li		$7, 0x12345678			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		slti		$0, $7, 0x1234	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		slti		$6, $7, 0x1234		# dst := src (slti) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, >
		
t_alui_slti_30:
		li		$9, 0x00001234			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		slti		$0, $9, 0x1234	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		slti		$8, $9, 0x1234		# dst := src (slti) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, =
		
t_alui_slti_31:
		li		$11, 0x00001234			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $11		# back up src
		li		$10, 0			# dst := 0
		slti		$0, $11, 0x5678	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		slti		$10, $11, 0x5678		# dst := src (slti) imm
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, <
		
t_alui_slti_32:
		li		$13, 0x12345678			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $13		# back up src
		li		$12, 0			# dst := 0
		slti		$0, $13, 0x8000	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		slti		$12, $13, 0x8000		# dst := src (slti) imm
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/-
		
t_alui_slti_33:
		li		$15, 0xFFFFFFFF			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $15		# back up src
		li		$14, 0			# dst := 0
		slti		$0, $15, 0x0001	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		slti		$14, $15, 0x0001		# dst := src (slti) imm
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/+
		
t_alui_slti_34:
		li		$17, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $17		# back up src
		li		$16, 0			# dst := 0
		slti		$0, $17, 0x8000	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		slti		$16, $17, 0x8000		# dst := src (slti) imm
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, >
		
t_alui_slti_35:
		li		$19, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $19		# back up src
		li		$18, 0			# dst := 0
		slti		$0, $19, 0xFFFF	# try to change r0
		bne		$0, $18, B1	# taken if r0 or $18 modified
		slti		$18, $19, 0xFFFF		# dst := src (slti) imm
		bne		$18, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$19, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, =
		
t_alui_slti_36:
		li		$21, 0x80000000			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $21		# back up src
		li		$20, 0			# dst := 0
		slti		$0, $21, 0xFFFF	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		slti		$20, $21, 0xFFFF		# dst := src (slti) imm
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, <

		
t_alui_sltiu_37:
		li		$28, 0x12345678			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $28		# back up src
		li		$22, 0			# dst := 0
		sltiu		$0, $28, 0x1234	# try to change r0
		bne		$0, $22, B1	# taken if r0 or $22 modified
		sltiu		$22, $28, 0x1234		# dst := src (sltiu) imm
		bne		$22, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$28, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, >
		
t_alui_sltiu_38:
		li		$30, 0x00001234			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $30		# back up src
		li		$29, 0			# dst := 0
		sltiu		$0, $30, 0x1234	# try to change r0
		bne		$0, $29, B1	# taken if r0 or $29 modified
		sltiu		$29, $30, 0x1234		# dst := src (sltiu) imm
		bne		$29, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$30, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, =
		
t_alui_sltiu_39:
		li		$2, 0x00001234			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $2		# back up src
		li		$31, 0			# dst := 0
		sltiu		$0, $2, 0x5678	# try to change r0
		bne		$0, $31, B1	# taken if r0 or $31 modified
		sltiu		$31, $2, 0x5678		# dst := src (sltiu) imm
		bne		$31, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$2, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, <
		
t_alui_sltiu_40:
		li		$4, 0x12345678			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $4		# back up src
		li		$3, 0			# dst := 0
		sltiu		$0, $4, 0x8000	# try to change r0
		bne		$0, $3, B1	# taken if r0 or $3 modified
		sltiu		$3, $4, 0x8000		# dst := src (sltiu) imm
		bne		$3, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$4, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/-
		
t_alui_sltiu_41:
		li		$6, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $6		# back up src
		li		$5, 0			# dst := 0
		sltiu		$0, $6, 0x0001	# try to change r0
		bne		$0, $5, B1	# taken if r0 or $5 modified
		sltiu		$5, $6, 0x0001		# dst := src (sltiu) imm
		bne		$5, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$6, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/+
		
t_alui_sltiu_42:
		li		$8, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $8		# back up src
		li		$7, 0			# dst := 0
		sltiu		$0, $8, 0x8000	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		sltiu		$7, $8, 0x8000		# dst := src (sltiu) imm
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, >
		
t_alui_sltiu_43:
		li		$10, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $10		# back up src
		li		$9, 0			# dst := 0
		sltiu		$0, $10, 0xFFFF	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		sltiu		$9, $10, 0xFFFF		# dst := src (sltiu) imm
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, =
		
t_alui_sltiu_44:
		li		$12, 0x80000000			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $12		# back up src
		li		$11, 0			# dst := 0
		sltiu		$0, $12, 0xFFFF	# try to change r0
		bne		$0, $11, B1	# taken if r0 or $11 modified
		sltiu		$11, $12, 0xFFFF		# dst := src (sltiu) imm
		bne		$11, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$12, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, <

		# 3-registed comparison predicates.

		
t_alu3_slt_45:
		li		$5, 0x12345678			# 1st source operand
		li		$6, 0x02345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $5		# back up src1
		move	$25, $6		# back up src2
		li		$4, 0			# dst := 0
		slt		$0, $5, $6	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		slt		$4, $5, $6		# dst := src1 (slt) src2
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# src1 modified
		nop						# branch slot
		bne		$6, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, >
		
t_alu3_slt_46:
		li		$8, 0x12345678			# 1st source operand
		li		$9, 0x12345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $8		# back up src1
		move	$25, $9		# back up src2
		li		$7, 0			# dst := 0
		slt		$0, $8, $9	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		slt		$7, $8, $9		# dst := src1 (slt) src2
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# src1 modified
		nop						# branch slot
		bne		$9, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, =
		
t_alu3_slt_47:
		li		$10, 0x12345678			# 1st source operand
		li		$11, 0x23456789			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $10		# back up src1
		move	$25, $11		# back up src2
		li		$9, 0			# dst := 0
		slt		$0, $10, $11	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		slt		$9, $10, $11		# dst := src1 (slt) src2
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# src1 modified
		nop						# branch slot
		bne		$11, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, <
		
t_alu3_slt_48:
		li		$13, 0x12345678			# 1st source operand
		li		$14, 0x80000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $13		# back up src1
		move	$25, $14		# back up src2
		li		$12, 0			# dst := 0
		slt		$0, $13, $14	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		slt		$12, $13, $14		# dst := src1 (slt) src2
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# src1 modified
		nop						# branch slot
		bne		$14, $25, B2	# operand2 modified
		nop						# branch slot
	# +/-
		
t_alu3_slt_49:
		li		$16, 0x80000000			# 1st source operand
		li		$17, 0x12345678			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $16		# back up src1
		move	$25, $17		# back up src2
		li		$15, 0			# dst := 0
		slt		$0, $16, $17	# try to change r0
		bne		$0, $15, B1	# taken if r0 or $15 modified
		slt		$15, $16, $17		# dst := src1 (slt) src2
		bne		$15, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$16, $24, B2	# src1 modified
		nop						# branch slot
		bne		$17, $25, B2	# operand2 modified
		nop						# branch slot
	# -/+
		
t_alu3_slt_50:
		li		$18, 0xFFFFFFFF			# 1st source operand
		li		$19, 0x80000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $18		# back up src1
		move	$25, $19		# back up src2
		li		$17, 0			# dst := 0
		slt		$0, $18, $19	# try to change r0
		bne		$0, $17, B1	# taken if r0 or $17 modified
		slt		$17, $18, $19		# dst := src1 (slt) src2
		bne		$17, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$18, $24, B2	# src1 modified
		nop						# branch slot
		bne		$19, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, >
		
t_alu3_slt_51:
		li		$21, 0x90000000			# 1st source operand
		li		$22, 0x90000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $21		# back up src1
		move	$25, $22		# back up src2
		li		$20, 0			# dst := 0
		slt		$0, $21, $22	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		slt		$20, $21, $22		# dst := src1 (slt) src2
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# src1 modified
		nop						# branch slot
		bne		$22, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, =
		
t_alu3_slt_52:
		li		$29, 0x90000000			# 1st source operand
		li		$30, 0xF0000000			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $29		# back up src1
		move	$25, $30		# back up src2
		li		$28, 0			# dst := 0
		slt		$0, $29, $30	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		slt		$28, $29, $30		# dst := src1 (slt) src2
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# src1 modified
		nop						# branch slot
		bne		$30, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, <

		
t_alu3_sltu_53:
		li		$5, 0x12345678			# 1st source operand
		li		$6, 0x02345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $5		# back up src1
		move	$25, $6		# back up src2
		li		$4, 0			# dst := 0
		sltu		$0, $5, $6	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		sltu		$4, $5, $6		# dst := src1 (sltu) src2
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# src1 modified
		nop						# branch slot
		bne		$6, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, >
		
t_alu3_sltu_54:
		li		$8, 0x12345678			# 1st source operand
		li		$9, 0x12345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $8		# back up src1
		move	$25, $9		# back up src2
		li		$7, 0			# dst := 0
		sltu		$0, $8, $9	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		sltu		$7, $8, $9		# dst := src1 (sltu) src2
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# src1 modified
		nop						# branch slot
		bne		$9, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, =
		
t_alu3_sltu_55:
		li		$10, 0x12345678			# 1st source operand
		li		$11, 0x23456789			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $10		# back up src1
		move	$25, $11		# back up src2
		li		$9, 0			# dst := 0
		sltu		$0, $10, $11	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		sltu		$9, $10, $11		# dst := src1 (sltu) src2
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# src1 modified
		nop						# branch slot
		bne		$11, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, <
		
t_alu3_sltu_56:
		li		$13, 0x12345678			# 1st source operand
		li		$14, 0x80000000			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $13		# back up src1
		move	$25, $14		# back up src2
		li		$12, 0			# dst := 0
		sltu		$0, $13, $14	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		sltu		$12, $13, $14		# dst := src1 (sltu) src2
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# src1 modified
		nop						# branch slot
		bne		$14, $25, B2	# operand2 modified
		nop						# branch slot
	# +/-
		
t_alu3_sltu_57:
		li		$16, 0x80000000			# 1st source operand
		li		$17, 0x12345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $16		# back up src1
		move	$25, $17		# back up src2
		li		$15, 0			# dst := 0
		sltu		$0, $16, $17	# try to change r0
		bne		$0, $15, B1	# taken if r0 or $15 modified
		sltu		$15, $16, $17		# dst := src1 (sltu) src2
		bne		$15, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$16, $24, B2	# src1 modified
		nop						# branch slot
		bne		$17, $25, B2	# operand2 modified
		nop						# branch slot
	# -/+
		
t_alu3_sltu_58:
		li		$18, 0xFFFFFFFF			# 1st source operand
		li		$19, 0x80000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $18		# back up src1
		move	$25, $19		# back up src2
		li		$17, 0			# dst := 0
		sltu		$0, $18, $19	# try to change r0
		bne		$0, $17, B1	# taken if r0 or $17 modified
		sltu		$17, $18, $19		# dst := src1 (sltu) src2
		bne		$17, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$18, $24, B2	# src1 modified
		nop						# branch slot
		bne		$19, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, >
		
t_alu3_sltu_59:
		li		$21, 0x90000000			# 1st source operand
		li		$22, 0x90000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $21		# back up src1
		move	$25, $22		# back up src2
		li		$20, 0			# dst := 0
		sltu		$0, $21, $22	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		sltu		$20, $21, $22		# dst := src1 (sltu) src2
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# src1 modified
		nop						# branch slot
		bne		$22, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, =
		
t_alu3_sltu_60:
		li		$29, 0x90000000			# 1st source operand
		li		$30, 0xF0000000			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $29		# back up src1
		move	$25, $30		# back up src2
		li		$28, 0			# dst := 0
		sltu		$0, $29, $30	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		sltu		$28, $29, $30		# dst := src1 (sltu) src2
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# src1 modified
		nop						# branch slot
		bne		$30, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, <
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
