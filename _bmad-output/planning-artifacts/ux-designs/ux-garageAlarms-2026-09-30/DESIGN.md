---
name: garageAlarms 2.0
description: Визуальный язык трёх поверхностей — Telegram-бота (текст, эмодзи, HTML, inline-клавиатуры), служебного локального веба (тёмная тема «Cold Steel NOC») и страниц ввода узла.
status: final
created: 2026-09-30
updated: 2026-09-30
sources:
  - .memlog.md
  - .working/color-themes-1.html
  - ../../../specs/spec-garage-alarms-2/SPEC.md
  - ../../../specs/spec-garage-alarms-2/external-channels.md
  - ../../../specs/spec-garage-alarms-2/glossary.md
  - ../../../specs/spec-garage-alarms-2/rule-templates.md
  - ../../../specs/spec-garage-alarms-2/tunables.md
  - ../../epics.md
colors:
  bg: '#0b1016'
  surface: '#111a24'
  raised: '#182433'
  border: '#243447'
  input-border: '#5a7189'
  text: '#dce6f0'
  text-2: '#7f93a8'
  accent: '#22d3ee'
  accent-tint: '#143844'
  on-accent: '#0b1016'
  ok: '#34d399'
  warn: '#fbbf24'
  warn-tint: '#2d2e24'
  critical: '#ff1f4b'
  critical-edge: '#ff849c'
  on-critical: '#0b1016'
  fault: '#c084fc'
  fault-tint: '#26273e'
  info: '#60a5fa'
  focus: '#67e8f9'
typography:
  display:
    fontFamily: "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Inter, sans-serif"
    fontSize: 22px
    fontWeight: '700'
    lineHeight: '1.25'
  heading:
    fontFamily: "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Inter, sans-serif"
    fontSize: 17px
    fontWeight: '600'
    lineHeight: '1.3'
  body:
    fontFamily: "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Inter, sans-serif"
    fontSize: 14px
    fontWeight: '400'
    lineHeight: '1.45'
  input-mobile:
    fontSize: 16px
  small:
    fontSize: 12px
    fontWeight: '400'
    lineHeight: '1.4'
  label:
    fontSize: 11px
    fontWeight: '600'
    letterSpacing: 0.04em
  mono:
    fontFamily: "ui-monospace, SFMono-Regular, Menlo, Consolas, monospace"
    fontSize: 13px
  alarm:
    fontSize: 18px
    fontWeight: '800'
    letterSpacing: 0.02em
  telegram:
    note: 'Шрифт задаёт клиент Telegram; доступны только <b>, <i>, <code> через parse_mode=HTML'
rounded:
  sm: 4px
  md: 6px
  lg: 8px
  xl: 10px
  full: 9999px
spacing:
  '1': 4px
  '2': 6px
  '3': 8px
  '4': 10px
  '5': 12px
  '6': 16px
  '7': 20px
  '8': 24px
  gutter-mobile: 16px
  sidebar: 200px     # размер, не шаг сетки
  touch-min: 44px    # размер, не шаг сетки
