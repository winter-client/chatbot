/*
 * INF1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "chat1002.h"

extern CORTANA_node* headptr_what;
extern CORTANA_node* headptr_where;
extern CORTANA_node* headptr_who;

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */

const char* chatbot_botname()
{
	return "CORTANA";
}

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */

const char* chatbot_username()
{
	return "User";
}

/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */

void remove_char(char* str, char charToRemove)
{
	int len = strlen(str);
	int i, j;
	for (i = 0; i < len; i++)
	{
		if (str[i] == charToRemove)
		{
			for (j = i; j < len; j++)
			{
				str[j] = str[j + 1];
			}
			len--;
			i--;
		}
	}
}

/* This function will validate whether the user enters a y/n it will re-ask if it detects anything outside of y/n. */
void userinput_ask(char* new_input, int n)
{
tryagain:
	fgets(new_input, n, stdin);
	size_t len = strlen(new_input);
	if (new_input[len - 1] != '\n')
	{
		printf("Enter only y/n! (y/n)\n");
		fseek(stdin, 0, SEEK_END); //clears the input buffer
		goto tryagain;
	}
	else
	{
		while (!(tolower(new_input[0]) == 'y' || tolower(new_input[0]) == 'n') || (tolower(new_input[0]) == ' '))
		{
			printf("Enter Y/N only! (y/n)\n");
			userinput_ask(new_input, n);
		}
	}
}


char yesno[4];
char save_yn[4];
char new_filename[255];
int checker_exit = 0;
int checker_reset = 0;

/*
 * Get a response to user input.
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */

