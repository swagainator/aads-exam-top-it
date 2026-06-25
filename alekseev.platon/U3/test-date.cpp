#define BOOST_TEST_MODULE U3_DATED_RANGES
#include <boost/test/included/unit_test.hpp>

#include "date.hpp"

BOOST_AUTO_TEST_CASE(compare_dates_chronologically)
{
  const alekseev::Date first = {31, 12, 2025};
  const alekseev::Date second = {1, 1, 2026};
  const alekseev::Date equal = {31, 12, 2025};

  BOOST_TEST(alekseev::compareDates(first, second) < 0);
  BOOST_TEST(alekseev::compareDates(second, first) > 0);
  BOOST_TEST(alekseev::areDatesEqual(first, equal));
}

BOOST_AUTO_TEST_CASE(sort_and_deduplicate_dates)
{
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::initArray(dates);
  alekseev::pushUniqueDate(dates, {5, 1, 2026});
  alekseev::pushUniqueDate(dates, {1, 1, 2026});
  alekseev::pushUniqueDate(dates, {5, 1, 2026});
  alekseev::pushUniqueDate(dates, {3, 12, 2025});
  alekseev::sortDates(dates);

  BOOST_TEST(dates.size == 3);
  BOOST_TEST(dates.data[0].year == 2025);
  BOOST_TEST(dates.data[1].day == 1);
  BOOST_TEST(dates.data[2].day == 5);

  alekseev::destroyArray(dates);
}

BOOST_AUTO_TEST_CASE(parse_command_date)
{
  const std::string text = " 12 33 1946 ";
  size_t position = 0;
  alekseev::Date date = {0, 0, 0};

  BOOST_REQUIRE(alekseev::parseDateFromCommand(text, position, date));
  BOOST_TEST(date.day == 12);
  BOOST_TEST(date.month == 33);
  BOOST_TEST(date.year == 1946);
}
