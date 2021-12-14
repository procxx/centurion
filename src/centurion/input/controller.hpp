#ifndef CENTURION_INPUT_GAME_CONTROLLER_HPP_
#define CENTURION_INPUT_GAME_CONTROLLER_HPP_

#include <SDL.h>

#include <array>        // array
#include <cassert>      // assert
#include <cstddef>      // size_t
#include <optional>     // optional
#include <ostream>      // ostream
#include <string>       // string
#include <string_view>  // string_view

#include "../core/features.hpp"

#if CENTURION_HAS_FEATURE_FORMAT

#include <format>  // format

#endif  // CENTURION_HAS_FEATURE_FORMAT

#include "../core/common.hpp"
#include "../core/exception.hpp"
#include "../core/sdl_string.hpp"
#include "../core/time.hpp"
#include "../detail/owner_handle_api.hpp"
#include "../detail/sdl_version_at_least.hpp"
#include "../detail/stdlib.hpp"
#include "../video/color.hpp"
#include "button_state.hpp"
#include "joystick.hpp"
#include "sensor.hpp"
#include "touch.hpp"

namespace cen {

/// \addtogroup input
/// \{

/**
 * \brief Represents different game controller buttons.
 *
 * \see `SDL_GameControllerButton`
 *
 * \since 4.0.0
 */
enum class controller_button {
  invalid = SDL_CONTROLLER_BUTTON_INVALID,
  a = SDL_CONTROLLER_BUTTON_A,
  b = SDL_CONTROLLER_BUTTON_B,
  x = SDL_CONTROLLER_BUTTON_X,
  y = SDL_CONTROLLER_BUTTON_Y,
  back = SDL_CONTROLLER_BUTTON_BACK,
  guide = SDL_CONTROLLER_BUTTON_GUIDE,
  start = SDL_CONTROLLER_BUTTON_START,
  left_stick = SDL_CONTROLLER_BUTTON_LEFTSTICK,
  right_stick = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
  left_shoulder = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
  right_shoulder = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
  dpad_up = SDL_CONTROLLER_BUTTON_DPAD_UP,
  dpad_down = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
  dpad_left = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
  dpad_right = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,

#if SDL_VERSION_ATLEAST(2, 0, 14)

  // clang-format off
  misc1 = SDL_CONTROLLER_BUTTON_MISC1, ///< Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button
  // clang-format on

  paddle1 = SDL_CONTROLLER_BUTTON_PADDLE1,    ///< Xbox Elite paddle P1
  paddle2 = SDL_CONTROLLER_BUTTON_PADDLE2,    ///< Xbox Elite paddle P3
  paddle3 = SDL_CONTROLLER_BUTTON_PADDLE3,    ///< Xbox Elite paddle P2
  paddle4 = SDL_CONTROLLER_BUTTON_PADDLE4,    ///< Xbox Elite paddle P4
  touchpad = SDL_CONTROLLER_BUTTON_TOUCHPAD,  ///< PS4/PS5 touchpad button

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

  max = SDL_CONTROLLER_BUTTON_MAX
};

/**
 * \brief Represents different game controller axes.
 *
 * \see `SDL_GameControllerAxis`
 *
 * \since 4.0.0
 */
enum class controller_axis {
  invalid = SDL_CONTROLLER_AXIS_INVALID,
  left_x = SDL_CONTROLLER_AXIS_LEFTX,
  left_y = SDL_CONTROLLER_AXIS_LEFTY,
  right_x = SDL_CONTROLLER_AXIS_RIGHTX,
  right_y = SDL_CONTROLLER_AXIS_RIGHTY,
  trigger_left = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
  trigger_right = SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
  max = SDL_CONTROLLER_AXIS_MAX
};

/**
 * \brief Represents different game controller bind types.
 *
 * \see `SDL_GameControllerBindType`
 *
 * \since 5.0.0
 */
enum class controller_bind_type {
  none = SDL_CONTROLLER_BINDTYPE_NONE,
  button = SDL_CONTROLLER_BINDTYPE_BUTTON,
  axis = SDL_CONTROLLER_BINDTYPE_AXIS,
  hat = SDL_CONTROLLER_BINDTYPE_HAT
};

#if SDL_VERSION_ATLEAST(2, 0, 12)

/**
 * \brief Represents different game controller types.
 *
 * \see `SDL_GameControllerType`
 *
 * \since 5.0.0
 */
enum class controller_type {
  // clang-format off
  unknown = SDL_CONTROLLER_TYPE_UNKNOWN,   ///< An unknown controller.
  xbox_360 = SDL_CONTROLLER_TYPE_XBOX360,  ///< An Xbox 360 controller.
  xbox_one = SDL_CONTROLLER_TYPE_XBOXONE,  ///< An Xbox One controller.
  ps3 = SDL_CONTROLLER_TYPE_PS3,           ///< A PS3 controller.
  ps4 = SDL_CONTROLLER_TYPE_PS4,           ///< A PS4 controller.

#if SDL_VERSION_ATLEAST(2, 0, 14)

  ps5 = SDL_CONTROLLER_TYPE_PS5,       ///< A PS5 controller.
  virt = SDL_CONTROLLER_TYPE_VIRTUAL,  ///< A virtual controller.

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

#if SDL_VERSION_ATLEAST(2, 0, 16)

  amazon_luna = SDL_CONTROLLER_TYPE_AMAZON_LUNA,     ///< An Amazon Luna controller.
  google_stadia = SDL_CONTROLLER_TYPE_GOOGLE_STADIA, ///< A Google Stadia controller.

#endif // SDL_VERSION_ATLEAST(2, 0, 16)

  nintendo_switch_pro = SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO  ///< A Nintendo Switch Pro controller.
  // clang-format on
};

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)

