#ifndef UTILS_H
#define UTILS_H
/* Your code here... */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Your code here... */


#define OPERATION_CONTENT_SIZE  32
#define OPERATION_OPCODE_SIZE  7
#define OPERATION_FUNCT3_SIZE  3
#define OPERATION_FUNCT7_SIZE  7
/* some size */
#define OPERATION_IMM_SIZE  12
#define OPERATION_NZIMM_SIZE  20
#define OPERATION_SHAMT_SIZE  5
#define OPERATION_RD_SIZE  5
/* some size */
#define RD_ZERO "00000"
#define RD_ONE  "00001"
#define FUNCT3_ZERO  "000"
#define FUNCT7_ZERO  "0000000"
/* some size */
#define FUNCT7_ONE  "0100000"

typedef struct operation_
{
    char*	content; /* the original code of 32 bit */
    int 	can_be_compressed; /* if the original code can be compressed */
    char*	opcode; /* the opcode of original code of 32 bit */
    char*	funct3;  /* the funct3 of original code of 32 bit */
    char*	funct7;  /* the funct7 of original code of 32 bit */
    char*	rd; /* store rd */
    char*	rs1;/* store rs1 */
    char*	rs2;/* store rs2 */
    char*	number; /* store imm/shamt */
} operation;
operation*  New_operation();

void substring(char* src,int pos,int length,char* subch);/*pos -> begin, length -> the len you want */

void substring_2(char* src,int pos,int length,char* subch,int subch_begin);/*pos -> begin, length -> the len you want */

operation*  IF_can_be_compressed(char* input);


int  Jalr_operation(operation* oper);/* jalr operation */

int  Calculate_operation_1(operation* oper);/*calculate 1 operation */

int  Calculate_operation_2(operation* oper);/*calculate 2 operation */

int  Lui_operation(operation* oper); /* lui operation */

int  Lw_operation(operation* oper);

int  Sw_operation(operation* oper);

int  Condition_operation(operation* oper); /* condition oper */

int  Jal_operation(operation* oper);



#endif