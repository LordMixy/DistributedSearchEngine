#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>

#include "arena.h"
#include "tokenizer.h"
#include "inverted_index.h"
#include "utils.h"
#include "query_parser.h"

#define DOCS_FOLDER "./books/"

#define E_FILE_ERROR -1
#define NO_ERROR 0

int32_t process_file(char path[FILENAME_MAX + MAX_WORD_LEN], inverted_index_t** inv_idx, arena_t* arena)
{
    FILE* fp;	
    if (!(fp = fopen(path, "r"))) 
   		return E_FILE_ERROR;

	token_pool_t* pool = NULL;   
    token_t* tks = get_tokens(fp, &pool, arena);
    
    for (token_t* node = tks; node != NULL; node = node->next) {
        inv_idx_ps_ins(
            inv_idx, 
            node->owner->term,
            posting_init(hash(path), node->owner->frequency, node->positions, arena), 
            arena
        );
    }

    fclose(fp);
    return NO_ERROR;
}

void load_files(char* path, inverted_index_t** inv_idx, arena_t* arena) 
{
    DIR* dir = opendir(path);
    struct dirent* dirent;
	
	int j = 0;
    char complete_path[FILENAME_MAX + MAX_WORD_LEN];
    while ((dirent = readdir(dir)) != NULL) {
        if (strstr(dirent->d_name, ".txt")) {
            memset(complete_path, 0, FILENAME_MAX + MAX_WORD_LEN);
            strncpy(complete_path, path, FILENAME_MAX);
            strncat(complete_path, dirent->d_name, MAX_WORD_LEN);
            complete_path[strlen(complete_path)] = 0;
			++j;
            
            if (process_file(complete_path, inv_idx, arena) == E_FILE_ERROR)
            	perror("file error:");
        }
    }
    
    closedir(dir);
}

int main(void)
{
	arena_t arena = arena_init();
    inverted_index_t* inv_idx = NULL;
    
    // load_files(DOCS_FOLDER, &inv_idx, &arena);

	char buff[200];
	do {

		printf("> ");

		fgets(buff, sizeof buff, stdin);
		buff[strcspn(buff, "\n")] = 0;

		Q_lexer_t lex = {
			.buff = buff,
			.pos = 0	
		};

		Q_token_t tk = Q_next_token(&lex);
		while (tk.type != Q_TK_END) { 
			printf("%s -> %s\n", tk.lexeme, (char*[]){"AND","OR","NOT","PHRASE","RIGHT PAREN","LEFT PAREN","LOAD","NOT RECOGNIZED","END"}[tk.type]);
			
			if (tk.type == Q_TK_LOAD)
				load_files(DOCS_FOLDER, &inv_idx, &arena);			

			tk = Q_next_token(&lex);
		} 
		
	} while (strcmp(buff, "q"));
	 	
	arena_free(&arena);
	return EXIT_SUCCESS;
}