template <typename T>
class basic_controller;

/**
 * \typedef controller
 *
 * \brief Represents an owning game controller.
 *
 * \since 5.0.0
 */
using controller = basic_controller<detail::OwnerTag>;

/**
 * \typedef controller_handle
 *
 * \brief Represents a non-owning game controller.
 *
 * \since 5.0.0
 */
using controller_handle = basic_controller<detail::HandleTag>;

/**
 * \class basic_controller
 *
 * \brief Represents a game controller, e.g. Xbox or Playstation controllers.
 *
 * \ownerhandle `controller`/`controller_handle`
 *
 * \details You may need to load appropriate game controller mappings before you can begin
 * using the game controller API with certain controllers. This can be accomplished using
 * the `cen::hint::controller::config_file` hint, or the `load_mappings()` and
 * `add_mapping()` functions.
 *
 * \details For a community managed database file of game controller mappings, see
 * `https://github.com/gabomdq/SDL_GameControllerDB` (if the link doesn’t work for some
 * reason, you should be able to find a copy in the Centurion test resources folder).
 *
 * \todo Centurion 7: Move `mapping_result` out of `basic_controller`.
 *
 * \since 5.0.0
 *
 * \see `controller`
 * \see `controller_handle`
 */
template <typename T>
class basic_controller final {
 public:
  using mapping_index = int;
  using joystick_index = int;
  using player_index = int;

  /**
   * \brief Used to indicate the result of adding controller mappings.
   *
   * \since 5.1.0
   */
  enum class mapping_result {
    error,    ///< Something went wrong.
    updated,  ///< Updated a previous mapping.
    added     ///< Successfully added a new mapping.
  };

  /// \name Construction
  /// \{

  // clang-format off

  /**
   * \brief Creates a game controller from an existing SDL game controller.
   *
   * \note Ownership of the supplied pointer is claimed if the joystick has
   * owning semantics.
   *
   * \param controller a pointer to the associated game controller.
   *
   * \since 5.0.0
   */
  explicit basic_controller(MaybeOwner<SDL_GameController*> controller) noexcept(detail::is_handle<T>)
      : m_controller{controller}
  {
    if constexpr (detail::is_owner<T>)
    {
      if (!m_controller) 
      {
        throw Error{"Cannot create controller from null pointer!"};
      }
    }
  }

  // clang-format on

  /**
   * \brief Creates a handle to an existing controller instance.
   *
   * \param owner the controller that owns the `SDL_GameController` pointer.
   *
   * \since 5.0.0
   */
  template <typename TT = T, detail::EnableHandle<TT> = 0>
  explicit basic_controller(const controller& owner) noexcept : m_controller{owner.get()}
  {}

  /**
   * \brief Attempts to create a game controller.
   *
   * \details The joystick index is the same as the device index passed to the
   * `joystick` constructor. The index passed as an argument refers to the
   * n'th game controller on the system.
   *
   * \note The supplied index is not the value which will identify the
   * controller in controller events. Instead, the joystick's instance id
   * (`SDL_JoystickID`) will be used.
   *
   * \remark This constructor is only available for owning game controllers.
   *
   * \param index the device index, can't be >= than the amount of number of
   * joysticks.
   *
   * \throws sdl_error if the game controller cannot be opened.
   *
   * \since 5.0.0
   */
  template <typename TT = T, detail::EnableOwner<TT> = 0>
  explicit basic_controller(const int index = 0) : m_controller{SDL_GameControllerOpen(index)}
  {
    if (!m_controller) {
      throw SDLError{};
    }
  }

  /**
   * \brief Creates a game controller from an existing joystick ID.
   *
   * \remark This function is only available for owning game controllers.
   *
   * \param id the identifier associated with the joystick to base the game
   * controller on.
   *
   * \return a game controller instance.
   *
   * \throws sdl_error if the game controller cannot be created.
   *
   * \since 5.0.0
   */
  template <typename TT = T, detail::EnableOwner<TT> = 0>
  [[nodiscard]] static auto from_joystick(const SDL_JoystickID id) -> basic_controller
  {
    if (auto* ptr = SDL_GameControllerFromInstanceID(id)) {
      return basic_controller{ptr};
    }
    else {
      throw SDLError{};
    }
  }

#if SDL_VERSION_ATLEAST(2, 0, 12)

  /**
   * \brief Creates a controller based on a player index.
   *
   * \param index the player index of the game controller.
   *
   * \return a game controller associated with the specified player index.
   *
   * \throws sdl_error if the game controller cannot be created.
   *
   * \since 5.0.0
   */
  template <typename TT = T, detail::EnableOwner<TT> = 0>
  [[nodiscard]] static auto from_index(const player_index index) -> basic_controller
  {
    if (auto* ptr = SDL_GameControllerFromPlayerIndex(index)) {
      return basic_controller{ptr};
    }
    else {
      throw SDLError{};
    }
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)

  /// \} End of construction

  /**
   * \brief Updates the state of all open game controllers.
   *
   * \note This is done automatically if game controller events are enabled.
   *
   * \since 5.0.0
   */
  static void update() { SDL_GameControllerUpdate(); }

  /**
   * \brief Indicates whether or not the specified value is usable as a
   * controller index.
   *
   * \param index the index that will be checked.
   *
   * \return `true` if the supplied index is supported; `false` otherwise.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto is_supported(const joystick_index index) noexcept -> bool
  {
    return SDL_IsGameController(index) == SDL_TRUE;
  }

  /**
   * \brief Sets whether or not game controller event polling is enabled.
   *
   * \details If this property is set to `false`, then you have to call
   * `update` by yourself.
   *
   * \param polling `true` to enable automatic game controller event polling;
   * `false` otherwise.
   *
   * \since 5.0.0
   */
  static void set_polling(const bool polling) noexcept
  {
    SDL_GameControllerEventState(polling ? SDL_ENABLE : SDL_DISABLE);
  }

