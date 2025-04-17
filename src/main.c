#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>

#include "arena.h"
#include "tokenizer.h"
#include "inverted_index.h"
#include "utils.h"

#define DOCS_FOLDER "./books/"

void process_file(char path[MAX_WORD_LEN], inverted_index_t** inv_idx, arena_t arena[static 1])
{
    FILE* fp;
    token_t* tks;

	// puts(path);
		
    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "%s: fopen failed\n", path);
        return;
    }

	token_pool_t* pool = NULL;   
    tks = get_tokens(fp, &pool, arena);
    
    for (token_t* node = tks; node != NULL; node = node->next) {
        inv_idx_ps_ins(
            inv_idx, 
            node->owner->term,
            posting_init(hash(path), node->owner->frequency, node->positions, arena), 
            arena
        );
    }

    fclose(fp);
}

void load_files(char* path, inverted_index_t** inv_idx,
    			  arena_t arena[static 1]) 
{
    DIR* dir = opendir(path);
    struct dirent* dirent;
	
	int i = 0;
	
    char complete_path[FILENAME_MAX + MAX_WORD_LEN];
    while ((dirent = readdir(dir)) != NULL) {
        if (strstr(dirent->d_name, ".txt")) {
			if (i == 100) break;
            memset(complete_path, 0, FILENAME_MAX + MAX_WORD_LEN);
            strncpy(complete_path, path, FILENAME_MAX);
            strncat(complete_path, dirent->d_name, MAX_WORD_LEN);
            complete_path[strlen(complete_path)] = 0;
            process_file(complete_path, inv_idx, arena);
			++i;
        }
    }
    
    closedir(dir);
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;
	
	arena_t arena = arena_init();
    
    inverted_index_t* inv_idx = NULL;
    load_files(DOCS_FOLDER, &inv_idx, &arena);

	// printf("argv[1]: %s\n", argv[1]);
 //    posting_t* head = get_postings(inv_idx, argv[1]);
	// while (head)
	// {
	// 	printf("id: %lu\n", head->doc_id);
	// 	printf("frequency: %lu\n", head->frequency);
	// 	head = head->next;
	// }
 		
	arena_free(&arena);
	return 0;
}
