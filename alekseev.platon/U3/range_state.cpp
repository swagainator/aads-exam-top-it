#include "range_state.hpp"

#include <ostream>

void alekseev::initInitialRange(
    const DateArray& dates,
    RangeState& range)
{
  range.empty = dates.size == 0;
  range.firstIndex = 0;
  range.lastIndex = dates.size == 0 ? 0 : dates.size - 1;
}

void alekseev::applyAfter(
    const DateArray& dates,
    const Date& date,
    RangeState& range)
{
  const size_t index = findFirstDateNotBefore(
      dates,
      range.firstIndex,
      range.lastIndex,
      date);
  if (index == dates.size)
  {
    range.empty = true;
    return;
  }
  range.firstIndex = index;
}

void alekseev::applyBefore(
    const DateArray& dates,
    const Date& date,
    RangeState& range)
{
  const size_t index = findLastDateNotAfter(
      dates,
      range.firstIndex,
      range.lastIndex,
      date);
  if (index == dates.size)
  {
    range.empty = true;
    return;
  }
  range.lastIndex = index;
}

void alekseev::pushRangeState(
    RangeStateArray& states,
    const RangeState& range)
{
  pushBack(states, range);
}

bool alekseev::popRangeState(
    RangeStateArray& states,
    RangeState& range)
{
  if (states.size == 0)
  {
    return false;
  }
  range = states.data[states.size - 1];
  --states.size;
  return true;
}

bool alekseev::isDateInRange(
    const DateArray& dates,
    const RangeState& range,
    const Date& date)
{
  if (range.empty)
  {
    return false;
  }
  return !isDateLess(date, dates.data[range.firstIndex]) &&
      !isDateLess(dates.data[range.lastIndex], date);
}

void alekseev::printRange(
    std::ostream& output,
    const DateArray& dates,
    const RangeState& range)
{
  if (range.empty)
  {
    output << "<EMPTY>\n";
    return;
  }
  printDate(output, dates.data[range.firstIndex]);
  output << " : ";
  printDate(output, dates.data[range.lastIndex]);
  output << '\n';
}
