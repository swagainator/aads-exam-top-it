#ifndef RANGE_STATE_HPP
#define RANGE_STATE_HPP

#include <exam_array.hpp>

#include <iosfwd>

#include "date.hpp"

namespace alekseev
{
  struct RangeState
  {
    bool empty;
    size_t firstIndex;
    size_t lastIndex;
  };

  using RangeStateArray = Array< RangeState >;

  void initInitialRange(const DateArray& dates, RangeState& range);
  void applyAfter(
      const DateArray& dates,
      const Date& date,
      RangeState& range);
  void applyBefore(
      const DateArray& dates,
      const Date& date,
      RangeState& range);
  void pushRangeState(
      RangeStateArray& states,
      const RangeState& range);
  bool popRangeState(
      RangeStateArray& states,
      RangeState& range);
  bool isDateInRange(
      const DateArray& dates,
      const RangeState& range,
      const Date& date);
  void printRange(
      std::ostream& output,
      const DateArray& dates,
      const RangeState& range);
}

#endif