components:
  sidebar-item-active:
    border-left: '3px solid {colors.accent}'
    background: '{colors.accent-tint}'
    foreground: '{colors.text}'
    min-height: '{spacing.touch-min}'
  sidebar-item:
    foreground: '{colors.text-2}'
    min-height: '{spacing.touch-min}'
  summary-strip:
    background: '{colors.surface}'
    border: '1px solid {colors.border}'
    radius: '{rounded.md}'
    foreground: '{colors.text-2}'
    value-foreground: '{colors.text}'
  panel:
    background: '{colors.surface}'
    border: '1px solid {colors.border}'
    radius: '{rounded.lg}'
    padding: '{spacing.6}'
  alarm-banner:
    background: '{colors.critical}'
    foreground: '{colors.on-critical}'
    border: '3px solid {colors.critical-edge}'
    glow: '0 0 22px {colors.critical}'
    animation: 'glow 1.6s ease-in-out infinite; при prefers-reduced-motion — статичная рамка без glow'
    typography: '{typography.alarm}'
    radius: '{rounded.md}'
  fault-row:
    background: '{colors.fault-tint}'
    border: '1px dashed {colors.fault}'
    foreground: '{colors.text}'
    radius: '{rounded.md}'
  warn-banner:
    background: '{colors.warn-tint}'
    border: '1px solid {colors.warn}'
    foreground: '{colors.text}'
    radius: '{rounded.md}'
  status-pill:
    border: '1px solid currentColor'
    radius: '{rounded.xl}'
    typography: '{typography.label}'
    variants: 'ok · warn · critical · fault · info · stale({colors.text-2})'
  data-table:
    background: '{colors.surface}'
    header-background: '{colors.raised}'
    header-typography: '{typography.label}'
    row-border: '1px solid {colors.border}'
    numeric-typography: '{typography.mono}'
    row-action-min-height: '{spacing.touch-min}'
  input:
    background: '{colors.bg}'
    border: '1px solid {colors.input-border}'
    border-error: '2px solid {colors.warn}'
    error-text: '{colors.warn} + значок ⚠ + текст под полем'
    focus: '2px solid {colors.focus}'
    radius: '{rounded.md}'
    min-height: '{spacing.touch-min}'
  secret-input:
    extends: input
    placeholder: '«задан · изменён ДД мес» или «не задан»; значение не показывается'
  when-editor:
    extends: input
    typography: '{typography.mono}'
    error-mark: 'волнистое подчёркивание {colors.warn} под позицией ошибки'
  duration-picker:
    extends: input
    typography: '{typography.mono}'
  button-primary:
    background: '{colors.accent}'
    foreground: '{colors.on-accent}'
    radius: '{rounded.md}'
    min-height: '{spacing.touch-min}'
  button-secondary:
    background: '{colors.raised}'
    foreground: '{colors.text}'
    border: '1px solid {colors.input-border}'
    radius: '{rounded.md}'
    min-height: '{spacing.touch-min}'
  button-danger:
    background: '{colors.critical}'
    foreground: '{colors.on-critical}'
    radius: '{rounded.md}'
    min-height: '{spacing.touch-min}'
    focus: '2px solid {colors.focus} со смещением 2px на тёмной подложке'
  new-badge:
    background: '{colors.info}'
    foreground: '{colors.bg}'
    radius: '{rounded.full}'
  countdown:
    typography: '{typography.mono}'
    foreground: '{colors.warn}'
  replication-status:
    typography: '{typography.small}'
    pending: '{colors.warn} · «Сохранено на этом узле»'
    done: '{colors.ok} · «На всех узлах»'
  step-page:
    max-width: 480px
    background: '{colors.bg}'
    step-label: '{typography.label} «Шаг N из M»'
    countdown: '{components.countdown}'
  key-package:
    extends: panel
    border: '1px solid {colors.warn}'
    typography: '{typography.mono}'
  diff-row:
    old: '{colors.text-2}, зачёркнуто'
    new: '{colors.text}, на {colors.accent-tint}'
  pager:
    extends: button-secondary
  tg-severity:
    critical: '🚨 + ЗАГЛАВНЫЙ жирный заголовок'
    warn: '⚠️ + жирный заголовок, обычный регистр'
    fault: '🔧 + жирный заголовок'
    ok: '✅'
    info: 'ℹ️'
  tg-kind:
    smoke: '🔥'
    water: '💧'
    temperature: '🌡'
    motion: '🚶'
    power: '🔌'
    node: '📟'
  tg-state-dot:
    ok: '🟢'
    alarm: '🔴'
    threshold: '🟡'
    fault: '🟣'
    stale: '⚪'
  tg-marker:
    signature: '🏠 <имя узла> · <статус>'
    late: '⏳ с опозданием · событие в ЧЧ:ММ'
    reminder: '🔁'
    mute-personal: '🔕'
    mute-shared: '🔇'
    mode: '🛡'
    reboot: '🔄'
    report: '📊'
    lease: '📌'
    ack: '✅ Принято'
  tg-button:
    label: 'эмодзи + слово; у тревоги не больше 2 кнопок в ряду, в меню — 2 в ряду'
---

> Палитра — вариант 3 из [вариантов тёмной темы](mockups/color-themes-1.html). При расхождении с макетом прав этот документ.

## Brand & Style

garageAlarms — сигнализация, а не приборная панель для любования. Порядок «не потерять событие > не флудить > остальное» задаёт и визуальный язык: тревога — самое громкое, что есть на любой поверхности; всё остальное намеренно тихое.

Веб — служебный пульт в духе Grafana/NOC: холодная сталь, чистые линии, моноширинные цифры, бирюзовый акцент для навигации и действий, красный только для тревоги и опасных действий. Он для настройки, не для наблюдения — никаких декоративных графиков и анимаций, кроме свечения активной тревоги.

