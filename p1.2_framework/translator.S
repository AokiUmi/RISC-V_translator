#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes".
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

# Your code to convert the RVC codes to RVI codes.
main:


    la      t0, lines_of_rvc_codes
    lw      t0, 0(t0)       # Number of instructions
    la      t1, rvc_codes

    # Allocate an array to store the address map
    addi    a1, t0, 1
    slli    a1, a1, 4   # Maximum size (int bytes) of the original instruction list
    li      a0, 9
    ecall
    mv      s11, a0

    # In the first pass, we create an address map, which maps addresses in the
    # original instructions to addresses in decompressed instructions.
    li      t5, 0       # address in the origin instructions
    li      t6, 0       # address in the decompressed instructions

main__first_pass_loop:
    slli    t2, t5, 2
    add     t2, s11, t2 # Address in the address map
    sw      t6, 0(t2)   # Add an entry to the address map

    blez    t0, main__first_pass_loop_end
    # Check the lowest 2 bits to determine whether the instruction is compressed
    # or not.
    lbu     t2, 0(t1)
    andi    t2, t2, 3
    li      t3, 3
    bne     t2, t3, main__first_pass_is_compressed

    addi    t1, t1, 4
    addi    t5, t5, 4
    j       main__first_pass_is_compressed_end
main__first_pass_is_compressed:
    addi    t1, t1, 2
    addi    t5, t5, 2
main__first_pass_is_compressed_end:

    addi    t6, t6, 4
    addi    t0, t0, -1
    j       main__first_pass_loop
main__first_pass_loop_end:
    slli    t2, t5, 2
    add     t2, s11, t2 # Address in the address map
    sw      t6, 0(t2)   # Add an entry to the address map

    # In the second pass, we decompress individual instruction
    la      s1, lines_of_rvc_codes
    lw      s1, 0(s1)   # Number of instruction
    la      s0, rvc_codes   # Starting address of instructions
    li      s2, 0       # Address in the original instructions
    li      s3, 0       # Address in the decompressed instructions
main__loop:
    blez    s1, main__loop_end

    lbu     t0, 0(s0)
    andi    t0, t0, 3
    li      t1, 3
    bne     t0, t1, main__is_compressed
    lw      a0, 0(s0)
    mv      a1, s2
    mv      a2, s3
    mv      a3, s11
    jal     update_offset
    addi    s0, s0, 4
    addi    s2, s2, 4
    j       main__is_compressed_end
main__is_compressed:
    lhu     a0, 0(s0)
    mv      a1, s2
    mv      a2, s3
    mv      a3, s11
    jal     decompress_inst
    addi    s0, s0, 2
    addi    s2, s2, 2
main__is_compressed_end:
    li      a1, 32
    jal     print_bin

    # Print a new line
    li      a1, 10
    li      a0, 11
    ecall

    addi    s3, s3, 4
    addi    s1, s1, -1
    j       main__loop
main__loop_end:

    # Exit with error code 0
    li      a1, 0
    li      a0, 17
    ecall

decompress_inst:
    # Decompress on 16 bit compressed instruction
    # parameters:
    #   a0 - the 16 bit compressed instruction
    #   a1 - address of current instruction in the RVC format instructions
    #   a2 - address of current instruction in the 32 bit format instructions
    #   a3 - the array of offset
    # return;
    #   a0 - the 32 bit decompressed instruction

    # Check if it is CR format instruction

    # A CR format instruction must has opcode 0b10 and FUNCT4 >= 0b1000
    li      t0, 2
    and     t1, a0, t0      # opcode
    bne     t1, t0, decompress_inst__is_not_cr_fmt
    srli    t0, a0, 15
    beqz    t0, decompress_inst__is_not_cr_fmt
    ############################################
    #            BEGIN CR format
    ############################################

    # Get rs2
    srli    t0, a0, 2
    andi    t0, t0, 0x1f    # src
    beqz    t0, decompress_inst__is_c_jr_or_jalr
    # ----------- C.ADD or C.MV -----------

    slli    t0, t0, 20      # left shift rs2
    li      a7, 0x33        # opcode = 0110011
    or      a7, a7, t0      # sr2 = src

    # Get dest
    srli    t1, a0, 7
    andi    t1, t1, 0x1f    # dest

    slli    t0, t1, 7
    or      a7, a7, t0      # rd = dest

    # Check if it is C.MV. If the lowest bit of FUNCT3 is 0, then it is C.MV
    srli    t0, a0, 12
    andi    t0, t0, 1
    beqz    t0, decompress_inst__return     # if it is C.MV, then rs1 = 0

    slli    t0, t1, 15
    or      a7, a7, t0      # rs1 = rd

    j       decompress_inst__return
