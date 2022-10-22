#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#else
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
#endif

#include "libclipboard.h"

#define MAX_WORD_LENGTH 100
#define CLIPBOARD_MAX_LENGTH (1024 * 2)

void extract_pairs(const char *file_path, const char sep1, const char sep2, char *(**_pairs)[2], size_t *_pair_count);

int main(int argc, char **argv)
{
// hide console if on windows
#ifdef _WIN32
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_MINIMIZE); // won't hide the window without SW_MINIMIZE
    ShowWindow(hWnd, SW_HIDE);
#endif

    char word_file[256] = "word_list.txt";

    if (argc == 2)
    {
        memset(word_file, 0, strlen(word_file));
        strcpy(word_file, argv[1]);
    }

    char *(*pairs)[2];
    size_t pair_count;

    // extract the pairs out the file
    extract_pairs(word_file, ';', '\n', &pairs, &pair_count);

    // clipboard object
    clipboard_c *cb = clipboard_new(NULL);
    if (cb == NULL)
    {
        fprintf(stderr, "ERROR: Failed to create the clipboard object\n");
        exit(1);
    }

    char last_cb_content[CLIPBOARD_MAX_LENGTH];

    while (1) {

        // different sleep functions dependent on os
        #ifndef _WIN32
                sleep(1);
        #else
                Sleep(1000);
        #endif

        int len = 0;
        // get the clipboard content
        char *cb_content = clipboard_text_ex(cb, &len, LCB_CLIPBOARD);

        // only search for matches if content changed since last iteration
        if (len == 0 || cb_content == NULL || strcmp(last_cb_content, cb_content) == 0) continue;

        size_t i;
        bool reverse = false;

        // look for matches in the first word of pairs
        for (i = 0; i < pair_count; i++)
            if (strstr(pairs[i][0], cb_content) != NULL) break;
        
        // didn't find any matching words in first word of pairs
        if (i == pair_count)
        {
            reverse = true;
            // check if second word of pairs is matching clipboard content
            for (i = 0; i < pair_count; i++)
                if (strstr(pairs[i][1], cb_content) != NULL) break;
        }

        // didn't find any matching word in second word of pairs either
        if (i == pair_count)
        {
            cb_content = (char *)realloc(cb_content, 5);
            if (cb_content == NULL) {
                fprintf(stderr, "ERROR: Failed to realloc memory!");
                exit(EXIT_FAILURE);
            }

            strcpy(cb_content, "none");
        }
        // found matching word in second word of pairs
        else if (reverse)
        {
            cb_content = (char *)realloc(cb_content, strlen(pairs[i][0]));
            if (cb_content == NULL) {
                fprintf(stderr, "ERROR: Failed to realloc memory!");
                exit(EXIT_FAILURE);
            }

            strcpy(cb_content, pairs[i][0]);
        }
        // found matching word in first word of pairs
        else
        {
            cb_content = (char *)realloc(cb_content, strlen(pairs[i][1]));
            if (cb_content == NULL) {
                fprintf(stderr, "ERROR: Failed to realloc memory!");
                exit(EXIT_FAILURE);
            }

            strcpy(cb_content, pairs[i][1]);
        }

        // copy the matching word to clipboard
        int res = clipboard_set_text_ex(cb, cb_content, strlen(cb_content), LCB_CLIPBOARD);
        if (res != true)
            fprintf(stderr, "ERROR: Failed to set clipboard text!\n");

        // update the last clipboard content with the current content
        strcpy(last_cb_content, cb_content);
    }

    clipboard_free(cb);
    for (size_t i = 0; i < pair_count; ++i)
    {
        free(pairs[i][0]);
        free(pairs[i][1]);
    }
    free(pairs);

    return 0;
}

void extract_pairs(const char *file_path, const char sep1, const char sep2, char *(**_pairs)[2], size_t *_pair_count)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "ERROR: Failed to open file %s: %s\n", file_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *(*pairs)[2] = (char *(*)[2])malloc(sizeof(char *[2]));
    if (pairs == NULL)
    {
        fprintf(stderr, "ERROR: failed to alloc memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(pairs, 0, sizeof(*pairs));

    size_t pair_count = 0;

    char word[MAX_WORD_LENGTH] = {0};
    char tmp [MAX_WORD_LENGTH] = {0};
    size_t index = 0;
    size_t tmp_len = 0;

    while (!feof(f))
    {
        assert(index < MAX_WORD_LENGTH);

        fread(&word[index], 1, 1, f);

        if (ferror(f))
        {
            fprintf(stderr, "ERROR: Failed to read from file %s: %s\n", file_path, strerror(ferror(f)));
            fclose(f);
            exit(EXIT_FAILURE);
        }

        if (word[index] == sep1)
        {
            word[index++] = '\0';
            memcpy(tmp, word, index);
            memset(word, 0, index);
            tmp_len = index;
            index = 0;
            continue;
        }
        else if (word[index] == sep2 || feof(f))
        {
            // check if first word exists
            if (tmp_len == 0) continue;

            // copy buffered first word into pair
            pairs[pair_count][0] = (char *)malloc(tmp_len);
            memcpy(pairs[pair_count][0], tmp, tmp_len);
            memset(tmp, 0, tmp_len);

            // copy second word into pair
            word[index++] = '\0';
            pairs[pair_count][1] = (char *)malloc(index);
            memcpy(pairs[pair_count][1], word, index);
            memset(word, 0, index);
            index = 0;
            tmp_len = 0;

            pair_count++;
            if (!feof(f))
                pairs = (char *(*)[2])realloc(pairs, (pair_count + 1) * sizeof(pairs[0]));
            continue;
        }
        index++;
    }
    *_pairs = pairs;
    *_pair_count = pair_count;

    fclose(f);
}
