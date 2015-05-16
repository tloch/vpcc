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
# Included as part of cputorture.sm4.  Tests load/store instructions for
# little-endian mode.
#

t_lui:
.globl t_lui
		li		$2, 1			# Test that lui zeroes low-order bits
		lui		$2, 0
		bne		$2, $0, B2

		# Constants for further tests.

		lui		$23, 0
		la		$28, w1
		la		$29, w2
		la		$30, w3
		la		$31, w4

		# Just test that it's possible to modify $26, $27

		la		$26, w1
		bne		$26, $28, B2
    nop
		la		$27, w2
		bne		$27, $29, B2
    nop

#
# Test load instructions.
# Arguments: (1:insn, 2:r1, 3:r2, 4:address, 5:expected)
# Temporary registers: $24, $25
#


#
# Test that unaligned loads raise exception.
# Arguments: (1:insn, 2:reg, 3:address, 4:label)
#


#
# Test that unaligned stores raise exception.
# Arguments: (1:insn, 2:address, 3:label)
#


		
t_ld_lb_1:
		lb		$2, 0($28)				# load from memory
		li		$3, 0x00000078				# load expected as immediate
		bne		$2, $3, B2			# branch if not equal
		lb		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_2:
		lb		$4, 1($28)				# load from memory
		li		$5, 0x00000056				# load expected as immediate
		bne		$4, $5, B2			# branch if not equal
		lb		$0, 1($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_3:
		lb		$6, 2($28)				# load from memory
		li		$7, 0x00000034				# load expected as immediate
		bne		$6, $7, B2			# branch if not equal
		lb		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_4:
		lb		$8, 3($28)				# load from memory
		li		$9, 0x00000012				# load expected as immediate
		bne		$8, $9, B2			# branch if not equal
		lb		$0, 3($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_5:
		lb		$10, 0($29)				# load from memory
		li		$11, 0xFFFFFFF8				# load expected as immediate
		bne		$10, $11, B2			# branch if not equal
		lb		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_6:
		lb		$12, 1($29)				# load from memory
		li		$13, 0xFFFFFFE7				# load expected as immediate
		bne		$12, $13, B2			# branch if not equal
		lb		$0, 1($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_7:
		lb		$14, 2($29)				# load from memory
		li		$15, 0xFFFFFFD6				# load expected as immediate
		bne		$14, $15, B2			# branch if not equal
		lb		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_8:
		lb		$16, 3($29)				# load from memory
		li		$17, 0xFFFFFFC5				# load expected as immediate
		bne		$16, $17, B2			# branch if not equal
		lb		$0, 3($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lbu_9:
		lbu		$2, 0($28)				# load from memory
		li		$3, 0x00000078				# load expected as immediate
		bne		$2, $3, B2			# branch if not equal
		lbu		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_10:
		lbu		$4, 1($28)				# load from memory
		li		$5, 0x00000056				# load expected as immediate
		bne		$4, $5, B2			# branch if not equal
		lbu		$0, 1($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_11:
		lbu		$6, 2($28)				# load from memory
		li		$7, 0x00000034				# load expected as immediate
		bne		$6, $7, B2			# branch if not equal
		lbu		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_12:
		lbu		$8, 3($28)				# load from memory
		li		$9, 0x00000012				# load expected as immediate
		bne		$8, $9, B2			# branch if not equal
		lbu		$0, 3($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_13:
		lbu		$10, 0($29)				# load from memory
		li		$11, 0x000000F8				# load expected as immediate
		bne		$10, $11, B2			# branch if not equal
		lbu		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_14:
		lbu		$12, 1($29)				# load from memory
		li		$13, 0x000000E7				# load expected as immediate
		bne		$12, $13, B2			# branch if not equal
		lbu		$0, 1($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_15:
		lbu		$14, 2($29)				# load from memory
		li		$15, 0x000000D6				# load expected as immediate
		bne		$14, $15, B2			# branch if not equal
		lbu		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_16:
		lbu		$16, 3($29)				# load from memory
		li		$17, 0x000000C5				# load expected as immediate
		bne		$16, $17, B2			# branch if not equal
		lbu		$0, 3($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lh_17:
		lh		$18, 0($28)				# load from memory
		li		$19, 0x00005678				# load expected as immediate
		bne		$18, $19, B2			# branch if not equal
		lh		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lh_18:
		lh		$20, 2($28)				# load from memory
		li		$21, 0x00001234				# load expected as immediate
		bne		$20, $21, B2			# branch if not equal
		lh		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lh_19:
		lh		$22, 0($29)				# load from memory
		li		$2, 0xFFFFE7F8				# load expected as immediate
		bne		$22, $2, B2			# branch if not equal
		lh		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lh_20:
		lh		$3, 2($29)				# load from memory
		li		$4, 0xFFFFC5D6				# load expected as immediate
		bne		$3, $4, B2			# branch if not equal
		lh		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lhu_21:
		lhu		$5, 0($28)				# load from memory
		li		$6, 0x00005678				# load expected as immediate
		bne		$5, $6, B2			# branch if not equal
		lhu		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lhu_22:
		lhu		$7, 2($28)				# load from memory
		li		$8, 0x00001234				# load expected as immediate
		bne		$7, $8, B2			# branch if not equal
		lhu		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lhu_23:
		lhu		$9, 0($29)				# load from memory
		li		$10, 0x0000E7F8				# load expected as immediate
		bne		$9, $10, B2			# branch if not equal
		lhu		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lhu_24:
		lhu		$11, 2($29)				# load from memory
		li		$12, 0x0000C5D6				# load expected as immediate
		bne		$11, $12, B2			# branch if not equal
		lhu		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lw_25:
		lw		$13, 0($28)				# load from memory
		li		$14, 0x12345678				# load expected as immediate
		bne		$13, $14, B2			# branch if not equal
		lw		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lw_26:
		lw		$15, 0($29)				# load from memory
		li		$16, 0xC5D6E7F8				# load expected as immediate
		bne		$15, $16, B2			# branch if not equal
		lw		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		# Exercise store instructions to recheck with loads
	
t_store:
.globl t_store
		li		$2, 0x12345678
		sb		$2, 1($30)
		sh		$2, 2($30)
		sw		$2, 0($31)
		
t_ld_lw_27:
		lw		$17, 0($30)				# load from memory
		li		$18, 0x56787800				# load expected as immediate
		bne		$17, $18, B2			# branch if not equal
		lw		$0, 0($30)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lw_28:
		lw		$19, 0($31)				# load from memory
		li		$20, 0x12345678				# load expected as immediate
		bne		$19, $20, B2			# branch if not equal
		lw		$0, 0($31)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		# Check for exceptions on unaligned loads/stores as well as on
		# accesses to addresses below MIPS_LOWBASE (0x1000).  Also check that
		# failed loads and stores do not change the target.

t_ldst_exn:
.globl t_ldst_exn
		li		$2, 0x12345678
		move	$3, $2
		move	$4, $0
		
t_unl_lh_29:
		.globl	EXN10
EXN10:		lh		$2, 1($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lhu_30:
		.globl	EXN11
EXN11:		lhu		$2, 1($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_31:
		.globl	EXN12
EXN12:		lw		$2, 1($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_32:
		.globl	EXN13
EXN13:		lw		$2, 2($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_33:
		.globl	EXN14
EXN14:		lw		$2, 3($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		#t_unl(lw,  $2, 0($4),  EXN15)		# null ptr check
		#t_unl(lw,  $2, 0xFFC($4), EXN16)	# null ptr check
		bne		$2, $3, B2

		
t_uns_sh_34:
		.globl	EXN17
EXN17:		sh		$0, 1($30)			# label to mark exception
		break	3					# if not skipped by exn handler

		
t_uns_sw_35:
		.globl	EXN18
EXN18:		sw		$0, 1($30)			# label to mark exception
		break	3					# if not skipped by exn handler

		
t_uns_sw_36:
		.globl	EXN19
EXN19:		sw		$0, 2($30)			# label to mark exception
		break	3					# if not skipped by exn handler

		
t_uns_sw_37:
		.globl	EXN20
EXN20:		sw		$0, 3($30)			# label to mark exception
		break	3					# if not skipped by exn handler


		
t_ld_lw_38:
		lw		$2, 0($30)				# load from memory
		li		$3, 0x56787800				# load expected as immediate
		bne		$2, $3, B2			# branch if not equal
		lw		$0, 0($30)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		#t_uns(sw, 0($4), EXN21)				# null ptr check
		#t_uns(sw, 0xFFC($4), EXN22)			# null ptr check

		# Test lwl/lwr/swl/swr.  TODO: should test all mod 4 combinations!

#t_unaligned:
#.globl t_unaligned
#		lw		$2, 0($28)
#		lwl		$2, 2($29)
#		li		$3, 0xD6E7F878
#		bne		$2, $3, B2
#		lwr		$2, 3($28)
#		li		$3, 0xD6E7F812
#		bne		$2, $3, B2
#		nop
#
#		li		$2, 0x12345678
#		sw		$0, 0($30)
#		sw		$0, 0($31)
#		swl		$2, 1($31)
#		li		$3, 0x00001234
#		lw		$4, 0($31)
#		bne		$3, $4, B2
#		swr		$2, 2($30)
#		li		$3, 0x56780000
#		lw		$4, 0($30)
#		bne		$3, $4, B2
#		nop

		.data	# Data used by load/store tests
		.align	4
w1:		.byte	0x78, 0x56, 0x34, 0x12
w2:		.byte	0xF8, 0xE7, 0xD6, 0xC5
w3:		.byte	0x00, 0x00, 0x00, 0x00
w4:		.byte	0x00, 0x00, 0x00, 0x00
		.text	# Switch back to .text segment

		.globl	SUCCESS
SUCCESS:
		break 0				# mark successful completion

		# Failure labels
		.globl	FAILURE
FAILURE:
B1:		break	1
B2:		break	2
B3:		break	3