  /**
   * \brief Indicates whether or not game controller event polling is enabled.
   *
   * \return `true` if game controller event polling is enabled; `false`
   * otherwise.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto is_polling() noexcept -> bool
  {
    return SDL_GameControllerEventState(SDL_QUERY);
  }

#if SDL_VERSION_ATLEAST(2, 0, 12)

  /**
   * \brief Sets the player index associated with the controller.
   *
   * \param index the player index that will be used.
   *
   * \since 5.0.0
   */
  void set_player_index(const player_index index) noexcept
  {
    SDL_GameControllerSetPlayerIndex(m_controller, index);
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)

#if SDL_VERSION_ATLEAST(2, 0, 16)

  /**
   * \brief Sends a packet of controller specific data.
   *
   * \param data the data that will be sent.
   * \param size the size of the data.
   *
   * \return `success` if the data was sent successfully; `failure` if the controller
   * or driver doesn't support effect packets.
   *
   * \since 6.2.0
   */
  auto send_effect(const void* data, const int size) -> Result
  {
    return SDL_GameControllerSendEffect(m_controller, data, size) == 0;
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 16)

  /// \name Button and axis functions
  /// \{

  /**
   * \brief Returns the button associated with the specified string.
   *
   * \param str the string that represents a controller button, e.g "a".
   *
   * \return a game controller button value.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto get_button(const char* str) noexcept -> controller_button
  {
    assert(str);
    return static_cast<controller_button>(SDL_GameControllerGetButtonFromString(str));
  }

  /**
   * \brief Returns the button associated with the specified string.
   *
   * \param str the string that represents a controller button, e.g "a".
   *
   * \return a game controller button value.
   *
   * \since 5.3.0
   */
  [[nodiscard]] static auto get_button(const std::string& str) noexcept -> controller_button
  {
    return get_button(str.c_str());
  }

  /**
   * \brief Returns a string representation of a controller axis.
   *
   * \param axis the controller axis that will be converted.
   *
   * \return a string that represents the axis, might be null.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto stringify(const controller_axis axis) noexcept -> const char*
  {
    return SDL_GameControllerGetStringForAxis(static_cast<SDL_GameControllerAxis>(axis));
  }

  /**
   * \brief Returns a string representation of a controller button.
   *
   * \param button the controller button that will be converted.
   *
   * \return a string that represents the button, might be null.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto stringify(const controller_button button) noexcept -> const char*
  {
    return SDL_GameControllerGetStringForButton(static_cast<SDL_GameControllerButton>(button));
  }

  /**
   * \brief Returns the bindings for a controller axis.
   *
   * \param axis the axis of the bindings.
   *
   * \return the bindings for a controller axis; `std::nullopt` on failure.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto get_binding(const controller_axis axis) const
      -> std::optional<SDL_GameControllerButtonBind>
  {
    const auto result =
        SDL_GameControllerGetBindForAxis(m_controller,
                                         static_cast<SDL_GameControllerAxis>(axis));
    if (result.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
      return result;
    }
    else {
      return std::nullopt;
    }
  }

  /**
   * \brief Returns the bindings for a controller button.
   *
   * \param button the button of the bindings.
   *
   * \return the bindings for a controller button; `std::nullopt` on failure.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto get_binding(const controller_button button) noexcept
      -> std::optional<SDL_GameControllerButtonBind>
  {
    const auto result =
        SDL_GameControllerGetBindForButton(m_controller,
                                           static_cast<SDL_GameControllerButton>(button));
    if (result.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
      return result;
    }
    else {
      return std::nullopt;
    }
  }

  /**
   * \brief Returns the state of the specified game controller button.
   *
   * \param button the button that will be checked.
   *
   * \return the current button state of the specified button.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto get_state(const controller_button button) const noexcept -> button_state
  {
    const auto state =
        SDL_GameControllerGetButton(m_controller,
                                    static_cast<SDL_GameControllerButton>(button));
    return static_cast<button_state>(state);
  }

  /**
   * \brief Indicates if the specified button is pressed.
   *
   * \param button the button that will be checked.
   *
   * \return `true` if the specified button is pressed; `false` otherwise.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto is_pressed(const controller_button button) const noexcept -> bool
  {
    return get_state(button) == button_state::pressed;
  }

  /**
   * \brief Indicates if the specified button is released.
   *
   * \param button the button that will be checked.
   *
   * \return `true` if the specified button is released; `false` otherwise.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto is_released(const controller_button button) const noexcept -> bool
  {
    return get_state(button) == button_state::released;
  }

  /**
   * \brief Returns the axis associated with the specified string.
   *
   * \note You don't need this function unless you are parsing game controller
   * mappings by yourself.
   *
   * \param str the string that represents a game controller axis, e.g "rightx".
   *
   * \return a game controller axis value.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto get_axis(const char* str) noexcept -> controller_axis
  {
    assert(str);
    return static_cast<controller_axis>(SDL_GameControllerGetAxisFromString(str));
  }

  /**
   * \brief Returns the axis associated with the specified string.
   *
   * \note You don't need this function unless you are parsing game controller
   * mappings by yourself.
   *
   * \param str the string that represents a game controller axis, e.g "rightx".
   *
   * \return a game controller axis value.
   *
   * \since 5.3.0
   */
  [[nodiscard]] static auto get_axis(const std::string& str) noexcept -> controller_axis
  {
    return get_axis(str.c_str());
  }

