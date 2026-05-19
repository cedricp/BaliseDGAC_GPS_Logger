#pragma once

template <class T, uint8_t alpha = 3>
class LPF {
  public:
    // alpha = 3 -> fast LPF
    // alpha = 6 -> very slow (ultra smooth) LPF
    LPF(T val = 0) : shift_factor(alpha), filtered_acc(val << alpha) {}

    void update(T input) {
      filtered_acc = filtered_acc - (filtered_acc >> shift_factor) + input;
    }
    T value() const {
      return filtered_acc >> shift_factor;
    }

    void operator=(T input) {
      update(input);
    }
    T operator()() const {
      return value();
    }

  protected:
    uint8_t shift_factor;
    T filtered_acc;
};

using LPF_U16_fast = LPF<uint16_t, 3>;
using LPF_U16_slow = LPF<uint16_t, 6>;

using LPF_I16_fast = LPF<int16_t, 3>;
using LPF_I16_slow = LPF<int16_t, 6>;

using LPF_I32_fast = LPF<int32_t, 3>;
using LPF_I32_slow = LPF<int32_t, 6>;

using LPF_U32_fast = LPF<uint32_t, 3>;
using LPF_U32_slow = LPF<uint32_t, 6>;
