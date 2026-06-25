#include "commands.hpp"

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>

#include <text_utils.hpp>

namespace alekseev
{
  namespace
  {
    bool parseSingleSize(const std::string& arguments, size_t& value)
    {
      size_t position = skipSpaces(arguments, 0);
      if (!parseSizeT(arguments, position, value))
      {
        return false;
      }
      position = skipSpaces(arguments, position);
      return position == arguments.size();
    }

    bool parseDateArguments(const std::string& arguments,
        Date& date)
    {
      size_t position = 0;
      if (!parseDateFromCommand(arguments, position, date))
      {
        return false;
      }
      position = skipSpaces(arguments, position);
      return position == arguments.size();
    }

    bool parseTwoSizes(const std::string& arguments,
        size_t& first,
        size_t& second)
    {
      size_t position = skipSpaces(arguments, 0);
      if (!parseSizeT(arguments, position, first))
      {
        return false;
      }
      const size_t secondBegin = skipSpaces(arguments, position);
      if (secondBegin == position)
      {
        return false;
      }
      position = secondBegin;
      if (!parseSizeT(arguments, position, second))
      {
        return false;
      }
      position = skipSpaces(arguments, position);
      return position == arguments.size();
    }

    bool parseDescription(const std::string& arguments,
        size_t& id,
        std::string& description)
    {
      size_t position = skipSpaces(arguments, 0);
      if (!parseSizeT(arguments, position, id))
      {
        return false;
      }
      const size_t quote = skipSpaces(arguments, position);
      if ((quote == position) ||
          (quote == arguments.size()) ||
          (arguments[quote] != '"'))
      {
        return false;
      }
      const size_t begin = quote + 1;
      size_t end = begin;
      while ((end < arguments.size()) && (arguments[end] != '"'))
      {
        ++end;
      }
      if ((end == begin) || (end == arguments.size()))
      {
        return false;
      }
      position = skipSpaces(arguments, end + 1);
      if (position != arguments.size())
      {
        return false;
      }
      description.assign(arguments, begin, end - begin);
      return true;
    }

    void writeMeetingViews(std::ostream& output,
        const MeetingViewArray& views)
    {
      for (size_t i = 0; i < views.size; ++i)
      {
        output << views.data[i].id << ' ' << views.data[i].time << '\n';
      }
    }

    bool handleTimeFilter(const std::string& arguments,
        std::ostream& output,
        const PersonArray& persons,
        const DatedMeetingArray& meetings,
        const DateArray& dates,
        const RangeState& range,
        bool less)
    {
      size_t time = 0;
      size_t id = 0;
      if (!parseTwoSizes(arguments, time, id) ||
          (findPersonIndex(persons, id) == persons.size))
      {
        return false;
      }
      MeetingViewArray views = {nullptr, 0, 0};
      MeetingViewArray filtered = {nullptr, 0, 0};
      initArray(views);
      initArray(filtered);
      try
      {
        collectMeetingViews(meetings, dates, range, id, views);
        for (size_t i = 0; i < views.size; ++i)
        {
          const bool matches = less ?
              views.data[i].time < time :
              views.data[i].time > time;
          if (matches)
          {
            pushBack(filtered, views.data[i]);
          }
        }
        sortMeetingViews(filtered);
        writeMeetingViews(output, filtered);
      }
      catch (...)
      {
        destroyArray(filtered);
        destroyArray(views);
        throw;
      }
      destroyArray(filtered);
      destroyArray(views);
      return true;
    }
  }
}

void alekseev::executeCommands(std::istream& input,
    std::ostream& output,
    PersonArray& persons,
    DatedMeetingArray& meetings,
    const DateArray& dates,
    RangeState& range,
    RangeStateArray& history)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (!executeCommandLine(line,
        output,
        persons,
        meetings,
        dates,
        range,
        history))
    {
      output << "<INVALID COMMAND>\n";
    }
    if (!output)
    {
      throw std::runtime_error("command output failure");
    }
  }
  if (!input.eof())
  {
    throw std::runtime_error("command input failure");
  }
}

