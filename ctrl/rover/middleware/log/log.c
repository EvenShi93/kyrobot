/*
 * debug.c
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#include "log.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef DEBUG_ENABLE

static char *log_cache = NULL;
static log_put_t log_put_func = NULL;
static osMutexId logMutex = NULL;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: _alert, _err, _warn, and _info
 *
 * Description:
 *  If the cross-compiler's pre-processor does not support variable
 *  length arguments, then these additional APIs will be built.
 *
 ****************************************************************************/

status_t log_init(log_put_t ptx)
{
  log_cache = kmm_alloc(128);
  if(log_cache == NULL) return status_nomem;

  /* Create the mutex  */
  osMutexDef(LOGMutex);
  logMutex = osMutexCreate(osMutex(LOGMutex));
  if(logMutex == NULL) return status_error;

  if(ptx == NULL) return status_error;
  log_put_func = ptx;
  return status_ok;
}

status_t log_write(const char *format, ...)
{
  if(log_cache == NULL) return status_error;
  va_list ap;

  va_start(ap, format);
  osMutexWait(logMutex, osWaitForever);
  vsprintf(log_cache, format, ap);
  log_put_func((const char *)log_cache);
  osMutexRelease(logMutex);
  va_end(ap);
  return status_ok;
}

//print number of bytes per line for esp_log_buffer_char and esp_log_buffer_hex
#define BYTES_PER_LINE 16

//void esp_log_buffer_hexdump_internal( const char *tag, const void *buffer, uint16_t buff_len)
//{
//
//    if ( buff_len == 0 ) return;
//    char temp_buffer[BYTES_PER_LINE+3];   //for not-byte-accessible memory
//    const char *ptr_line;
//    //format: field[length]
//    // ADDR[10]+"   "+DATA_HEX[8*3]+" "+DATA_HEX[8*3]+"  |"+DATA_CHAR[8]+"|"
//    char hd_buffer[10+3+BYTES_PER_LINE*3+3+BYTES_PER_LINE+1+1];
//    char *ptr_hd;
//    int bytes_cur_line;
//
//    do {
//        if ( buff_len > BYTES_PER_LINE ) {
//            bytes_cur_line = BYTES_PER_LINE;
//        } else {
//            bytes_cur_line = buff_len;
//        }
//        if ( !esp_ptr_byte_accessible(buffer) ) {
//            //use memcpy to get around alignment issue
//            memcpy( temp_buffer, buffer, (bytes_cur_line+3)/4*4 );
//            ptr_line = temp_buffer;
//        } else {
//            ptr_line = buffer;
//        }
//        ptr_hd = hd_buffer;
//
//        ptr_hd += sprintf( ptr_hd, "%p ", buffer );
//        for( int i = 0; i < BYTES_PER_LINE; i ++ ) {
//            if ( (i&7)==0 ) {
//                ptr_hd += sprintf( ptr_hd, " " );
//            }
//            if ( i < bytes_cur_line ) {
//                ptr_hd += sprintf( ptr_hd, " %02x", ptr_line[i] );
//            } else {
//                ptr_hd += sprintf( ptr_hd, "   " );
//            }
//        }
//        ptr_hd += sprintf( ptr_hd, "  |" );
//        for( int i = 0; i < bytes_cur_line; i ++ ) {
//            if ( isprint((int)ptr_line[i]) ) {
//                ptr_hd += sprintf( ptr_hd, "%c", ptr_line[i] );
//            } else {
//                ptr_hd += sprintf( ptr_hd, "." );
//            }
//        }
//        ptr_hd += sprintf( ptr_hd, "|" );
//
////        ESP_LOG_LEVEL( log_level, tag, "%s", hd_buffer );
//        log_write();
//        buffer += bytes_cur_line;
//        buff_len -= bytes_cur_line;
//    } while( buff_len );
//}

uint32_t log_timestamp(void)
{
  if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
    return 0;
  }

  return xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
}

#endif /* DEBUG_ENABLE */
