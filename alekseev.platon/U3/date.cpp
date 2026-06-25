#include "date.hpp"

#include <ostream>

#include <text_utils.hpp>

int alekseev::compareDates(const Date& first, const Date& second)
{
  if (first.year != second.year)
  {
    return first.year < second.year ? -1 : 1;
  }
  if (first.month != second.month)
  {
    return first.month < second.month ? -1 : 1;
  }
  if (first.day != second.day)
  {
    return first.day < second.day ? -1 : 1;
  }
  return 0;
}

bool alekseev::areDatesEqual(const Date& first, const Date& second)
{
  return compareDates(first, second) == 0;
}

bool alekseev::isDateLess(const Date& first, const Date& second)
{
  return compareDates(first, second) < 0;
}

bool alekseev::isDateLessOrEqual(const Date& first, const Date& second)
{
  return compareDates(first, second) <= 0;
}

void alekseev::printDate(std::ostream& output, const Date& date)
{
  output << date.day << ' ' << date.month << ' ' << date.year;
}

bool alekseev::parseDateFromCommand(const std::string& text,
    size_t& position,
    Date& date)
{
  position = skipSpaces(text, position);
  if (!parseSizeT(text, position, date.day))
  {
    return false;
  }
  size_t next = skipSpaces(text, position);
  if ((next == position) || !parseSizeT(text, next, date.month))
  {
    return false;
  }
  position = next;
  next = skipSpaces(text, position);
  if ((next == position) || !parseSizeT(text, next, date.year))
  {
    return false;
  }
  position = next;
  return true;
}

void alekseev::pushUniqueDate(DateArray& dates, const Date& date)
{
  for (size_t i = 0; i < dates.size; ++i)
  {
    if (areDatesEqual(dates.data[i], date))
    {
      return;
    }
  }
  pushBack(dates, date);
}

void alekseev::sortDates(DateArray& dates)
{
  for (size_t i = 1; i < dates.size; ++i)
  {
    const Date value = dates.data[i];
    size_t position = i;
    while ((position > 0) &&
        isDateLess(value, dates.data[position - 1]))
    {
      dates.data[position] = dates.data[position - 1];
      --position;
    }
    dates.data[position] = value;
  }
}

size_t alekseev::findFirstDateNotBefore(const DateArray& dates,
    size_t first,
    size_t last,
    const Date& date)
{
  for (size_t i = first; i <= last; ++i)
  {
    if (!isDateLess(dates.data[i], date))
    {
      return i;
    }
  }
  return dates.size;
}

size_t alekseev::findLastDateNotAfter(const DateArray& dates,
    size_t first,
    size_t last,
    const Date& date)
{
  size_t result = dates.size;
  for (size_t i = first; i <= last; ++i)
  {
    if (isDateLessOrEqual(dates.data[i], date))
    {
      result = i;
    }
    else
    {
      break;
    }
  }
  return result;
}
