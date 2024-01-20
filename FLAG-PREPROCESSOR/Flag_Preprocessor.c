//
// Created by jamescoward on 09/11/2023.
//

#include "Flag_Preprocessor.h"

int main(int argc, char** argv) {
    puts("Welcome to the Flag preprocessor for ATOMIC\n");

    if (argc != 2) {
        puts(Error("Usage", ": ./Flag_Preprocessor <Dir of Flags.h & Flags.c>"));
        exit(1);
    }

    if (argv[1] == NULL) {
        puts(Error("Args", ": First cmd line arg is NULL"));
        exit(1);
    }

    uint dir_end = len(argv[1]);

    char* dir_buff = malloc((dir_end + 1 + 20) * sizeof(char)); //+20 for the /Flags.c/h and FlagsTemp.c/h
    memcpy(dir_buff, argv[1], dir_end + 1);

    char* hpath = get_path(dir_buff, path_sep_s "Flags.h");
    char* cpath = get_path(dir_buff, path_sep_s "Flags.c");

    char* nhpath = get_path(dir_buff, path_sep_s "FlagsTemp.h");
    char* ncpath = get_path(dir_buff, path_sep_s "FlagsTemp.c");

    FILE* hptr = fopen(hpath, "r");
    FILE* cptr = fopen(cpath, "r");

    if (hptr == NULL || cptr == NULL) {
        printf(Error("FILE", ": Unable to open Flags.h or Flags.c. Current Dir: %s"), dir_buff);
        exit(2);
    }

    FILE* nhptr = fopen(nhpath, "w");
    FILE* ncptr = fopen(ncpath, "w");

    if (nhptr == NULL || ncptr == NULL) {
        printf(Error("FILE", ": Unable to create temp files. Current Dir: %s"), dir_buff);
        exit(2);
    }

    charp_vec flag_enums = charp_vec_create(16);
    charp_vec option_enums = charp_vec_create(16);

    parse_h(hptr, nhptr, &flag_enums, &option_enums);
    parse_c(cptr, ncptr, &flag_enums);

    free_enums(&flag_enums);
    free_enums(&option_enums);

    //cleanup_write
    fclose(hptr);
    fclose(cptr);

    fclose(nhptr);
    fclose(ncptr);

    int remh = remove(hpath);
    int remc = remove(cpath);

    if (remh != 0 || remc != 0) {
        puts(Error("FileIO", ": Unable to remove file Flags.h/c\n"));
        exit(4);
    }

    int renh = rename(nhpath, hpath);
    int renc = rename(ncpath, cpath);

    if (renh != 0 || renc != 0) {
        puts(Error("FileIO", ": Unable to rename temp files to permanent\n"));
        exit(4);
    }

    free(dir_buff);
    free(cpath);
    free(hpath);
    free(nhpath);
    free(ncpath);

    charp_vec_destroy(&flag_enums);
    charp_vec_destroy(&option_enums);

    printf("SUCCESS! Files parsed and renamed");

    return 0;
}

void free_enums(charp_vec* enums) {
    for (uint i = 0; i < enums->pos; i++) {
        free(charp_vec_get(enums, i));
    }

    free(enums->arr);

    *enums = (charp_vec) {NULL, -1, -1};
}

void close_files(uint file_count, ...) {
    va_list vl;

    va_start(vl, file_count);

    for (uint i = 0; i < file_count; i++) {
        FILE* file = va_arg(vl, FILE*);

        if (!file) continue;

        fclose(file);
    }
}

void parse_h(FILE* hptr, FILE* nhptr, charp_vec* flag_enums, charp_vec* option_enums) {
    Buffer buffer = buffer_create(32);

    while (get_line(hptr, &buffer)) {
        if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_OPT_ENUM) != -1) {
            collect_enums(hptr, nhptr, ATOM_CT__FLAGS_PRE_OPT_START, &buffer, option_enums);
        }
        else if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_FLG_ENUM) != -1) {
            collect_enums(hptr, nhptr, ATOM_CT__FLAGS_PRE_FLG_START, &buffer, flag_enums);

            char* countEnum = charp_vec_pop(flag_enums); //remove the count
            free(countEnum);
        }
        else if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_OPT_DEF) != -1) {
            parse_def(hptr, nhptr, option_enums, ATOM_CT__FLAGS_PRE_OPT_START, &buffer);
        }
        else if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_FLG_DEF) != -1) {
            parse_def(hptr, nhptr, flag_enums, ATOM_CT__FLAGS_PRE_FLG_START, &buffer);
        }
        else if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_FLG_STR) != -1) {
            parse_string(hptr, nhptr, flag_enums, ATOM_CT__FLAGS_PRE_FLG_START, &buffer);
        }
        else if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_OPT_STR) != -1) {
            parse_string(hptr, nhptr, option_enums, ATOM_CT__FLAGS_PRE_OPT_START, &buffer);
        }
        else {
            fputs(buffer.data, nhptr);
        }
    }

    buffer_destroy(&buffer);
}