int chatbot_main(int inc, char* inv[], char* response, int n)
{
	checker_exit = 0;
	checker_reset = 0;
	/* look for an intent and invoke the corresponding do_* function */

	// Check if the userinput is exiting the program
	if (chatbot_is_exit(inv[0]))
	{
		/* Validate whether user wants to save the knowledege of the chatbot before exiting.  */
		checker_exit = 1;
		printf("Save chat history before exiting? (y/n)\n");
		userinput_ask(yesno, 3);
		if (tolower(yesno[0]) == 'y')
		{
			printf("Name of file to save to: (eg. 'Notepad.ini')\n");
			fgets(new_filename, 255, stdin);
			fseek(stdin, 0, SEEK_END);		//to clear the input buffer
			remove_char(new_filename, '\n'); //remove \n from input
			while (strlen(new_filename) == 0)
			{
				printf("No filepath detected!\n");
				// Ask for a file path and save to that file
				printf("Enter filepath to save to: (eg. 'test.txt')\n");
				fgets(new_filename, 255, stdin);
				remove_char(new_filename, '\n'); //remove \n from input
			}
			//replace '\0' with '_'
			int qq = 0;
			while (new_filename[qq])
			{
				if ((isspace(new_filename[qq])) > 0) //if white-space character, change to underscore
				{
					new_filename[qq] = '_';
				}
				qq++;
			}
			inv[1] = new_filename;
			return chatbot_do_save(inc, inv, response, n);
		}
		else
			return chatbot_do_exit(inc, inv, response, n);
	}
	else if (chatbot_is_load(inv[0]))
		// Check if the user want to load a knowledgde file
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		// Check if the user input is a question
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
	{
		// Check if the chatbot user input is reset,
		// checker_reset variable have been implemented to prevent the chatbot from resetting itself.
		checker_reset = 1;
		return chatbot_do_reset(inc, inv, response, n);
	}
	else if (chatbot_is_update(inv[0]))
		// Check if the chatbot is update,
		return chatbot_do_update(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
	{
		// Check if the chatbot user input is save,
		/* Validate if there is no file path being input by the user.  */
		if (inv[1] == NULL)
		{
			printf("No filepath detected!'\n");
			// Ask for a file path and save to that file
			printf("Enter filepath to save to: (eg. 'test.txt')\n");
			fgets(new_filename, 255, stdin);
			remove_char(new_filename, '\n');
			while (strlen(new_filename) == 0)
			{
				printf("No filepath detected!\n");
				// Ask for a file path and save to that file
				printf("Enter filepath to save to: (eg. 'test.txt')\n");
				fgets(new_filename, 255, stdin);
				//remove \n from input
				remove_char(new_filename, '\n');
			}
			//to clear the input buffer
			fseek(stdin, 0, SEEK_END);
			//replace '\0' with '_'
			int qq = 0;
			while (new_filename[qq])
			{
				if ((isspace(new_filename[qq])) > 0) //if white-space character, we change to underscore
				{
					new_filename[qq] = '_';
				}
				qq++;
			}
			inv[1] = new_filename;
			return chatbot_do_save(inc, inv, response, n);
		}
		/*Confirmation to save*/
		printf("Save chat history? (y/n)\n");
		/* Validate user input.  */
		userinput_ask(save_yn, 3);
		if (tolower(save_yn[0]) == 'y')
		{
			return chatbot_do_save(inc, inv, response, n);
		}
		else
			return 0;
	}
	else
	{
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}
}

/*
* Determine whether an intent is to LOAD.
* Returns:
*  1, if the intent is "load"
*  0, otherwise
*/

int chatbot_is_load(const char* intent)
{
	return compare_token(intent, "load") == 0;
}

/*
 * Load a chatbot's knowledge base from a file.
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */

int chatbot_do_load(int inc, char* inv[], char* response, int n)
{
	/*
		fp:		The file pointer.
		ctr:	The number of successful results retrieved from the file.
	*/

	if (inv[1] != NULL)
	{
		FILE* fp;
		int ctr = 0;
		char file_path[MAX_INPUT];

		// Get the file path from the user input.
		if (compare_token(inv[1], "from") == 0)
		{
			// LOAD[0] from[1] /path/to/file[2]
			strcpy(file_path, inv[2]);
		}
		else
		{
			// LOAD[0] /path/to/file[1]
			strcpy(file_path, inv[1]);
		}

		// Open the file in read mode.
		fp = fopen(file_path, "r");

		if (fp != NULL)
		{
			// File exists.
			ctr = knowledge_read(fp);
			fclose(fp);
			snprintf(response, n, "Loaded %d results from the knowledge base [%s].", ctr, file_path);
		}
		else
		{
			// File does not exist.
			snprintf(response, n, "Unable to load knowledge base [%s].", file_path);
		}
	}
	else
	{
		snprintf(response, n, "File name is required.");
	}

	return 0;
}

/*
 * Determine whether an intent is a question.
 * Input = Intent
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */

int chatbot_is_question(const char* intent)
{
	return (
		compare_token(intent, "what") == 0 ||
		compare_token(intent, "where") == 0 ||
		compare_token(intent, "who") == 0);
}

/*
* Answer a question.
*
* inv[0] contains the the question word.
* inv[1] may contain "is" or "are"; if so, it is skipped.
* The remainder of the words form the entity.
* Returns:
*   0 (the chatbot always continues chatting after a question)
*/

int chatbot_do_question(int inc, char* inv[], char* response, int n)
{
	/*
		unsure:		A string for unsure questions.
		enti:		A string to store the entity.
		ans:		A string to store the answer to the question.
	*/
	char unsure[MAX_RESPONSE] = "I don't know. ";
	char enti[MAX_ENTITY] = "";
	char ans[MAX_RESPONSE] = "";

	size_t offset = 1;

	// Check where does the question start. If inv[1] is empty, if it is, re-enter question.
	if (inv[1] != NULL)
	{
		if (
			compare_token(inv[1], "is") == 0 ||
			compare_token(inv[1], "are") == 0)
		{
			offset = 2;
		}

		str_concat(enti, inv, inc, (MAX_ENTITY - 1), offset);

		if (knowledge_get(inv[0], enti, response, n) == KB_NOTFOUND)
		{
			str_concat(unsure, inv, inc, (MAX_RESPONSE - 1), 0);
			strcat(unsure, "?");

			prompt_user(ans, MAX_RESPONSE, "%s", unsure);
			knowledge_put(inv[0], enti, ans);
			snprintf(response, n, "Thank you.");
		}
	}
	else
	{
		snprintf(response, n, "Unable to comprehend, please rewrite.");
	}
	return 0;
}





/*
 * Determine whether an intent is a update.
 * Input = Intent
 * Returns:
 *  1, if the intent is update
 *  0, otherwise
 */
int chatbot_is_update(const char* intent)
{
	return compare_token(intent, "update") == 0;
}

/*
 * Responds to update.
 * Description: This function is to update an existing entity, if the entity does not exist, it will create a new entity.
 * Returns:
 *   0, if the chatbot continues chatting
 *   1, if the chatbot stop chatting (e.g. the conversation was "goodbye" etc.)
 */
int chatbot_do_update(int inc, char* inv[], char* response, int n)
{
	if (inv[1] != NULL && inv[2] != NULL && (strcmp(inv[1], "where") == 0 || strcmp(inv[1], "what") == 0 || strcmp(inv[1], "how") == 0))
	{
		char NewMeaning[MAX_RESPONSE] = "Enter the new meaning.";
		char enti[MAX_ENTITY] = "";
		char ans[MAX_RESPONSE] = "";
		size_t offset = 1;

		// Check where does the question start.
		if (

			compare_token(inv[2], "is") == 0 ||
			compare_token(inv[2], "are") == 0)
		{
			offset = 3;
		}

		str_concat(enti, inv, inc, (MAX_ENTITY - 1), offset);
		knowledge_get(inv[1], enti, response, n);
		prompt_user(ans, MAX_RESPONSE, "%s", NewMeaning);
		knowledge_put(inv[1], enti, ans);
		snprintf(response, n, "Meaning updated.");
	}
	else
	{
		snprintf(response, n, "Sorry, what are you trying to update?");
	}
	return 0;
}

/*
* Determine whether an intent is to SAVE.
* Input = Intent
* Returns:
*  1, if the intent is "what", "where", or "who"
*  0, otherwise
*/

int chatbot_is_save(const char* intent)
{
	return compare_token(intent, "save") == 0;
}

/*
* Save the chatbot's knowledge to a file.
* Input = Intent
* Returns:
*   0 (the chatbot always continues chatting after saving knowledge)
*/

int chatbot_do_save(int inc, char* inv[], char* response, int n)
{
	/*fp: The file pointer.*/
	FILE* fp;
	char file_path[MAX_INPUT];
	char full_name[MAX_INPUT];
	char underscore[] = "_";

	// Get the file path from the user input.
	if (
		compare_token(inv[1], "to") == 0 ||
		compare_token(inv[1], "as") == 0)
	{
		// Save[0] to[1] /path/to/file[2]
		// Save[0] as[1] /path/to/file[2]
		strcpy(full_name, inv[2]);

		for (int i = 3; i < inc; i++)
		{
			strcat(full_name, underscore);
			strcat(full_name, inv[i]);
		}
		strcpy(file_path, full_name);
	}
	else
	{
		// save[0] /path/to/file[1]
		strcpy(full_name, inv[1]);

		for (int i = 2; i < inc; i++)
		{
			strcat(full_name, underscore);
			strcat(full_name, inv[i]);
		}
		strcpy(file_path, full_name);
	}

	// Check if file exist, if not, create one.
	fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		fp = fopen(file_path, "w");
		knowledge_write(fp);
		fclose(fp);
		loading_bar();
		printf("\nResults saved to [%s].\n", file_path);
		if (checker_exit == 1)
		{
			printf("Exiting program now.\n");
			exit(0);
		}
		else
		{
			fseek(stdin, 0, SEEK_END); //to clear the input buffer
			return 0;
		}
	}
	else
	{
		fclose(fp);
		// Ask if user wants to overwrite the file.
		printf("File exist, overwrite? (y/n)\n");
		char overwrite[4];
		userinput_ask(overwrite, 5);
		if (tolower(overwrite[0]) == 'y')
		{
			loading_bar();
			fp = fopen(file_path, "w");
			knowledge_write(fp);
			fclose(fp);
			printf("Results overwritten to [%s].\n", file_path);
			if (checker_exit == 1)
			{
				printf("Exiting program now.\n");
				exit(0);
			}
			else
			{
				return 0;
			}
		}
		else
		{
			printf("Results not overwritten to [%s].\n", file_path);
			return 0;
		}
	}
	return 0;
}

