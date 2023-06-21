/*
 * INF1002 (C Language) Group Project.
 *
 * This file contains the definitions and function prototypes for all of
 * features of the INF1002 chatbot.
 */

#ifndef _CHAT1002_H
#define _CHAT1002_H

#include <stdio.h>

 /* the maximum number of characters we expect in a line of input (including the terminating null)  */
#define MAX_INPUT    256

/* the maximum number of characters allowed in the name of an intent (including the terminating null)  */
#define MAX_INTENT   32

/* the maximum number of characters allowed in the name of an entity (including the terminating null)  */
#define MAX_ENTITY   64

/* the maximum number of characters allowed in a response (including the terminating null) */
#define MAX_RESPONSE 256

/* return codes for knowledge_get() and knowledge_put() */
#define KB_OK         0
#define KB_NOTFOUND  -1
#define KB_INVALID   -2
#define KB_NOMEM     -3


/* Intent-Entity-Response structure. */
typedef struct node_struct {
	char entity[MAX_ENTITY];
	char response[MAX_RESPONSE];
	struct node_struct* next; // Pointer to the next node in the linked list.
} CORTANA_node;

/* functions defined in main.c */
int compare_token(const char* token1, const char* token2);
void prompt_user(char* buffer, int n, const char* format, ...);

/* functions defined in chatbot.c */
const char* chatbot_botname();
const char* chatbot_username();
int chatbot_main(int inc, char* inv[], char* response, int n);
int chatbot_is_load(const char* intent);
int chatbot_do_load(int inc, char* inv[], char* response, int n);
int chatbot_is_question(const char* intent);
int chatbot_do_question(int inc, char* inv[], char* response, int n);
int chatbot_is_update(const char* intent);
int chatbot_do_update(int inc, char* inv[], char* response, int n);
int chatbot_is_save(const char* intent);
int chatbot_do_save(int inc, char* inv[], char* response, int n);
int chatbot_is_reset(const char* intent);
int chatbot_do_reset(int inc, char* inv[], char* response, int n);
int chatbot_is_exit(const char* intent);
int chatbot_do_exit(int inc, char* inv[], char* response, int n);

/* functions defined in knowledge.c */
int knowledge_get(const char* intent, const char* entity, char* response, int n);
int knowledge_put(const char* intent, const char* entity, const char* response);
int knowledge_read(FILE* f);
void knowledge_write(FILE* f);
void knowledge_reset();

/* functions defined in knowledge_data_structure.c */
void str_concat(char* ptr_dest, char* ptr_src[], size_t ptr_src_size, size_t n, int offset);
CORTANA_node* CORTANA_node_create(const char* entity, const char* resp);
void node_append(CORTANA_node* head, CORTANA_node* node);
void node_free(CORTANA_node* node);
int knowledge_what_update(CORTANA_node* new_node);
int knowledge_where_update(CORTANA_node* new_node);
int knowledge_who_update(CORTANA_node* new_node);

/* Initialize loading bar for the animation, this is for saving function */
void loading_bar();

/* Chatbot Linked lists, our chatbot named CORTANA. */
CORTANA_node* headptr_what;
CORTANA_node* headptr_where;
CORTANA_node* headptr_who;

#endif
