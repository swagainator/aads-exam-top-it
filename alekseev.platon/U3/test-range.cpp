#include <boost/test/unit_test.hpp>

#include "range_state.hpp"

namespace alekseev
{
  namespace
  {
    void fillRangeDates(DateArray& dates)
    {
      initArray(dates);
      pushBack(dates, Date{1, 1, 2026});
      pushBack(dates, Date{3, 1, 2026});
      pushBack(dates, Date{5, 1, 2026});
    }
  }
}

BOOST_AUTO_TEST_CASE(initial_range_is_empty_without_dates)
{
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState range = {false, 0, 0};
  alekseev::initArray(dates);
  alekseev::initInitialRange(dates, range);

  BOOST_TEST(range.empty);
  alekseev::destroyArray(dates);
}

BOOST_AUTO_TEST_CASE(initial_range_covers_all_dates)
{
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState range = {true, 0, 0};
  alekseev::fillRangeDates(dates);
  alekseev::initInitialRange(dates, range);

  BOOST_TEST(!range.empty);
  BOOST_TEST(range.firstIndex == 0);
  BOOST_TEST(range.lastIndex == 2);
  alekseev::destroyArray(dates);
}

BOOST_AUTO_TEST_CASE(after_and_before_snap_to_input_dates)
{
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState range = {true, 0, 0};
  alekseev::fillRangeDates(dates);
  alekseev::initInitialRange(dates, range);

  alekseev::applyAfter(dates, {2, 1, 2026}, range);
  BOOST_TEST(range.firstIndex == 1);
  alekseev::applyBefore(dates, {4, 1, 2026}, range);
  BOOST_TEST(range.lastIndex == 1);

  alekseev::destroyArray(dates);
}

BOOST_AUTO_TEST_CASE(range_can_become_empty)
{
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState afterRange = {true, 0, 0};
  alekseev::RangeState beforeRange = {true, 0, 0};
  alekseev::fillRangeDates(dates);
  alekseev::initInitialRange(dates, afterRange);
  alekseev::initInitialRange(dates, beforeRange);

  alekseev::applyAfter(dates, {6, 1, 2026}, afterRange);
  alekseev::applyBefore(dates, {31, 12, 2025}, beforeRange);
  BOOST_TEST(afterRange.empty);
  BOOST_TEST(beforeRange.empty);

  alekseev::destroyArray(dates);
}

BOOST_AUTO_TEST_CASE(pop_restores_previous_range)
{
  alekseev::RangeStateArray history = {nullptr, 0, 0};
  alekseev::RangeState range = {false, 1, 2};
  const alekseev::RangeState original = range;
  alekseev::initArray(history);
  alekseev::pushRangeState(history, original);
  range.empty = true;

  BOOST_REQUIRE(alekseev::popRangeState(history, range));
  BOOST_TEST(!range.empty);
  BOOST_TEST(range.firstIndex == 1);
  BOOST_TEST(range.lastIndex == 2);
  BOOST_TEST(!alekseev::popRangeState(history, range));

  alekseev::destroyArray(history);
}
