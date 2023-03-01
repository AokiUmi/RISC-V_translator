#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
/* judge if the code can be compressed */
/* Your code here... */

void substring(char* src,int pos,int length,char* subch)/*pos -> begin, length -> the len you want */
{
    int i,j;
    j=pos;
    for(i=0;i<length;i++)
        subch[i]=src[j++];
    return;     /*return */
}
void substring_2(char* src,int pos,int length,char* subch,int subch_begin)/*pos -> begin, length -> the len you want */
{
    int i,j;
    j=pos;
    for(i=subch_begin;i<length+subch_begin;i++)
        subch[i]=src[j++];
    return;       /*return */
}
operation* New_operation()/*malloc all argument of operation */
{
    operation* new_oper;
    new_oper= (operation* )malloc(sizeof(operation));
    new_oper->content=(char* )malloc((OPERATION_CONTENT_SIZE+1)*sizeof(char));
    new_oper->opcode=(char* )malloc((OPERATION_OPCODE_SIZE+1)*sizeof(char)); /* opcode */
    new_oper->funct3=(char* )malloc((OPERATION_FUNCT3_SIZE+1)*sizeof(char));
    new_oper->funct7=(char* )malloc((OPERATION_FUNCT7_SIZE+1)*sizeof(char));
    new_oper->number=(char* )malloc((OPERATION_IMM_SIZE+1)*sizeof(char)); /* number */
    new_oper->rd=(char* )malloc((OPERATION_RD_SIZE+1)*sizeof(char));
    new_oper->rs1=(char* )malloc((OPERATION_RD_SIZE+1)*sizeof(char));
    new_oper->rs2=(char* )malloc((OPERATION_RD_SIZE+1)*sizeof(char));/* rs1 */
    new_oper->content[32]='\0';
    new_oper->opcode[7]='\0';
    new_oper->funct3[3]='\0';
    new_oper->funct7[7]='\0';/*funct7 */
    new_oper->number[12]='\0';
    new_oper->rd[5]='\0';
    new_oper->rs1[5]='\0';
    new_oper->rs2[5]='\0';/*rs2  */
    return new_oper;
}
operation*  IF_can_be_compressed(char* input)
{
    operation* Newoper=New_operation();/* copy content */
    strcpy(Newoper->content, input); /* copy the content */
    substring(Newoper->content, 25, 7,Newoper->opcode);
    if(!strcmp(Newoper->opcode,"1100111")) /*judge opcode 1*/
        Newoper->can_be_compressed=Jalr_operation(Newoper);
    else if(!strcmp(Newoper->opcode,"0010011"))
        Newoper->can_be_compressed=Calculate_operation_1(Newoper);/*judge opcode 2*/
    else if(!strcmp(Newoper->opcode,"0110111"))
        Newoper->can_be_compressed=Lui_operation(Newoper);
    else if(!strcmp(Newoper->opcode,"0000011"))/*judge opcode 3*/
        Newoper->can_be_compressed=Lw_operation(Newoper);
    else if(!strcmp(Newoper->opcode,"0100011"))
        Newoper->can_be_compressed=Sw_operation(Newoper);
    else if(!strcmp(Newoper->opcode,"0110011"))/*judge opcode 4*/
        Newoper->can_be_compressed=Calculate_operation_2(Newoper);
    else if(!strcmp(Newoper->opcode,"1100011"))
        Newoper->can_be_compressed=Condition_operation(Newoper);
    else if(!strcmp(Newoper->opcode,"1101111"))/*judge opcode 5*/
        Newoper->can_be_compressed=Jal_operation(Newoper);
    else Newoper->can_be_compressed=0;
    return Newoper;/*judge opcode 6*/
}
 
