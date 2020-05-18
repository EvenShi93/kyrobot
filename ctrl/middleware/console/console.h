/*
 * console.h
 *
 *  Created on: May 14, 2020
 *      Author: kychu
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include "SysConfig.h"

#define CONSOLE_LINE_CACHE_SIZE        128
#define CONSOLE_MAX_CMDLINE_ARGS       8

typedef struct {
  const char *prompt;
  uint32_t (* console_rx)(uint8_t *p, uint32_t l);
  status_t (* console_tx)(uint8_t *p, uint32_t l);
} console_cfg_t;

/**
 * @brief Console command main function
 * @param argc number of arguments
 * @param argv array with argc entries, each pointing to a zero-terminated string argument
 * @return console command return code, 0 indicates "success"
 */
typedef int (*console_cmd_func_t)(int argc, char **argv);

/**
 * @brief Console command description
 */
typedef struct {
    /**
     * Command name. Must not be NULL, must not contain spaces.
     * The pointer must be valid until the call to esp_console_deinit.
     */
    const char *command;
    /**
     * Help text for the command, shown by help command.
     * If set, the pointer must be valid until the call to esp_console_deinit.
     * If not set, the command will not be listed in 'help' output.
     */
    const char *help;
    /**
     * Hint text, usually lists possible arguments.
     * If set to NULL, and 'argtable' field is non-NULL, hint will be generated
     * automatically
     */
    const char *hint;
    /**
     * Pointer to a function which implements the command.
     */
    console_cmd_func_t func;
    /**
     * Array or structure of pointers to arg_xxx structures, may be NULL.
     * Used to generate hint text if 'hint' is set to NULL.
     * Array/structure which this field points to must end with an arg_end.
     * Only used for the duration of esp_console_cmd_register call.
     */
    void *argtable;
} console_cmd_t;

status_t console_start(console_cfg_t *cfg);
status_t console_cmd_register(const console_cmd_t *cmd);
status_t console_cmd_run(int argc, char **argv, int *cmd_ret);

/**
 * @brief Split command line into arguments in place
 * @verbatim
 * - This function finds whitespace-separated arguments in the given input line.
 *
 *     'abc def 1 20 .3' -> [ 'abc', 'def', '1', '20', '.3' ]
 *
 * - Argument which include spaces may be surrounded with quotes. In this case
 *   spaces are preserved and quotes are stripped.
 *
 *     'abc "123 456" def' -> [ 'abc', '123 456', 'def' ]
 *
 * - Escape sequences may be used to produce backslash, double quote, and space:
 *
 *     'a\ b\\c\"' -> [ 'a b\c"' ]
 * @endverbatim
 * @note Pointers to at most argv_size - 1 arguments are returned in argv array.
 *       The pointer after the last one (i.e. argv[argc]) is set to NULL.
 *
 * @param line pointer to buffer to parse; it is modified in place
 * @param argv array where the pointers to arguments are written
 * @param argv_size number of elements in argv_array (max. number of arguments)
 * @return number of arguments found (argc)
 */
size_t esp_console_split_argv(char *line, char **argv, size_t argv_size);

#endif /* CONSOLE_CONSOLE_H_ */
