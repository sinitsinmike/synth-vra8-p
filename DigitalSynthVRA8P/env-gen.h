#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class EnvGen {
  static const uint8_t STATE_ATTACK        = 0;
  static const uint8_t STATE_DECAY_SUSTAIN = 1;
  static const uint8_t STATE_RELEASE       = 2;
  static const uint8_t STATE_IDLE          = 3;

  static const uint8_t RELEASE_UPDATE_INTERVAL = 1;

  static uint8_t m_state;
  static uint8_t m_level;
  static uint8_t m_count;
  static uint8_t m_attack_update_interval;
  static uint8_t m_decay_update_interval;
  static uint8_t m_sustain_level;
  static uint8_t m_rest;

public:
  INLINE static void initialize() {
    m_state = STATE_RELEASE;
    m_level = 0;
    m_count = 0;
    m_attack_update_interval = 1;
    m_decay_update_interval = 1;
    m_sustain_level = 255;
    m_rest = RELEASE_UPDATE_INTERVAL;
  }

  INLINE static void set_attack(uint8_t controller_value) {
    m_attack_update_interval = controller_value + 1;
  }

  INLINE static void set_decay(uint8_t controller_value) {
    m_decay_update_interval = controller_value + 1;
  }

  INLINE static void set_sustain(uint8_t controller_value) {
    m_sustain_level = (controller_value << 1) + 1;
    m_sustain_level = (m_sustain_level / 3) * 3;
  }

  INLINE static void note_on() {
    m_state = STATE_ATTACK;
    m_rest = m_attack_update_interval;
  }

  INLINE static void note_off() {
    m_state = STATE_RELEASE;
    m_rest = RELEASE_UPDATE_INTERVAL;
  }

  INLINE static uint8_t clock() {
    m_count++;
    if ((m_count & 0x03) == 0) {
      switch (m_state) {
      case STATE_ATTACK:
        m_rest--;
        if (m_rest == 0) {
          m_rest = m_attack_update_interval;
          if (m_level == 255) {
            m_state = STATE_DECAY_SUSTAIN;
            m_rest = m_decay_update_interval;
          } else {
            m_level += 3;
          }
        }
        break;
      case STATE_DECAY_SUSTAIN:
        m_rest--;
        if (m_rest == 0) {
          m_rest = m_decay_update_interval;
          if (m_level <= m_sustain_level) {
            m_level = m_sustain_level;
          } else {
            m_level -= 3;
          }
        }
        break;
      case STATE_RELEASE:
        m_rest--;
        if (m_rest == 0) {
          m_rest = RELEASE_UPDATE_INTERVAL;
          if (m_level == 0) {
            m_state = STATE_IDLE;
          } else {
            m_level -= 3;
          }
        }
        break;
      default:  // case STATE_IDLE:
        m_level = 0;
        break;
      }
    }

    return m_level;
  }
};

template <uint8_t T> uint8_t EnvGen<T>::m_count;
template <uint8_t T> uint8_t EnvGen<T>::m_state;
template <uint8_t T> uint8_t EnvGen<T>::m_level;
template <uint8_t T> uint8_t EnvGen<T>::m_attack_update_interval;
template <uint8_t T> uint8_t EnvGen<T>::m_decay_update_interval;
template <uint8_t T> uint8_t EnvGen<T>::m_sustain_level;
template <uint8_t T> uint8_t EnvGen<T>::m_rest;