  /**
   * \brief Returns the value of the specified axis.
   *
   * \param axis the controller axis that will be checked.
   *
   * \return the current value of the specified axis.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto get_axis(const controller_axis axis) const noexcept -> Sint16
  {
    return SDL_GameControllerGetAxis(m_controller, static_cast<SDL_GameControllerAxis>(axis));
  }

#if SDL_VERSION_ATLEAST(2, 0, 14)

  /**
   * \brief Indicates whether or not the controller has the specified axis.
   *
   * \return `true` if the controller has the specified axis; `false` otherwise.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto has_axis(const controller_axis axis) const noexcept -> bool
  {
    const auto value = static_cast<SDL_GameControllerAxis>(axis);
    return SDL_GameControllerHasAxis(m_controller, value) == SDL_TRUE;
  }

  /**
   * \brief Indicates whether or not the controller has the specified button.
   *
   * \param button the button that will be checked.
   *
   * \return `true` if the controller features the specified button; `false`
   * otherwise.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto has_button(const controller_button button) const noexcept -> bool
  {
    const auto value = static_cast<SDL_GameControllerButton>(button);
    return SDL_GameControllerHasButton(m_controller, value) == SDL_TRUE;
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

  /// \} End of button and axis functions

  /// \name Rumble functions
  /// \{

  /**
   * \brief Starts a rumble effect.
   *
   * \details Calls to this function cancels any previously active rumble
   * effect. Furthermore, supplying 0 as intensities will stop the rumble
   * effect.
   *
   * \note This function has no effect if rumbling isn't supported by the
   * controller.
   *
   * \param lo the intensity of the low frequency motor.
   * \param hi the intensity of the high frequency motor.
   * \param duration the duration of the rumble effect.
   *
   * \return `success` if the rumble is successful; `failure` otherwise.
   *
   * \since 5.0.0
   */
  auto rumble(const Uint16 lo,
              const Uint16 hi,
              const milliseconds<Uint32> duration) noexcept(noexcept(duration.count()))
      -> Result
  {
    return SDL_GameControllerRumble(m_controller, lo, hi, duration.count()) == 0;
  }

#if SDL_VERSION_ATLEAST(2, 0, 14)

  // clang-format off

  /**
   * \brief Starts a rumble effect in the controller's triggers.
   *
   * \details Calls to this function cancels any previously active rumble
   * effect. Furthermore, supplying 0 as intensities will stop the rumble
   * effect.
   *
   * \note This function has no effect if rumbling isn't supported by the
   * controller.
   *
   * \param lo the intensity of the low frequency motor.
   * \param hi the intensity of the high frequency motor.
   * \param duration the duration of the rumble effect.
   *
   * \return `success` if the rumble is successful; `failure` otherwise.
   *
   * \since 5.2.0
   */
  auto rumble_triggers(const Uint16 lo,
                       const Uint16 hi,
                       const milliseconds<Uint32> duration) noexcept(noexcept(duration.count()))
      -> Result
  {
    return SDL_GameControllerRumbleTriggers(m_controller, lo, hi, duration.count()) == 0;
  }

  // clang-format on

#endif  // SDL_VERSION(2, 0, 14)

  /**
   * \brief Stops any currently active rumble effect.
   *
   * \since 5.0.0
   */
  void stop_rumble() noexcept { rumble(0, 0, milliseconds<Uint32>::zero()); }

  /// \} End of rumble functions

  /// \name Queries
  /// \{

  /**
   * \brief Returns the USB product ID of the controller.
   *
   * \return the USB product ID; `std::nullopt` if the product ID isn't
   * available.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto product() const noexcept -> std::optional<Uint16>
  {
    const auto id = SDL_GameControllerGetProduct(m_controller);
    if (id != 0) {
      return id;
    }
    else {
      return std::nullopt;
    }
  }

  /**
   * \brief Returns the USB vendor ID of the controller.
   *
   * \return the USB vendor ID; `std::nullopt` if the vendor ID isn't available.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto vendor() const noexcept -> std::optional<Uint16>
  {
    const auto id = SDL_GameControllerGetVendor(m_controller);
    if (id != 0) {
      return id;
    }
    else {
      return std::nullopt;
    }
  }

  /**
   * \brief Returns the product version of the controller.
   *
   * \return the product version; `std::nullopt` if the product version isn't
   * available.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto product_version() const noexcept -> std::optional<Uint16>
  {
    const auto id = SDL_GameControllerGetProductVersion(m_controller);
    if (id != 0) {
      return id;
    }
    else {
      return std::nullopt;
    }
  }

#if SDL_VERSION_ATLEAST(2, 0, 14)

  /**
   * \brief Returns the serial number associated with the controller.
   *
   * \return the serial number associated with the controller; a null pointer if
   * no serial number is available.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto serial() const noexcept -> const char*
  {
    return SDL_GameControllerGetSerial(m_controller);
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

  /**
   * \brief Returns the player index associated with the controller.
   *
   * \note If this is an XInput controller, the returned value is the user
   * index.
   *
   * \return the player index associated with the controller; `std::nullopt`
   * if the index isn't available.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto index() const noexcept -> std::optional<player_index>
  {
    const auto result = SDL_GameControllerGetPlayerIndex(m_controller);
    if (result != -1) {
      return result;
    }
    else {
      return std::nullopt;
    }
  }

  /**
   * \brief Indicates whether or not the game controller is currently connected.
   *
   * \return `true` if the game controller is connected; `false` otherwise.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto is_connected() const noexcept -> bool
  {
    return SDL_GameControllerGetAttached(m_controller) == SDL_TRUE;
  }

  /**
   * \brief Returns the name associated with the game controller.
   *
   * \note This function might return a null pointer if there is no name
   * associated with the game controller.
   *
   * \return the name of the game controller, might be null.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto name() const noexcept -> const char*
  {
    return SDL_GameControllerName(m_controller);
  }

#if SDL_VERSION_ATLEAST(2, 0, 12)

  /**
   * \brief Returns the type of the controller.
   *
   * \return the type of the controller.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto type() const noexcept -> controller_type
  {
    return static_cast<controller_type>(SDL_GameControllerGetType(m_controller));
  }

  /**
   * \brief Returns the type of the controller associated with the specified
   * joystick index.
   *
   * \param index the joystick index of the desired game controller.
   *
   * \return the type of the game controller associated with the index.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto type(const joystick_index index) noexcept -> controller_type
  {
    return static_cast<controller_type>(SDL_GameControllerTypeForIndex(index));
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)

  /**
   * \brief Returns a handle to the associated joystick.
   *
   * \return a handle to the associated joystick.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto get_joystick() noexcept -> joystick_handle
  {
    return joystick_handle{SDL_GameControllerGetJoystick(m_controller)};
  }

  /**
   * \brief Returns the amount of available game controllers on the system.
   *
   * \return the amount of available game controllers.
   *
   * \since 5.2.0
   */
  [[nodiscard]] static auto count() noexcept -> int
  {
    const auto joysticks = SDL_NumJoysticks();

    auto amount = 0;
    for (auto i = 0; i < joysticks; ++i) {
      if (is_supported(i)) {
        ++amount;
      }
    }

    return amount;
  }

