//
// Created by jamescoward on 04/11/2023.
//

#include "FLAG_PRE.h"

//a preprocessor for creating the flag hashes and index in the FLAGS.h file

/* STEPS
 *  Find the ATOM__FLAGS enum to get a list of the enums to use
 *
 *  Find the %%FLAG%% and remove everything to %FLAG END%
 *  Add in the #defines for the hashes of the enum list
 *
 *  Find the next %%FLAG%% to %%FLAG END%% and remove
 *  Create a new switch case statement for the hash->index
 */

int main(int argc, char** argv) {
    char* flags_path = "./../FLAGS.h";

    if (argc > 2) {
        printf("Error: only one argument; file path to FLAGS.h\n");
        exit(1);
    }

    if (argc == 2) {
        if (argv[1] == NULL) {
            printf("Error: NULL ptr given in first arg\n");
            exit(1);
        }

        flags_path = argv[1];
    }

    FILE *fptr = fopen(flags_path, "r");

    if (fptr == NULL) {
        printf("Error: cannot find file specified \"%s\"\n", argv[1]);
        exit(1);
    }

    char* dir = get_dir(flags_path);

    State state = SEARCHING;

    char* dir_cpy = malloc(len(dir));
    strcpy(dir_cpy, dir);

    char* flag_temp_loc = strcat(dir_cpy, "/FLAGSTEMP.h");
    char* flag_main_loc = strcat(dir, "/FLAGS.h");

    FILE *nfile = fopen(flag_temp_loc, "w"); //this is very not good. just to escape the build folder

    if (nfile == NULL) {
        printf("Error: couldn't create temp file for writing\n");
        exit(1);
    }

    char buff[BUFF_SIZE];
    Enums enums = (Enums) {NULL, 0};
    while (fgets(buff, BUFF_SIZE, fptr) != NULL) {
        if (state != SEARCHING) {
            switch (state) {
                case FOUND_FLAG_ENUM:
                    enums = readEnums(fptr, nfile, buff, ATOM_CT__FLAG_PRE_FLAG_REG);
                    if (enums.enumNames == NULL) {
                        printf("Error: Reading enums returned NULL ptr\n");
                        exit(1);
                    }
                    break;
                case FOUND_OPTION_ENUM:
                    enums = readEnums(fptr, nfile, buff, ATOM_CT__FLAG_PRE_OPT_REG);
                    if (enums.enumNames == NULL) {
                        printf("Error: Reading enums returned NULL ptr\n");
                        exit(1);
                    }
                    break;
                case FOUND_STRINGS:
                    parseStrings(nfile, enums, ATOM_CT__FLAG_PRE_FLAG_REG);
                    cleanup(fptr, nfile, buff);
                    break;
                case FOUND_F_HASH:
                    parseDefs(nfile, enums, ATOM_CT__FLAG_PRE_FLAG_REG);
                    cleanup(fptr, nfile, buff);
                    break;
                case FOUND_O_HASH:
                    parseDefs(nfile, enums, ATOM_CT__FLAG_PRE_OPT_REG);
                    cleanup(fptr, nfile, buff);
                    break;
                case FOUND_IDX_SWITCH:
                    parseIdxSwitch(nfile, enums);
                    cleanup(fptr, nfile, buff);
                    break;
                case FOUND_STR_SWITCH:
                    parseStrSwitch(nfile, enums);
                    cleanup(fptr, nfile, buff);
                    break;
                default:
                    break;
            }
            state = SEARCHING;
            continue;
        }

        int code = fputs(buff, nfile);
        if (code == EOF) {
            printf("Error: fputs returned EOF\n");
        }

        int fdefine = startswith_ips(buff, ATOM_CT__FLAG_PRE_DEF_START);
        int fswitchidx = startswith_ips(buff, ATOM_CT__FLAG_PRE_IDX_SWT_START);
        int fswitchstr = startswith_ips(buff, ATOM_CT__FLAG_PRE_STR_SWT_START);
        int fenum = startswith_ips(buff, ATOM_CT__FLAG_PRE_ENUM_START);
        int fstrings = startswith_ips(buff, ATOM_CT__FLAG_PRE_STRING_START);
        int oenum = startswith_ips(buff, ATOM_CT__FLAG_PRE_OPT_ENUM_START);
        int odefine = startswith_ips(buff, ATOM_CT__FLAG_PRE_OPT_DEF_START);
        //shouldn't really do them all if one before is found but it's mostly going to not match any.

        //[[TODO]] FIX THIS!
        if (fenum != -1) state = FOUND_FLAG_ENUM;
        else if (fdefine != -1) state = FOUND_F_HASH;
        else if (fswitchidx != -1) state = FOUND_IDX_SWITCH;
        else if (oenum != -1) state = FOUND_OPTION_ENUM;
        else if (fstrings != -1) state = FOUND_STRINGS;
        else if (odefine != -1) state = FOUND_O_HASH;
        else if (fswitchstr != -1) state = FOUND_STR_SWITCH;
    }

    fclose(fptr);
    fclose(nfile);

    int ret = remove(flag_main_loc);

    if (ret != 0) {
        printf("Error: Failed to remove old \"%s\"\n", flag_main_loc);
        exit(1);
    }

    ret = rename(flag_temp_loc, flag_main_loc);

    if (ret != 0) {
        printf("Error: Failed to overwrite old \"%s\"\n", flag_main_loc);
        exit(1);
    }

    printf("Success! File can be found here: \"%s\"\n", flag_main_loc);

    return 0;
}

