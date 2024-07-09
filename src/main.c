#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <libnotify/notification.h>
#include <libnotify/notify.h>

#include "lateify.h"

static void cleanup_lateify_config(lateify_config_t *config) {
  free(config->notification_content);
  free(config->times);
}

static void send_notification(lateify_config_t *config) {
  NotifyNotification *notify =
      notify_notification_new("Lateify", config->notification_content, NULL);
  notify_notification_show(notify, NULL);
}

static uint32_t time_until_notification_seconds(notify_time_t *time_notify,
                                                const struct tm *time_now) {
  int32_t time_until = (time_notify->minute - time_now->tm_min) * 60 +
                       (time_notify->hour - time_now->tm_hour) * 60 * 60 -
                       time_now->tm_sec;
  if (time_until < 0) {
    time_until += 24 * 60 * 60;
  }
  return (uint32_t)time_until;
}

static void main_loop(lateify_config_t *config) {
  while (1) {
    time_t time_unix_now = time(NULL);
    struct tm *now = localtime(&time_unix_now);
    uint16_t wait_seconds =
        time_until_notification_seconds(&config->times[0], now);
    for (size_t i = 1; i < config->num_times; ++i) {
      notify_time_t *notify_time = &config->times[i];
      uint16_t new_wait_seconds =
          time_until_notification_seconds(notify_time, now);
      if (new_wait_seconds < wait_seconds) {
        wait_seconds = new_wait_seconds;
      }
    }
    sleep(wait_seconds);
    send_notification(config);
    // prevent notification spam
    sleep(60);
  }
}

int main(void) {
  gboolean ret = notify_init("lateify");
  if (ret == FALSE) {
    fprintf(stderr, "Error initializing notifications. Exiting now\n");
    return 1;
  }

  lateify_config_t config = {0};
  if (parse_config(&config)) {
    fprintf(stderr, "Error parsing config file. Exiting now\n");
    cleanup_lateify_config(&config);
    return 1;
  }

  if (config.num_times == 0) {
    fprintf(stdout, "Error: The amount of specified times in the config is 0. "
                    "Exiting now\n");
    cleanup_lateify_config(&config);
    return 1;
  }

  main_loop(&config);
  cleanup_lateify_config(&config);
  return 0;
}
