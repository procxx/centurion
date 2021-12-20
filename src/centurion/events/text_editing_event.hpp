#ifndef CENTURION_EVENTS_TEXT_EDITING_EVENT_HPP_
#define CENTURION_EVENTS_TEXT_EDITING_EVENT_HPP_

#include <SDL.h>

#include <string_view>  // string_view

#include "../common.hpp"
#include "../detail/stdlib.hpp"
#include "event_base.hpp"

namespace cen {

/// \addtogroup event
/// \{

/**
 * \class text_editing_event
 *
 * \brief Represents an event triggered by keyboard text editing.
 *
 * \note This class doesn't provide a setter for the text, since it's
 * impossible to assign a value to a variable of type `char[32]` in C++.
 *
 * \see `SDL_TextEditingEvent`
 *
 * \since 4.0.0
 */
class text_editing_event final : public EventBase<SDL_TextEditingEvent> {
 public:
  /**
   * \brief Creates a text editing event.
   *
   * \since 4.0.0
   */
  text_editing_event() noexcept : EventBase{EventType::TextEditing} { check_length(); }

  /**
   * \brief Creates an event that is based on the supplied SDL event.
   *
   * \param event the SDL event that will be copied.
   *
   * \since 4.0.0
   */
  explicit text_editing_event(const SDL_TextEditingEvent& event) noexcept : EventBase{event}
  {
    check_length();
  }

  /**
   * \brief Sets the window ID associated with the event.
   *
   * \param id the window ID associated with the event.
   *
   * \since 4.0.0
   */
  void set_window_id(const Uint32 id) noexcept { mEvent.windowID = id; }

  /**
   * \brief Sets the location to begin editing from.
   *
   * \param start the location to begin editing from.
   *
   * \since 4.0.0
   */
  void set_start(const Sint32 start) noexcept { mEvent.start = start; }

  /**
   * \brief Sets the number of characters to edit from the start point.
   *
   * \details The supplied value will be clamped to the range [0, 32].
   *
   * \param length the number of characters to edit from the start point.
   *
   * \since 4.0.0
   */
  void set_length(const Sint32 length) noexcept
  {
    mEvent.length = detail::clamp(length, 0, 32);
  }

  /**
   * \brief Returns the window ID associated with the event.
   *
   * \return the window ID associated with the event.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto window_id() const noexcept -> Uint32 { return mEvent.windowID; }

  /**
   * \brief Returns the text that will be used, as a null-terminated string in
   * UTF-8 encoding.
   *
   * \return the text that will be used.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto text() const noexcept -> std::string_view
  {
    return std::string_view{static_cast<const char*>(mEvent.text)};
  }

  /**
   * \brief Returns the location to begin editing from.
   *
   * \return the location to begin editing from.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto start() const noexcept -> Sint32 { return mEvent.start; }

  /**
   * \brief Returns the number of characters to edit from the start point.
   *
   * \details The returned value will always be in the range [0, 32].
   *
   * \return the number of characters to edit from the start point.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto length() const noexcept -> Sint32 { return mEvent.length; }

 private:
  void check_length() noexcept { mEvent.length = detail::clamp(mEvent.length, 0, 32); }
};

/// \name SDL event conversions
/// \{

template <>
inline auto AsSDLEvent(const EventBase<SDL_TextEditingEvent>& event) -> SDL_Event
{
  SDL_Event e;
  e.edit = event.get();
  return e;
}

/// \} End of SDL event conversions

/// \} End of group event

}  // namespace cen

#endif  // CENTURION_EVENTS_TEXT_EDITING_EVENT_HPP_
