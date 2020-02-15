#ifndef CENTURION_SYSTEM_SOURCE
#define CENTURION_SYSTEM_SOURCE

#include "system.h"

#include <string>

namespace centurion {
namespace system {

CENTURION_DEF Platform get_platform() noexcept {
  const std::string name{SDL_GetPlatform()};
  if (name == "Windows") {
    return Platform::Windows;
  } else if (name == "Mac OS X") {
    return Platform::MacOSX;
  } else if (name == "Linux") {
    return Platform::Linux;
  } else if (name == "iOS") {
    return Platform::Ios;
  } else if (name == "Android") {
    return Platform::Android;
  } else {
    return Platform::Unknown;
  }
}

#ifdef CENTURION_HAS_OPTIONAL

CENTURION_DEF std::optional<std::string> get_platform_name() noexcept {
  const std::string name{SDL_GetPlatform()};
  if (name == "Unknown") {
    return std::nullopt;
  } else {
    return name;
  }
}

#endif

CENTURION_DEF int CPU::get_cache_line_size() noexcept {
  return SDL_GetCPUCacheLineSize();
}

CENTURION_DEF int CPU::get_cores() noexcept { return SDL_GetCPUCount(); }

CENTURION_DEF int RAM::get_size_mb() noexcept { return SDL_GetSystemRAM(); }

CENTURION_DEF int RAM::get_size_gb() noexcept { return get_size_mb() / 1'000; }

#ifdef CENTURION_HAS_OPTIONAL

CENTURION_DEF std::optional<int> Power::get_battery_seconds_left() noexcept {
  int secondsLeft = -1;
  SDL_GetPowerInfo(&secondsLeft, nullptr);
  if (secondsLeft == -1) {
    return std::nullopt;
  } else {
    return secondsLeft;
  }
}

CENTURION_DEF std::optional<int> Power::get_battery_minutes_left() noexcept {
  const auto secondsLeft = get_battery_seconds_left();
  if (secondsLeft) {
    return *secondsLeft / 60;
  } else {
    return std::nullopt;
  }
}

CENTURION_DEF std::optional<int> Power::get_battery_percentage() noexcept {
  int percentageLeft = -1;
  SDL_GetPowerInfo(nullptr, &percentageLeft);
  if (percentageLeft == -1) {
    return std::nullopt;
  } else {
    return percentageLeft;
  }
}

#endif

// TODO rename Power to Battery and PowerState to BatteryState in version 4

CENTURION_DEF PowerState Power::get_state() noexcept {
  return static_cast<PowerState>(SDL_GetPowerInfo(nullptr, nullptr));
}

CENTURION_DEF int Screen::get_width() noexcept {
  SDL_DisplayMode mode;
  SDL_GetDesktopDisplayMode(0, &mode);
  return mode.w;
}

CENTURION_DEF int Screen::get_height() noexcept {
  SDL_DisplayMode mode;
  SDL_GetDesktopDisplayMode(0, &mode);
  return mode.h;
}

CENTURION_DEF int Screen::get_refresh_rate() noexcept {
  SDL_DisplayMode mode;
  SDL_GetDesktopDisplayMode(0, &mode);
  return mode.refresh_rate;
}

CENTURION_DEF uint32_t
Screen::get_pixel_format() noexcept {  // TODO replace return type
  SDL_DisplayMode mode;
  SDL_GetDesktopDisplayMode(0, &mode);
  return mode.format;
}

}  // namespace system
}  // namespace centurion

#endif  // CENTURION_SYSTEM_SOURCE