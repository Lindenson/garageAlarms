#pragma once
#include <Arduino.h>

class AsyncTelegram2;

// Telegram-side UX: command handling, inline menus, and the native command list.
namespace BotUI {

void begin(AsyncTelegram2 *bot);
void loop();

} // namespace BotUI
