#ifndef CENTURION_EVENTS_SENSOR_EVENT_HPP_
#define CENTURION_EVENTS_SENSOR_EVENT_HPP_

#include <SDL.h>

#include <array>  // array

#include "../common.hpp"
#include "../detail/array_utils.hpp"
#include "event_base.hpp"

namespace cen {

/// \addtogroup event
/// \{

/**
 * \class sensor_event
 *
 * \brief Represents events related to updates of sensors.
 *
 * \since 6.3.0
 */
class sensor_event final : public EventBase<SDL_SensorEvent> {
 public:
  using data_type = std::array<float, 6>;

  /**
   * \brief Creates a sensor event.
   *
   * \since 6.3.0
   */
  sensor_event() noexcept : EventBase{EventType::SensorUpdate} {}

  /**
   * \brief Creates a sensor event based on an SDL event.
   *
   * \param event the event that will be copied.
   *
   * \since 6.3.0
   */
  explicit sensor_event(const SDL_SensorEvent& event) noexcept : EventBase{event} {}

  /**
   * \brief Sets the sensor instance ID associated with the event.
   *
   * \param id the sensor instance ID.
   *
   * \since 6.3.0
   */
  void set_which(const Sint32 id) noexcept { mEvent.which = id; }

  /**
   * \brief Sets the sensor values associated with the event.
   *
   * \param values the sensor values.
   *
   * \since 6.3.0
   */
  void set_data(const data_type& values) { detail::assign(values, mEvent.data); }

  /**
   * \brief Returns the instance ID of the associated sensor.
   *
   * \return the associated sensor instance ID.
   *
   * \since 6.3.0
   */
  [[nodiscard]] auto which() const noexcept -> Sint32 { return mEvent.which; }

  /**
   * \brief Returns up to 6 values from the sensor.
   *
   * \return values from the sensor.
   *
   * \see `basic_sensor::data()`
   *
   * \since 6.3.0
   */
  [[nodiscard]] auto data() const -> data_type { return detail::to_array(mEvent.data); }
};

template <>
inline auto AsSDLEvent(const EventBase<SDL_SensorEvent>& event) -> SDL_Event
{
  SDL_Event e;
  e.sensor = event.get();
  return e;
}

/// \} End of group event

}  // namespace cen

#endif  // CENTURION_EVENTS_SENSOR_EVENT_HPP_
