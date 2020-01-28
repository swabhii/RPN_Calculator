/* Fill in your Name and GNumber in the following two comment fields
 * Name:Amish Papneja
 * GNumber:01211503
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "hash.h"

Symtab *hash_initialize() {
  /* Implement this function */
	Symtab *table_ref=malloc(sizeof(Symtab));//New Symtab creation
	if(table_ref==NULL)return NULL;
	Symbol ** table=malloc(sizeof(Symbol *)*HASH_TABLE_INITIAL);// Actual table which is array of pointers
	if(table==NULL)return NULL;
	int i=0;
	for(i=0;i<HASH_TABLE_INITIAL;i++)
	{//Assigning each index to NULL
		table[i]=NULL;
	}
	table_ref->table=table;//assigning table to the Symtab Instance
	table_ref->capacity=HASH_TABLE_INITIAL;//Default Capacity
	table_ref->size=0;// Default size=0 because no element present

      	return table_ref;
}

void hash_destroy(Symtab *symtab) {
	if(symtab==NULL)exit(-1);
	int i=0;
	Symbol * walker=malloc(sizeof(Symbol));
	if(walker==NULL)exit(-1);
	Symbol * deleter =malloc(sizeof(Symbol));
	if(deleter==NULL)exit(-1);
	for(i=0;i<symtab->capacity;i++)
	{//iterating over each index
		walker=(symtab->table)[i];//assigning each index to walker
		while(walker!=NULL)
		{//iterating walker.
			deleter=walker;//maintaining one less than walker
			walker=walker->next;//walker increment
			free(deleter);//free one previous than walker
		}
	}
	free(walker);
	free(symtab->table);
	free(symtab);
      	
}

int hash_get_capacity(Symtab *symtab) {
  /* Implement this function */
	if(symtab==NULL)return -1;
	return symtab->capacity;
  
}

int hash_get_size(Symtab *symtab) {
	if(symtab==NULL)return -1;
 	return symtab->size;
}

int hash_put(Symtab *symtab, char *var, int val) {
 	if(symtab==NULL)return -1;
	int flag=0;
	long hashCode=hash_code(var);
	int hashIndex=hashCode % (symtab->capacity);
	Symbol * walker=malloc(sizeof(Symbol));
	Symbol * trav=malloc(sizeof(Symbol));
	if(walker==NULL || trav==NULL)return -1;
	walker=(symtab->table)[hashIndex];//finding the right index
		while(walker!=NULL)
		{
			if(strcmp( walker->variable,var)==0)
			{//if found in hashtable. 
				walker->val=val;///Update the value
				flag=1;
			}
		walker=walker->next;
		}
	if(flag==0)
	{
		if( ((symtab->size *1.0)/(symtab->capacity*1.0)) >2.0 )
		{//load checking
			hash_rehash(symtab,(symtab->capacity)*2 );//rehash with double capacity
		}
		Symbol * node =symbol_create(var,val);//New symbol for insertion
		Symbol * head= (symtab->table)[hashIndex];//
		if(head==NULL)
		{//insertion if list is empty
			head=node;
		}
		else
		{//insertion at the end
			trav=head;
			while(trav->next!=NULL)
			{
				trav=trav->next;
			}
			trav->next=node;
		}
		(symtab->table)[hashIndex]=head;//updating head globally in the table
		free(walker);
		symtab->size=(symtab->size)+1;//SIZE INCREMENT
	}
       	return 0;
}
/* (IMPLEMENT THIS FUNCTION)
 * Gets the Symbol for a variable in the Hash Table.
* Implement this function */

Symbol * hash_get(Symtab *symtab, char *var)
{
	if(symtab==NULL)return NULL;
	long hashCode=hash_code(var);
	int hashIndex=hashCode % (symtab->capacity);
	
	Symbol * walker =malloc(sizeof(Symbol));
	if(walker==NULL)return NULL ;
	walker=(symtab->table)[hashIndex];//assigning the right indexed pointer to walker
		while(walker!=NULL)
		{
			if(strcmp( walker->variable,var)==0)
			{//check for the varibale passed	
				Symbol * found=symbol_copy(walker);//creating a copy of Symbol structure
				return found;//returning it
			}
			walker=walker->next;
		}
      	return NULL;
}

void hash_rehash(Symtab *symtab, int new_capacity) {
  /* Implement this function */
	if(symtab==NULL)return;
	Symbol ** fresh_table=malloc(sizeof(Symbol *)*new_capacity);// new table with new capacity
	int i=0;
	for(i=0;i<new_capacity;i++)
	{//assigning all indexes to NULL
		fresh_table[i]=NULL;
	}
	int old_capacity=symtab->capacity;//holding the old capacity in a temp variable
	Symbol ** old_table=symtab->table;// holding the old table in a temp variable
	symtab->table=fresh_table;//assigning the new table to symtab instance
	symtab->capacity=new_capacity;//assigning the new capacity
	symtab->size=0;//size =zero because we will rehash everything

	Symbol * walker =malloc(sizeof(Symbol));
	if(walker==NULL)return ;
	Symbol * deleter =malloc(sizeof(Symbol));
	if(deleter==NULL)return ;
	for(i=0;i<old_capacity;i++)
	{//iterating the old table
		walker=old_table[i];
		while(walker!=NULL)
		{//iterating over every linked list.
			hash_put(symtab,walker->variable,walker->val);//putting new all values to updated table
			deleter=walker;//maintaining one less than walker
			walker=walker->next;//walker increment
			free(deleter);//free one less than walker
		}
	}
	free(old_table);
}

/* Implemented for you.
 * Provided function to print the symbol table */
void hash_print_symtab(Symtab *symtab) {
  if(symtab == NULL) {
    return;
  }
  printf("|-----Symbol Table [%d size/%d cap]\n", symtab->size, symtab->capacity);

  int i = 0;
  Symbol *walker = NULL;

  /* Iterate every index, looking for symbols to print */
  for(i = 0; i < symtab->capacity; i++) {
    walker = symtab->table[i];
    /* For each found linked list, print every symbol therein */
    while(walker != NULL) {
      printf("| %10s: %d \n", walker->variable, walker->val);
      walker = walker->next;
    }
  }
  return;
}

/* This function is written for you.
 * This computes the hash function for a String
 */
long hash_code(char *var) {
  long code = 0;
  int i;
  int size = strlen(var);

  for(i = 0; i < size; i++) {
    code = (code + var[i]);
    if(size == 1 || i < (size - 1)) {
      code *= 128;
    }
  }

  return code;
}
/* Fill in your Name and GNumber in the following two comment fields
 * Name:
 * GNumber:
 */