  /**
   * \brief Returns a pointer to the associated SDL game controller.
   *
   * \warning Don't take ownership of the returned pointer!
   *
   * \return a pointer to the associated SDL game controller.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto get() const noexcept -> SDL_GameController* { return m_controller.get(); }

  /// \} End of queries

  /// \name Touchpad functions
  /// \{

#if SDL_VERSION_ATLEAST(2, 0, 14)

  /**
   * \brief Returns the amount of touchpads on the controller.
   *
   * \return the amount of touchpads on the controller.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto touchpad_count() const noexcept -> int
  {
    return SDL_GameControllerGetNumTouchpads(m_controller);
  }

  /**
   * \brief Returns the amount of supported simultaneous fingers for a touchpad.
   *
   * \param touchpad the index associated with the touchpad that will be
   * queried.
   *
   * \return the maximum amount of supported simultaneous fingers for the
   * specified touchpad.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto touchpad_finger_capacity(const int touchpad) const noexcept -> int
  {
    return SDL_GameControllerGetNumTouchpadFingers(m_controller, touchpad);
  }

  /**
   * \brief Returns the state of a finger on a touchpad.
   *
   * \param touchpad the touchpad to query.
   * \param finger the index of the finger that will be queried.
   *
   * \return the current state of a touchpad finger; `std::nullopt` if something
   * goes wrong.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto touchpad_finger_state(const int touchpad, const int finger) const noexcept
      -> std::optional<touch::finger_state>
  {
    touch::finger_state result{};
    Uint8 state{};

    const auto res = SDL_GameControllerGetTouchpadFinger(m_controller,
                                                         touchpad,
                                                         finger,
                                                         &state,
                                                         &result.x,
                                                         &result.y,
                                                         &result.pressure);
    result.state = static_cast<button_state>(state);

    if (res != -1) {
      return result;
    }
    else {
      return std::nullopt;
    }
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

  /// \} End of touchpad functions

  /// \name Sensor functions
  /// \{

#if SDL_VERSION_ATLEAST(2, 0, 14)

  /**
   * \brief Sets whether or not data reporting is enabled for a sensor.
   *
   * \param type the type of the sensor that will be changed.
   * \param enabled `true` if data reporting should be enabled; `false`
   * otherwise.
   *
   * \return `success` if the sensor was successfully enabled or disabled;
   * `failure` otherwise.
   *
   * \since 5.2.0
   */
  auto set_sensor_enabled(const sensor_type type, const bool enabled) noexcept -> Result
  {
    const auto value = static_cast<SDL_SensorType>(type);
    const auto state = enabled ? SDL_TRUE : SDL_FALSE;
    return SDL_GameControllerSetSensorEnabled(m_controller, value, state) == 0;
  }

