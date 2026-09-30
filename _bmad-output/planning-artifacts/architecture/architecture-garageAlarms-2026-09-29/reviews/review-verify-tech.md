---
review: verify-tech
target: ../ARCHITECTURE-SPINE.md
date: '2026-09-29'
method: 'WebSearch/WebFetch + GitHub API (исходники и манифесты компонентов), по состоянию на 2026-09-29'
---

# Проверка технологических решений — ARCHITECTURE-SPINE (garageAlarms 2.0)

## Вердикт

Стек в целом реален и актуален: ESP-IDF v6.1, esp-zigbee-lib 2.0.4, cbor 7.0.0, secure boot v2 / flash encryption на C5, модули C5 с PSRAM и FM25V20A подтверждаются. Но в документе есть одна устаревшая версия (network_provisioning), одна неверная механика (409 у Telegram в AD-10) и два непроверенных сочетания. Первое: esp-zigbee-lib 2.0.4 официально собран под IDF v5.5.4, а не под v6.1. Второе: нативный Zigbee-роутер одновременно с Wi-Fi на C5 Espressif помечает как «работает, но нестабильно». Это пятый вопрос, без которого нельзя назвать AD-12 и AD-13 надёжными.

## Сводка

| # | Решение в спайне | Статус | Коротко |
|---|---|---|---|
| 1 | ESP-IDF v6.1 | ✅ подтверждено | Latest stable, релиз 27 авг 2026 |
| 2 | esp-zigbee-lib 2.0.4 — текущая | ✅ подтверждено | Релиз 14 авг 2026, последняя версия |
| 3 | esp-zigbee-lib 2.0.4 на ESP-IDF v6.1 | ⚠️ не подтверждено официально | 2.0.4 «based on esp-idf v5.5.4»; v6.1 добавлен в CI только 24 сен 2026, после релиза |
| 4 | Поддержка C5 и H2 в esp-zigbee-lib | ✅ подтверждено | Оба в списке целей, в том числе в примерах OTA |
| 5 | Distributed security, `ezb_aps_secur_enable_distributed_security` | ✅ подтверждено | Есть в `ezbee/aps.h` v2.x |
| 6 | Сеть без координатора, её формирует роутер | ⚠️ частично | API есть; пример формирования distributed-сети роутером в v2.x не найден |
| 7 | Zigbee OTA Upgrade: клиент на H2, сервер на C5 (LEADER-роутер) | ⚠️ частично | Кластер и примеры есть для C5 и H2, но в примере сервер — координатор, клиент — end device |
| 8 | Нативный Zigbee на C5 одновременно с Wi-Fi | ⚠️ условно | Таблица coexist: Wi-Fi STA + Zigbee Router = **C1**, то есть работает, но нестабильно; Espressif советует двухчиповое решение |
| 9 | espressif/cbor 7.0.0 | ✅ подтверждено (свежий мажор) | Вышел ~13 сен 2026; до этого была 0.6.1~4 |
| 10 | espressif/network_provisioning 1.1.0 | ❌ устарело | Текущая 1.2.5 (9 сен 2026); в 1.2.x исправлены уязвимости |
| 11 | network_provisioning на H2 (только BLE, без Wi-Fi) с кастомными эндпоинтами | ⚠️ не подходит по назначению | Кастомные эндпоинты есть, но тип сети — только Wi-Fi или Thread; на Zigbee-H2 нет валидного варианта |
| 12 | Secure boot v2 + flash encryption на C5 | ✅ подтверждено | Для SBv2 брать RSA-3072, ECDSA не рекомендуется |
| 13 | Telegram 409 → «узел с худшим приоритетом уступает» | ❌ механика неверна | 409 получает **более ранний** висящий long poll, а не «худший» узел |
| 14 | Pushover emergency + Receipts API | ✅ подтверждено | retry ≥ 30 с, expire ≤ 10800 с, не больше 50 повторов; опрос receipt не чаще раза в 5 с |
| 15 | Модуль C5 с PSRAM | ✅ подтверждено | ESP32-C5-WROOM-1(U) N4R2…N32R8, в массовом производстве |
| 16 | FRAM Infineon FM25V20A, SPI, 2 Мбит | ✅ подтверждено | 256K×8, SPI до 40 МГц, активный, поставки до ≥ 2033 |

---

## Подробно

### 1. ESP-IDF v6.1 — ✅
- На странице GitHub Releases v6.1 помечен как **Latest** и датирован 27 августа. Параллельно поддерживаются v6.0.3 (2 сен), v5.5.5 и v5.3.6.
- ESP32-C5 (ревизии v1.0/v1.2) поддерживается начиная с v5.5.2, ESP32-H2 v1.2 — начиная с v5.5, так что v6.1 покрывает обе цели.
- Источники: https://github.com/espressif/esp-idf/releases , https://github.com/espressif/esp-idf/blob/release/v6.1/COMPATIBILITY.md