bool alekseev::executeCommandLine(const std::string& line,
    std::ostream& output,
    PersonArray& persons,
    DatedMeetingArray& meetings,
    const DateArray& dates,
    RangeState& range,
    RangeStateArray& history)
{
  const size_t commandBegin = skipSpaces(line, 0);
  size_t commandEnd = commandBegin;
  while ((commandEnd < line.size()) && !isSpace(line[commandEnd]))
  {
    ++commandEnd;
  }
  if (commandBegin == commandEnd)
  {
    return false;
  }

  const std::string command(line, commandBegin, commandEnd - commandBegin);
  const std::string arguments(line, commandEnd);
  if (command == "anons")
  {
    return handleAnons(arguments, output, persons);
  }
  if (command == "desc")
  {
    return handleDesc(arguments, output, persons);
  }
  if ((command == "meet") || (command == "meets"))
  {
    return handleMeets(arguments,
        output,
        persons,
        meetings,
        dates,
        range);
  }
  if (command == "range")
  {
    return handleRange(arguments, output, dates, range);
  }
  if (command == "commons")
  {
    return handleCommons(arguments,
        output,
        persons,
        meetings,
        dates,
        range);
  }
  if (command == "less")
  {
    return handleLess(arguments,
        output,
        persons,
        meetings,
        dates,
        range);
  }
  if (command == "greater")
  {
    return handleGreater(arguments,
        output,
        persons,
        meetings,
        dates,
        range);
  }
  if (command == "redesc")
  {
    return handleRedesc(arguments, persons);
  }
  if (command == "deanon")
  {
    return handleDeanon(arguments, persons, meetings);
  }
  if (command == "out-persons")
  {
    return handleOutPersons(arguments, persons);
  }
  if (command == "after")
  {
    return handleAfter(arguments, dates, range, history);
  }
  if (command == "before")
  {
    return handleBefore(arguments, dates, range, history);
  }
  if (command == "pop-range")
  {
    return handlePopRange(arguments, range, history);
  }
  return false;
}

bool alekseev::handleAnons(const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons)
{
  if (skipSpaces(arguments, 0) != arguments.size())
  {
    return false;
  }
  Array< size_t > ids = {nullptr, 0, 0};
  initArray(ids);
  try
  {
    for (size_t i = 0; i < persons.size; ++i)
    {
      if (!hasPersonInfo(persons.data[i]))
      {
        pushBack(ids, persons.data[i].id);
      }
    }
    sortIds(ids);
    for (size_t i = 0; i < ids.size; ++i)
    {
      output << ids.data[i] << '\n';
    }
  }
  catch (...)
  {
    destroyArray(ids);
    throw;
  }
  destroyArray(ids);
  return true;
}

bool alekseev::handleDesc(const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons)
{
  size_t id = 0;
  if (!parseSingleSize(arguments, id))
  {
    return false;
  }
  const size_t index = findPersonIndex(persons, id);
  if (index == persons.size)
  {
    return false;
  }
  if (hasPersonInfo(persons.data[index]))
  {
    output << persons.data[index].info << '\n';
  }
  else
  {
    output << "<ANON>\n";
  }
  return true;
}

bool alekseev::handleMeets(const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range)
{
  size_t id = 0;
  if (!parseSingleSize(arguments, id) ||
      (findPersonIndex(persons, id) == persons.size))
  {
    return false;
  }
  MeetingViewArray views = {nullptr, 0, 0};
  initArray(views);
  try
  {
    collectMeetingViews(meetings, dates, range, id, views);
    sortMeetingViews(views);
    writeMeetingViews(output, views);
  }
  catch (...)
  {
    destroyArray(views);
    throw;
  }
  destroyArray(views);
  return true;
}

bool alekseev::handleCommons(const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range)
{
  size_t first = 0;
  size_t second = 0;
  if (!parseTwoSizes(arguments, first, second) ||
      (findPersonIndex(persons, first) == persons.size) ||
      (findPersonIndex(persons, second) == persons.size))
  {
    return false;
  }
  Array< size_t > ids = {nullptr, 0, 0};
  initArray(ids);
  try
  {
    collectCommonPersons(meetings,
        dates,
        range,
        first,
        second,
        ids);
    sortIds(ids);
    for (size_t i = 0; i < ids.size; ++i)
    {
      output << ids.data[i] << '\n';
    }
  }
  catch (...)
  {
    destroyArray(ids);
    throw;
  }
  destroyArray(ids);
  return true;
}

