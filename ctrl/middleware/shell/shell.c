/*
 * shell.c
 *
 *  Created on: May 25, 2020
 *      Author: kychu
 */

#include "shell.h"

#include <string.h>

#if SHELL_DEBUG_LOG_ENABLE
#include "log.h"

static const char *TAG = "SHELL";
#endif /* SHELL_DEBUG_LOG_ENABLE */

struct cmd_item_t {
  const shell_cmd_t *cmd;
  struct cmd_item_t *next;
} ;

/** linked list of command structures */
static struct cmd_item_t *s_cmd_list = NULL;

static const shell_cmd_t *find_command_by_name(const char *name);

status_t shell_cmd_register(const shell_cmd_t *cmd)
{
  struct cmd_item_t *p = s_cmd_list;
  struct cmd_item_t *item = NULL;
  if(!cmd || cmd->command == NULL) {
    return status_inv_arg;
  }
  if (strchr(cmd->command, ' ') != NULL) {
    return status_inv_arg;
  }
  if ((shell_cmd_t *)find_command_by_name(cmd->command) == NULL) {
    item = kmm_alloc(sizeof(struct cmd_item_t));
    if(item == NULL) {
      return status_nomem;
    }
    item->cmd = cmd;
    item->next = NULL;
    if(p == NULL) {
      s_cmd_list = item;
    } else {
      for(; p->next != NULL; p = p->next) {}
      p->next = item;
    }
#if SHELL_DEBUG_LOG_ENABLE
    ky_info(TAG, "item: 0x%x, 0x%x", item, s_cmd_list);
    ky_info(TAG, "item-> cmd: 0x%x, %s", item->cmd, item->cmd->command);
    ky_info(TAG, "list-> cmd: 0x%x, %s", s_cmd_list->cmd, s_cmd_list->cmd->command);
#endif /* SHELL_DEBUG_LOG_ENABLE */
    return status_ok;
  }
  return status_error;
}

status_t shell_cmd_run(int argc, char **argv, int *cmd_ret)
{
  const shell_cmd_t *cmd = find_command_by_name(argv[0]);
  if (cmd == NULL) {
    return status_not_found;
  }
  *cmd_ret = (*cmd->func)(argc, argv);
  return status_ok;
}

static const shell_cmd_t *find_command_by_name(const char *name)
{
  struct cmd_item_t *p = s_cmd_list;
  int len = strlen(name);
#if SHELL_DEBUG_LOG_ENABLE
  ky_info(TAG, "&p: 0x%x, p->cmd: 0x%x, len = %d", p, p->cmd, len);
  ky_info(TAG, "p->cmd->name: %s", (*(p->cmd)).command);
#endif /* SHELL_DEBUG_LOG_ENABLE */
  for(; p != NULL; p = p->next) {
#if SHELL_DEBUG_LOG_ENABLE
    ky_info(TAG, "name: %s", p->cmd->command);
#endif /* SHELL_DEBUG_LOG_ENABLE */
    if(strlen(p->cmd->command) == len && strcmp(name, p->cmd->command) == 0) {
      return p->cmd;
    }
  }
  return NULL;
}