### 2–3. esp-zigbee-lib 2.0.4 и совместимость с IDF v6.1 — ✅ / ⚠️
- 2.0.4 — последняя версия в реестре (опубликована около месяца назад), по RELEASE_NOTES вышла **14-Aug-2026**.
- В RELEASE_NOTES прямо сказано: «2.0.4 version release of ESP-ZIGBEE-SDK is based on esp-idf v5.5.4». README SDK: «The SDK is recommended to be used with ESP-IDF v5.5.4».
- Поддержку IDF v6.0 добавили в 2.0.1 («Added esp-idf v6.0 support»). IDF **v6.1** появился в CI только коммитом `ef6cf6f780` от **2026-09-24** («ci: add IDF 6.1 support and remove IDF 5.2 support»). Это позже релиза 2.0.4, и нового релиза после этого коммита ещё нет.
- Манифест компонента объявляет `idf: ">=5.0"`, поэтому сборка на 6.1 формально разрешена. Но официально протестированной пары «2.0.4 + v6.1» нет.
- **Рекомендация:** указать в Stack, что это пара, которую должен подтвердить прототип. Запасной вариант — IDF v6.0.x (официально поддерживается с 2.0.1). Либо дождаться следующего релиза, 2.0.5 или новее, который будет собран с v6.1 в CI.
- Ветка v2.x — «Latest version, recommended for new designs and production» и работает на собственном стеке Espressif. Ветка v1.x (ZBOSS) — только исправление ошибок. Выбор v2 верный.
- Источники: https://components.espressif.com/components/espressif/esp-zigbee-lib , https://github.com/espressif/esp-zigbee-sdk/blob/main/RELEASE_NOTES.md , https://github.com/espressif/esp-zigbee-sdk , https://github.com/espressif/esp-zigbee-sdk/commit/ef6cf6f780 , https://github.com/espressif/esp-zigbee-sdk/blob/main/components/esp-zigbee-lib/idf_component.yml

### 4. Поддержка C5 и H2 — ✅
- В README примеров OTA (`ota_client`, `ota_server`) целевые платформы: ESP32-H2, C6, **C5**, H21, H4, S31. В README SDK среди эталонных плат есть ESP32-C5-DevKit.
- Источники: https://github.com/espressif/esp-zigbee-sdk/tree/main/examples/ota_upgrade/ota_server , https://github.com/espressif/esp-zigbee-sdk/tree/main/examples/ota_upgrade/ota_client

### 5–6. Distributed security и сеть без координатора (AD-13) — ✅ / ⚠️
- `void ezb_aps_secur_enable_distributed_security(bool enable);` и `bool ezb_aps_secur_is_distributed_security(void);` объявлены в `components/esp-zigbee-lib/include/ezbee/aps.h`.
- В migration guide для v2.x сказано: политика distributed-сети теперь управляется этой функцией, а старые `esp_zb_enable_distributed_network()`, `esp_zb_zdo_setup_network_as_distributed()` и подобные удалены. В `all_device_types_app` стоит вызов `ezb_aps_secur_enable_distributed_security(true)`.
- **Не подтверждено:** отдельного примера, где **роутер формирует** distributed-сеть (BDB formation в роли router), в v2.x не найдено. В `bdb.h` есть `EZB_BDB_MODE_NETWORK_FORMATION`, но ограничения по роли не описаны. Это надо проверить на прототипе до того, как фиксировать AD-13.
- **Уточнение для AD-13 и AD-17:** в v2.x пока нет нескольких глобальных link key («multiple global link key is not supported yet»; `esp_zb_secur_multi_standard_distributed_key_add` удалена). `ezb_secur_switch_network_key()` (2.0.3) переключает ключ только **на локальном устройстве**. Значит, ротацию сетевого ключа в distributed-сети без TC придётся делать своими средствами на уровне приложения.
- Источники: https://github.com/espressif/esp-zigbee-sdk/blob/main/components/esp-zigbee-lib/include/ezbee/aps.h , https://github.com/espressif/esp-zigbee-sdk/blob/main/docs/en/migration-guide/v2.x/common.rst , https://github.com/espressif/esp-zigbee-sdk/blob/main/examples/all_device_types_app/main/all_device_types_app.c

