#pragma once
#include <Arduino.h>

enum class EventType : uint8_t {
    None           = 0,
    AlarmSmoke     = 1,
    Motion         = 2,
    BootCold       = 3,
    BootUnexpected = 4,
    Heartbeat      = 5,
    Test           = 6,
    AlarmOngoing   = 7,   // smoke contact still closed; eventTs is when it started
    SensorStuck    = 8,   // motion input held active far longer than any real event
};

namespace Events {

// Render the Telegram HTML body for an event. `late` marks a message that sat in the queue
// across a reboot or an outage, so the reader knows the timestamp is not "just now".
String render(EventType type, int64_t ts, bool late);

// Short label for logs and /status.
const char *name(EventType type);

// Alarms ignore mute; informational events do not.
bool isCritical(EventType type);

} // namespace Events