Бот — текст в клиенте Telegram. Визуальных средств четыре: эмодзи, `<b>/<i>/<code>`, регистр и inline-клавиатура. Эмодзи — фиксированный словарь важности и типов. Компоненты `tg-*` — словарь контента, а не стилевые токены.

## Colors

Одна тема — тёмная «Cold Steel NOC»; светлой нет. Палитра — вариант 3 из `.working/color-themes-1.html`; при расхождении главнее этот файл.

- **`{colors.bg}` / `{colors.surface}` / `{colors.raised}`** — три тона стали: страница, панели, заголовки таблиц и кнопки второго ряда. Иерархия — тоном, не тенью.
- **`{colors.border}`** — волосяные линии таблиц и панелей. Граница полей и вторичных кнопок — `{colors.input-border}` (≥ 3:1 к фону).
- **`{colors.text}` / `{colors.text-2}`** — основной и вторичный текст. `text-2` не ставится на тинты (`warn-tint`, `fault-tint`) — там только `{colors.text}`.
- **`{colors.accent}`** / **`{colors.accent-tint}`** — навигация, активный пункт, основная кнопка, ссылки, новое значение в отличиях импорта. Никогда не означает состояние.
- **`{colors.ok}`** — норма, «на связи», «На всех узлах».
- **`{colors.warn}`** / **`{colors.warn-tint}`** — некритичный порог, выключенные тревоги канала, тишина, обратный отсчёт, «Сохранено на этом узле», **ошибки форм** (со значком ⚠ и текстом).
- **`{colors.critical}`** — только критичная тревога и кнопка опасного действия. Текст на красном — всегда `{colors.on-critical}`. `{colors.critical-edge}` — рамка плашки тревоги.
- **`{colors.fault}`** / **`{colors.fault-tint}`** — неисправность: обрыв, КЗ, молчащий канал, пропавший узел. Фиолетовый, чтобы «сломалось» не путалось с «горит».
- **`{colors.info}`** — новые каналы «без имени», справка.
- **`{colors.focus}`** — кольцо фокуса клавиатуры; на красной кнопке — со смещением на тёмной подложке.

Цвет никогда не единственный носитель состояния: рядом всегда слово и форма значка. Эмодзи-точки бота (`tg-state-dot`) повторяют цвета веба: 🟢 ok, 🔴 critical, 🟡 warn, 🟣 fault, ⚪ stale.

**Контраст важных пар (WCAG):**

| Пара | Контраст |
|---|---|
| `on-critical` на `critical` (плашка, кнопка опасного действия) | 5,05:1 |
| `text` на `bg` / `surface` / `raised` | 15,1 / 13,9 / 12,4 |
| `text-2` на `surface` / `raised` | 5,6 / 4,96 |
| `text` на `warn-tint` / `accent-tint` | 10,9 / 9,9 |
| `warn` на `surface` (ошибка поля) | 10,5 |
| `fault` на `fault-tint` | 5,5 |
| `on-accent` на `accent` | 10,6 |
| `bg` на `info` (`new-badge`) | 7,5 |
| `input-border` на `bg` / `surface` (граница, не текст) | 3,8 / 3,5 |
| `focus` на `bg` / `raised` (кольцо) | 13,2 / 10,8 |

Отвергнуто: белый на `critical` (3,8), контурная красная кнопка на `raised` (4,1), `text-2` на `warn-tint` (4,35).

## Typography

Системный sans-стек, без загружаемых шрифтов (веб отдаёт сам узел). Стили без `fontFamily` (`input-mobile`, `small`, `label`, `alarm`) наследуют `{typography.body}`.

- `{typography.display}` — заголовок раздела, один на страницу.
- `{typography.heading}` — заголовки панелей.
- `{typography.body}` — основной текст и строки таблиц.
- `{typography.label}` — заголовки столбцов, подписи полей и «Шаг N из M», ЗАГЛАВНЫМИ.
- `{typography.mono}` — все числа, время, RSSI, EUI-64, версии, пакет ключей, поле `when`: столбцы не «прыгают».
- `{typography.alarm}` — только текст плашки тревоги.
- Поля ввода на ширине телефона — `{typography.input-mobile}` (16px), чтобы iOS не увеличивал страницу.

