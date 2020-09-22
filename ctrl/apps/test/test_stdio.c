/*
 * test_stdio.c
 *
 *  Created on: May 25, 2020
 *      Author: kychu
 */

#include "test_conf.h"

#if TEST_CASE_STDIO_ENABLE

#include <stdio.h>
#include <errno.h>

#include "log.h"

void test_stdio(void)
{
  char str[10];
  printf("Hello World!\n");
  fprintf(stdout, "Hello \n");
  fprintf(stderr, "World!\n");
  memset(str, 0, 10);
  scanf("%s", str);
  ky_info("scanf", "%s", str);
  memset(str, 0, 10);
  fgets(str, 10, stdin);
  ky_info("fgets", "%s", str);
  errno = ENXIO;
  perror("test");
}

#endif /* TEST_CASE_STDIO_ENABLE */