decompress_inst__is_c_jr_or_jalr:
    # ----------- C.JR or C.JALR -----------

    # Get src
    srli    t1, a0, 7
    andi    t1, t1, 0x1f    # src

    li      a7, 0x67        # opcode = 1100111
    slli    t1, t1, 15
    or      a7, a7, t1      # rs1 = src

    # Check if it is C.JR or C.JALR
    srli    t0, a0, 12
    andi    t0, t0, 1       # get rd

    slli    t0, t0, 7
    or      a7, a7, t0      # rd = x0 or x1

    j       decompress_inst__return
    ############################################
    #            END CR format
    ############################################
decompress_inst__is_not_cr_fmt:
    # A CR format instruction must has opcode of 0b10 or 0b01
    andi    t0, a0, 3
    li      t1, 1
    beq     t0, t1, decompress_inst__maybe_ci_fmt
    li      t1, 2
    beq     t0, t1, decompress_inst__maybe_ci_fmt
    j       decompress_inst__is_not_ci_fmt

decompress_inst__maybe_ci_fmt:
    # FUNCT3 must be 0b010, 0b011, or 0b000
    srli    t0, a0, 13      # FUNCT3
    beqz    t0, decompress_inst__is_ci_fmt
    li      t1, 2
    beq     t0, t1, decompress_inst__is_ci_fmt
    li      t1, 3
    beq     t0, t1, decompress_inst__is_ci_fmt
    bnez    t0, decompress_inst__is_not_ci_fmt
decompress_inst__is_ci_fmt:
    ############################################
    #            BEGIN CI format
    ############################################
    # Get dest
    srli    t5, a0, 7
    andi    t5, t5, 0x1f    # dest

    slli    a7, t5, 7       # rd = dest

    # Get the immediate
    srli    t6, a0, 2
    andi    t6, t6, 0x1f    # imm[4:0]
    # Get imm[5]
    srli    t2, a0, 12
    andi    t2, t2, 1

    slli    t2, t2, 5
    or      t6, t6, t2      # imm
    # Shift the most significant bit to the left.
    slli    t6, t6, 26

    li      t1, 2
    bne     t0, t1, decompress_inst__is_not_c_li
    # ----------- C.LI -----------
    li      t1, 0x13
    or      a7, a7, t1      # opcode = 0010011


    # Sign-extend imm to 12 bits
    srai    t1, t6, 6
    or      a7, a7, t1      # set imm in the result.
    j       decompress_inst__return
decompress_inst__is_not_c_li:
    li      t1, 3
    bne     t0, t1, decompress_inst__is_not_c_lui
    # ----------- C.LUI -----------
    li      t1, 0x37
    or      a7, a7, t1,     # opcode = 0110111

    # Sign-extend imm to 20 bits
    srai    t1, t6, 14
    or      a7, a7, t1      # set imm in the result.
    j       decompress_inst__return
decompress_inst__is_not_c_lui:
    slli    t0, t5, 15
    or      a7, a7, t0      # rs1 = dest

    andi    t0, a0, 3       # opcode
    li      t1, 1
    bne     t0, t1, decompress_inst__is_not_c_addi
    # ----------- C.ADDI -----------

    li      t0, 0x13
    or      a7, a7, t0      # opcode = 0010011

    # Sign-extend imm to 12 bits
    srai    t0, t6, 6
    or      a7, a7, t0      # set imm in the result

    j       decompress_inst__return
decompress_inst__is_not_c_addi:
    # ----------- C.SLLI -----------
    li      t0, 0x13
    or      a7, a7, t0      # opcode = 0010011

    li      t0, 1
    slli    t0, t0, 12
    or      a7, a7, t0      # funct3 = 001

    srli    t0, t6, 6
    or      a7, a7, t0      # shamt

    j       decompress_inst__return
    ############################################
    #            END CI format
    ############################################