  /**
   * \brief Indicates whether or not the controller has a sensor.
   *
   * \param type the type of the sensor to look for.
   *
   * \return `true` if the controller has the specified sensor; `false`
   * otherwise.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto has_sensor(const sensor_type type) const noexcept -> bool
  {
    const auto value = static_cast<SDL_SensorType>(type);
    return SDL_GameControllerHasSensor(m_controller, value) == SDL_TRUE;
  }

  /**
   * \brief Indicates whether or not data reporting is enabled for a sensor.
   *
   * \param type the type of the sensor that will be queried.
   *
   * \return `true` if data reporting is enabled for the sensor; `false`
   * otherwise.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto is_sensor_enabled(const sensor_type type) const noexcept -> bool
  {
    const auto value = static_cast<SDL_SensorType>(type);
    return SDL_GameControllerIsSensorEnabled(m_controller, value) == SDL_TRUE;
  }

  /**
   * \brief Returns the state of the specified sensor.
   *
   * \tparam Size the amount of data elements.
   *
   * \param type the type of the sensor that will be queried.
   *
   * \return the sensor data; `std::nullopt` if something went wrong.
   *
   * \since 5.2.0
   */
  template <std::size_t Size>
  [[nodiscard]] auto get_sensor_data(const sensor_type type) const noexcept
      -> std::optional<std::array<float, Size>>
  {
    std::array<float, Size> array{};
    if (SDL_GameControllerGetSensorData(m_controller,
                                        static_cast<SDL_SensorType>(type),
                                        array.data(),
                                        isize(array)) != -1) {
      return array;
    }
    else {
      return std::nullopt;
    }
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

#if SDL_VERSION_ATLEAST(2, 0, 16)

  /**
   * \brief Returns the data rate of a controller sensor, i.e. the number of supported
   * events per second.
   *
   * \param type the sensor type that will be queried.
   *
   * \return the data rate (may be zero if the data rate isn't available).
   *
   * \since 6.2.0
   */
  [[nodiscard]] auto get_sensor_data_rate(const sensor_type type) const noexcept -> float
  {
    return SDL_GameControllerGetSensorDataRate(m_controller,
                                               static_cast<SDL_SensorType>(type));
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 16)

  /// \} End of sensor functions

  /// \name LED functions
  /// \{

#if SDL_VERSION_ATLEAST(2, 0, 14)

  /**
   * \brief Sets the color of the associated LED light.
   *
   * \param color the new color of the controller's LED.
   *
   * \return `success` if the color of the LED was set; `failure` otherwise.
   *
   * \since 5.2.0
   */
  auto set_led(const Color& color) noexcept -> Result
  {
    return SDL_GameControllerSetLED(m_controller,
                                    color.GetRed(),
                                    color.GetGreen(),
                                    color.GetBlue()) == 0;
  }

  /**
   * \brief Indicates whether or not the controller features a LED light.
   *
   * \return `true` if the controller features a LED light; `false` otherwise.
   *
   * \since 5.2.0
   */
  [[nodiscard]] auto has_led() const noexcept -> bool
  {
    return SDL_GameControllerHasLED(m_controller) == SDL_TRUE;
  }

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

  /// \} End of LED functions

  /// \name Mapping functions
  /// \{

  /**
   * \brief Adds a game controller mapping.
   *
   * \param mapping the string that encodes the game controller mapping.
   *
   * \return `added` if a new mapping was added; `updated` if a previous mapping
   * was updated; `error` if something went wrong.
   *
   * \since 5.0.0
   */
  static auto add_mapping(const char* mapping) noexcept -> mapping_result
  {
    assert(mapping);
    const auto result = SDL_GameControllerAddMapping(mapping);
    if (result == 1) {
      return mapping_result::added;
    }
    else if (result == 0) {
      return mapping_result::updated;
    }
    else {
      return mapping_result::error;
    }
  }

  /**
   * \brief Adds a game controller mapping.
   *
   * \param mapping the string that encodes the game controller mapping.
   *
   * \return `added` if a new mapping was added; `updated` if a previous mapping
   * was updated; `error` if something went wrong.
   *
   * \since 5.3.0
   */
  static auto add_mapping(const std::string& mapping) noexcept -> mapping_result
  {
    return add_mapping(mapping.c_str());
  }

  /**
   * \brief Loads a set of game controller mappings from a file.
   *
   * \details A collection of game controller mappings can be found at <a
   * href="https://github.com/gabomdq/SDL_GameControllerDB">here</a>. New
   * mappings for previously known GUIDs will overwrite the previous mappings.
   * Furthermore, mappings for different platforms than the current platform
   * will be ignored.
   *
   * \remarks It's possible to call this function several times to use multiple
   * mapping files.
   *
   * \note The text database is stored entirely in memory during processing.
   *
   * \param file the path of the mapping file.
   *
   * \return the amount of mappings added; `std::nullopt` if something went
   * wrong.
   *
   * \since 5.0.0
   */
  static auto load_mappings(const char* file) noexcept -> std::optional<int>
  {
    assert(file);
    const auto result = SDL_GameControllerAddMappingsFromFile(file);
    if (result != -1) {
      return result;
    }
    else {
      return std::nullopt;
    }
  }

  /**
   * \brief Loads a set of game controller mappings from a file.
   *
   * \details A collection of game controller mappings can be found at <a
   * href="https://github.com/gabomdq/SDL_GameControllerDB">here</a>. New mappings for
   * previously known GUIDs will overwrite the previous mappings. Furthermore, mappings
   * for different platforms than the current platform will be ignored.
   *
   * \remarks It's possible to call this function several times to use multiple mapping
   * files.
   *
   * \note The text database is stored entirely in memory during processing.
   *
   * \param file the path of the mapping file.
   *
   * \return the amount of mappings added; `std::nullopt` if something went wrong.
   *
   * \since 5.3.0
   */
  static auto load_mappings(const std::string& file) noexcept -> std::optional<int>
  {
    return load_mappings(file.c_str());
  }

  /**
   * \brief Returns the mapping associated with the controller.
   *
   * \return the mapping string associated with the controller.
   *
   * \since 5.0.0
   */
  [[nodiscard]] auto mapping() const noexcept -> sdl_string
  {
    return sdl_string{SDL_GameControllerMapping(m_controller)};
  }

  /**
   * \brief Returns the mapping associated with a game controller.
   *
   * \param index the joystick index of the desired game controller.
   *
   * \return the mapping string associated with a controller.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto mapping(const joystick_index index) noexcept -> sdl_string
  {
    return sdl_string{SDL_GameControllerMappingForDeviceIndex(index)};
  }

  /**
   * \brief Returns the mapping string associated with a joystick GUID.
   *
   * \param guid the GUID to obtain the mapping for.
   *
   * \return the mapping string for a GUID:
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto mapping(const SDL_JoystickGUID guid) noexcept -> sdl_string
  {
    return sdl_string{SDL_GameControllerMappingForGUID(guid)};
  }

  /**
   * \brief Returns the mapping at a specific index.
   *
   * \param index the index of the desired mapping.
   *
   * \return the mapping at the specified index.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto mapping_by_index(const mapping_index index) noexcept -> sdl_string
  {
    return sdl_string{SDL_GameControllerMappingForIndex(index)};
  }

  /**
   * \brief Returns the number of installed mappings.
   *
   * \return the amount of installed mappings.
   *
   * \since 5.0.0
   */
  [[nodiscard]] static auto mapping_count() noexcept -> int
  {
    return SDL_GameControllerNumMappings();
  }

  /// \} End of mapping functions

  /// \name Conversions
  /// \{

  /**
   * \brief Indicates whether or not the handle contains a non-null pointer.
   *
   * \return `true` if the handle holds a non-null pointer; `false` otherwise.
   *
   * \since 5.0.0
   */
  template <typename TT = T, detail::EnableHandle<TT> = 0>
  explicit operator bool() const noexcept
  {
    return m_controller != nullptr;
  }

  /// \} End of conversions

 private:
  detail::Pointer<T, SDL_GameController> m_controller;
};

/// \name String conversions
/// \{

/**
 * \brief Returns a textual version of the supplied controller button.
 *
 * \details This function returns a string that mirrors the name of the enumerator, e.g.
 * `to_string(controller_button::start) == "start"`.
 *
 * \param button the controller button that will be converted.
 *
 * \return a string that mirrors the name of the enumerator.
 *
 * \throws cen_error if the enumerator is not recognized.
 *
 * \since 6.2.0
 */
[[nodiscard]] constexpr auto to_string(const controller_button button) -> std::string_view
{
  switch (button) {
    case controller_button::invalid:
      return "invalid";

    case controller_button::a:
      return "a";

    case controller_button::b:
      return "b";

    case controller_button::x:
      return "x";

    case controller_button::y:
      return "y";

    case controller_button::back:
      return "back";

    case controller_button::guide:
      return "guide";

    case controller_button::start:
      return "start";

    case controller_button::left_stick:
      return "left_stick";

    case controller_button::right_stick:
      return "right_stick";

    case controller_button::left_shoulder:
      return "left_shoulder";

    case controller_button::right_shoulder:
      return "right_shoulder";

    case controller_button::dpad_up:
      return "dpad_up";

    case controller_button::dpad_down:
      return "dpad_down";

    case controller_button::dpad_left:
      return "dpad_left";

    case controller_button::dpad_right:
      return "dpad_right";

#if SDL_VERSION_ATLEAST(2, 0, 14)

    case controller_button::misc1:
      return "misc1";

    case controller_button::paddle1:
      return "paddle1";

    case controller_button::paddle2:
      return "paddle2";

    case controller_button::paddle3:
      return "paddle3";

    case controller_button::paddle4:
      return "paddle4";

    case controller_button::touchpad:
      return "touchpad";

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

    case controller_button::max:
      return "max";

    default:
      throw Error{"Did not recognize controller button!"};
  }
}

/**
 * \brief Returns a textual version of the supplied controller axis.
 *
 * \details This function returns a string that mirrors the name of the enumerator, e.g.
 * `to_string(controller_axis::right_x) == "right_x"`.
 *
 * \param axis the controller axis that will be converted.
 *
 * \return a string that mirrors the name of the enumerator.
 *
 * \throws cen_error if the enumerator is not recognized.
 *
 * \since 6.2.0
 */
[[nodiscard]] constexpr auto to_string(const controller_axis axis) -> std::string_view
{
  switch (axis) {
    case controller_axis::invalid:
      return "invalid";

    case controller_axis::left_x:
      return "left_x";

    case controller_axis::left_y:
      return "left_y";

    case controller_axis::right_x:
      return "right_x";

    case controller_axis::right_y:
      return "right_y";

    case controller_axis::trigger_left:
      return "trigger_left";

    case controller_axis::trigger_right:
      return "trigger_right";

    case controller_axis::max:
      return "max";

    default:
      throw Error{"Did not recognize controller axis!"};
  }
}

/**
 * \brief Returns a textual version of the supplied controller bind type.
 *
 * \details This function returns a string that mirrors the name of the enumerator, e.g.
 * `to_string(controller_bind_type::button) == "button"`.
 *
 * \param type the controller bind type that will be converted.
 *
 * \return a string that mirrors the name of the enumerator.
 *
 * \throws cen_error if the enumerator is not recognized.
 *
 * \since 6.2.0
 */
[[nodiscard]] constexpr auto to_string(const controller_bind_type type) -> std::string_view
{
  switch (type) {
    case controller_bind_type::none:
      return "none";

    case controller_bind_type::button:
      return "button";

    case controller_bind_type::axis:
      return "axis";

    case controller_bind_type::hat:
      return "hat";

    default:
      throw Error{"Did not recognzie controller bind type!"};
  }
}

#if SDL_VERSION_ATLEAST(2, 0, 12)

/**
 * \brief Returns a textual version of the supplied controller type.
 *
 * \details This function returns a string that mirrors the name of the enumerator, e.g.
 * `to_string(controller_type::ps4) == "ps4"`.
 *
 * \param type the controller type that will be converted.
 *
 * \return a string that mirrors the name of the enumerator.
 *
 * \throws cen_error if the enumerator is not recognized.
 *
 * \since 6.2.0
 */
[[nodiscard]] constexpr auto to_string(const controller_type type) -> std::string_view
{
  switch (type) {
    case controller_type::unknown:
      return "unknown";

    case controller_type::nintendo_switch_pro:
      return "nintendo_switch_pro";

    case controller_type::xbox_360:
      return "xbox_360";

    case controller_type::xbox_one:
      return "xbox_one";

    case controller_type::ps3:
      return "ps3";

    case controller_type::ps4:
      return "ps4";

#if SDL_VERSION_ATLEAST(2, 0, 14)

    case controller_type::ps5:
      return "ps5";

    case controller_type::virt:
      return "virt";

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

#if SDL_VERSION_ATLEAST(2, 0, 16)

    case controller_type::amazon_luna:
      return "amazon_luna";

    case controller_type::google_stadia:
      return "google_stadia";

#endif  // SDL_VERSION_ATLEAST(2, 0, 16)

    default:
      throw Error{"Did not recognize controller type!"};
  }
}

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)

