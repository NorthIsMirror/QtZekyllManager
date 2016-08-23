#ifndef EXTERNAL_FUNCTIONS_H
#define EXTERNAL_FUNCTIONS_H

// From Git project, licensed under GPL v2
int levenshtein(const char *string1, const char *string2,
        int swap_penalty, int substitution_penalty,
        int insertion_penalty, int deletion_penalty);

#endif // EXTERNAL_FUNCTIONS_H