decompress_inst__is_not_ci_fmt:
    andi    t0, a0, 3
    bnez    t0, decompress_inst__is_not_cl_cs_fmt   # opcode must be 0b00

    srli    t5, a0, 7
    andi    t5, t5, 7   # base
    addi    t5, t5, 8

    srli    t0, a0, 5
    andi    t6, t0, 1
    slli    t6, t6, 6   # offset[6]

    srli    t0, t0, 1
    andi    t0, t0, 1
    slli    t0, t0, 2
    or      t6, t6, t0  # offset[2]

    srli    t0, a0, 10
    andi    t0, t0, 7   # offset[5:3]
    slli    t0, t0, 3
    or      t6, t6, t0  # offset[6:2]

    li      a7, 2
    slli    a7, a7, 12  # funct3 = 010

    srli    t0, a0, 13  # funct3
    li      t1, 2
    bne     t0, t1, decompress_inst__is_not_c_lw

    ############################################
    #            BEGIN CL format
    ############################################
    # ----------- C.LW -----------
    ori     a7, a7, 3       # opcode = 0000011
    slli    t6, t6, 20
    or      a7, a7, t6  # set the offset

    slli    t5, t5, 15
    or      a7, a7, t5  # rs1 = base

    srli    t0, a0, 2
    andi    t0, t0, 7   # dest
    addi    t0, t0, 8
    slli    t0, t0, 7
    or      a7, a7, t0  # rd = dest

    j       decompress_inst__return
    ############################################
    #            END CL format
    ############################################
decompress_inst__is_not_c_lw:
    ############################################
    #            BEGIN CS format
    ############################################
    # ----------- C.SW -----------
    li      t0, 0x23
    or      a7, a7, t0  # opcode = 0100011

    srli    t0, t6, 5   # offset[11:5]
    slli    t0, t0, 25
    or      a7, a7, t0  # offset[11:5]

    andi    t0, t6, 0x1f
    slli    t0, t0, 7
    or      a7, a7, t0  # offset[4:0]

    slli    t5, t5, 15
    or      a7, a7, t5  # rs1 = base

    srli    t0, a0, 2
    andi    t0, t0, 7   # src
    addi    t0, t0, 8
    slli    t0, t0, 20
    or      a7, a7, t0  # rs2 = src

    j       decompress_inst__return
    ############################################
    #            END CS format
    ############################################
decompress_inst__is_not_cl_cs_fmt:
    andi    t0, a0, 3
    li      t1, 1
    bne     t0, t1, decompress_inst__is_not_ca_fmt

    # Check if funct6 == 100011
    srli    t0, a0, 10
    li      t1, 0x23
    bne     t0, t1, decompress_inst__is_not_ca_fmt
    ############################################
    #            BEGIN CA format
    ############################################
    li      a7, 0x33    # opcode = 0110011

    srli    t0, a0, 2
    andi    t0, t0, 7   # src
    addi    t0, t0, 8
    slli    t0, t0, 20
    or      a7, a7, t0  # rs2 = src

    srli    t0, a0, 7
    andi    t0, t0, 7   # dest
    addi    t0, t0, 8
    slli    t1, t0, 7
    or      a7, a7, t1  # rd = dest
    slli    t1, t0, 15
    or      a7, a7, t1  # rs1 = dest

    srli    t0, a0, 5
    andi    t0, t0, 3   # funct2

    li      t1, 3
    bne     t0, t1, decompress_inst__is_not_c_and
    # ----------- C.AND -----------
    li      t1, 7
    slli    t1, t1, 12
    or      a7, a7, t1  # funct3 = 111
    j       decompress_inst__return
decompress_inst__is_not_c_and:
    li      t1, 2
    bne     t0, t1, decompress_inst__is_not_c_or
    # ----------- C.OR -----------
    li      t1, 6
    slli    t1, t1, 12
    or      a7, a7, t1  # funct3 = 110
    j       decompress_inst__return
decompress_inst__is_not_c_or:
    li      t1, 1
    bne     t0, t1, decompress_inst__is_not_c_xor
    # ----------- C.XOR -----------
    li      t1, 4
    slli    t1, t1, 12
    or      a7, a7, t1  # funct3 = 100
    j       decompress_inst__return
