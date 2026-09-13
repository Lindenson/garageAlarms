#pragma once
#include <Arduino.h>

// Implemented in the .ino; declared here so BotUI can read live sensor state and request a
// reboot without the modules having to know about each other.
bool appAlarmActive();
bool appMotionActive();
void appRequestReboot();
