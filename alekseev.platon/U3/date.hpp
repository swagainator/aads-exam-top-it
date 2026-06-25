#ifndef DATE_HPP
#define DATE_HPP

#include <cstddef>
#include <iosfwd>
#include <string>

#include <exam_array.hpp>

namespace alekseev
{
  struct Date
  {
    size_t day;
    size_t month;
    size_t year;
  };

  using DateArray = Array< Date >;

  int compareDates(const Date& first, const Date& second);
  bool areDatesEqual(const Date& first, const Date& second);
  bool isDateLess(const Date& first, const Date& second);
  bool isDateLessOrEqual(const Date& first, const Date& second);
  void printDate(std::ostream& output, const Date& date);
  bool parseDateFromCommand(const std::string& text,
      size_t& position,
      Date& date);
  void pushUniqueDate(DateArray& dates, const Date& date);
  void sortDates(DateArray& dates);
  size_t findFirstDateNotBefore(const DateArray& dates,
      size_t first,
      size_t last,
      const Date& date);
  size_t findLastDateNotAfter(const DateArray& dates,
      size_t first,
      size_t last,
      const Date& date);
}

#endif