/*
* Determine whether an intent is to RESET.
* Input = Intent
* Returns:
*  1, if the intent is "reset"
*  0, otherwise
*/

int chatbot_is_reset(const char* intent)
{
	return compare_token(intent, "reset") == 0;
}

/*
* Reset the chatbot.
* Input = Intent
* Returns:
*   0 (the chatbot always continues chatting after beign reset)
*/

int chatbot_do_reset(int inc, char* inv[], char* response, int n)
{
	// Reseed the random number generator.
	srand((unsigned)(time(NULL)));
	if (checker_reset == 1)
	{
		// Reset the knowledge base in memory.
		knowledge_reset();
		snprintf(response, n, "I have reset my knowledge for this session.");
	}
	else {
		snprintf(response, n, "Hello, welcome back :)");
	}
	return 0;
}

/*
 * Determine whether an intent is to EXIT.
 * Input = Intent
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */

int chatbot_is_exit(const char* intent)
{
	return (
		compare_token(intent, "exit") == 0 ||
		compare_token(intent, "quit") == 0);
}

/*
* Perform the EXIT intent.
* Input = Intent
* Returns:
*   0 (the chatbot always continues chatting after a question)
*/

int chatbot_do_exit(int inc, char* inv[], char* response, int n)
{
	// Free the allocated memory.
	knowledge_reset();
	snprintf(response, n, "Goodbye!");
	return 1;
}