void cleanup(FILE* fptr, FILE* nfile, char* buff) {
    //seek the %%flag end%%
    //The buffer should still have the stat
    do {
        if (startswith_ips(buff, ATOM_CT__FLAG_PRE_END) != -1) {
            fputs(buff, nfile);
            return;
        }
    } while (fgets(buff, BUFF_SIZE, fptr) != NULL);
}

void parseStrings(FILE* nfile, Enums enums, char* prefix) {
    for (int i = 0; i < enums.count; i++) {
        char* enumName = enums.enumNames[i];

        fprintf(nfile, "#define %s%s%s \"%s\"\n", prefix, enumName, "_STR", enumName);
    }
}

void parseIdxSwitch(FILE* nfile, Enums enums) {
    fputs("\tswitch (fi) {\n", nfile);
    for (int i = 0; i < enums.count; i++) {
        char* enumName = enums.enumNames[i];

        fprintf(nfile, "\t\tcase %s%s%s:\n"
                       "\t\t\treturn %s%s;\n", ATOM_CT__FLAG_PRE_FLAG_REG, enumName, "_HASH", ATOM_CT__FLAG_PRE_FLAG_REG, enumName);
    }
    fprintf(nfile, "\t\tdefault:\n"
                   "\t\t\treturn -1;\n"
                   "\t}\n");
}

void parseStrSwitch(FILE* nfile, Enums enums) {
    fputs("\tswitch(index) {\n", nfile);
    for (int i = 0; i < enums.count; i++) {
        char *enumName = enums.enumNames[i];

        fprintf(nfile, "\t\tcase %s%s:\n"
                       "\t\t\treturn %s%s%s;\n", ATOM_CT__FLAG_PRE_FLAG_REG, enumName, ATOM_CT__FLAG_PRE_FLAG_REG,
                enumName, "_STR");
    }
    fprintf(nfile, "\t\tdefault:\n"
                   "\t\t\treturn \"ERROR NO ENUM NAME\";\n"
                   "\t}\n");
}

void parseDefs(FILE* nfile, Enums enums, char* prefix) {
    //write all the new defs

    if (enums.enumNames == NULL) {
        printf("Error: Found define flags before enum flags");
        exit(1);
    }

    for (int i = 0; i < enums.count; i++) {
        //need to replace `_` with `-`
        const char* enumName = enums.enumNames[i];

        uint nl = len(enumName);
        char* converted_name = malloc(nl * sizeof (char));

        if (converted_name == NULL) {
            printf("Error: malloc fail");
            exit(-1);
        }

        int pos = 0;
        while (enumName[pos] != '\0') {
            if (enumName[pos] == '_') {
                converted_name[pos] = '-';
            } else {
                converted_name[pos] = enumName[pos];
            }
            pos++;
        }
        converted_name[pos] = '\0';

        long long int hash = flag_to_int(converted_name);

        char* buff = malloc(17); //it's in hex and 64 bit
        sprintf(buff, "%llx", hash);
        fprintf(nfile, "#define %s%s_HASH 0x%s\n", prefix, enumName, buff);

        free(buff);
    }
}

Enums readEnums(FILE* fptr, FILE* nfile, char buff[BUFF_SIZE], char* enum_prefix) {
    uint enumBuffSize = ENUM_BUFF_MIN_SIZE;
    uint enumBuffPos = 0;
    char** enumNames = malloc(enumBuffSize * sizeof (char *));

    fputs(buff, nfile); //for the enums there is a enum def header that need to be written
    while (fgets(buff, BUFF_SIZE, fptr) != NULL) {
        fputs(buff, nfile);

        //these should be similar to `    ATOM__FLAG_TOK_OUT,`
        if (startswith_ips(buff, ATOM_CT__FLAG_PRE_END) != -1) {
            //found the enum count so return the gotten enums
            return (Enums) {enumNames, enumBuffPos};
        }

        int pos = startswith_ips(buff, enum_prefix);
        if (pos == -1) {
            continue;
        }

        uint l2comma = 0; //length from start to comma
        while (buff[pos + l2comma] != '\0' && buff[pos + l2comma++] != ',');

        char* flagname = malloc(l2comma); //includes \0
        memcpy(flagname, &buff[pos], l2comma - 1);
        flagname[l2comma - 1] = '\0';

        enumNames[enumBuffPos++] = flagname;
        if (enumBuffPos >= enumBuffSize) {
            void* ra = realloc(enumNames, ((int) (enumBuffSize * 1.5)) * sizeof(char*));
            if (ra == NULL) {
                free(enumNames);
                return (Enums) {NULL,-1};
            }
        }
    }
    return (Enums) {NULL,-1};
}