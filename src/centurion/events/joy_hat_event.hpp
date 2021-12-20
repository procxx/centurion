#ifndef CENTURION_EVENTS_JOY_HAT_EVENT_HPP_
#define CENTURION_EVENTS_JOY_HAT_EVENT_HPP_

#include <SDL.h>

#include "../common.hpp"
#include "common_event.hpp"
#include "joy_hat_position.hpp"

namespace cen {

/// \addtogroup event
/// \{

/**
 * \class joy_hat_event
 *
 * \brief Represents an event that is triggered whenever a user moves a hat
 * on a joystick.
 *
 * \see `SDL_JoyHatEvent`
 *
 * \since 4.0.0
 */
class joy_hat_event final : public common_event<SDL_JoyHatEvent> {
 public:
  /**
   * \brief Creates a joy hat event.
   *
   * \since 4.0.0
   */
  joy_hat_event() noexcept : common_event{EventType::JoyHatMotion} {}

  /**
   * \brief Creates a joy hat event based on the supplied SDL event.
   *
   * \param event the SDL event that will be copied.
   *
   * \since 4.0.0
   */
  explicit joy_hat_event(const SDL_JoyHatEvent& event) noexcept : common_event{event} {}

  /**
   * \brief Sets the hat index associated with the event.
   *
   * \param hat the hat index.
   *
   * \since 4.0.0
   */
  void set_hat(const Uint8 hat) noexcept { m_event.hat = hat; }

  /**
   * \brief Sets the joystick hat position associated with the event.
   *
   * \param value the joystick hat position associated with the event.
   *
   * \since 4.0.0
   */
  void set_position(const joy_hat_position value) noexcept
  {
    m_event.value = ToUnderlying(value);
  }

  /**
   * \brief Returns the index of the hat that changed.
   *
   * \return the index of the hat that changed.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto hat() const noexcept -> Uint8 { return m_event.hat; }

  /**
   * \brief Returns the position of the associated joystick hat.
   *
   * \return the position of the associated joystick hat.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto position() const noexcept -> joy_hat_position
  {
    return static_cast<joy_hat_position>(m_event.value);
  }
};

/// \name SDL event conversions
/// \{

template <>
inline auto as_sdl_event(const common_event<SDL_JoyHatEvent>& event) -> SDL_Event
{
  SDL_Event e;
  e.jhat = event.get();
  return e;
}

/// \} End of SDL event conversions

/// \} End of group event

}  // namespace cen

#endif  // CENTURION_EVENTS_JOY_HAT_EVENT_HPP_
