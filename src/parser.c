#include "lateify.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static int parse_single_time(char *time, notify_time_t *res) {
  char *column = strchr(time, ':');
  if (!column) {
    return 1;
  }
  *column = '\0';
  errno = 0;
  unsigned long hour = strtoul(time, NULL, 10);
  if (hour > 24 || errno) {
    fprintf(stderr,
            "Error: Couldn't convert first part of time to a valid number");
    return 1;
  }
  unsigned long minute = strtoul(column + 1, NULL, 10);
  if (minute > 59 || errno) {
    fprintf(stderr,
            "Error: Couldn't convert second part of time to a valid number");
    return 1;
  }
  res->hour = hour;
  res->minute = minute;
  return 0;
}

static int parse_time(lateify_config_t *config, char *line) {
  size_t num = 0;
  for (char *s = line; *s != '\0'; ++s) {
    char c = *s;
    if (c == ':') {
      ++num;
    }
  }

  if (num == 0) {
    fprintf(stderr, "Error: No amount of times found\n");
    return 1;
  }

  config->times = malloc(sizeof(notify_time_t) * num);
  if (!config->times) {
    fprintf(stderr, "Error: Couldn't allocate memory for all time objects\n");
    return 1;
  }

  size_t i = 0;
  char *token = strtok(line, " ");
  while (token) {
    if (parse_single_time(token, &config->times[i])) {
      fprintf(stderr, "Error: Couldn't parse '%s' as valid time\n", token);
      return 1;
    }
    token = strtok(NULL, " ");
    ++i;
  }

  if (i != num) {
    fprintf(stderr, "Error: Wrong amount of times\n");
    return 1;
  }

  config->num_times = num;
  return 0;
}

int parse_config(lateify_config_t *config) {
  errno = 0;
  static char buffer[PARSER_BUFFER_SIZE];
  const char *HOME = getenv("HOME");
  if (snprintf(buffer, PARSER_BUFFER_SIZE, "%s/.config/lateify/config", HOME) >=
      PARSER_BUFFER_SIZE) {
    fprintf(stderr, "Error creating path for config file\n");
    return -1;
  }

  FILE *config_file = fopen(buffer, "r");

  if (!config_file) {
    const char *reason;
    if (errno == EACCES) {
      reason = "No access to config";
    } else if (errno == ENOENT) {
      reason = "No such file or directory";
    } else {
      reason = "Unknown";
    }

    fprintf(stderr, "Error opening config file. Reason: %s\n", reason);
    return -1;
  }

  while (fgets(buffer, PARSER_BUFFER_SIZE, config_file)) {
    size_t len = strlen(buffer);
    // remove newlines
    if (len >= 1) {
      buffer[len - 1] = '\0';
    }
    char *equals = strchr(buffer, '=');
    if (equals == NULL) {
      fprintf(
          stderr,
          "Error: Config file has this invalid line without equal sign: '%s'\n",
          buffer);
      continue;
    }

    size_t num_chars_before_equals = equals - buffer;
    char *after_equals = buffer + num_chars_before_equals + 1;

    if (strncmp(buffer, "Time", num_chars_before_equals) == 0) {
      if (config->times) {
        fprintf(stderr,
                "Error: Multiple Time lines found. Skipping the '%s' one\n",
                buffer);
        continue;
      }
      parse_time(config, after_equals);
    } else if (strncmp(buffer, "Message", num_chars_before_equals) == 0) {
      if (config->notification_content) {
        fprintf(stderr,
                "Error: Multiple Message lines found. Skipping the '%s' one\n",
                buffer);
        continue;
      }
      config->notification_content = strdup(after_equals);
    } else {
      fprintf(stderr,
              "Error: Config file has this invalid line: '%s'. Ignoring it\n",
              buffer);
      continue;
    }
  }

  fclose(config_file);
  return 0;
}
