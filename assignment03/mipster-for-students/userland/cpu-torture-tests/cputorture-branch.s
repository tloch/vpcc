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
# Included as part of cputorture.sm4.  Tests branching + delay slots.
#

		move	$2, $0
		j		t_j
		addi	$2, $0, 7		# add 7 in branch slot
		break	2					# should never be reached

t_j:
.globl t_j
		addi	$2, $2, -7		# subtract 7 to get 0
		bne		$2, $0, B2		# branch if != 0
		nop							# branch slot

t_jal:
.globl t_jal
		li		$2, 11
		jal		t_link				# call subroutine
		addi	$2, $2, 1		# branch slot; add 1 to get R(2)==12
		addi	$2, $2, 1		# executed after return, set R(2) to 14
		li		$3, 14
		bne		$2, $3, B2
		nop							# branch slot

t_jalr:
.globl t_jalr
		li		$2, 11			# same as above, but with jalr
		la		$4, t_link		# to test indirect call
		jalr	$31, $4			# test explicit store of retaddr
		addi	$2, $2, 1		# branch slot; sets R(2)==12
		addi	$2, $2, 1		# executed after return, set R(2) to 14
		li		$3, 14
		bne		$2, $3, B2
		nop							# branch slot
		j		t_beq				# continue execution
		nop							# branch slot
		
t_link:
.globl t_link
		li		$3, 12			# Entered with R(2)==12
		bne		$2, $3, B2		# Branch if not equal
		jr		$31				# Return
		addi	$2, $2, 1		# Branch slot: set R(2) to 13
		break	2					# Should never be executed

		# Test forward jumps, backward jumps (bltzal/bgtzal to t_link)
		# and branch slots.

t_beq:
.globl t_beq
		li		$3, 1
		beq		$3, $0, B2		# Not equal, should NOT branch
		xor		$2, $2, $2	# Set to 0
		beq		$0, $2, t_bne	# Equal, SHOULD branch
		addi	$2, $2, 1
		break	2

t_bne:
.globl t_bne
		bne		$0, $0, B2		# equal, should NOT branch
		nop			  				# branch slot
		bne		$2, $3, B2		# must be 1
		addi	$3, $3, 1
		bne		$2, $0, t_blez	# not equal, SHOULD branch
		addi	$2, $2, 1
		break	2

t_blez:
.globl t_blez
		bne		$2, $3, B2		# did last branch slot got executed?
		li		$4, -1
		blez	$4, 1f			# R(4) < 0, should be taken
		addi	$4, $4, 2		# set R(4) to +1 in branch slot
		break	2
1:		blez	$4, B2			# R(4)==1, should not be taken
		addi	$4, $4, -1		# set R(4) to 0
		blez	$4, t_bltz		# should be taken
		addi	$4, $4, 1		# set R(4) to 1
		break	2

t_bltz:
.globl t_bltz
		bltz	$4, B2			# should not branch as R(4)==+1
		addi	$4, $4, -1		# set R(4)==0
		bltz	$4, B2			# should not branch (as 0 < 0 == false)
		addi	$4, $4, -1		# set R(4) to -1
		bltz	$4, t_bltzal		# must branch
		addi	$4, $4, 1		# set to 0 in branch slot
		break	2

t_bltzal:
.globl t_bltzal
		bne		$4, $0, B2
		addi	$4, $4, 1		# set R(4) to 1
		bltzal	$4, B2			# should not branch as R(4)==+1
		addi	$4, $4, -1		# set R(4) to 0
		li		$2, 11
		bltzal	$4, B2			# should not branch as R(4)==0
		addi	$4, $4, -1		# set R(4) to -1
		li		$2, 11
		bltzal	$4, t_link		# should branch as R(4)==-1
		addi	$2, $2, 1
		addi	$2, $2, 1
		addi	$2, $2, -14
		bne		$2, $0, B2		# R(2) must be 14
		nop

t_bgez:
.globl t_bgez
		li		$4, -1
		bgez	$4, B2			# must NOT branch
		addi	$4, $4, 1		# set R(4) to 0
		bgez	$4, 1f			# must branch as R(4)==0
		addi	$4, $4, 1		# set R(4) to 1 in branch slot
		break	2
1:		bgez	$4, t_bgtz		# must branch
		addi	$4, $4, 1
		break	2

t_bgtz:
.globl t_bgtz
		bgtz	$4, 1f			# R(4)==2, must branch
		addi	$4, $4, -2		# set R(4)=0
		break	2
1:		bgtz	$4, B2			# R(4)==0, must not branch
		addi	$4, $4, -1
		bgtz	$4, B2			# R(4)==-1, must not branch
		nop

t_bgezal:
.globl t_bgezal
		li		$2, 11
		bgezal	$4, B2			# should not branch as R(4)==-1
		addi	$4, $4, 1
		bgezal	$4, t_link		# should branch as R(4)==0
		addi	$2, $2, 1
		addi	$2, $2, 1
		addi	$2, $2, -14
		bne		$2, $0, B2		# R(2) must be 14
		addi	$4, $4, 1		# set R(4) to 1
		li		$2, 11
		bgezal	$4, t_link		# should branch as R(4)==1
		addi	$2, $2, 1
		addi	$2, $2, 1
		addi	$2, $2, -14
		bne		$2, $0, B2		# R(2) must be 14
		nop
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