Числа — по-русски: `23,4 °C`, время `ЧЧ:ММ` 24-часовое, даты `12 сен`.

В боте ЗАГЛАВНЫМИ пишется только заголовок критичной тревоги (`🚨 🔥 <b>ДЫМ · ГАРАЖ</b>`). Остальное — обычный регистр, жирным выделена одна мысль на сообщение.

## Layout & Spacing

Шкала `{spacing.1}`…`{spacing.8}` (4–24px) снята с макета варианта 3. Внутри плотных строк таблиц — `{spacing.1}`–`{spacing.3}`; между панелями — `{spacing.7}`. `sidebar` и `touch-min` — размеры, не шаги.

Веб: слева боковое меню `{spacing.sidebar}`, справа содержимое. Сверху страницы всегда `summary-strip`, под ним — `alarm-banner`, если есть активная тревога. На ширине < 560px меню превращается в переносящийся ряд вкладок над содержимым, отступы по краям — `{spacing.gutter-mobile}`, формы — в одну колонку, таблицы узлов и каналов — карточками. Горизонтальной прокрутки страницы нет. Все интерактивные элементы — не ниже `{spacing.touch-min}`.

Страницы ввода и мастера добавления узла (`step-page`) — одна колонка, максимум 480px, одно действие на экран.

## Elevation & Depth

Теней нет. Слои различаются тоном (`bg` → `surface` → `raised`) и линией `{colors.border}`. Единственное исключение — свечение `alarm-banner`: оно означает «горит сейчас» и больше нигде не используется.

## Shapes

`{rounded.sm}` — мелкие значки; `{rounded.md}` — поля, кнопки, полосы, плашки, таблицы; `{rounded.lg}` — панели; `{rounded.xl}` — таблетки статуса. Скругления сдержанные: пульт, а не потребительское приложение.

## Components

### Веб

- **Боковое меню** — `sidebar-item`, активный — `sidebar-item-active` (бирюзовая черта и тинт). У «Конфликты» и «Датчики» — счётчик `new-badge`, если есть что разобрать.
- **Сводная полоса** (`summary-strip`) — пары «подпись: значение»: узлы на связи `N/M`, ведущий, uplink, режим, тишина до, MQTT, конфликты. Пропавший узел — `{colors.fault}` и словом «пропал».
- **Плашка тревоги** (`alarm-banner`) — `🚨 ДЫМ · ГАРАЖ` крупно и строка контекста: время, узел, «не подтверждено» или «✅ Принято: <кто>, ЧЧ:ММ». Справа — подсказка `{typography.small}` «Подтвердить можно в Telegram или Pushover». Кнопок нет.
- **Строка неисправности** (`fault-row`) — пунктирная фиолетовая рамка, 🔧, что сломалось и с какого времени.
- **Плашка предупреждения** (`warn-banner`) — выключенные тревоги канала «до ЧЧ:ММ, кем», идущее обновление, сетевая блокировка OTA, «репликация ждёт узел».
- **Таблетка статуса** (`status-pill`) — контурная, цвет по состоянию, всегда со словом: `норма`, `тревога`, `порог`, `обрыв`, `КЗ`, `молчит`, `пропал`, `ведущий`, `ведомый`, `уступил`, `изолирован`.
- **Таблица** (`data-table`) — узлы, каналы, события, подписчики, коллекторы в «Обновлениях»; числа в `{typography.mono}` вправо; на < 560px — карточки.
- **История uplink** — полоса за 24 ч из отрезков `{colors.ok}`/`{colors.fault}` в строке узла; без осей; рядом текст «разрывов за сутки: 3».
- **Поле ввода** (`input`) — граница `{colors.input-border}`; ошибка — граница `{colors.warn}` 2px, под полем `⚠` и текст цветом `{colors.warn}`. Красный в формах не используется.
- **Поле секрета** (`secret-input`) — только запись; «задан · изменён 12 сен», кнопка «Заменить».
- **Редактор `when`** (`when-editor`) — моноширинное поле, счётчик `n/256`, ошибка — подчёркивание `{colors.warn}` в позиции и текст под полем.
- **Выбор срока** (`duration-picker`) — число часов с шагом, подпись «до ЧЧ:ММ» пересчитывается сразу.
- **Статус репликации** (`replication-status`) — под кнопкой «Сохранить».
- **Кнопки** — `button-primary` («Сохранить», «Найти коллектор»), `button-secondary`, `button-danger` (залитая красная: «Забыть узел», «Выключить тревоги», «Перезагрузить») — только внутри окна подтверждения; в строке таблицы действие открывает окно кнопкой `button-secondary`.
- **Окно подтверждения** — панель `{colors.raised}` поверх затемнения; заголовок — что произойдёт; «Отмена» слева, `button-danger` с названием действия справа.
- **Мастер** (`step-page`) — «Добавить коллектор», «Добавить большой узел», страницы ввода: «Шаг N из M», `countdown` окна справа сверху, одна основная кнопка внизу.
- **Пакет ключей** (`key-package`) — рамка `{colors.warn}`, кнопка «Скопировать ключи», подпись «Это секрет. Вставляйте только на страницу ввода узла.» Сам пакет на экране не раскрывается.
- **Выбор при конфликте** — две карточки `panel` рядом (на телефоне — друг под другом): значение, узел, время; под каждой `button-secondary` «Оставить это».
- **Отличия импорта** (`diff-row`) — ключ, старое (зачёркнуто, `text-2`), новое (на `accent-tint`); итог «изменится N настроек».
- **Листание** (`pager`) — «‹ Раньше» / «Позже ›».

