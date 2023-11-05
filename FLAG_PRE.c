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
    if (argc != 2) {
        printf("Error: only accepts one file the path to the FLAGS.h file");
        exit(1);
    }

    if (argv[1] == NULL) {
        printf("Error: NULL ptr given in first arg");
        exit(1);
    }

    FILE *fptr = fopen(argv[1], "r");

    if (fptr == NULL) {
        printf("Error: cannot find file specified \"%s\"", argv[1]);
        exit(1);
    }

    State state = SEARCHING;

    FILE *nfile = fopen("./../FLAGSTEMP.h", "w"); //this is very not good. just to escape the build folder

    if (nfile == NULL) {
        printf("Error: couldn't create temp file for writing");
        exit(1);
    }

    char buff[BUFF_SIZE];
    Enums enums = (Enums) {NULL, 0};
    while (fgets(buff, BUFF_SIZE, fptr) != NULL) {
        if (state != SEARCHING) {
            switch (state) {
                case FOUND_ENUM:
                    enums = readEnums(fptr, nfile, buff);
                    if (enums.enumNames == NULL) {
                        printf("Error: Reading enums returned NULL ptr");
                        exit(1);
                    }
                    break;
                case FOUND_HASH:
                    parseDefs(nfile, enums);
                    cleanup(fptr, nfile, buff);
                    break;
                case FOUND_SWITCH:
                    parseSwitch(nfile, enums);
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
            printf("Error: fputs returned EOF");
        }

        int define = startswith_ips(buff, ATOM__FLAG_PRE_DEF_START);
        int switcho = startswith_ips(buff, ATOM__FLAG_PRE_SWT_START);
        int enumo = startswith_ips(buff, ATOM__FLAG_PRE_ENUM_START);
        //shouldn't really do them all if one before is found but it's mostly going to not match any.
        if (enumo != -1) {
            state = FOUND_ENUM;
        }
        else if (define != -1) {
            state = FOUND_HASH;
        }
        else if (switcho != -1) {
            state = FOUND_SWITCH;
        }
    }

    fclose(fptr);
    fclose(nfile);

//    int ret = rename("./../FLAGSTEMP.h", "./../FLAGTEMP.h");

    return 0;
}

void cleanup(FILE* fptr, FILE* nfile, char* buff) {
    //seek the %%flag end%%

    while (fgets(buff, BUFF_SIZE, fptr) != NULL) {
        if (startswith_ips(buff, ATOM__FLAG_PRE_END) != -1) {
            fputs(buff, nfile);
            return;
        }
    }
}

void parseSwitch(FILE* nfile, Enums enums) {
    fputs("\tswitch (fi) {\n", nfile);
    for (int i = 0; i < enums.count; i++) {
        char* enumName = enums.enumNames[i];

        fprintf(nfile, "\t\tcase %s%s%s:\n"
                       "\t\t\t return %s%s;\n", ATOM__FLAG_PRE_FLAG_REG, enumName, "_HASH", ATOM__FLAG_PRE_FLAG_REG, enumName);
    }
    fprintf(nfile, "\t\tdefault:\n"
                   "\t\t\treturn -1;\n"
                   "\t}\n");
}

void parseDefs(FILE* nfile, Enums enums) {
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

        long long int hash = ATOM__FLAG_TO_INT(converted_name);

        char* buff = malloc(17); //it's in hex and 64 bit
        lltoa(hash, buff, 16);
        fprintf(nfile, "#define %s%s_HASH 0x%s\n", ATOM__FLAG_PRE_FLAG_REG, enumName, buff);

        free(buff);
    }
}

Enums readEnums(FILE* fptr, FILE* nfile, char buff[BUFF_SIZE]) {
    uint enumBuffSize = ENUM_BUFF_MIN_SIZE;
    uint enumBuffPos = 0;
    char** enumNames = malloc(enumBuffSize * sizeof (char *));

    fputs(buff, nfile); //for the enums there is a enum def header that need to be written
    while (fgets(buff, BUFF_SIZE, fptr) != NULL) {
        fputs(buff, nfile);

        //these should be similar to `    ATOM__FLAG_TOK_OUT,`
        if (startswith_ips(buff, ATOM__FLAG_PRE_END) != -1) {
            //found the enum count so return the gotten enums
            return (Enums) {enumNames, enumBuffPos};
        }

        int pos = startswith_ips(buff, ATOM__FLAG_PRE_FLAG_REG);
        if (pos == -1) {
            continue;
        }

        uint l2comma = 0; //length from start to comma
        while (buff[pos + l2comma] != '\0' && buff[pos + l2comma++] != ',');

        char* flagname = malloc(l2comma); //includes \0
        memcpy(flagname, &buff[pos], l2comma - 1);
        flagname[l2comma - 1] = '\0';

        printf("This is the length %d of %s\n", l2comma, flagname);

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