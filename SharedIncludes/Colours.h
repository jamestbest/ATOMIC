//
// Created by jamescoward on 09/11/2023.
//

#ifndef ATOMIC_COLOURS_H
#define ATOMIC_COLOURS_H

#define C_RST  "\x1B[0m"
#define C_RED  "\x1B[31m"
#define C_GRN  "\x1B[32m"
#define C_YLW  "\x1B[33m"
#define C_BLU  "\x1B[34m"
#define C_MGN  "\x1B[35m"
#define C_CYN  "\x1B[36m"
#define C_WHT  "\x1B[37m"

#define ERROR_BASE C_RED"ERROR"C_RST": "
#define WARN_BASE C_YLW"Warning"C_RST": "

#define INFO_BASE C_BLU"Info"C_RST": "
#define SUCC_BASE C_GRN"SUCCESS"C_RST": "

#endif //ATOMIC_COLOURS_H