### 7. Zigbee OTA Upgrade cluster (AD-18) — ⚠️
- Кластер поддерживается в v2.x и активно дорабатывается: в 2.0.2 добавили настраиваемые retry/timeout для OTA-клиента, в 2.0.4 исправили прерывание OTA и обработку Image Notify с одним jitter.
- В примере `ota_server` — **координатор**, а `ota_client` — **end device**, причём с `ezb_aps_secur_enable_distributed_security(false)`, то есть в централизованной сети. Спайн предлагает иное: сервер на **роутере** (LEADER), сеть distributed, клиент H2 — тоже роутер. С точки зрения ZCL это допустимо, но пример этого не покрывает.
- **Риск:** LEADER может смениться посреди загрузки образа, и тогда OTA-сервер «переедет». Нужно правило: OTA коллектора привязывается к конкретному C5-серверу на всю сессию или возобновляется у нового сервера с того же offset.
- Источник: https://github.com/espressif/esp-zigbee-sdk/blob/main/examples/ota_upgrade/ota_client/main/ota_client.c , RELEASE_NOTES (см. выше)

### 8. Нативный Zigbee на C5 одновременно с Wi-Fi (AD-12) — ⚠️ условно
- Таблица сосуществования в ESP-IDF для ESP32-C5 (Wi-Fi + IEEE 802.15.4):
  - Wi-Fi STA Scan / Connecting / Connected × Zigbee **Router** = **C1**, то есть «supported but the performance is unstable»;
  - Wi-Fi SoftAP × Zigbee Router = **X** (не поддерживается);
  - BLE **Scan** × Zigbee Router = **X**; BLE Advertising/Connected × Router = Y.
- Цитата: «Routers in Thread and Zigbee networks maintain unsynchronized links… With only a single RF path, increased Wi-Fi or BLE traffic may lead to higher packet loss rates». Espressif рекомендует «a dual-SoC solution (e.g., ESP32-S3 + ESP32-H2) with separate antennas».
- **Следствия для спайна:**
  - Логический C5 — Zigbee-роутер со STA: это режим C1. Резерв в Deferred («Замена C5 на S3+H2») выбран правильно, и AD-12 его изолирует. Но это не маловероятный запасной путь, а сценарий, который Espressif сам рекомендует. Проверку на прототипе стоит поднять в приоритете.
  - Ввод в сеть через SoftAP на C5 **несовместим** с режимом роутера, это X. На C5 через SoftAP подключать нельзя, только USB или BLE.
  - Если компоненту Ui или provisioning на C5 нужен BLE-**скан**, при активном роутере он не поддерживается (X). Advertising и connected работают.
  - Для H2 в таблице тоже Router × BLE Scan = X, Advertising/Connected = Y. Окно BLE-provisioning на коллекторе (advertising + подключение) допустимо.
- 5 ГГц Wi-Fi на C5 снимает помехи в эфире, но не снимает разделение единственного RF-тракта. Помогает ли это в режиме C1, в документации **не сказано**, поэтому пункт остаётся непроверенным.
- Источники: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c5/api-guides/coexist.html , https://docs.espressif.com/projects/esp-idf/en/latest/esp32h2/api-guides/coexist.html , https://github.com/espressif/esp-zigbee-sdk/issues/361

### 9. espressif/cbor 7.0.0 — ✅ (свежий мажор)
- Версия 7.0.0 — последняя в реестре (около 2 недель назад, коммит от 2026-09-13 «update tinycbor to v7.0 and build it through the upstream CMake»). Предыдущая версия — `0.6.1~4`. Upstream TinyCBOR v7.0 вышел 2026-02-18.
- `cborjson.h` в компоненте есть, так что правило логирования через `cborjson` выполнимо. Зависимость `idf: ">=5.0"`.
- **Риск:** скачок 0.6 → 7.0 и новая сборка через upstream CMake. Версии всего две недели, возможны регрессии и изменения API относительно примеров из интернета. Версию стоит закрепить точно (`==7.0.0`) и закрыть host-тестами `proto`.
- Источники: https://components.espressif.com/components/espressif/cbor , https://github.com/espressif/idf-extra-components/tree/master/cbor , https://github.com/intel/tinycbor/releases