int  Jalr_operation(operation* oper) /* jalr oper */
{
    substring(oper->content,20,OPERATION_RD_SIZE,oper->rd);
    substring(oper->content,12,OPERATION_RD_SIZE,oper->rs1);
    substring(oper->content,7,OPERATION_RD_SIZE,oper->rs2);/* copy someting */
    substring(oper->content,17,OPERATION_FUNCT3_SIZE,oper->funct3);
    substring(oper->content,0,OPERATION_FUNCT7_SIZE,oper->funct7);
    if(!strcmp(oper->rs1,RD_ZERO))/* rs1!=0 */
        return 0;
    if((!strcmp(oper->rd,RD_ZERO)||!strcmp(oper->rd,RD_ONE))&&!strcmp(oper->funct7,FUNCT7_ZERO)&&!strcmp(oper->rs2,RD_ZERO)) /* rd==1 || rd == 0 */
        return 1;
    return 0;/*negative judge*/
}
int negative_judge(char* immediate,int length,int sum)
{
    char* tmp,tmp2;int i;/*negative judge*/
    tmp*=tmp=(char* )malloc((sum+1)*sizeof(char));/*negative judge*/
    tmp[sum]='\0';
    tmp2=(char* )malloc((length+1)*sizeof(char));
    tmp2[length]='\0';
    strcpy(tmp,immediate);
    if(tmp[sum-1]=='1')tmp[sum-1]='0';
    else /*negative judge*/
    {
        tmp[sum-1]='1';
        for(i=sum-2;i>=0;i--){/*negative judge*/
            if(tmp[i]=='1'){
                tmp[i]='0';break;/*negative judge*/
            }
            tmp[i]='1';/*negative judge*/
        }
    }/*negative judge*/
    substring(tmp,0,length,tmp2);
    if(length== 7&&!strcmp(tmp2,"1111111"))
    {
        free(tmp);tmp=NULL;
        free(tmp2);tmp2=NULL;
        return 1;
    }
    else if(length== 15&& !strcmp(tmp2,"111111111111111"))
    {
        free(tmp);tmp=NULL;
        free(tmp2);tmp2=NULL;
        return 1;
    }
    else if(length ==4 &&!strcmp(tmp2,"1111"))
    {
        free(tmp);tmp=NULL;
        free(tmp2);tmp2=NULL;
        return 1;
    }
    else if(length == 9&& !strcmp(tmp2,"111111111"))
    {
        free(tmp);tmp=NULL;
        free(tmp2);tmp2=NULL;
        return 1;
    }
    free(tmp);tmp=NULL;
    free(tmp2);tmp2=NULL;
    return 0;

}
int Imm_is_ok(char* Immediate,int length)
{
    char* tmp; /* judege immediate */
    tmp=(char* )malloc((length+1)*sizeof(char));
    tmp[length]='\0';
    substring(Immediate,0,length,tmp);
    if(length == 7)/* judege immediate len =7 */
    {
        if(!strcmp(tmp,FUNCT7_ZERO)||negative_judge(Immediate,length,12))
        {
            free(tmp);tmp=NULL;
            return 1;/* judege immediate len =7 */
        }
    }
    else if(length == 15)/* judege immediate len =15 */
    {
        if(!strcmp(tmp,"000000000000000")||negative_judge(Immediate,length,20))
        {
            free(tmp);tmp=NULL;
            return 1;/* judege immediate len =15 */
        }
    }
    else if(length == 5)/* judege immediate len =5 */
    {
        char* tmp2;
        tmp2=(char* )malloc(3*sizeof(char));
        tmp2[2]='\0';
        substring(Immediate,10,2,tmp2);/* judege immediate len =5 */
        if(!strcmp(tmp,"00000")&&!strcmp(tmp2,"00"))
        {
            free(tmp);tmp=NULL;
            free(tmp2);tmp2=NULL;
            return 1;/* judege immediate len =5 */
        }
        free(tmp2);tmp2=NULL;
    }
    else if(length == 4)/* judege immediate len =4 */
    {
        if(!strcmp(tmp,"0000")||negative_judge(Immediate,length,12))
        {
            free(tmp);tmp=NULL;/* judege immediate len =4 */
            return 1;
        }
    }
    else if(length == 9)/* judege immediate len =9 */
    {
        if(!strcmp(tmp,"000000000")||negative_judge(Immediate,length,20))
        {
            free(tmp);tmp=NULL;
            return 1;
        }/* judege immediate len =9 */
    }
    free(tmp);tmp=NULL;
    return 0;/* judege immediate len =9 */
}
int Rd_is_ok(char* rd)/*judge rd/rs1/rs2*/
{
    int tmp,i,pow;
    tmp=0;pow=1;
    for(i=4;i>=0;i--)/*judge rd/rs1/rs2*/
    {
        tmp+=(rd[i]-'0')*pow;
        pow*=2;
    }/*judge rd/rs1/rs2*/
    if(tmp>=8&&tmp<=15) /*x8-x15 */
        return 1;
    return 0;
        
}
int Calculate_operation_1(operation* oper) /*addi slli oper */
{
    substring(oper->content,20,OPERATION_RD_SIZE,oper->rd);
    substring(oper->content,12,OPERATION_RD_SIZE,oper->rs1);
    substring(oper->content,17,OPERATION_FUNCT3_SIZE,oper->funct3);
    if(!strcmp(oper->funct3,FUNCT3_ZERO))/* addi oper */
    {
        substring(oper->content,0,OPERATION_IMM_SIZE,oper->number);
        if((!strcmp(oper->rs1,oper->rd)||!strcmp(oper->rs1,RD_ZERO))&&strcmp(oper->rd,RD_ZERO)&&Imm_is_ok(oper->number,7)&&strcmp(oper->number,"000000000000"))
            return 1;/* rs1 == rd || rs1 == 0 && rd != 0 ,imm !=0 */
        return 0;
    }
    else if(!strcmp(oper->funct3,"001"))/* slli oper */
    {
        substring(oper->content,0,OPERATION_FUNCT7_SIZE,oper->funct7);
        if(!strcmp(oper->funct7,FUNCT7_ZERO)&&!strcmp(oper->rs1,oper->rd)&&strcmp(oper->rd,RD_ZERO))
        {/* rs1==rd && rd != 0 */
            oper->number=(char* )realloc(oper->number,(OPERATION_SHAMT_SIZE+1)*sizeof(char));
            oper->number[5]='\0';
            substring(oper->content,7,OPERATION_SHAMT_SIZE,oper->number); 
            return 1; /*return */
        }
        return 0;

    }
    else if(!strcmp(oper->funct3,"101"))/*srli srali oper*/
    {
        substring(oper->content,0,OPERATION_FUNCT7_SIZE,oper->funct7);
        if(!strcmp(oper->funct7,FUNCT7_ZERO)||!strcmp(oper->funct7,FUNCT7_ONE))/*srli or srali oper*/
        {
            if(!strcmp(oper->rs1,oper->rd)&&Rd_is_ok(oper->rd))/* rd is x8-x15 && rs1==rd */
            {
                oper->number=(char* )realloc(oper->number,(OPERATION_SHAMT_SIZE+1)*sizeof(char));
                oper->number[5]='\0';
                substring(oper->content,7,OPERATION_SHAMT_SIZE,oper->number); /*substring */
                return 1;
            }
            return 0; /*return */
        }
        return 0; /*return */

    }
    else if(!strcmp(oper->funct3,"111"))/*andi oper */
    {
        substring(oper->content,0,OPERATION_IMM_SIZE,oper->number); 
        if(!strcmp(oper->rs1,oper->rd)&&Rd_is_ok(oper->rd)&&Imm_is_ok(oper->number,7))/* rd is x8-x15 && rs1==rd %% imm is ok*/
            return 1;
        return 0; /*return */
    }
    return 0; /*return */
}
int Lui_operation(operation* oper)/* lui oper */
{
    substring(oper->content,20,OPERATION_RD_SIZE,oper->rd);
    oper->number=(char* )realloc(oper->number,(OPERATION_NZIMM_SIZE+1)*sizeof(char)); /*change size*/
    oper->number[20]='\0';
    substring(oper->content,0,OPERATION_NZIMM_SIZE,oper->number);
    if(strcmp(oper->rd,RD_ZERO)&&strcmp(oper->rd,"00010")&&Imm_is_ok(oper->number,15)&&strcmp(oper->number,"00000000000000000000"))/*rd != {2,0}&& nzimm is !=0 */
        return 1;
    return 0;/*return */
}