bool alekseev::handleLess(const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range)
{
  return handleTimeFilter(arguments,
      output,
      persons,
      meetings,
      dates,
      range,
      true);
}

bool alekseev::handleGreater(const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range)
{
  return handleTimeFilter(arguments,
      output,
      persons,
      meetings,
      dates,
      range,
      false);
}

bool alekseev::handleRedesc(const std::string& arguments,
    PersonArray& persons)
{
  size_t id = 0;
  std::string description;
  if (!parseDescription(arguments, id, description))
  {
    return false;
  }
  const size_t index = findPersonIndex(persons, id);
  if (index == persons.size)
  {
    return false;
  }
  persons.data[index].info = description;
  return true;
}

bool alekseev::handleDeanon(const std::string& arguments,
    PersonArray& persons,
    DatedMeetingArray& meetings)
{
  size_t anonymousId = 0;
  size_t describedId = 0;
  if (!parseTwoSizes(arguments, anonymousId, describedId) ||
      (anonymousId == describedId))
  {
    return false;
  }
  const size_t anonymousIndex = findPersonIndex(persons, anonymousId);
  const size_t describedIndex = findPersonIndex(persons, describedId);
  if ((anonymousIndex == persons.size) ||
      (describedIndex == persons.size) ||
      hasPersonInfo(persons.data[anonymousIndex]) ||
      !hasPersonInfo(persons.data[describedIndex]))
  {
    return false;
  }
  replacePersonInMeetings(meetings, anonymousId, describedId);
  removeSelfMeetings(meetings);
  removePersonAt(persons, anonymousIndex);
  return true;
}

bool alekseev::handleOutPersons(const std::string& arguments,
    const PersonArray& persons)
{
  const size_t begin = skipSpaces(arguments, 0);
  const size_t end = trimRight(arguments, begin);
  if (begin == end)
  {
    return false;
  }
  for (size_t i = begin; i < end; ++i)
  {
    if (isSpace(arguments[i]))
    {
      return false;
    }
  }
  const std::string filename(arguments, begin, end - begin);
  std::ofstream output(filename.c_str());
  if (!output)
  {
    return false;
  }

  PersonArray described = {nullptr, 0, 0};
  initPersonArray(described);
  try
  {
    for (size_t i = 0; i < persons.size; ++i)
    {
      if (hasPersonInfo(persons.data[i]))
      {
        pushPerson(described, persons.data[i]);
      }
    }
    writePersons(output, described);
    output.close();
    if (!output)
    {
      destroyPersonArray(described);
      return false;
    }
  }
  catch (const std::exception&)
  {
    destroyPersonArray(described);
    return false;
  }
  destroyPersonArray(described);
  return true;
}

bool alekseev::handleRange(const std::string& arguments,
    std::ostream& output,
    const DateArray& dates,
    const RangeState& range)
{
  if (skipSpaces(arguments, 0) != arguments.size())
  {
    return false;
  }
  printRange(output, dates, range);
  return true;
}

bool alekseev::handleAfter(const std::string& arguments,
    const DateArray& dates,
    RangeState& range,
    RangeStateArray& history)
{
  Date date = {0, 0, 0};
  if (range.empty || !parseDateArguments(arguments, date))
  {
    return false;
  }
  pushRangeState(history, range);
  applyAfter(dates, date, range);
  return true;
}

bool alekseev::handleBefore(const std::string& arguments,
    const DateArray& dates,
    RangeState& range,
    RangeStateArray& history)
{
  Date date = {0, 0, 0};
  if (range.empty || !parseDateArguments(arguments, date))
  {
    return false;
  }
  pushRangeState(history, range);
  applyBefore(dates, date, range);
  return true;
}

bool alekseev::handlePopRange(const std::string& arguments,
    RangeState& range,
    RangeStateArray& history)
{
  if (skipSpaces(arguments, 0) != arguments.size())
  {
    return false;
  }
  return popRangeState(history, range);
}