/**
 * \brief Returns a textual version of the supplied controller mapping result.
 *
 * \details This function returns a string that mirrors the name of the enumerator, e.g.
 * `to_string(controller::mapping_result::added) == "added"`.
 *
 * \param result the enumerator that will be converted.
 *
 * \return a string that mirrors the name of the enumerator.
 *
 * \throws cen_error if the enumerator is not recognized.
 *
 * \since 6.2.0
 */
[[nodiscard]] inline auto to_string(const controller::mapping_result result)
    -> std::string_view
{
  switch (result) {
    case controller::mapping_result::error:
      return "error";

    case controller::mapping_result::updated:
      return "updated";

    case controller::mapping_result::added:
      return "added";

    default:
      throw Error{"Did not recognize controller mapping result!"};
  }
}

/// \see to_string(controller::mapping_result)
[[nodiscard]] constexpr auto to_string(const controller_handle::mapping_result result)
    -> std::string_view
{
  switch (result) {
    case controller_handle::mapping_result::error:
      return "error";

    case controller_handle::mapping_result::updated:
      return "updated";

    case controller_handle::mapping_result::added:
      return "added";

    default:
      throw Error{"Did not recognize controller mapping result!"};
  }
}

/**
 * \brief Returns a textual representation of a game controller.
 *
 * \param controller the game controller that will be converted.
 *
 * \return a string that represents a game controller.
 *
 * \since 5.0.0
 */
