/*
 * INF1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chat1002.h"
extern CORTANA_node* headptr_what;
extern CORTANA_node* headptr_where;
extern CORTANA_node* headptr_who;


/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */

int knowledge_get(const char* intent, const char* entity, char* response, int n) {
	CORTANA_node* search_intent;
	int search_result = KB_NOTFOUND;

	if (compare_token(intent, "what") == 0) {
		// Intent is what.
		search_intent = headptr_what;
	}
	else if (compare_token(intent, "where") == 0) {
		// Intent is where.
		search_intent = headptr_where;
	}
	else if (compare_token(intent, "who") == 0) {
		// Intent is who.
		search_intent = headptr_who;
	}
	else {
		// Invalid intent.
		return KB_INVALID;
	}

	while (search_intent != NULL) {
		if (compare_token(entity, search_intent->entity) == 0) {
			// Found response in knowledge base.
			snprintf(response, n, "%s", search_intent->response);
			search_result = KB_OK;
			break;
		}
		search_intent = search_intent->next;
	}

	return search_result;
}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_OK, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char* intent, const char* entity, const char* response) {
	int new_response;
	if (compare_token(intent, "what") == 0) {
		// Intent is what.
		new_response = knowledge_what_update(CORTANA_node_create(entity, response));
	}
	else if (compare_token(intent, "where") == 0) {
		// Intent is where.
		new_response = knowledge_where_update(CORTANA_node_create(entity, response));
	}
	else if (compare_token(intent, "who") == 0) {
		// Intent is who.
		new_response = knowledge_who_update(CORTANA_node_create(entity, response));
	}
	else {
		// Invalid intent.
		new_response = KB_INVALID;
	}

	return new_response;
}

/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */

int knowledge_read(FILE* f) {
	/*
		S_results:	The number of successful results retrieved from the file.
		i_results:	The result of inserting the entity-response into the knowledge base.
		c_lines:	The buffer to store content of the file.
	*/
	int S_results = 0, i_results;
	char c_lines[MAX_INPUT];
	char* f_intent = NULL;

	const size_t MAX_READLEN = (size_t)(MAX_ENTITY + MAX_RESPONSE);

	while (fgets(c_lines, MAX_READLEN, (FILE*)f)) {
		char* f_entity, * f_response;
		char* cr, * newline;

		// Read line by line.
		if (
			strcmp(c_lines, "\n") == 0 ||
			strcmp(c_lines, "\r\n") == 0
			) {
			// Empty line.
			continue;
		}
		else {
			cr = strchr(c_lines, '\r');
			newline = strchr(c_lines, '\n');

			if (cr != NULL) {
				// Carriage return found, replace it with null terminator.
				*cr = '\0';
			}
			else if (newline != NULL) {
				// newline found, replace it with null terminator.
				*newline = '\0';
			}
			else {
				// Clear any remaining input to prevent overflow.
				int c;
				while ((c = getchar()) != '\n' && c != EOF) {
					continue;
				}
			}

			if (
				strchr(c_lines, '[') != NULL &&
				strchr(c_lines, ']') != NULL
				) {
				// Square brackets found. Check intent.
				if (compare_token(c_lines, "[what]") == 0) {
					// Intent: what.
					f_intent = "what";
				}
				else if (compare_token(c_lines, "[where]") == 0) {
					// Intent: where.
					f_intent = "where";
				}
				else if (compare_token(c_lines, "[who]") == 0) {
					// Intent: who.
					f_intent = "who";
				}
				else {
					// Invalid intent.
					f_intent = NULL;
				}
			}
			else if (
				f_intent != NULL &&
				strchr(c_lines, '=') != NULL
				) {
				// Entity-Response pair line.
				f_entity = strtok(c_lines, "=");
				f_response = strtok(NULL, "=");
				i_results = knowledge_put(f_intent, f_entity, f_response);

				if (i_results == KB_OK) {
					// Increment the successful counter.
					S_results++;
				}
			}
		}
	}

	return S_results;
}

/*
 * Reset the knowledge base, removing all known entities from all intents.
 */

void knowledge_reset() {
	node_free(headptr_what);
	node_free(headptr_where);
	node_free(headptr_who);

	headptr_what = NULL;
	headptr_where = NULL;
	headptr_who = NULL;
}

/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */

void knowledge_write(FILE* f) {
	CORTANA_node* what = headptr_what;
	CORTANA_node* where = headptr_where;
	CORTANA_node* who = headptr_who;

	// Save "what" intent's entity-responses.
	fprintf(f, "[what]\n");
	while (what != NULL) {
		fprintf(f, "%s=%s\n", what->entity, what->response);
		what = what->next;
	}

	// Save "where" intent's entity-responses.
	fprintf(f, "\n[where]\n");
	while (where != NULL) {
		fprintf(f, "%s=%s\n", where->entity, where->response);
		where = where->next;
	}

	// Save "what" intent's entity-responses.
	fprintf(f, "\n[who]\n");
	while (who != NULL) {
		fprintf(f, "%s=%s\n", who->entity, who->response);
		who = who->next;
	}
}
