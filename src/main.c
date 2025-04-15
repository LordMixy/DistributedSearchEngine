#include <stdio.h>
#include <stdlib.h>
#include "arena.h"
#include <assert.h>
#include "tokenizer.h"
#include "inverted_index.h"

#ifdef __linux__
#include <dirent.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define DOCS_FOLDER "./books/"

typedef void (*file_processor)(char[FILENAME_MAX], inverted_index_t*, arena_t[static 1]);

void foreach_file(
    char* path, 
    inverted_index_t* inv_idx,
    arena_t arena[static 1],
    file_processor callback
) 
{
#ifdef __linux__
    DIR* dir = opendir(path);
    struct dirent* dirent;

    int i = 0;

    char complete_path[FILENAME_MAX];
    while ((dirent = readdir(dir)) != NULL) {
        if (i == 101) break;

        if (strstr(dirent->d_name, ".txt")) {
            memset(complete_path, 0, FILENAME_MAX);
            strncpy(complete_path, path, FILENAME_MAX);
            strncat(complete_path, dirent->d_name, FILENAME_MAX);
            complete_path[strlen(complete_path)] = 0;
            callback(complete_path, inv_idx, arena);
            ++i;
        }
    }
    
    closedir(dir);
#elif _WIN32
    WIN32_FIND_DATA file;
    HANDLE file_handle = NULL;

    if ((file_handle = FindFirstFile(path, &file)) == INVALID_HANDLE_VALUE) {
        // fprintf(stderr, "Could not find specified directory!\n");
        // exit(1);
        return;
    }

    do {
        if (file.cFileName[0] == '.') continue;
        callback(file.cFileName, inv_idx, arena);
    } while (FindNextFile(file_handle, &file));

    FindClose(file_handle);
#endif
}

void process_file(char path[256], inverted_index_t* inv_idx, arena_t arena[static 1])
{
    FILE* fp;
    token_t* tks;

    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "%s: fopen failed\n", path);
        return;
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

inverted_index_t* load_files(char* docs_folder, arena_t* arena)
{
    inverted_index_t* inv_idx = NULL;
    foreach_file(docs_folder, inv_idx, arena, process_file);
    return inv_idx;
}

int main()
{
	arena_t arena = arena_init();
    
    inverted_index_t* inv_idx = load_files(DOCS_FOLDER, &arena);
    (void) inv_idx;

	arena_free(&arena);
	return 0;
}