void parse_string(FILE* file, FILE* nfile, charp_vec* enums, const char* prefix, Buffer* buffer) {
    fputs(buffer->data, nfile); //write the //%%XXXX STRINGS%%

    for (uint i = 0; i < enums->pos; i++) {
        const char* current_enum = enums->arr[i];

        fprintf(nfile, "#define %s%s_STR \"%s\"\n", prefix, current_enum, current_enum);
    }

    cleanup_skip(file, nfile, buffer);
}

void parse_def(FILE* file, FILE* nfile, charp_vec* enums, const char* prefix, Buffer* buffer) {
    fputs(buffer->data, nfile); //write the //%%XXXX DEFINE%%

    for (uint i = 0; i < enums->pos; i++) {
        const char* current_enum = enums->arr[i];
        char* current_enum_dashed = str_cpy_replace(current_enum, '_', '-');

        fprintf(nfile, "#define %s%s_HASH 0x%llx\n", prefix, current_enum, flag_to_int(current_enum_dashed));

        free(current_enum_dashed);
    }

    cleanup_skip(file, nfile, buffer);
}

void parse_c(FILE* cptr, FILE* ncptr, charp_vec* flag_enums) {
    Buffer buffer = buffer_create(32);
    while (get_line(cptr,&buffer)) {
        if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_FLG_IDX_SWT) != -1) {
            parse_switch(cptr, ncptr, ATOM_CT__FLAGS_PRE_FLG_START, "", "_HASH", "-1", &buffer, flag_enums);
        }
        else if (starts_with_ips(buffer.data, ATOM_CT__FLAGS_PRE_FLG_STR_SWT) != -1) {
            parse_switch(cptr, ncptr, ATOM_CT__FLAGS_PRE_FLG_START, "_STR", "", "\"ERROR NO ENUM NAME\"", &buffer, flag_enums);
        }
        else {
            fputs(buffer.data, ncptr);
        }
    }

    buffer_destroy(&buffer);
}

void parse_switch(FILE* file, FILE* nfile, const char* prefix,
                  const char* to, const char* from, const char* default_value,
                  Buffer* buffer, charp_vec* flag_enums) {

    fputs(buffer->data, nfile); //write the %%XX%%

    for (uint i = 0; i < flag_enums->pos; i++) {
        const char* flag_name = flag_enums->arr[i];

        fprintf(nfile, "\t\tcase %s%s%s:\n"
                       "\t\t\treturn %s%s%s;\n", prefix, flag_name, from, prefix, flag_name, to);
    }
    fprintf(nfile, "\t\tdefault:\n"
                   "\t\t\treturn %s;\n", default_value);

    cleanup_skip(file, nfile, buffer);
}

/*
* Find the next %%END%% while writing to the newfile
*/
void cleanup_write(FILE* file, FILE* nfile, Buffer* buffer) {
    fputs(buffer->data, nfile);

    while (get_line(file, buffer)) {
        fputs(buffer->data, nfile);
        if (starts_with_ips(buffer->data, ATOM_CT__FLAGS_PRE_END)) {
            break;
        }
    }
}

/*
 * Find the next %%END%% skip writing to the nfile bar the end phrase
 */
void cleanup_skip(FILE* file, FILE* nfile, Buffer* buffer) {
    while (get_line(file, buffer)) {
        if (starts_with_ips(buffer->data, ATOM_CT__FLAGS_PRE_END) != -1) {
            break;
        }
    }
    fputs(buffer->data, nfile); //write the `//%%END%%`
}

void collect_enums(FILE* file, FILE* nfile, const char* prefix, Buffer* buffer, charp_vec* enum_vec) {
    //We're at the //%%XXXX ENUM%% label so write this to the file
    fputs(buffer->data, nfile);

    get_line(file, buffer);
    if (starts_with_ips(buffer->data, "enum") == -1) {
        puts(Error("FLAG.c", ": enum not found after //%%XXXX ENUM%%"));
    }
    fputs(buffer->data, nfile);

    while (get_line(file, buffer)) {
        int pos = starts_with_ips(buffer->data, prefix);
        if (pos == -1) {
            break;
        }

        fputs(buffer->data, nfile);

        uint tot_length = len_from_to(buffer->data, pos, ',') + 1;

        char* s_enum = malloc(tot_length * sizeof(char));
        memcpy(s_enum, &buffer->data[pos], tot_length);
        s_enum[tot_length - 1] = '\0';

        charp_vec_add(enum_vec, s_enum);
    }
    cleanup_write(file, nfile, buffer);
}