int Lw_operation(operation* oper)/* lw oper */
{
    substring(oper->content,17,OPERATION_FUNCT3_SIZE,oper->funct3);
    if(!strcmp(oper->funct3,"010"))
    {
        substring(oper->content,20,OPERATION_RD_SIZE,oper->rd);
        substring(oper->content,12,OPERATION_RD_SIZE,oper->rs1);/*substring some */
        substring(oper->content,0,OPERATION_IMM_SIZE,oper->number);
        if(Imm_is_ok(oper->number,5)&&Rd_is_ok(oper->rd)&&Rd_is_ok(oper->rs1))/*imm is ok, rd,rs1 = x8-x15 */
            return 1;
        return 0;/*return */
    }
    return 0;/*return */

}
int Sw_operation(operation* oper)/* sw oper */
{
    substring(oper->content,17,OPERATION_FUNCT3_SIZE,oper->funct3);
    if(!strcmp(oper->funct3,"010"))
    {
        substring(oper->content,12,OPERATION_RD_SIZE,oper->rs1);
        substring(oper->content,7,OPERATION_RD_SIZE,oper->rs2);/*substring some */
        substring_2(oper->content,0,OPERATION_FUNCT7_SIZE,oper->number,0);
        substring_2(oper->content,20,OPERATION_RD_SIZE,oper->number,7);
        if(Imm_is_ok(oper->number,5)&&Rd_is_ok(oper->rs2)&&Rd_is_ok(oper->rs1))/*imm is ok, rs2,rs1 = x8-x15 */
            return 1;
        return 0;/*return */
    }
    return 0;/*return */

}
int Calculate_operation_2(operation* oper) /* and or xor oper */
{
    substring(oper->content,20,OPERATION_RD_SIZE,oper->rd);
    substring(oper->content,12,OPERATION_RD_SIZE,oper->rs1);
    substring(oper->content,7,OPERATION_RD_SIZE,oper->rs2);/*substring some */
    substring(oper->content,17,OPERATION_FUNCT3_SIZE,oper->funct3);
    substring(oper->content,0,OPERATION_FUNCT7_SIZE,oper->funct7);/*substring some */
    if(!strcmp(oper->funct3,"000")&&!strcmp(oper->funct7,FUNCT7_ZERO))/*add oper*/
    {
    	if((!strcmp(oper->rs1,oper->rd)||!strcmp(oper->rs1,RD_ZERO))&&(strcmp(oper->rd,RD_ZERO)&&strcmp(oper->rs2,RD_ZERO)))
	{/* rs1==rd|| rs1==0 && rd!=0 && rs2!=0 */
	   return 1;
	}
	return 0;
    }
    if(!Rd_is_ok(oper->rd)||!Rd_is_ok(oper->rs1)||!Rd_is_ok(oper->rs2))/* rd ,rs1, rs2 = x8-x15 */
        return 0;
    if(strcmp(oper->rd,oper->rs1))/* rd==rs1 */
        return 0;
    if(!strcmp(oper->funct3,"000")&&!strcmp(oper->funct7,FUNCT7_ONE))/*sub oper  */
        return 1;
    else if((!strcmp(oper->funct3,"100")||!strcmp(oper->funct3,"110")||!strcmp(oper->funct3,"111"))&&!strcmp(oper->funct7,FUNCT7_ZERO))/* or xor oper*/
        return 1;/*return */
    return 0;/*return */
}
void Replace_imm_for_condition(char* Immediate) /*replace imm */
{
    char* tmp;
    tmp=(char* )malloc((OPERATION_IMM_SIZE+1)*sizeof(char));
    tmp[12]='\0';
    strcpy(tmp,Immediate);/*replace imm */
    Immediate[0]=tmp[0];
    Immediate[1]=tmp[11];
    substring_2(tmp,1,6,Immediate,2);
    substring_2(tmp,7,4,Immediate,8);/*replace imm */
    free(tmp);tmp=NULL;

}
int Condition_operation(operation* oper) /* beq ,bne oper */
{
    substring(oper->content,12,OPERATION_RD_SIZE,oper->rs1);
    substring(oper->content,7,OPERATION_RD_SIZE,oper->rs2);
    substring(oper->content,17,OPERATION_FUNCT3_SIZE,oper->funct3);/*substring something */
    substring_2(oper->content,0,OPERATION_FUNCT7_SIZE,oper->number,0);
    substring_2(oper->content,20,OPERATION_RD_SIZE,oper->number,7);
    if(strcmp(oper->rs2,RD_ZERO)||!Rd_is_ok(oper->rs1))/*rs2 ==0, rs1 = x8-x15 */
        return 0;
    if(!strcmp(oper->funct3,"000")||!strcmp(oper->funct3,"001"))
    {
        Replace_imm_for_condition(oper->number);/*replace imm */
        if(Imm_is_ok(oper->number,4))
            return 1;
        return 0;/*return */
    }
    return 0;/*return */
}
void Replace_imm_for_jal(char* Immediate) /*replace imm for jal */
{
    char* tmp;
    tmp=(char* )malloc((OPERATION_NZIMM_SIZE+1)*sizeof(char));
    tmp[20]='\0';
    strcpy(tmp,Immediate);/*replace imm for jal */
    Immediate[0]=tmp[0];
    substring_2(tmp,12,8,Immediate,1);
    Immediate[9]=tmp[11];/*replace imm for jal */
    substring_2(tmp,1,10,Immediate,10);
    free(tmp);tmp=NULL;/*replace imm for jal */
}
int Jal_operation(operation* oper) /*jal oper */
{
    substring(oper->content,20,OPERATION_RD_SIZE,oper->rd);
    if(strcmp(oper->rd,RD_ONE)&&strcmp(oper->rd,RD_ZERO))/*jal oper 3*/
        return 0;
    oper->number=(char* )realloc(oper->number,(OPERATION_NZIMM_SIZE+1)*sizeof(char));
    oper->number[20]='\0';/*jal oper 2 */
    substring(oper->content,0,OPERATION_NZIMM_SIZE,oper->number);
    Replace_imm_for_jal(oper->number);
    if(Imm_is_ok(oper->number,9))/*jal oper 1*/
        return 1;
    return 0; /*retrun 0*/
}