template <typename T>
[[nodiscard]] auto to_string(const basic_controller<T>& controller) -> std::string
{
  const auto* name = controller.name();

  const char* serial{};
  if constexpr (detail::sdl_version_at_least(2, 0, 14)) {
    serial = controller.serial();
  }

#if CENTURION_HAS_FEATURE_FORMAT
  return std::format("controller{{data: {}, name: {}, serial: {}}}",
                     detail::address_of(controller.get()),
                     str_or_na(name),
                     str_or_na(serial));
#else
  return "controller{data: " + detail::address_of(controller.get()) +
         ", name: " + str_or_na(name) + ", serial: " + str_or_na(serial) + "}";
#endif  // CENTURION_HAS_FEATURE_FORMAT
}

/// \} End of string conversions

/// \name Streaming
/// \{

/**
 * \brief Prints a textual representation of a controller button enumerator.
 *
 * \param stream the output stream that will be used.
 * \param button the controller button that will be printed.
 *
 * \see `to_string(controller_button)`
 *
 * \return the used stream.
 *
 * \since 6.2.0
 */
inline auto operator<<(std::ostream& stream, const controller_button button) -> std::ostream&
{
  return stream << to_string(button);
}

/**
 * \brief Prints a textual representation of a controller axis enumerator.
 *
 * \param stream the output stream that will be used.
 * \param axis the controller axis that will be printed.
 *
 * \see `to_string(controller_axis)`
 *
 * \return the used stream.
 *
 * \since 6.2.0
 */
inline auto operator<<(std::ostream& stream, const controller_axis axis) -> std::ostream&
{
  return stream << to_string(axis);
}

/**
 * \brief Prints a textual representation of a controller bind type enumerator.
 *
 * \param stream the output stream that will be used.
 * \param type the controller bind type that will be printed.
 *
 * \see `to_string(controller_bind_type)`
 *
 * \return the used stream.
 *
 * \since 6.2.0
 */
inline auto operator<<(std::ostream& stream, const controller_bind_type type) -> std::ostream&
{
  return stream << to_string(type);
}

#if SDL_VERSION_ATLEAST(2, 0, 12)

/**
 * \brief Prints a textual representation of a controller type enumerator.
 *
 * \param stream the output stream that will be used.
 * \param type the controller type that will be printed.
 *
 * \see `to_string(controller_type)`
 *
 * \return the used stream.
 *
 * \since 6.2.0
 */
inline auto operator<<(std::ostream& stream, const controller_type type) -> std::ostream&
{
  return stream << to_string(type);
}

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)

/**
 * \brief Prints a textual representation of a controller mapping result enumerator.
 *
 * \param stream the output stream that will be used.
 * \param result the enumerator that will be printed.
 *
 * \see `to_string(controller::mapping_result)`
 *
 * \return the used stream.
 *
 * \since 6.2.0
 */
inline auto operator<<(std::ostream& stream, const controller::mapping_result result)
    -> std::ostream&
{
  return stream << to_string(result);
}

/// \see operator<<(std::ostream&, controller::mapping_result)
inline auto operator<<(std::ostream& stream, const controller_handle::mapping_result result)
    -> std::ostream&
{
  return stream << to_string(result);
}

/**
 * \brief Prints a textual representation of a game controller.
 *
 * \param stream the stream that will be used.
 * \param controller the game controller that will be printed.
 *
 * \return the used stream.
 *
 * \since 5.0.0
 */
template <typename T>
auto operator<<(std::ostream& stream, const basic_controller<T>& controller) -> std::ostream&
{
  return stream << to_string(controller);
}

/// \} End of streaming

/// \} End of group input

}  // namespace cen

#endif  // CENTURION_GAME_CONTROLLER_HPP_