decompress_inst__is_not_c_xor:
    # ----------- C.SUB -----------
    # funct3 = 000
    # funct7 = 0100000
    li      t0, 0x20
    slli    t0, t0, 25
    or      a7, a7, t0  # funct7 = 0100000
    j       decompress_inst__return
    ############################################
    #            END CA format
    ############################################
decompress_inst__is_not_ca_fmt:
    andi    t0, a0, 3
    li      t1, 1
    bne     t0, t1, decompress_inst__not_cb_fmt # opcode must be 0b1
    srli    t0, a0, 13  # FUNCT3
    li      t1, 6
    beq     t0, t1, decompress_inst__is_type1_fmt
    li      t1, 7
    beq     t0, t1, decompress_inst__is_type1_fmt
    li      t1, 4
    beq     t0, t1, decompress_inst__is_type2_fmt
    j       decompress_inst__not_cb_fmt
decompress_inst__is_type1_fmt:
    ############################################
    #            BEGIN CB-TYPE1 format
    ############################################
    li      a7, 0x63    # opcode = 1100011
    srli    t0, a0, 2
    andi    t0, t0, 0x1f    # offset[7:6|2:1|5]

    andi    t1, t0, 1
    slli    t6, t1, 5       # offset[5]

    andi    t1, t0, 6       # offset[2:1]
    or      t6, t6, t1

    srli    t1, t0, 3
    andi    t1, t1, 3
    slli    t1, t1, 6       # offset[7:6]
    or      t6, t6, t1

    srli    t0, a0, 10
    andi    t0, t0, 7       # offset[8|4:3]
    andi    t1, t0, 3
    slli    t1, t1, 3       # offset[4:3]
    or      t6, t6, t1

    srli    t1, t0, 2
    slli    t1, t1, 8       # offset[8]
    or      t6, t6, t1      # offset

    # Sign-extend the offset
    slli    t6, t6, 23
    srai    t6, t6, 23

    # Recalculate the offset
    add     t0, a1, t6      # Address of the target in the original instructions
    slli    t0, t0, 2
    add     t0, a3, t0      # Get address of the target in decompressed instructions
    lw      t0, 0(t0)
    sub     t6, t0, a2      # New offset in the 32-bit format instructions

    srli    t0, t6, 11
    andi    t0, t0, 1
    slli    t0, t0, 7
    or      a7, a7, t0      # imm[11]

    srli    t0, t6, 1
    andi    t0, t0, 0xf
    slli    t0, t0, 8
    or      a7, a7, t0      # imm[4:1]

    srli    t0, t6, 5
    andi    t0, t0, 0x3f
    slli    t0, t0, 25
    or      a7, a7, t0      # imm[10:5]

    srli    t0, t6, 12
    andi    t0, t0, 1
    slli    t0, t0, 31
    or      a7, a7, t0      # imm[12]

    # Find src
    srli    t0, a0, 7
    andi    t0, t0, 7       # src
    addi    t0, t0, 8
    slli    t0, t0, 15
    or      a7, a7, t0      # rs1 = src

    srli    t0, a0, 13
    andi    t0, t0, 1       # FUNCT3
    slli    t0, t0, 12
    or      a7, a7, t0

    j       decompress_inst__return
    ############################################
    #            END CB-TYPE1 format
    ############################################
decompress_inst__is_type2_fmt:
    ############################################
    #            BEGIN CB-TYPE2 format
    ############################################
    srli    t5, a0, 2
    andi    t5, t5, 0x1f    # imm/shamt[4:0]

    srli    t0, a0, 12
    andi    t0, t0, 1
    slli    t0, t0, 5       # imm/shamt[5]
    or      t5, t5, t0      # imm/shamt

    li      a7, 0x13        # opcode = 0010011
    srli    t0, a0, 7
    andi    t0, t0, 7       # dest
    addi    t0, t0, 8

    slli    t1, t0, 7
    or      a7, a7, t1      # rd = dest

    slli    t1, t0, 15
    or      a7, a7, t1      # rs1 = dest

    srli    t0, a0, 10      # FUNCT2
    andi    t0, t0, 3
    li      t1, 2
    bne     t0, t1, decompress_inst__is_not_c_andi
    # ----------- C.ANDI -----------
    # Sign-extend imm
    slli    t5, t5, 26
    srai    t5, t5, 6

    or      a7, a7, t5  # set imm

    li      t0, 7       # funct3 = 111
    slli    t0, t0, 12
    or      a7, a7, t0

    j       decompress_inst__return
