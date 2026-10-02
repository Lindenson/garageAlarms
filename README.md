<p align="center">
  <img src="docs/assets/hero.svg" alt="garageAlarms: a distributed alarm network with no central hub" width="100%">
</p>

<p align="center">
  <img alt="ESP-IDF v6.0" src="https://img.shields.io/badge/ESP--IDF-v6.0-22d3ee?style=flat-square&labelColor=111a24">
  <img alt="ESP32-S3 + ESP32-H2" src="https://img.shields.io/badge/MCU-ESP32--S3%20%2B%20ESP32--H2-22d3ee?style=flat-square&labelColor=111a24">
  <img alt="Zigbee distributed" src="https://img.shields.io/badge/Zigbee-distributed-34d399?style=flat-square&labelColor=111a24">
  <img alt="FreeRTOS" src="https://img.shields.io/badge/RTOS-FreeRTOS-60a5fa?style=flat-square&labelColor=111a24">
  <img alt="Status: planning complete" src="https://img.shields.io/badge/status-planning%20complete-fbbf24?style=flat-square&labelColor=111a24">
  <img alt="License MIT" src="https://img.shields.io/badge/license-MIT-7f93a8?style=flat-square&labelColor=111a24">
  <img alt="UI language: ru" src="https://img.shields.io/badge/UI-ru-c084fc?style=flat-square&labelColor=111a24">
</p>

<p align="center">
  <b>Alarms for a garage and the rooms next to it</b> — smoke, water, motion, temperature —<br>
  delivered to Telegram (and, in 2.0, to Pushover and MQTT).
</p>

<p align="center">
  <a href="#why">Why</a> ·
  <a href="#concept-garagealarms-20">Concept</a> ·
  <a href="#how-an-alarm-survives-failures">Resilience</a> ·
  <a href="#screens">Screens</a> ·
  <a href="#features">Features</a> ·
  <a href="#architecture-at-a-glance">Architecture</a> ·
  <a href="#roadmap">Roadmap</a> ·
  <a href="#garagealarms-1x-current-firmware">1.x firmware</a>
</p>

---

This repository holds two things:

| | |
| --- | --- |
| 🟢 **garageAlarms 1.x** — *in service* | The firmware running in the garage today: one ESP32-S3, two dry-contact inputs, a Telegram bot. See [1.x firmware](#garagealarms-1x-current-firmware). |
| 🧭 **The plan for garageAlarms 2.0** — *planning complete* | A decentralized network of nodes that replaces the single box. Implementation starts in a new repository, `garageAlarms2.0`. See [Why](#why), [Concept](#concept-garagealarms-20) and [Roadmap](#roadmap). |

## Why

> [!IMPORTANT]
> It is an alarm system, so the priorities are, in order:
> **1. never miss or silently drop an event** › **2. never flood the user** › **3. everything else.**
> Every decision in both versions is resolved against that ordering.

1.x serves that ordering well within one box — but the box itself is the limit:

| 1.x limitation | Consequence | 2.0 answer |
| --- | --- | --- |
| 📶 One device, one WiFi link | a weak router signal or a dead box silences the garage | 2+ logic nodes, Zigbee mesh, UDP second path, leader failover |
| ✂️ Inputs are pulled toward "quiet" | a cut wire looks exactly like "nothing happening" | end-of-line resistors: a cut or shorted loop is a fault |
| 🔌 Two fixed inputs | no water, temperature, CO₂, or sensors in other rooms | collectors in every room, sensor driver gateway |
| 💬 Telegram only | an alarm at 3 a.m. on a silenced phone is easy to sleep through | Pushover emergency alerts, repeating until acknowledged |
| 🧱 Logic is compiled in | no "water in two rooms", "motion garage → corridor", modes like "away" | nine rule templates with `when`, modes, chains, traces |

> [!NOTE]
> **2.0 exists to remove the single point of failure and add shared logic across rooms — without
> giving up the ordering above.** Duplicates are acceptable; losses are not.

## Concept: garageAlarms 2.0

A network of nodes in adjacent rooms, **with no central hub**.

```mermaid
flowchart LR
  subgraph Rooms["Rooms: garage, corridor, basement"]
    C1(["Collector<br/>ESP32-H2"]) -- Zigbee mesh --- C2(["Collector<br/>ESP32-H2"])
    C1 -- Zigbee --- L1["Logic node 1<br/>ESP32-S3 + H2<br/>LEADER"]
    C2 -- Zigbee --- L2["Logic node 2<br/>ESP32-S3 + H2<br/>FOLLOWER"]
    L1 -. replication .- L2
  end
  L1 -- WiFi / UDP --- R{{Router}}
  L2 -- WiFi / UDP --- R
  R --- TG[Telegram]
  R --- PO[Pushover]
  R --- MQ[MQTT broker]

  classDef collector fill:#111a24,stroke:#7f93a8,color:#dce6f0
  classDef logic fill:#182433,stroke:#22d3ee,stroke-width:2px,color:#dce6f0
  classDef router fill:#111a24,stroke:#60a5fa,color:#dce6f0
  classDef ext fill:#0b1016,stroke:#34d399,color:#dce6f0
  class C1,C2 collector
  class L1,L2 logic
  class R router
  class TG,PO,MQ ext
  style Rooms fill:#0b1016,stroke:#243447,color:#7f93a8
```

<table>
<tr>
<td width="50%" valign="top">

### 🧠 Logic nodes

**2 or more** · ESP32-S3 with an ESP32-H2 as its Zigbee radio, FRAM for persistence.

- hold a full replica of the outbound queue, configuration and rules
- run **every** rule
- serve the local web UI
- one is the **leader** and talks to the outside world; the others shadow it and take over

</td>
<td width="50%" valign="top">

### 📡 Collectors

**ESP32-H2**

- read sensors and forward events over the Zigbee mesh
- relay for each other
- no logic of their own

</td>
</tr>
</table>

### How an alarm survives failures

```mermaid
sequenceDiagram
  autonumber
  participant S as Collector
  participant A as Logic node 1 (leader)
  participant B as Logic node 2 (follower)
  participant T as Telegram / Pushover
  S->>S: store event (node, boot, seq)
  par to every live logic node
    S->>A: event
    A-->>S: ACK
  and
    S->>B: event
    B-->>S: ACK
  end
  A-->>B: reconcile received events
  A->>T: send alarm (leader only)
  Note over A,T: if the leader is dead or mute
  B->>B: no "delivered" for the critical alarm within ~45 s
  B->>T: sends it itself, publishes "delivered"
```

| | Guarantee |
| :---: | --- |
| 🆔 | Every event carries a unique id `(node, boot, seq)`, is **stored before it is acknowledged**, and is retried to **every** live logic node until each confirms. Logic nodes also reconcile what they have received with each other. |
| 👑 | The leader is the live logic node with a working uplink, the best configured priority, then the lowest hardware id. |
| 🛟 | A follower that does not see a critical alarm marked as delivered within **~45 s** sends it itself. |
| 🔌 | Wires are supervised with end-of-line resistors, so a cut or shorted loop is a fault, not silence. Silent nodes and sensors raise health events. |
| 🔕 | Mute never silences a critical event. |

> [!TIP]
> **Success signal.** On the bench (2 logic nodes + 2 collectors), during a smoke alarm the leader's power,
> the router and one relaying collector are switched off in turn — the alarm reaches Telegram and Pushover
> every time within **60 s**, **with no losses and at most one duplicate**; a cut loop and a disconnected node
> raise their own fault events.

### ⚖️ Telegram as the arbiter

If the internal network splits and two nodes both think they lead, **Telegram decides**:

```mermaid
flowchart LR
  P["Second poller calls getUpdates"] --> C{"409 Conflict"}
  C --> R["Read the lease:<br/>getChat.pinned_message"]
  R --> F{"Lease fresh and<br/>held by another node?"}
  F -- yes --> Y["YIELDED until it expires:<br/>no getUpdates, no non-critical sends"]
  F -- "no, expired" --> L["Take the lease, stay LEADER"]

  classDef crit fill:#182433,stroke:#ff1f4b,color:#dce6f0
  classDef ok fill:#182433,stroke:#34d399,color:#dce6f0
  classDef warn fill:#182433,stroke:#fbbf24,color:#dce6f0
  classDef base fill:#111a24,stroke:#243447,color:#dce6f0
  class C crit
  class L ok
  class Y warn
  class P,R,F base
```

- Only the `LEADER` sends and long-polls `getUpdates`. It holds a **pinned message in the owner's chat** —
  the lease `lease{node, priority, until}` — and renews it before it expires.
- A `409 Conflict` means only "there is a second poller"; the lease decides who yields. Verified against the real Bot API.
- The update `offset` is stored and replicated **before** a command is executed, and executed `update_id`s are
  kept in a replicated set — a new leader never replays old commands.

## Screens

UX mockups in the dark **Cold Steel NOC** theme (bot and web text is Russian). Source:
[mockups](_bmad-output/planning-artifacts/ux-designs/ux-garageAlarms-2026-09-30/mockups/).

<table>
<tr>
<td width="50%" align="center">
  <img src="docs/assets/screen-bot-alarm.png" alt="Telegram alarm: Pushover breaks through silent mode, the alarm message with Принято and Статус buttons, acknowledged state">
  <br><sub><b>Bot · alarm</b> — Pushover breaks through silent mode, «Принято» stops repeats everywhere</sub>
</td>
<td width="50%" align="center">
  <img src="docs/assets/screen-bot-menu.png" alt="Telegram main menu for subscriber, operator and owner, with the leader lease pinned">
  <br><sub><b>Bot · menu</b> — per-role menus; the owner's chat pins the leader lease</sub>
</td>
</tr>
<tr>
<td width="50%" align="center">
  <img src="docs/assets/screen-web-summary.png" alt="Web UI summary: active smoke alarm banner, faults, recent events, nodes with uplink history">
  <br><sub><b>Web · summary</b> — active alarm, faults, recent events, nodes and uplink history</sub>
</td>
<td width="50%" align="center">
  <img src="docs/assets/screen-web-add-collector.png" alt="Web UI: add a collector over Web Bluetooth in four steps">
  <br><sub><b>Web · add collector</b> — Web Bluetooth setup with the code from the sticker</sub>
</td>
</tr>
</table>

## Features

| | | |
| --- | --- | --- |
| 💬 **Telegram bot**<br>Everyday use: status, sensors by zone, event history, «Принято» (ack) on alarms, modes (home / away / night), personal and global mute. Roles: owner, operator, subscriber. | 🚨 **Pushover**<br>Emergency alerts for critical events to the whole family, repeating until someone acknowledges; an ack in any channel stops repeats everywhere. | 🖥️ **Local web UI**<br>LAN only, HTTPS with the system's own CA: nodes, sensors, zones and their adjacency graph, rules, credentials, MQTT, updates. |
| 🧩 **Rules**<br>Nine templates — threshold, rate of change, health, coincidence, chain, trace across zones, aggregation, action, mode/schedule — with an optional `when` condition. | 🌡️ **Sensor driver gateway**<br>1-Wire and I²C; a new sensor type is one driver file and a rebuild. Starting set: DS18B20, SHT3x/4x, BME280/680/688, SGP40/41, SCD40/41, BH1750, VEML7700, INA219, ADS1115, plus dry contacts and pulse counters. | 📤 **MQTT mirror**<br>Optional output with its own bounded buffer — it can never push an alarm out of the main queue. |
| 🔄 **OTA**<br>Every node, one at a time, with rollback; collectors via the Zigbee OTA cluster, logic nodes over UDP together with their H2 radio firmware. | 🔐 **Security**<br>Own CA for HTTPS, every frame signed (HMAC), anti-replay, secrets in encrypted NVS, signed OTA images, secure boot v2 + flash encryption on production logic nodes. | 🛠️ **Setup**<br>USB provisioning; a logic node via its own WiFi access point, a collector via Web Bluetooth (BLE Security 2, code on the sticker). |

## Architecture at a glance

**Stack:** ESP-IDF v6.0 + FreeRTOS, actor model (one task per layer, message passing),
esp-zigbee-lib 2.0 in distributed-security mode (no coordinator), CBOR between nodes, OTA for
every node one at a time with rollback. Model AP: availability over consistency — a duplicate is
acceptable, a loss is not.

Every actor is a FreeRTOS task, the sole owner of its state, talking only through messages on its
input queue. Two node classes run different subsets of the same stack:

| Layer | Actor | Owns | Logic node<br>S3 + H2 | Collector<br>H2 |
| :---: | --- | --- | :---: | :---: |
| L1 | `Mesh` | Zigbee stack, UDP transport, framing, channel choice | ✅ | ✅ Zigbee only |
| L2 | `Delivery` | ACK, retry, anti-replay, dedup, unacked buffer, replication of source events | ✅ | ✅ |
| L3 | `Membership` | live-node table, heartbeat, leader computation | ✅ | ✅ no leader |
| L4 | `Config` | soft config, rules, subscribers, modes, versions, merge | ✅ | — |
| L5 | `Rules` | rule execution, windows, derived events | ✅ | — |
| ext | `Notify` · `Ui` | outbound queue, Telegram, Pushover, MQTT · web UI and commands | ✅ | — |
| — | `Sensor` · `Sys` · `Storage` | drivers and channels · OTA, watchdog, time, provisioning · FRAM / NVS | ✅ | ✅ |

25 binding decisions (AD-1…AD-25) are recorded in the
[architecture spine](_bmad-output/planning-artifacts/architecture/architecture-garageAlarms-2026-09-29/ARCHITECTURE-SPINE.md).

## Roadmap

Nine epics, 67 stories. Each epic delivers something that works on its own.

```mermaid
flowchart LR
  E1["1 · Prototype"] --> E2["2 · First alarm"] --> E3["3 · No SPOF"] --> E4["4 · Notify + ack"] --> E5["5 · OTA + secure setup"]
  E5 --> M(("1.x off"))
  M --> E6["6 · Web config"] --> E7["7 · Sensors"] --> E8["8 · Rules"] --> E9["9 · MQTT"]

  classDef next fill:#182433,stroke:#fbbf24,color:#dce6f0
  classDef todo fill:#111a24,stroke:#243447,color:#dce6f0
  classDef mile fill:#ff1f4b,stroke:#ff1f4b,color:#0b1016
  class E1 next
  class E2,E3,E4,E5,E6,E7,E8,E9 todo
  class M mile
```

| | # | Epic | Stories | Outcome |
| :---: | :---: | --- | :---: | --- |
| 🟡 | 1 | Prototype confirms the hardware | 7 | S3 ↔ H2 radio link, Zigbee without a coordinator, loss under WiFi load, Zigbee OTA from a router — measured before building on them |
| ⚪ | 2 | First alarm through the network | 6 | dry contact on a collector → mesh → logic node → FRAM → Telegram; USB provisioning, encrypted secrets, signed frames |
| ⚪ | 3 | No single point of failure | 11 | two logic nodes, heartbeat, leader and Telegram lease, replication, UDP second path, critical-alarm backup; bench chaos test |
| ⚪ | 4 | Notification and acknowledgement | 7 | full Telegram UI, Pushover until acknowledged, shared ack, daily report, reboot notices |
| 🏁 | 5 | OTA, secure setup, **replacing 1.x** | 7 | OTA for all nodes, phone-based setup, secure boot for production nodes, end-of-line loops; acceptance test, then 1.x is switched off |
| ⚪ | 6 | Local web configuration | 7 | nodes, sensors, zones, leadership, subscribers, conflicts, export/import |
| ⚪ | 7 | Sensors and their health | 10 | channel model, driver gateway and starting set, 12 V and 220 V supervision |
| ⚪ | 8 | Rules and shared logic | 9 | nine templates, `when`, modes, chains, coincidences, traces, rule editor |
| ⚪ | 9 | MQTT mirror | 3 | events, states and health to the user's broker |

<sub>🟡 next up · ⚪ planned · 🏁 milestone: 1.x is switched off after epic 5</sub>

> [!NOTE]
> **Status (2026-09-30):** planning complete; prototype hardware ordered
> ([bill of materials](_bmad-output/planning-artifacts/bill-of-materials.md)). Stories 1.1 and
> 1.2 need no hardware and can start now. The 1.x box stays in service until epic 5.

### Planning documents

All planning was done with the BMad method and lives under `_bmad-output/` (in Russian):

| Document | What it fixes |
| --- | --- |
| 💡 [Idea: device network](_bmad-output/forge/garage-device-network/forged-idea.md) | decentralized AP model, node classes, Telegram arbiter |
| 💡 [Idea: rules and config](_bmad-output/forge/rules-config-model/forged-idea.md) | configuration levels, channel model, rule templates |
| 🏛️ [Architecture spine](_bmad-output/planning-artifacts/architecture/architecture-garageAlarms-2026-09-29/ARCHITECTURE-SPINE.md) | 25 binding decisions (AD-1…AD-25) |
| 📜 [Specification](_bmad-output/specs/spec-garage-alarms-2/SPEC.md) | 13 capabilities, constraints, non-goals, success signal + companions: [external channels](_bmad-output/specs/spec-garage-alarms-2/external-channels.md) · [sensor catalog](_bmad-output/specs/spec-garage-alarms-2/sensor-catalog.md) · [rule templates](_bmad-output/specs/spec-garage-alarms-2/rule-templates.md) · [glossary](_bmad-output/specs/spec-garage-alarms-2/glossary.md) |
| 🎨 [UX: design](_bmad-output/planning-artifacts/ux-designs/ux-garageAlarms-2026-09-30/DESIGN.md) · [UX: experience](_bmad-output/planning-artifacts/ux-designs/ux-garageAlarms-2026-09-30/EXPERIENCE.md) | dark "Cold Steel" theme, bot menus, web sections, setup flows, mockups |
| 🗂️ [Epics and stories](_bmad-output/planning-artifacts/epics.md) | 36 FR, 12 NFR, 69 UX requirements, 9 epics, 67 stories |
| 🧾 [Bill of materials](_bmad-output/planning-artifacts/bill-of-materials.md) | prototype bench: 2 logic nodes + 2 collectors |

---

## garageAlarms 1.x (current firmware)

An ESP32-S3 bridge between two dry-contact sensors in a garage — smoke and motion — and
Telegram. Contacts close, the people who subscribed to the bot get a message.

<details open>
<summary><b>Features</b></summary>

- Alerts fan out to every subscriber, queued and retried until delivered
- The queue, the subscriber list and the mute window live in NVS, so a power cut loses nothing
- Time-based debounce with a per-input cooldown — one real event produces one message
- Inline Telegram menu that edits itself in place instead of filling the chat with copies
- `/status` with link quality, uptime, reboot count and the reason for the last reset
- `/mute 4` silences chatter for four hours; a smoke alarm is never silenced
- Daily heartbeat, so silence from the bot is unambiguous
- Subscription is gated by a shared PIN
- Stuck-input detection: a motion contact held for hours is reported as a wiring fault
- LED status: hunting for WiFi, alerts queued, or idle

</details>

<details>
<summary><b>Hardware</b></summary>

| | |
|---|---|
| Board | ESP32-S3 (developed on an N16R8 module, 16 MB flash) |
| GPIO 17 | smoke / alarm contact — `INPUT_PULLUP`, active **LOW** |
| GPIO 18 | motion detector — `INPUT_PULLDOWN`, active **HIGH** |
| GPIO 48 | status LED (the variant's addressable `LED_BUILTIN`) |

The two inputs have **opposite polarity**, and each is pulled toward its *inactive* level, so
a cut wire reads as quiet rather than as an alarm.

Plain ESP32 is not supported: the code uses `INPUT_PULLDOWN`, and the reset-reason mapping
covers the S3's USB and JTAG resets. On an ESP32-C3 these pin choices would clash with USB
(GPIO18/19 are D−/D+ there).

</details>

<details>
<summary><b>Build</b></summary>

```bash
cp secrets.example.h secrets.h    # then fill it in
arduino-cli compile --fqbn "esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=min_spiffs" .
arduino-cli upload  --fqbn "esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=min_spiffs" -p /dev/ttyACM0 .
```

`CDCOnBoot=cdc` matters: without it `Serial` goes to the UART pins and you get no log over
USB. Verified against ESP32 core 3.3.11 and ArduinoJson 7.4.2.

`secrets.h` holds the WiFi credentials, the bot token from [@BotFather](https://t.me/BotFather),
your own chat id, and the subscription PIN. It is gitignored and has never been committed.

> [!WARNING]
> `OWNER_CHAT_ID` must be **your** Telegram user id (get it from @JsonDumpBot), not the bot's id
> — the part of the token before the colon is the bot, and using it silently disables the
> owner-only features.

</details>

<details>
<summary><b>Bot commands</b></summary>

| | |
|---|---|
| `/subscribe <PIN>` | start receiving alerts |
| `/status` | system state |
| `/mute 4` / `/unmute` | silence non-critical alerts for N hours |
| `/test` | check that delivery works |
| `/menu`, `/help` | menu and reference |
| `/who`, `/reboot` | owner only |

Bot messages are in Russian; code and comments are in English.

</details>

<details>
<summary><b>The vendored library</b></summary>

`src/AsyncTelegram2/` is [AsyncTelegram2](https://github.com/cotestatnt/AsyncTelegram2) 2.3.3
with fixes, each marked `PATCHED (garageAlarms)`. It is vendored rather than used from
`libraries/` because without these fixes the firmware reboots in a loop on a weak link.

The one that mattered: the body-read loop in `getUpdates()` used

```c
for (uint32_t timeout = millis(); (millis() - timeout > 1000) || pos < len;)
```

The first clause becomes true once a second has elapsed and never becomes false again, so any
response that takes over a second to read spins forever with no yield — task watchdog, reset.
On a weak WiFi link that fires constantly, and since the original sketch announced itself from
`setup()`, every reset became a Telegram message. The "flood of reconnect messages" was a
reboot loop.

Also fixed: an unbounded header-skip loop, a tight spin in the blocking send path, ArduinoJson
7 compatibility, a missing `parse_mode` on `editMessage`, `callback_query_id` sent as a number
instead of a string, and a non-blocking send that always reported failure — which made a
retrying caller deliver every message six times.

See [`CLAUDE.md`](CLAUDE.md) for the architecture in more detail.

</details>

<details>
<summary><b>Known limitation</b></summary>

Sensors are sampled from `loop()`, and a Telegram send blocks for up to a few hundred
milliseconds, so a pulse shorter than one send could be missed. This is deliberate: latching
edges in an ISR would also latch microsecond noise spikes, which is the failure mode the
debounce exists to prevent. It is safe here because both inputs are latching contacts that
hold for seconds.

</details>

## License

[MIT](LICENSE) for this project's own code. The vendored library keeps its upstream license.
