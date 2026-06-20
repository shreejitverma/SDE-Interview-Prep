# Chapter 47: Calendars and Time Zones in `<chrono>`

> *C++20 extends `<chrono>` from a duration-and-clock library into a full date, calendar, and time-zone library — the standardized successor to Howard Hinnant's widely used `date` library. You can now construct calendar dates with natural syntax, convert between `system_clock` time points and year/month/day, query the IANA time-zone database, and format the results with `std::format`, all type-safely and mostly at compile time. This chapter covers the calendar types, the system/local clock distinction, `zoned_time`, and the formatting and parsing facilities.*

Before C++20, anything calendar-related in standard C++ meant falling back to the C `<ctime>` API — `std::tm`, `mktime`, `localtime` — which is mutable, not thread-safe, error-prone (months are 0-based, years are offset from 1900), and time-zone-naive. C++20's `<chrono>` extension replaces all of it with strongly-typed calendar types, a real time-zone database, and integration with `std::format`. Dates become first-class values you can construct, validate, arithmetic on, and print without ever touching a `struct tm`.

---

## Table of Contents

- [47.1 The chrono Expansion at a Glance](#471-the-chrono-expansion-at-a-glance)
- [47.2 Calendar Types and the Civil-Date Syntax](#472-calendar-types-and-the-civil-date-syntax)
- [47.3 sys_days: Bridging Dates and Time Points](#473-sys_days-bridging-dates-and-time-points)
- [47.4 Date Validation and Arithmetic](#474-date-validation-and-arithmetic)
- [47.5 Calendar Clocks and Time-of-Day](#475-calendar-clocks-and-time-of-day)
- [47.6 Time Zones and zoned_time](#476-time-zones-and-zoned_time)
- [47.7 Formatting and Parsing Dates](#477-formatting-and-parsing-dates)
- [47.8 Professional Insights](#478-professional-insights)

---

## 47.1 The chrono Expansion at a Glance

C++20 adds to `<chrono>` a layered set of facilities built on the existing `duration`/`time_point`/`clock` foundation:

| Layer | What it adds | Key types |
|-------|--------------|-----------|
| Calendar | year/month/day field types and dates | `year`, `month`, `day`, `year_month_day`, `weekday` |
| Date↔time-point bridge | serial-day conversions | `sys_days`, `local_days` |
| Clocks | additional clocks for civil time | `system_clock`, `utc_clock`, `local_t` |
| Time-of-day | split a time point into h:m:s | `hh_mm_ss` |
| Time zones | the IANA tz database | `time_zone`, `zoned_time`, `tzdb` |
| Formatting | `std::format` integration | `{:%Y-%m-%d}` specifiers |

The design principle is **strong typing throughout**: a `year` is not an `int`, a `month` is not a number, and mixing them up is a compile error. The calendar types are also literal types, so dates can be constructed and validated in `constexpr` contexts.

---

## 47.2 Calendar Types and the Civil-Date Syntax

The calendar field types — `std::chrono::year`, `month`, `day` — combine via overloaded `operator/` into a `year_month_day`, giving a readable, locale-independent date literal syntax. User-defined literals (`2026y`, `15d`) and named month constants (`std::chrono::June`) make it natural.

```cpp
// Listing 47.1: constructing civil dates with the / syntax
#include <chrono>
using namespace std::chrono;

year_month_day d1 = 2026y / June / 18;        // June 18, 2026
year_month_day d2 = 2026y / 6 / 18;           // same, numeric month
year_month_day d3 = June / 18 / 2026y;        // US-style ordering also works
year_month_day d4 = 18d / June / 2026;        // day/month/year ordering

// Field access:
year  y = d1.year();      // 2026y
month m = d1.month();     // June
day   dd = d1.day();      // 18d

// Named constants and literals:
auto today = 2026y / June / 18d;
weekday wd{sys_days{today}};   // what day of the week is it?
```

The `operator/` is overloaded to accept the three field types in any sensible order, so `2026y/June/18`, `June/18/2026y`, and `18d/June/2026` all build the same date — the library disambiguates by type, not position. This eliminates the perennial "is it month/day or day/month?" ambiguity: the *types* make the ordering unambiguous. The `y`/`d` literals require `using namespace std::chrono` (or the `_literals` sub-namespace).

---

## 47.3 sys_days: Bridging Dates and Time Points

A `year_month_day` is a *field-based* representation; to do arithmetic or convert to/from a clock you turn it into **`sys_days`** — a `time_point` on `system_clock` measured in days since the epoch (1970-01-01). This serial form is the pivot between calendar dates and clock time points.

```cpp
// Listing 47.2: converting between field dates and serial time points
#include <chrono>
using namespace std::chrono;

year_month_day ymd = 2026y / June / 18;

sys_days sd = sys_days{ymd};          // serial day number (time_point<system_clock, days>)
year_month_day back = year_month_day{sd};   // convert back to fields

// Days between two dates is just subtraction of sys_days:
sys_days a = sys_days{2026y / 1 / 1};
sys_days b = sys_days{2026y / 12 / 31};
days span = b - a;                     // 364 days

// A full timestamp: midnight UTC on the date, as a system_clock time_point.
system_clock::time_point tp = sys_days{ymd};
```

`sys_days` is where calendar dates meet the rest of `<chrono>`: subtract two of them to get a `days` duration, add a `days` to advance, or implicitly widen to a `system_clock::time_point` for sub-day precision. The round trip `year_month_day` → `sys_days` → `year_month_day` is exact and `constexpr`.

---

## 47.4 Date Validation and Arithmetic

Calendar types know whether they represent a real date. `ok()` reports validity (catching February 30), and calendar-aware arithmetic handles month and year boundaries correctly — including the subtlety that "one month after January 31" is not a valid date.

```cpp
// Listing 47.3: validation and calendar arithmetic
#include <chrono>
using namespace std::chrono;

year_month_day bad = 2026y / February / 30;
bool valid = bad.ok();                 // false — Feb 30 does not exist

// Adding months/years operates on fields and may produce an invalid date:
year_month_day jan31 = 2026y / January / 31;
year_month_day feb   = jan31 + months{1};   // 2026y/February/31 — NOT ok()!
bool feb_ok = feb.ok();                       // false

// The fix: normalize with sys_days arithmetic, or clamp to last day of month.
year_month_day last_feb = 2026y / February / last;   // 'last' = last day (Feb 28/29)

// Day arithmetic is always exact via sys_days:
sys_days plus10 = sys_days{jan31} + days{10};  // Feb 10, 2026 — always valid
```

Two arithmetic models coexist deliberately. **Field arithmetic** (`+ months{1}`, `+ years{1}`) preserves the day-of-month and may yield an invalid date you must check with `ok()` and normalize — useful for "same day next month" semantics. **Serial arithmetic** (`sys_days + days{n}`) is always exact and never invalid. The `last` specifier (`year/month/last`) and `weekday` indexing (`Monday[2]/June/2026` = the second Monday) handle the common "last day of month" and "nth weekday" cases directly.

---

## 47.5 Calendar Clocks and Time-of-Day

To split a time point into hours, minutes, seconds, and subseconds, C++20 provides **`hh_mm_ss`**, which decomposes a duration into civil time-of-day fields. Combined with `sys_days` (the date part), it reconstructs a full civil timestamp.

```cpp
// Listing 47.4: extracting time-of-day from a time point
#include <chrono>
using namespace std::chrono;

system_clock::time_point now = system_clock::now();

sys_days today = floor<days>(now);             // truncate to midnight (the date part)
auto since_midnight = now - today;             // the time-of-day part as a duration

hh_mm_ss tod{since_midnight};                  // decompose into h:m:s
auto h = tod.hours();
auto m = tod.minutes();
auto s = tod.seconds();
auto frac = tod.subseconds();

year_month_day date{today};                    // the calendar date
```

`floor<days>(tp)` truncates a time point down to the start of its day (the date), and the remainder is the time-of-day, which `hh_mm_ss` splits into named fields. This `floor`/subtract pattern is the standard way to separate "what date" from "what time" without the C `localtime` dance. Note `hh_mm_ss` works on the *duration since midnight*, independent of any time zone — zone conversion is the next layer.

---

## 47.6 Time Zones and zoned_time

C++20 ships access to the **IANA time-zone database**. A `time_zone` (looked up by name) converts between UTC (`sys_time`) and local wall-clock time (`local_time`), and **`zoned_time`** pairs a time zone with a time point so the same instant can be displayed in any zone — correctly handling DST transitions and historical offset changes.

```cpp
// Listing 47.5: time-zone-aware time points
#include <chrono>
using namespace std::chrono;

system_clock::time_point now = system_clock::now();

// A zoned_time = a time zone + an instant. Display 'now' in two zones:
zoned_time ny{"America/New_York", now};
zoned_time tokyo{"Asia/Tokyo", now};
// ny and tokyo refer to the SAME instant, shown in different local times.

// Look up a zone explicitly and convert:
const time_zone* tz = locate_zone("Europe/London");
local_time<system_clock::duration> london = tz->to_local(now);
sys_time<system_clock::duration>   utc    = tz->to_sys(london);

// The user's local zone:
zoned_time local{current_zone(), now};
```

`zoned_time{"America/New_York", now}` does not change the instant — it attaches a zone for display, so converting an instant between zones is a *view* operation, not a mutation. The library consults the tz database for the correct UTC offset *at that instant*, so DST and historical rule changes are handled automatically. `locate_zone(name)` fetches a `time_zone*`, `current_zone()` returns the system's zone, and `to_local`/`to_sys` do the explicit conversions. A caveat: the tz database must be available on the platform (some standard libraries require linking a tz data component, e.g. older libstdc++ needed a separate build flag).

---

## 47.7 Formatting and Parsing Dates

The calendar and zoned types integrate with `std::format` (Chapter 46) through `strftime`-style `%` specifiers, and `std::chrono::parse` reads dates back from text.

```cpp
// Listing 47.6: formatting and parsing chrono types
#include <chrono>
#include <format>
#include <sstream>
using namespace std::chrono;

year_month_day d = 2026y / June / 18;

std::string iso  = std::format("{:%Y-%m-%d}", d);          // "2026-06-18"
std::string full = std::format("{:%A, %B %d, %Y}", d);     // "Thursday, June 18, 2026"

zoned_time zt{"Asia/Tokyo", system_clock::now()};
std::string z = std::format("{:%Y-%m-%d %H:%M:%S %Z}", zt); // includes zone abbrev

// Default formatting (no spec) also works:
std::string def = std::format("{}", d);                     // "2026-06-18"

// Parsing text back into a chrono type:
std::istringstream in{"2026-06-18"};
year_month_day parsed;
in >> parse("%Y-%m-%d", parsed);                            // read using a format
```

The format specifiers mirror C's `strftime` (`%Y` year, `%m` month, `%d` day, `%H:%M:%S` time, `%A`/`%B` names, `%Z` zone, `%z` offset) but are type-checked against the chrono type by `std::format`. `std::chrono::parse`, used with `operator>>`, reads a date/time from a stream according to a format string, replacing the brittle `strptime`. Together these close the loop: construct, compute, format, and re-parse dates entirely within the type system.

---

## 47.8 Professional Insights

**Abandon `<ctime>`/`struct tm` for all new date code.** The C time API is mutable, not thread-safe (`localtime` returns a shared static buffer), zone-naive, and riddled with off-by-one traps (0-based months, year-minus-1900). C++20 `<chrono>` replaces every part of it with strongly-typed, thread-safe, zone-aware, `constexpr`-capable types. The only reason to touch `struct tm` now is interop with a legacy API at a boundary, and even there you should convert in and out of chrono types immediately.

**Choose field arithmetic vs serial arithmetic deliberately.** `ymd + months{1}` preserves day-of-month and can produce an invalid date (Jan 31 + 1 month = Feb 31), which you must check with `.ok()` and normalize — this is correct for "same day next month" billing semantics. `sys_days{ymd} + days{n}` is always exact and never invalid — correct for elapsed-time calculations. Picking the wrong model silently produces wrong dates around month-ends; decide which semantics you mean and use the matching operation, validating `ok()` after any field arithmetic.

**Treat `zoned_time` as a display view, never a stored instant.** Store and compute with UTC (`system_clock` time points / `sys_time`); attach a `zoned_time` only at the presentation boundary. The same instant displayed in two zones is two `zoned_time`s over one time point — converting between zones must not change the underlying instant. Storing local times invites the classic DST bugs (ambiguous or nonexistent wall-clock times during transitions); keep the source of truth in UTC and let the tz database handle offsets at display time.

**Verify the tz database is present on every target platform.** The IANA database access (`locate_zone`, `zoned_time`, `current_zone`) depends on the platform shipping or linking tz data; some standard-library versions (notably older libstdc++) require a specific build configuration or a separate tzdata component, and a missing database throws at runtime. Test zone lookups on each deployment target rather than assuming the calendar-only features (which need no database) imply zone support.

**Use `std::format` `%` specifiers and `std::chrono::parse` instead of `strftime`/`strptime`.** The chrono formatting integration is type-checked, thread-safe, and locale-aware, and `parse` round-trips text back into strongly-typed chrono values. This keeps the entire construct-compute-format-reparse cycle inside the type system, eliminating the format-string/argument mismatches and the shared-static-buffer hazards of the C functions — and it composes with custom `std::formatter`s for your own timestamped types.
