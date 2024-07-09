#ifndef LATEIFY_H
#define LATEIFY_H

#include <stdint.h>
#include <stdlib.h>

#define PARSER_BUFFER_SIZE 4096

typedef struct {
  uint8_t hour;
  uint8_t minute;
} notify_time_t;

typedef struct {
  notify_time_t *times;
  size_t num_times;
  char *notification_content;
} lateify_config_t;

int parse_config(lateify_config_t *config);

#endif