### 10–11. espressif/network_provisioning 1.1.0 и H2 — ❌ / ⚠️
- **Версия устарела.** 1.1.0 — сентябрь–октябрь 2025. Текущая — **1.2.5** (9 сен 2026). В 1.2.3 исправлены «possible NULL pointer dereference with malformed protobuf messages» и «possible buffer overflow in Thread provisioning», в 1.2.5 — «buffer overreads when Wi-Fi SSIDs or passwords are not null-terminated». Для AD-17 это критично: нужна как минимум `^1.2.5`.
- **Кастомные эндпоинты** поддерживаются: `network_prov_mgr_endpoint_create()` / `network_prov_mgr_endpoint_register()` в `manager.h`.
- **H2 (Zigbee, без Wi-Fi):** в Kconfig выбор `NETWORK_PROV_NETWORK_TYPE` допускает только `WIFI` (при `ESP_WIFI_ENABLED` или `ESP_WIFI_REMOTE_ENABLED`) или `THREAD` (при `OPENTHREAD_ENABLED`). В Zigbee-прошивке H2 ни того ни другого нет. Компонент сделан для провижининга Wi-Fi и Thread, а сценарий «только BLE-транспорт и собственная запись в NVS» в документации не описан. Собирается ли он без обоих типов сети, **не проверено**.
- **Рекомендация:** на H2, а для единообразия и на C5, брать напрямую `protocomm` из ESP-IDF: транспорт `protocomm_ble`, `protocomm_security2`, собственные эндпоинты через `protocomm_add_endpoint`. На нём и построен network_provisioning, но лишней Wi-Fi/Thread-логики там нет. Строку Stack исправить: либо «protocomm (ESP-IDF)», либо «network_provisioning ^1.2.5 (только C5)».
- Источники: https://components.espressif.com/components/espressif/network_provisioning , https://github.com/espressif/idf-extra-components/blob/master/network_provisioning/CHANGELOG.md , https://github.com/espressif/idf-extra-components/blob/master/network_provisioning/Kconfig , https://github.com/espressif/idf-extra-components/blob/master/network_provisioning/include/network_provisioning/manager.h

### 12. Secure boot v2 и flash encryption на C5 (AD-17) — ✅
- Secure Boot v2 поддерживается со схемами RSA-3072, ECDSA P-256 и ECDSA P-384, до трёх ключей с отзывом. **Важно:** в документации сказано, что «the ECDSA based Secure Boot V2 scheme is not functional for certain input vectors and is therefore not recommended». Значит, брать **RSA-3072**.
- Flash encryption поддерживает XTS-AES-128 и XTS-AES-256. PSRAM по умолчанию тоже шифруется, постранично через MMU. NVS encryption тоже на месте, это нужно для «секретов в зашифрованном NVS».
- Источники: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c5/security/secure-boot-v2.html , https://docs.espressif.com/projects/esp-idf/en/latest/esp32c5/security/flash-encryption.html

