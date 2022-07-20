#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#define MAX_WORD_LENGTH 100

void extract_pairs(const char *file_path, const char sep1, const char sep2, char *(**_pairs)[2], size_t *_pair_count)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Failed to open file %s: %s\n", file_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
 
    char *(*pairs)[2] = (char *(*)[2]) malloc(sizeof(char *[2]));
    if (pairs == NULL) {
        fprintf(stderr, "ERROR: failed to alloc memory: %s\n", strerror(errno));
        exit(1);
    }
    memset(pairs, 0, sizeof(*pairs));

    size_t pair_count = 0;

    char word[MAX_WORD_LENGTH] = {0};
    size_t index = 0;

    while(!feof(f)) {
        assert(index < MAX_WORD_LENGTH);

        fread(&word[index], 1, 1, f);

        if (ferror(f)) {
            fprintf(stderr, "ERROR: Failed to read from file %s: %s\n", file_path, strerror(ferror(f)));
            fclose(f);
            exit(1);
        }

        if (word[index] == sep1) {
            word[index++] = '\0';
            pairs[pair_count][0] = (char *) malloc (index);
            memcpy(pairs[pair_count][0], word, index);
            memset(word, 0, index);
            index = 0;
            continue;
        } else if (word[index] == sep2 || feof(f)) {
            word[index++] = '\0';
            pairs[pair_count][1] = (char *) malloc (index);
            memcpy(pairs[pair_count][1], word, index);
            memset(word, 0, index);
            index = 0;

            pair_count++;
            if (!feof(f))
                pairs = (char *(*)[2])reallocarray(pairs, pair_count + 1, sizeof(pairs[0]));
            continue;
        }
        index++;
    }
    *_pairs = pairs;
    *_pair_count = pair_count;

    fclose(f);
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    char *(*pairs)[2];
    size_t pair_count;

    extract_pairs("./hello", ' ', '\n', &pairs, &pair_count);

    for (size_t i = 0; i < pair_count; ++i) {
        printf("%s     \t%s\n", pairs[i][0], pairs[i][1]);
    }

    return 0;
}