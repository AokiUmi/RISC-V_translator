#ifndef COMPRESSION_H
#define COMPRESSION_H
/* Your code here... */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
/* Your code here... */
#define OPERATION_ANS_SIZE  16

void Compress_operation(char** input,char** ans,int num);

void free_operation(operation* op);
#endif