### 13. Telegram getUpdates и 409 Conflict (AD-10) — ❌ механика неверна
- Сервер Bot API (tdlib/telegram-bot-api, `Client.cpp`) работает так. Когда приходит новый `getUpdates` с `timeout > 0` и новых апдейтов нет, вызывается `abort_long_poll(false)`. Этот вызов завершает **ранее висящий** long poll с `409 "Conflict: terminated by other getUpdates request; make sure that only one bot instance is running"`, а новый запрос занимает его место. Повторные 409 сервер задерживает на 3 с (`fail_query_conflict`, `next_get_updates_conflict_time_ = now + 3.0`).
- Итак, 409 получает **тот, кто опрашивал раньше**, а не тот, у кого хуже (priority, EUI-64). В 409 нет данных о другом опрашивающем. Два узла будут по очереди выбивать друг друга, и ни один не узнает, что уступать должен именно он.
- Кроме того, `offset` подтверждает апдейты. Если два опрашивающих используют разные `offset`, один может подтвердить апдейты, которые обработает только другой. Идемпотентность по `update_id` защищает от дублей, но не от ситуации, когда команду получил не лидер.
- **Рекомендация:** в AD-10 считать 409 только **сигналом конфликта**, а не арбитром. Узел, получивший 409, прекращает опрос и сверяет лидерство через `Membership`: уступает, если по правилам AD-9 он не лидер. Если он всё же лидер, возобновляет опрос с экспоненциальной паузой. Арбитром остаётся AD-9, 409 лишь указывает на расхождение.
- Официальная документация (https://core.telegram.org/bots/api#getupdates) 409 не описывает, поэтому поведение подтверждено по исходнику сервера: https://github.com/tdlib/telegram-bot-api/blob/master/telegram-bot-api/Client.cpp (функции `abort_long_poll`, `fail_query_conflict`, `process_get_updates_query`). Обсуждения: https://github.com/yagop/node-telegram-bot-api/issues/550

### 14. Pushover emergency и Receipts (AD-10) — ✅
- Priority 2: `retry` **≥ 30 с**; `expire` **≤ 10800 с** (3 ч); число повторов ограничено **50**, какой бы ни был `expire`. В ответе приходит `receipt`, дополнительно можно указать `callback` URL и `tags`.
- Receipts API:
  - `GET /1/receipts/{receipt}.json?token=…` — **не чаще раза в 5 с**. Возвращает `acknowledged`, `acknowledged_at`, `acknowledged_by`, `acknowledged_by_device`, `expired`, `expires_at`, `called_back`, `called_back_at`.
  - `POST /1/receipts/{receipt}/cancel.json`.
  - `POST /1/receipts/cancel_by_tag/{tag}.json`.
  - Receipt действителен до 1 недели.
- Следствие для AD-10 и AD-11. Если follower отправит критичное событие, пока лидер уже отправил своё, пользователь получит две emergency-сессии, и каждая будет повторяться до 50 раз. Стоит ставить `tags=<event_id>`: тогда при `delivered` можно снять дубль через `cancel_by_tag`. Опрос receipts с нескольких узлов вместе должен укладываться в лимит «раз в 5 с». `callback` в LAN без публичного URL не работает, остаётся только опрос.
- Источники: https://pushover.net/api , https://pushover.net/api/receipts

### 15. Модули ESP32-C5 с PSRAM — ✅
- ESP32-C5-WROOM-1: N4R2, N8R2, N16R2, **N8R8, N16R8, N32R8**. -1U: N16R2, N8R8, N16R8, N32R8. Все в статусе **Mass Production**. PSRAM — Quad SPI; у модулей с PSRAM вывод SPICS1 занят.
- Источники: https://documentation.espressif.com/esp32-c5-wroom-1_wroom-1u_datasheet_en.html , https://www.digikey.com/en/products/detail/espressif-systems/ESP32-C5-WROOM-1-N8R8/27566803

### 16. Infineon FM25V20A — ✅
- 2 Мбит (256K × 8), SPI до 40 МГц, 10^14 циклов, хранение 151 год, −40…+85 °C. Есть расширенная версия по температуре. Позиции FM25V20A-DG/-DGQ «planned to be available until at least 2033».
- Замечание: 2 Мбит — это **ровно** 256 КБ, то есть нижняя граница AD-4 без запаса. Для роста очереди или журнала можно рассмотреть 4 Мбит (FM25V40 / CY15B104Q) — не проверялось.
- Замечание (не проверено в TRM): у C5 для пользователя, похоже, есть только один GP-SPI (SPI2), а SPI0/1 заняты flash и PSRAM. Требование «FRAM на своей шине» выполнимо, пока других SPI-устройств нет. По AD-15 выносные датчики — 1-Wire, внутренние — I²C, так что конфликта не будет.
- Источники: https://www.infineon.com/part/FM25V20A-G , https://www.infineon.com/part/FM25V20A-DG , https://www.infineon.com/dgdl/Infineon-FM25V20A_2-Mbit_256_K_8_Serial_SPI_F-RAM_Serial_SPI_256_K_8_33_MHz_extended_industrial-DataSheet-v08_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ecbeec345a2

---

## Что может устареть или требует уточнения в спайне

1. **Stack → Provisioning `1.1.0`**: заменить на `^1.2.5` или на `protocomm` (ESP-IDF). Для H2 компонент не подходит (п. 10–11).
2. **Stack → ESP-IDF v6.1 + esp-zigbee-lib 2.0.4**: указать, что эта пара официально не выпускалась. SDK рекомендует v5.5.4, поддержка v6.0 появилась в 2.0.1, v6.1 пока есть только в CI на `main`. Закрепить версию и проверить на прототипе (п. 3).
3. **AD-10**: переписать правило про 409 (п. 13).
4. **AD-12 / Deferred «Замена C5 на S3+H2»**: Espressif оценивает этот режим как C1 и сам рекомендует двухчиповое решение. Проверку потерь Zigbee на C5 при активном Wi-Fi поставить первым экспериментом прототипа (п. 8). SoftAP-провижининг на C5 исключить.
5. **AD-13**: формирование distributed-сети роутером в v2.x и ротация сетевого ключа без TC не подтверждены примерами (п. 5–6).
6. **AD-18**: OTA-сервер на роутере в distributed-сети и смена LEADER во время сессии не описаны. Нужно правило привязки сессии к серверу (п. 7).
7. **AD-17**: явно указать RSA-3072 для Secure Boot v2 на C5 (п. 12).
8. **espressif/cbor 7.0.0**: версии две недели, это мажорный скачок с 0.6.x. Закрепить точно и покрыть host-тестами (п. 9).
9. **Telegram Bot API и Pushover** в Stack без версии — это нормально. Pushover добавить в AD-10/11 `tags=<event_id>` и `cancel_by_tag` против дублей emergency-сессий (п. 14).
