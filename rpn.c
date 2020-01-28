/* Fill in your Name and GNumber in the following two comment fields
 * Name:Amish Papneja
 * GNumber:01211503
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "token.h"
#include "hash.h"

/* Local Function Declarations (that you need to finish implementing) */
static int read_file(char *filename, char *line);
static int parse_token(Symtab *symtab, Stack_head *stack, Token *tok);

/* Pre-Written Local Function Declarations */
static void print_header(char *filename, int step);
static void print_step_header(int step);
static void print_step_footer(Symtab *symtab, Stack_head *stack);
static void print_step_output(int val);

#define MAX_LINE_LEN 255
static void parse_operation(Token * tok, Token * first_pop, Token * second_pop, Token * result, Stack_head * stack);
static void parse_assignment(Symtab * symtab, Stack_head * stack);
int rpn(Stack_head *stack, Symtab *symtab, char *filename) {
  int step = 0; /* Used to track the program steps */
  int ret = 0;
  char line[MAX_LINE_LEN];
  Token *tok = NULL;

  ret = read_file(filename, line);
  if(ret != 0) {
    printf("Error: Cannot Read File %s.  Exiting\n", filename);
    exit(-1);
  }

  token_read_line(line, strlen(line));

  print_header(filename, step);

  while(token_has_next()) {
    step++; /* Begin the next step of execution */
    print_step_header(step);

    /* Get the next token */
    tok = token_get_next();
    /* Complete the implementation of this function later in this file. */
    ret = parse_token(symtab, stack, tok);
    if(ret != 0) {
      printf("Critical Error in Parsing.  Exiting Program!\n");
      exit(-1);
    }

    /* Prints out the end of step information */
    print_step_footer(symtab, stack);
  }

  return 0;
}

static int read_file(char *filename, char *line) {
  /* Implement This Function */
  	FILE *fp =fopen(filename,"r");
	if(fp==NULL)
	{
		printf("File Not Found\n");
		return -1;
	}
	char * ret=fgets(line,MAX_LINE_LEN,fp);//Reading exactly one line from the file
	if(ret==NULL)
	{
		printf("Nothing to read from file");
		return -1;
	}
	fclose(fp);//closing the FILE pointer
	return 0;
}

static int parse_token(Symtab *symtab, Stack_head *stack, Token *tok) {
  /* Implement This Function */
	if(symtab==NULL || stack==NULL || tok==NULL)return -1;
	if(tok->type==TYPE_VARIABLE)
	{
		stack_push(stack,tok);
	}
	if(tok->type==TYPE_OPERATOR)
	{
		Token * first_pop=malloc(sizeof(Token));
		if(first_pop==NULL){
			printf("Out of Heap Memory");exit(-1);}
		Token * second_pop=malloc(sizeof(Token));
		if(second_pop==NULL){
			printf("Out of Heap Memory");exit(-1);}
		Token * result=malloc(sizeof(Token));
		if(result==NULL){
			printf("Out of Heap Memory");exit(-1);}
		first_pop=stack_pop(stack);//Popped first token in the Stack
		second_pop=stack_pop(stack);// Popped another token in the Stack
		if( (first_pop->type == TYPE_VALUE) && (second_pop->type == TYPE_VALUE) )
		{//both opped tokens of value types -> Arithmetic operation and push to stack
			parse_operation(tok,second_pop,first_pop,result,stack);
		}
		else
		{
			if(first_pop->type == TYPE_VARIABLE)
			{//changing variable token to value
				Symbol * first_sym=malloc(sizeof(Symbol));
				if(first_sym==NULL){
					printf("Out of Heap Memory");exit(-1);}
				first_sym=hash_get(symtab,first_pop->variable);//getting symbol of that variable present from hashtable
				first_pop=token_create_value(first_sym->val);//creating new token of that particular value
				free(first_sym);
				first_sym=NULL;
			}
			if(second_pop->type == TYPE_VARIABLE)
			{
				Symbol * second_sym=malloc(sizeof(Symbol));
				if(second_sym==NULL){
					printf("Out of Heap Memory");exit(-1);}
				second_sym=hash_get(symtab,second_pop->variable);//getting symbol of that variable from hash_table
				second_pop=token_create_value(second_sym->val);//creating new token of that particular value
				free(second_sym);
				second_sym=NULL;
			}
			parse_operation(tok,second_pop,first_pop,result,stack);//after all both the tokens are changed to value type -> arithmatic and push
		}
//	free(first_pop);
//	free(second_pop);
//	first_pop=NULL;
//	second_pop=NULL;	
	}
	if(tok->type==TYPE_VALUE)
	{
		stack_push(stack,tok);//if value type. simply push it to stack
	}
	if(tok->type==TYPE_ASSIGNMENT)
	{
		parse_assignment(symtab,stack);
	}
	if(tok->type==TYPE_PRINT)
	{//print the value for last token present in the stack. which is the answer
		Token * popped=stack_pop(stack);
		if(popped->type==TYPE_VALUE)
		{
			print_step_output(popped->value);
		}
		if(popped->type==TYPE_VARIABLE)
		{// if variable type, getting symbol from hashtable and calling print
			Symbol * sym=hash_get(symtab,popped->variable);
			print_step_output(sym->val);
		}
	}
      	return 0;
}

