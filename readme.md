## 写前需读：[RISC-V 32位到16位压缩对照表](https://docs.qq.com/sheet/DR2RQR21nb2lscXVX?tab=BB08J2)

分工：李欣然`utils.c` ，`translator.c` 

`translator.c`已经处理完成。`utils.c`已经测试完成，没有memlack和编译错误
**更新了预处理方法**

陈雨瑶 `compression.c`

# 整体思路

`utils.c` 从`translator.c`接受input字符串，写判断返回结果

`compression.c`接受由`utils.c`传递过来的判断结果，如果是否则输出原来的字符串，如果是是则执行压缩

`translator.c` 接收需要压缩的字符串`input`,接收由`compression.c`传递过来的压缩后的字符串并输出结果到`output`

## 1. `utils.c`怎么写

用以下结构体返回信息，总判断函数return类型为`operation*`

**update：number存放的数字顺序位11-0或者4-0或者其他不是乱序，需要处理成位数由高到低！！**

**新增了功能函数substring，substring_2，建议调用非常好用**

```c
#define OPERATION_CONTENT_SIZE  32
#define OPERATION_OPCODE_SIZE  7
#define OPERATION_FUNCT3_SIZE  3
#define OPERATION_FUNCT7_SIZE  7
#define OPERATION_IMM_SIZE  12
#define OPERATION_NZIMM_SIZE  20
#define OPERATION_SHAMT_SIZE  5
#define OPERATION_RD_SIZE  5
#define OPERATION_LONGIMM_SIZE  9
#define RD_ZERO "00000"
#define RD_ONE  "00001"

typedef struct operation_{
    char*    content; /* the original code of 32 bit */
    int     can_be_compressed; /* if the original code can be compressed */
    char*    opcode; /* the opcode of original code of 32 bit */
    char*    funct3;  /* the funct3 of original code of 32 bit */
    char*    funct7;  /* the funct7 of original code of 32 bit */
    char*    rd; /* store rd */
    char*    rs1;/* store rs */
    char*    rs2;/* store rs2 */
    char*    number; /* store imm/shamt */ 
} operation;

void substring(char* src,int pos,int length,char* subch);/*pos -> begin, length -> the len you want */

void substring_2(char* src,int pos,int length,char* subch,int subch_begin);
```

~~其中`bool`类型c中并没有，参与自定义类型如下:~~**由于报错太多，此条作废，改为int类型，1/0**

- 首先先提取`opcode`，以此为第一筛选标准，进而根据`funct3`基本可以确定所有操作

- 根据操作不同，判断`rd/rs1/rs2`是否符合数据标准，在`CL/CS/CB`中需要满足`rd/rs1/rs2`均在`00100-01111`之间，还有其他特殊标准另外判断

- 判断`number`是否符合标准，高位从第5位往后开始一定都是0

以上两条只要有不和规范的，`can_be_compressed=0`，直接return

从未进过`opcode`和`funct3`判断的,`can_be_compressed=0`，直接return

## 2. `compression.c`怎么写

### 给字符串`malloc`的时候要赋予`(target_len+1)*sizeof(char)，` 并且需要手动给结束符号`target[target_len]='\0'`

- 首先调用`utils.c`的判断函数`IF_can_be_compressed(char* input)`得到`operarion*` 来访问结构体的值

- 如果`can_be_compressed=0`，直接输出原代码

- 其次进行压缩,由于所有的提取和筛选已经做好，只需要基本根据`opcode`和`funct3`两重判断来输出对应的16bit操作

返回值为`char*`的函数，需在返回前free掉所有的结构体内存。**该字符串只包含0，1，不含任何空格和换行符**

**在某些操作中32bit的`imm/shamt`所对应的数字顺序和16bit中的`imm/offset`有的有很大区别，请重新排序，表中用灰色标注**

**在CI操作中，16bit有非负数规定，故所有`nzimm[17],nzimm[5],shamt[5]`一定为0**

~~最后需要注意的是，`utils.c`负责malloc各个变量的空间，根据指令类型不同需要赋予不同大小的空间，如果没有该变量则不赋予空间。~~

update：**目前决策是统一所有变量都赋予空间大小，number根据长度不同赋不同大小，其他变量统一一起初始化，但如果指令没有该变量就不赋予值**

**由于返回值是`char* `的函数无法free，所以改为传递参数的模式，`void Compress_operation(char* input,char* ans)`**

**该`ans`提前赋予了空间`33*sizeof(char)`，故若字符串可以压缩，需要`ans=(char* ) realloc(ans,17*sizeof(char));ans[16]='\0';`**

## Final

最后只需要调用`compression.c`中的函数即可，将得到的`char*`写入output文件中

**update：**

## 细节处理

- 在`lw,sw,beq,bne,jal`操作中需要重新排序，经处理后保证`oper->number` 的数据顺序为高位到低位以此递减即一定保证为`imm[11:0],imm[20:1],imm[12:1]`的顺序储存其中`oper->numer[0]=imm[11],oper->num[11]=imm[0]`这个已经申明过，同时`imm[12:1]=oper->numer[0:11],imm[20:1]=oper->number[0:19]`请做好对应关系的处理

- 同时`offset[6:2]=imm[6:2]=oper->number[5:9]`

- `offset[8:1]=imm[8:1]=oper->number[4:11]``offset[11:1]=imm[11:1]=oper->number[9:19]`

- `offset[11|4|9:8|10|6|7|3:1|5]`等于重新排序再按照这个顺序存一遍，建议先从`oper->number`复制过去得到`offset[11:1]`再重新排序

- 在CL,CS,CB中`rd,rs1,rs2`已经保证是`x8-x15`只需截取其`rd[2:0]`到16bit中即可

- 所有shift操作必须`shamt[0]=0`，以及所有lw,sw的操作也是`offset`非负，这是已知的，其他的最高位的符号需要保持原来的符号是1就是1，是0就是0，其他数字直接复制过去
