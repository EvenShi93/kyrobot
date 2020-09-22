/*
 * test.c
 *
 *  Created on: May 25, 2020
 *      Author: kychu
 */

#include "test.h"

#include "test_conf.h"

static int test_main(int argc, char *argv[]);

typedef void (*test_func_t)(void);

const shell_cmd_t test_cmd = {
  .command = "test",
  .help = "test",
  .hint = NULL,
  .func = &test_main,
};

#include <string.h>

#include "test_stdio.c"

static int test_main(int argc, char *argv[])
{
  if(argc < 2) return -1;
  if(!strcmp(argv[1], "stdio")) {
    test_stdio();
    return 0;
  }
  return -1;
}