decompress_inst__is_not_c_andi:
    # ----------- C.SRLI or S.SRAI -----------
    andi    t5, t5, 0x1f    # shamt[5] must be 0
    slli    t5, t5, 20
    or      a7, a7, t5  # Set shamt

    li      t0, 5       # FUNCT3 = 101
    slli    t0, t0, 12
    or      a7, a7, t0

    srli    t0, a0, 10
    andi    t0, t0, 3   # get FUNCT2

    slli    t0, t0, 30  # 5 + 25
    or      a7, a7, t0  # Set FUNCT7

    j       decompress_inst__return
    ############################################
    #            END CB-TYPE2 format
    ############################################
decompress_inst__not_cb_fmt:
    ############################################
    #            BEGIN CJ format
    ############################################
    srli    t0, a0, 2
    andi    t0, t0, 0x7ff

    andi    t6, t0, 1   # offset[5]
    slli    t6, t6, 5

    srli    t1, t0, 1
    andi    t1, t1, 7   # offset[3:1]
    slli    t1, t1, 1
    or      t6, t6, t1

    srli    t1, t0, 4
    andi    t1, t1, 1   # offset[7]
    slli    t1, t1, 7
    or      t6, t6, t1

    srli    t1, t0, 5
    andi    t1, t1, 1   # offset[6]
    slli    t1, t1, 6
    or      t6, t6, t1

    srli    t1, t0, 6
    andi    t1, t1, 1   # offset[10]
    slli    t1, t1, 10
    or      t6, t6, t1

    srli    t1, t0, 7
    andi    t1, t1, 3   # offset[9:8]
    slli    t1, t1, 8
    or      t6, t6, t1

    srli    t1, t0, 9
    andi    t1, t1, 1   # offset[4]
    slli    t1, t1, 4
    or      t6, t6, t1

    srli    t1, t0, 10
    andi    t1, t1, 1   # offset[11]
    slli    t1, t1, 11
    or      t6, t6, t1

    # Sign-extend the offset
    slli    t6, t6, 20
    srai    t6, t6, 20

    # Recalculate the offset
    add     t0, a1, t6      # Address of the target in the original instructions
    slli    t0, t0, 2
    add     t0, a3, t0      # Get address of the target in decompressed instructions
    lw      t0, 0(t0)
    sub     t6, t0, a2      # New offset in the 32-bit format instructions

    li      a7, 0x6f    # opcode = 1101111

    # Set imm in the result
    srli    t0, t6, 1
    andi    t0, t0, 0x3ff
    slli    t0, t0, 21
    or      a7, a7, t0

    srli    t0, t6, 11
    andi    t0, t0, 1       # imm[11]
    slli    t0, t0, 20
    or      a7, a7, t0

    srli    t0, t6, 12
    andi    t0, t0, 0xff    # imm[19:12]
    slli    t0, t0, 12
    or      a7, a7, t0

    srli    t0, t6, 20
    andi    t0, t0, 1       # imm[20]
    slli    t0, t0, 31
    or      a7, a7, t0

    # Get dest
    srli    t0, a0, 15
    xori    t0, t0, 1
    slli    t0, t0, 7       # dest
    or      a7, a7, t0      # rd = dest
    ############################################
    #            END CJ format
    ############################################
decompress_inst__return:
    mv      a0, a7
    jr      ra

