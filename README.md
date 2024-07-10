# Lateify
A simple program that will send you notifications with a configurable text at configurable times. (originally to remind you to go to sleep when it's late).

The functionality could also be implemented via a cronjob or similar but I decided to write this program so I don't have to mess with passing the environment variables properly.
To build this project just build it via meson. It requires the libnotify library for building and running.

# Config
The program looks for it's config file at `$HOME/.config/lateify/config`. It is line based where the line starts with the config name and then a equals sign after which the value is specified.
The config keys are
- `Message` to set the notification content (not necessarily needed)
- `Time` to set the times where you want to get a notification. It's a space separated list of times in the 24h format. The hour can be in the range between 0 and 23 and the minute needs to be between 0 and 59. Both are inclusive

Here is an example of a valid config file:
```
Message=Consider going to sleep
Time=23:45 00:00 00:15 01:00
```

When an error occurs while parsing it will be logged to the stderr output. Depending on the severity of the error program might stop.
