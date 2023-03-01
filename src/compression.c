#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compression.h"
#include "utils.h"
/* If the code can be compressed, then compress it, else output the oringinal code */
/* Your code here... */

void Copy_string(char* str,int len,int str_begin,char* goal,int begin)/*copy str to goal*/
{
	int i,j;
	j=str_begin;/*begin pos*/
	for(i=begin;i<begin+len;i++)
		goal[i]=str[j++];
	return;/*end*/
	
}

void Jalr_op(char* ans,operation* oper)/*jalr oper*/
{
	Copy_string("0000010",7,0,ans,9);
	Copy_string(oper->rs1,5,0,ans,4);
	if(!strcmp(oper->rd,RD_ZERO))/*rd ==0 */
	{
		Copy_string("1000",4,0,ans,0);
		return;
	}
	Copy_string("1001",4,0,ans,0);/* rd== 1 */
	return;
}
void Change_rd(char* rd,char* str,int begin)
{
	int tmp,pow,i;
	tmp=0;pow=1;
	for(i=4;i>=0;i--)
	{
		tmp+=(rd[i]-'0')*pow;
		pow*=2;
	}
	if(tmp==8)
		Copy_string("000",3,0,str,begin);
	else if(tmp==9)
		Copy_string("001",3,0,str,begin);
	else if(tmp==10)
		Copy_string("010",3,0,str,begin);
	else if(tmp==11)
		Copy_string("011",3,0,str,begin);
	else if(tmp==12)
		Copy_string("100",3,0,str,begin);
	else if(tmp==13)
		Copy_string("101",3,0,str,begin);
	else if(tmp==14)
		Copy_string("110",3,0,str,begin);
	else if(tmp==15)
		Copy_string("111",3,0,str,begin);
	return;

}
void  Calculate_op_1(char* ans,operation* oper)/*aaddi, sub */
{
	Copy_string("01",2,0,ans,14);
	if(!strcmp(oper->funct3,"000"))/*addi oper*/
	{
		if(!strcmp(oper->rs1,RD_ZERO))/*rs1=0*/
		{
			Copy_string(oper->rd,5,0,ans,4);
			Copy_string("010",3,0,ans,0);
			ans[3]=oper->number[0];
			Copy_string(oper->number,5,7,ans,9);
			return;
		}
		else /*rs1= rd,nzimm */
		{
			Copy_string(oper->rd,5,0,ans,4);
			Copy_string("0000",4,0,ans,0);/*imm[5]=0*/
			Copy_string(oper->number,5,7,ans,9);
			return;
		}
	}
	else if(!strcmp(oper->funct3,"001"))/*slli oper*/
	{
		Copy_string("10",2,0,ans,14);
		Copy_string("0000",4,0,ans,0);/*imm[5]=0*/
		Copy_string(oper->rd,5,0,ans,4);
		Copy_string(oper->number,5,0,ans,9);
		return;
	}
	else if(!strcmp(oper->funct3,"101"))/*srai,srli oper*/
	{
		Change_rd(oper->rd,ans,6);
		Copy_string(oper->number,5,0,ans,9);
		Copy_string("1000",4,0,ans,0);/*shamt[5]=0*/
		if(!strcmp(oper->funct7,FUNCT7_ZERO)) /*funct7 ==0000000*/
			Copy_string("00",2,0,ans,4);
		else Copy_string("01",2,0,ans,4);/*funct7 ==0100000*/
		return;
	}
	else /*andi*/
	{
		Change_rd(oper->rd,ans,6);
		Copy_string(oper->number,5,7,ans,9);
		ans[3]=oper->number[0];/*copy imm[0]*/
		Copy_string("100",3,0,ans,0);
		Copy_string("10",2,0,ans,4);
		return;
	}/*end*/
}
void Lui_op(char* ans,operation* oper)/*lui oper*/
{
	Copy_string("0110",4,0,ans,0);
	Copy_string(oper->rd,5,0,ans,4);
	Copy_string(oper->number,5,15,ans,9);
	Copy_string("01",2,0,ans,14);/* copy*/
	return;
}
void Lw_op(char* ans,operation* oper)/*lw oper*/
{
	char* tmp;
	tmp=(char* )malloc(6*sizeof(char));
	tmp[5]='\0';
	Copy_string("00",2,0,ans,14);/* copy*/
	Copy_string("010",3,0,ans,0);
	Change_rd(oper->rd,ans,11);/*rd*/
	Change_rd(oper->rs1,ans,6);/*rs1*/
	Copy_string(oper->number,5,5,tmp,0);/*reorder imm*/
	Copy_string(tmp,3,1,ans,3);
	ans[9]=tmp[4];
	ans[10]=tmp[0];/*reorder imm end*/
	free(tmp);tmp=NULL;
	return;
}
void Sw_op(char* ans,operation* oper)/*sw oper */
{
	char* tmp;
	tmp=(char* )malloc(6*sizeof(char));
	tmp[5]='\0';
	Copy_string("00",2,0,ans,14);/* copy*/
	Copy_string("110",3,0,ans,0);
	Change_rd(oper->rs2,ans,11);/*rs2*/
	Change_rd(oper->rs1,ans,6);/*rs1*/
	Copy_string(oper->number,5,5,tmp,0);/*reorder imm*/
	Copy_string(tmp,3,1,ans,3);
	ans[9]=tmp[4];
	ans[10]=tmp[0];/*reorder imm end*/
	free(tmp);tmp=NULL;
	return;
}
void Calculate_op_2(char* ans,operation* oper)
{
	if(!strcmp(oper->funct3,"000")&&!strcmp(oper->funct7,FUNCT7_ZERO))/*add oper*/
	{
	   	Copy_string("10",2,0,ans,14);
		Copy_string(oper->rd,5,0,ans,4);
		Copy_string(oper->rs2,5,0,ans,9);
		if(!strcmp(oper->rs1,oper->rd))/*if rs1==rd */
		{
			Copy_string("1001",4,0,ans,0);
			return;
		}
		Copy_string("1000",4,0,ans,0);/* rs1== 0 */
		return;
	}
	Copy_string("01",2,0,ans,14);/* copy*/
	Copy_string("100011",6,0,ans,0);
	Change_rd(oper->rd,ans,6);
	Change_rd(oper->rs2,ans,11);
	if(!strcmp(oper->funct3,"000"))/*sub oper*/
	{
		Copy_string("00",2,0,ans,9);
		return;
	}
	else if(!strcmp(oper->funct3,"111"))/*and*/
	{
		/*and oper*/
	        Copy_string("11",2,0,ans,9);
		return;
	}
	else if(!strcmp(oper->funct3,"100"))/*xor oper */
	{
		Copy_string("01",2,0,ans,9);
		return;
	}
	else Copy_string("10",2,0,ans,9);/*or oper*/
	return;
}
int Transform_10(char* offset,int len)/*transform 10*/
{
	int i,tmp,pow;
	tmp=0;pow=1;
	for(i=len-1;i>=0;i--)/*transform 10*/
	{
		tmp+=(offset[i]-'0')*pow;
		pow*=2;/*transform 10*/
	}
	return tmp;

}/*transform 10*/
void Transform_2(char* offset,int goal,int len)/*transform 2*/
{
	int i,x;
	i=len-1;x=goal;/*transform 2*/
	while(x)
	{
		offset[i--]=x%2+'0';/*transform 2*/
		x/=2;
	}
	for(i;i>=0;i--)offset[i]='0';/*transform 2*/
}
void Recalculate_offset(char* offset,int num,int pos,operation** alloper,int len)/*recalculate*/
{
	int i,offset_dec,num_compressed;
	if(offset[0]=='1')/*offset < 0 */
	{
		num_compressed=0;
		if(offset[len-1]=='1')offset[len-1]='0';/*offset -1*/
		else {
			offset[len-1]='1';/*offset -1*/
			for(i=len-2;i>=0;i--){
				if(offset[i]=='1'){/*offset -1*/
					offset[i]='0';
					break;
				}/*offset -1*/
				offset[i]='1';
			}
		}/*offset -1*/
		for(i=0;i<len;i++){
			if(offset[i]=='1')offset[i]='0';/*reverse*/
			else offset[i]='1';
		}
		offset_dec=Transform_10(offset,len);
		for(i=pos-1;i>=pos-offset_dec/2;i--)/*transform*/
			if(alloper[i]->can_be_compressed==0)
				num_compressed++;
		offset_dec-=num_compressed;/*transform*/
		Transform_2(offset,offset_dec,len);
		if(offset[len-1]=='0')offset[len-1]='1';/*transform*/
		else {
				offset[len-1]='0';
				for(i=len-2;i>=0;i--){
				if(offset[i]=='0'){/*offset -1*/
					offset[i]='1';
					break;
				}/*offset -1*/
				offset[i]='0';
				}
		}/*reverse*/
		for(i=0;i<len;i++){
			if(offset[i]=='1')offset[i]='0';/*reverse*/
			else offset[i]='1';
		}
		return;
	}
	else {
		num_compressed=0;/*transform 2*/
		offset_dec=Transform_10(offset,len);
		for(i=pos+1;i<=pos+offset_dec/2;i++)/*transform*/
			if(alloper[i]->can_be_compressed==0)
				num_compressed++;
		offset_dec+=num_compressed;/*transform*/
		Transform_2(offset,offset_dec,len);

	}/*transform 4*/
}
void  Condition_op(char* ans,operation* oper,int num,int pos,operation** alloper)/*beq, bne oper*/
{
	char* tmp;
	tmp=(char* )malloc(9*sizeof(char));
	tmp[8]='\0';
	Change_rd(oper->rs1,ans,6);
	Copy_string("01",2,0,ans,14);/* copy*/
	Recalculate_offset(oper->number,num,pos,alloper,12);
	Copy_string(oper->number,8,4,tmp,0);
	ans[3]=tmp[0];
	Copy_string(tmp,2,4,ans,4);
	Copy_string(tmp,2,1,ans,9);/*reorder offset */
	Copy_string(tmp,2,6,ans,11);
	ans[13]=tmp[3];
	if(!strcmp(oper->funct3,"000"))/*beq*/
		Copy_string("110",3,0,ans,0);
	else Copy_string("111",3,0,ans,0);/*bne*/
	
	free(tmp);tmp=NULL;
	return;

}
void Jal_op(char* ans,operation* oper,int num,int pos,operation** alloper)/*jal oper */
{
	char* tmp;
	tmp=(char* )malloc(12*sizeof(char));
	tmp[11]='\0';
	Copy_string("01",2,0,ans,14);/* copy*/
	Recalculate_offset(oper->number,num,pos,alloper,20);
	Copy_string(oper->number,11,9,tmp,0);
	ans[3]=tmp[0];
	ans[4]=tmp[7];
	Copy_string(tmp,2,2,ans,5);/*reorder offset */
	ans[7]=tmp[1];
	ans[8]=tmp[5];
	ans[9]=tmp[4];
	Copy_string(tmp,3,8,ans,10);/*reorder offset */
	ans[13]=tmp[6];
	if(!strcmp(oper->rd,RD_ZERO))/*rd=x0*/
		Copy_string("101",3,0,ans,0);
	else Copy_string("001",3,0,ans,0);/*rd=x1*/
	free(tmp);tmp=NULL;
	return;
}
void free_operation(operation** op,int num)
{
	/*free r*/
	int i;
	for(i=0;i<num;i++)
	{
		free(op[i]->content);
		free(op[i]->funct3);
		free(op[i]->funct7);
		free(op[i]->rd); /*free rd */
		free(op[i]->rs1);
		free(op[i]->rs2);
		free(op[i]->number);/*free number */
		free(op[i]->opcode);
		free(op[i]);
	}
	free(op);
	op=NULL; /*free oper */
	return;
}
void Compress_operation(char** input,char** ans,int num)
{
	operation** alloper;/*store alloper*/
	int i;
	alloper=(operation** )malloc(num*sizeof(operation));
	for(i=0;i<num;i++)
		alloper[i]=IF_can_be_compressed(input[i]);/*store alloper ans*/
	/*
	printf("Case : %d\n",1);
	printf("***********************************\n");
	printf("content :%s\n",Newoper->content);
	printf("can_be_compressed :%d\n",Newoper->can_be_compressed);
	printf("opcode :%s\n",Newoper->opcode);
	printf("rs1 :%s\n",Newoper->rs1);
	printf("rd :%s\n",Newoper->rd);
	printf("number :%s\n",Newoper->number);
	printf("***********************************\n");
	*/
	for(i=0;i<num;i++)/*look into each oper*/
	{
		if(alloper[i]->can_be_compressed==0)/*oper can not be compressed*/
		{
			ans[i]=(char *)realloc(ans[i],(OPERATION_ANS_SIZE+1)*sizeof(char));
			ans[i][16]='\0';
			strcpy(ans[i],alloper[i]->content);
			continue;
		}
		if(!strcmp(alloper[i]->opcode,"1100111")) /*judge opcode 1*/
			Jalr_op(ans[i],alloper[i]);
		else if(!strcmp(alloper[i]->opcode,"0010011"))
			Calculate_op_1(ans[i],alloper[i]);/*judge opcode 2*/
		else if(!strcmp(alloper[i]->opcode,"0110111"))
			Lui_op(ans[i],alloper[i]);
		else if(!strcmp(alloper[i]->opcode,"0000011"))/*judge opcode 3*/
			Lw_op(ans[i],alloper[i]);
		else if(!strcmp(alloper[i]->opcode,"0100011"))
			Sw_op(ans[i],alloper[i]);
		else if(!strcmp(alloper[i]->opcode,"0110011"))/*judge opcode 4*/
			Calculate_op_2(ans[i],alloper[i]);
		else if(!strcmp(alloper[i]->opcode,"1100011"))
			Condition_op(ans[i],alloper[i],num,i,alloper);
		else if(!strcmp(alloper[i]->opcode,"1101111"))/*judge opcode 5*/
			Jal_op(ans[i],alloper[i],num,i,alloper);
	}
    free_operation(alloper,num);
    return;

}
