#ifndef BELMOOR_DEHUMIDIFIER_CONTROL_HPP_
#define BELMOOR_DEHUMIDIFIER_CONTROL_HPP_ 1

#include "am2320.hpp"
#include "sys_tick.hpp"

namespace belmoor {

  class Dehumidifier_control {
    static constexpr auto MeasurementPeriod = 2000; // ms

    int timer_ {MeasurementPeriod};

   public:
    bool operator()(const Duration period) {
      timer_ -= period;
    }
  };

} // namespace belmoor

#endif // BELMOOR_DEHUMIDIFIER_CONTROL_HPP_