### Telegram-бот

- **Словарь важности** (`tg-severity`) — первая строка события начинается одним значком важности: 🚨 критично, ⚠️ порог/предупреждение, 🔧 неисправность, ✅ норма восстановлена/принято, ℹ️ служебное.
- **Словарь типов** (`tg-kind`) — второй значок, тип канала: 🔥 дым, 💧 вода, 🌡 температура, 🚶 движение, 🔌 питание, 📟 узел.
- **Строка канала** — `<точка> <тип> <имя> — <состояние>`. Числовой канал у порога — 🟡 рядом со значком типа: `🟡 🌡 Температура — 41,0 °C · выше порога 40 °C`.
- **Маркеры** (`tg-marker`) — подпись узла последней строкой каждого сообщения (`🏠 гараж · ведущий`); `⏳ с опозданием · событие в 03:12` второй строкой; 🔁 напоминание; 🔕 моя тишина; 🔇 общая тишина; 🛡 режим; 🔄 перезагрузка; 📊 отчёт; 📌 аренда.
- **Анатомия тревоги** — (1) `🚨 🔥 <b>ДЫМ · ГАРАЖ</b>`; (2) при необходимости `⏳ с опозданием…`; (3) «Сработал в 03:12.»; (4) тренд зоны `🌡 гараж: 24,0 → 31,5 °C за 10 мин ↑` (окно правила скорости изменения); (5) `Соседние зоны: 🟢 коридор — норма · 🔴 подвал — дым`; (6) «Не подтверждено.» или `✅ Принято: <кто>, ЧЧ:ММ. Повторы остановлены.`; (7) подпись узла; клавиатура `[✅ Принято] [📋 Статус]`.
- **Кнопки** (`tg-button`) — эмодзи + слово; в меню по 2 в ряду; последняя строка экрана — `‹ Меню`.

## Do's and Don'ts

| Делать | Не делать |
|---|---|
| Красный — только для критичной тревоги и кнопки опасного действия | Красить красным неисправности, ошибки форм, «пропал» |
| Ошибка поля — `{colors.warn}` + ⚠ + текст | Только цветная рамка без текста |
| Текст на красном — `{colors.on-critical}` | Белый текст на `{colors.critical}` |
| Неисправность — фиолетовым и 🔧 | Смешивать «сломалось» и «горит» одним цветом |
| Состояние = цвет + слово + форма значка | Показывать состояние только цветом или только эмодзи |
| Числа моноширинным, по-русски (`23,4 °C`) | Точка как десятичный разделитель, 12-часовое время |
| ЗАГЛАВНЫЕ — только заголовок критичной тревоги в боте | ЗАГЛАВНЫЕ в предупреждениях, меню, отчётах |
| Один значок важности в начале сообщения | Гирлянды эмодзи, эмодзи в середине фраз |
| Свечение — только у активной тревоги; при `prefers-reduced-motion` — статичная рамка | Мигание всей страницы, анимации загрузки, декоративные графики |
| Секреты — только запись, «задан · изменён …»; пакет ключей не раскрывается | Показывать сохранённые токены, пароли и ключи, даже частично |