static void parse_assignment(Symtab * symtab, Stack_head * stack)
{
		Token * first_pop=malloc(sizeof(Token));//value
		if(first_pop==NULL){
			printf("Out of Heap Memory");exit(-1);}
		Token * second_pop=malloc(sizeof(Token));
		if(second_pop==NULL){//variable
			printf("Out of Heap Memory");exit(-1);}
		Token * result=malloc(sizeof(Token));
		if(result==NULL){
			printf("Out of Heap Memory");exit(-1);}
		first_pop=stack_pop(stack);
		second_pop=stack_pop(stack);
		if(first_pop->type==TYPE_VALUE && second_pop->type==TYPE_VARIABLE)
		{//generic case. Second is variable, first is value.x 7 = 
			hash_put(symtab,second_pop->variable,first_pop->value);
		}
		else if(first_pop->type==TYPE_VARIABLE && second_pop->type==TYPE_VALUE)
		{// opposite case. 7 x =
			hash_put(symtab,first_pop->variable,second_pop->value);
		}
		else if(first_pop->type==TYPE_VARIABLE && second_pop->type==TYPE_VARIABLE)
		{// cases when both are variables
			Symbol * first_sym=hash_get(symtab,first_pop->variable);
			Symbol * second_sym=hash_get(symtab,second_pop->variable);
			if(second_sym==NULL && first_sym!=NULL)
			{//case when only second variable is present in the hashtable
				hash_put(symtab,second_pop->variable,first_sym->val);
			}
			if(second_sym!=NULL && first_sym!=NULL)
			{//case whrn on;y both variables are present in the hasttable
				hash_put(symtab,second_pop->variable,first_sym->val);
			}
			if(second_sym!=NULL && first_sym==NULL)
			{//case when first variable is present in the hashtble
				hash_put(symtab,first_pop->variable,second_sym->val);
			}	
		}
		token_free(first_pop);
		token_free(second_pop);
}
static void parse_operation(Token * tok, Token * first_pop, Token * second_pop, Token * result, Stack_head * stack)
{//ARITHMATIC OPERATIONS on the values of the tokens.
	if(tok->oper==OPERATOR_PLUS)
	{	
		result=token_create_value( (first_pop->value) + (second_pop->value) );
	}
	if(tok->oper==OPERATOR_MINUS)
	{
		result=token_create_value( (first_pop->value) - (second_pop->value) );
	}
	if(tok->oper==OPERATOR_MULT)
	{
		result=token_create_value( (first_pop->value) * (second_pop->value) );
	}
	if(tok->oper==OPERATOR_DIV)
	{
		result=token_create_value( (first_pop->value) / (second_pop->value) );
	}
	// Created result as the final token to be pushed
	stack_push(stack,result);//pushing result to the stack
	token_free(first_pop);
	token_free(second_pop);
}

/* Prints out the main output header
 */
static void print_header(char *filename, int step) {
  printf("######### Beginning Program (%s) ###########\n", filename);
  printf("\n.-------------------\n");
  printf("| Program Step = %2d\n", step);
  token_print_remaining();
  printf("o-------------------\n");
}

/* This has been written for you.
 * Prints out the information at the top of each step
 */
static void print_step_header(int step) {
  printf("\n.-------------------\n");
  printf("| Program Step = %2d\n", step++);
}

/* This has been written for you.
 * Prints out the output value (print token) nicely
 */
static void print_step_output(int val) {
  printf("|-----Program Output\n");
  printf("| %d\n", val);
}

/* This has been written for you.
 * Prints out the information at the bottom of each step
 */
static void print_step_footer(Symtab *symtab, Stack_head *stack) {
  hash_print_symtab(symtab);
  stack_print(stack);
  token_print_remaining();
  printf("o-------------------\n");
}
