/*  Project 1.1: RISC-V instructions to RISC-V compressed instructions in C89.
    The following is the starter code provided for you. To finish the task, you 
    should define and implement your own functions in translator.c, compression.c, 
    utils.c and their header files.
    Please read the problem description before you start.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "src/compression.h"
#include "src/utils.h"

#include "translator.h"

/*check if file can be correctly opened */
static int open_files(FILE** input, FILE** output, const char* input_name, const char* output_name)
{ 
    *input = fopen(input_name, "r");
    if (!*input)
    { /* open input file failed */
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    *output = fopen(output_name, "w");
    if (!*output)
    { /* open output file failed */
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0; /* no problem opening files */
}

static int close_files(FILE** input, FILE** output){
    fclose(*input);
    fclose(*output); /* close the files at the end */
    return 0;
}

static void print_usage_and_exit() {
    printf("Usage:\n");
    printf("Run program with translator <input file> <output file>\n"); /* print the correct usage of the program */
    exit(0);
}


/*Run the translator 
*/
int translate(const char*in, const char*out){
    FILE *input, *output;
    int err = 0,tmp,i; 
    char* oper;
    char** all_oper;/*store all oper*/
    char**  ans;/*initial the ans */
    char ch;
    if (in){    /* correct input file name */
        if(open_files(&input, &output, in, out) != 0)
            exit(1);
        
    	oper=(char* )malloc(33*sizeof(char));/* read test message in oper */
    	while(fscanf(input, "%s", oper)!=EOF)
    	{
    		num++;
    		ch=fgetc(input);/* read test message in oper */
    	}
	all_oper=(char** )malloc(tmp*sizeof(char*));
	ans=(char** )malloc(tmp*sizeof(char*));
	for(i=0;i<tmp;i++)*prepare for store*/
	{
		all_oper[i]=(char* )malloc(33*sizeof(char));/*prepare for store*/
		all_oper[i][32]='\0';
		ans[i]=(char* )malloc(33*sizeof(char));/*prepare for store*/
		ans[i][32]='\0';
	}
	for(i=0;i<tmp;i++)
	{
		fscanf(input, "%s", all_oper[i]);/* read test message in oper */
		ch=fgetc(input);
	}
    	
    	Compress_operation(all_oper,ans,tmp);
    	fputs(ans, output);
        close_files(&input, &output);/* read test message in oper */
      	free(oper);oper=NULL;/*free*/
      	for(i=0;i<tmp;i++)
	{
		free(all_oper[i]);/*free*/
		free(ans[i]);
	}
	free(all_oper);free(ans);/*free*/
    }
    return err;/* read test message in oper */
}

/* main func */
int main(int argc, char **argv){
    char* input_fname, *output_fname;
    int err;
  


    if (argc != 3) /* need correct arguments */
        print_usage_and_exit();

    input_fname = argv[1];
    output_fname = argv[2];

    err = translate(input_fname, output_fname); /* main translation process */
    if (err)
        printf("One or more errors encountered during translation operation.\n"); /* something wrong */
    else
        printf("Translation process completed successfully.\n"); /* correctly output */

    return 0;
}