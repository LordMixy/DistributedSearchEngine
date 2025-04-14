#include <stdio.h>
#include <stdlib.h>
#include "arena.h"
#include <assert.h>
#include <dirent.h>
#include "tokenizer.h"
#include "inverted_index.h"

#define DOCS_FOLDER "./books/"

void push_terms(char* docs_folder, inverted_index_t* inv_idx, arena_t* arena)
{
    DIR* dir = opendir(docs_folder);
    struct dirent* dirent;

    FILE* fp;
    token_t* tks;

    char path[FILENAME_MAX];
    while ((dirent = readdir(dir)) != NULL)
    {
        memset(path, 0, FILENAME_MAX);
        strncpy(path, docs_folder, FILENAME_MAX);
        strncat(path, dirent->d_name, FILENAME_MAX);
        path[strlen(path)] = 0;

        fp = fopen(path, "r");
        if (!fp) {
            fprintf(stderr, "%s: fopen failed\n", path);
            continue;
        }
        
        tks = get_tokens(fp, arena);
        for (token_t* node = tks; node != NULL; node = node->next) {
            inv_idx_ps_ins(
                &inv_idx, 
                node->buff,
                posting_init(42, 42, 42, arena), 
                arena
            );
        }

        fclose(fp);
    }

    closedir(dir);
}

int main()
{
	arena_t arena = arena_init();
    
    inverted_index_t* inv_idx = NULL;
    push_terms(DOCS_FOLDER, inv_idx, &arena);

    

	arena_free(&arena);
	return 0;
}