update_offset:
    # Update the offset in a 32-bit instruction
    # parameters:
    #   a0 - the 16 bit compressed instruction
    #   a1 - address of current instruction in the RVC format instructions
    #   a2 - address of current instruction in the 32 bit format instructions
    #   a3 - the array of offset

    andi    t0, a0, 0x7f    # The opcode
    li      t1, 0x63
    bne     t0, t1, update_offset__is_not_branch
    # ---------- beq or bne ----------
    srli    t0, a0, 7
    andi    t0, t0, 1
    slli    t0, t0, 11      # offset[11]

    srli    t1, a0, 8
    andi    t1, t1, 0xf     # offset[4:1]
    slli    t1, t1, 1
    or      t0, t0, t1

    srli    t1, a0, 25
    andi    t1, t1, 0x3f    # offset[10:5]
    slli    t1, t1, 5
    or      t0, t0, t1

    srli    t1, a0, 31
    andi    t1, t1, 1       # offset[12]
    slli    t1, t1, 12
    or      t0, t0, t1      # offset

    # sign-extend the offset
    slli    t0, t0, 19
    srai    t0, t0, 19

    # Recalculate the offset
    add     t1, a1, t0      # Address of the target in the original instructions
    slli    t1, t1, 2
    add     t1, a3, t1      # Get address of the target in decompressed instructions
    lw      t1, 0(t1)
    sub     t0, t1, a2      # New offset in the 32-bit format instructions

    # Update the offset in the instruction
    # First, clear the offset
    li      t1, 0x1f
    slli    t1, t1, 7
    not     t1, t1
    and     a0, a0, t1  # Clear bit 7:11

    li      t1, 0x7f
    slli    t1, t1, 25
    not     t1, t1
    and     a0, a0, t1  # Clear bit 25:31

    # Set the offset
    srli    t1, t0, 11
    andi    t1, t1, 1       # offset[11]
    slli    t1, t1, 7
    or      a0, a0, t1

    srli    t1, t0, 1
    andi    t1, t1, 0xf     # offset[4:1]
    slli    t1, t1, 8
    or      a0, a0, t1

    srli    t1, t0, 5
    andi    t1, t1, 0x3f    # offset[10:5]
    slli    t1, t1, 25
    or      a0, a0, t1

    srli    t1, t0, 12
    andi    t1, t1, 1       # offset[12]
    slli    t1, t1, 31
    or      a0, a0, t1

    j       update_offset__return
update_offset__is_not_branch:
    li      t1, 0x6f
    bne     t0, t1, update_offset__return
    # ------------ jal ------------
    # Get the offset
    srli    t0, a0, 12
    andi    t0, t0, 0xff
    slli    t0, t0, 12      # offset[19:12]

    srli    t1, a0, 20
    andi    t1, t1, 1       # offset[11]
    slli    t1, t1, 11
    or      t0, t0, t1

    srli    t1, a0, 21
    andi    t1, t1, 0x3ff   # offset[10:1]
    slli    t1, t1, 1
    or      t0, t0, t1

    srli    t1, a0, 31
    andi    t1, t1, 1       # offset[20]
    slli    t1, t1, 20
    or      t0, t0, t1

    # sign-extend the offset
    slli    t0, t0, 11
    srai    t0, t0, 11

    # Recalculate the offset
    add     t1, a1, t0      # Address of the target in the original instructions
    slli    t1, t1, 2
    add     t1, a3, t1      # Get address of the target in decompressed instructions
    lw      t1, 0(t1)
    sub     t0, t1, a2      # New offset in the 32-bit format instructions

    # Update the offset in the instruction
    # First, clear the offset
    li      t1, 0xfff
    and     a0, a0, t1

    srli    t1, t0, 12
    andi    t1, t1, 0xff    # offset[19:12]
    slli    t1, t1, 12
    or      a0, a0, t1

    srli    t1, t0, 11
    andi    t1, t1, 1       # offset[11]
    slli    t1, t1, 20
    or      a0, a0, t1

    srli    t1, t0, 1
    andi    t1, t1, 0x3ff   # offset[10:1]
    slli    t1, t1, 21
    or      a0, a0, t1

    srli    t1, t0, 31
    andi    t1, t1, 1       # offset[31]
    slli    t1, t1, 31
    or      a0, a0, t1

update_offset__return:
    jr      ra

print_bin:
    # Print the given integer in binary
    # parameters:
    #   a0 - the integer to print
    #   a1 - number of bits to print
    mv      t0, a0
    addi    t1, a1, -1
    li      t2, 1
    sll     t2, t2, t1
print_bin__loop:
    bltz    t1, print_bin__loop_end

    and     t3, t0, t2
    beqz    t3, print_bin__is_0
    li      a1, 49
    j       print_bin__is_0_end_if
print_bin__is_0:
    li      a1, 48
print_bin__is_0_end_if:
    li      a0, 11
    ecall

    srli    t2, t2, 1
    addi    t1, t1, -1
    j       print_bin__loop
print_bin__loop_end:
    jr      ra
