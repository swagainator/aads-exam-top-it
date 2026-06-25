#include "commands.hpp"

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

    void writeMeetingViews(
        std::ostream& output,
        const MeetingViewArray& views)
    {
      for (size_t i = 0; i < views.size; ++i)
      {
        output << views.data[i].id << ' ' << views.data[i].time << '\n';
      }
    }
  }
}

void alekseev::executeCommands(
    std::istream& input,
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
    if (!executeCommandLine(
        line,
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

bool alekseev::executeCommandLine(
    const std::string& line,
    std::ostream& output,
    PersonArray& persons,
    DatedMeetingArray& meetings,
    const DateArray& dates,
    RangeState& range,
    RangeStateArray& history)
{
  static_cast< void >(meetings);
  static_cast< void >(history);
  const size_t commandBegin = skipSpaces(line, 0);
  size_t commandEnd = commandBegin;
  while (commandEnd < line.size() && !isSpace(line[commandEnd]))
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
  if (command == "meet" || command == "meets")
  {
    return handleMeets(
        arguments,
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
  return false;
}

bool alekseev::handleAnons(
    const std::string& arguments,
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

bool alekseev::handleDesc(
    const std::string& arguments,
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

bool alekseev::handleMeets(
    const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range)
{
  size_t id = 0;
  if (!parseSingleSize(arguments, id) ||
      findPersonIndex(persons, id) == persons.size)
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

bool alekseev::handleRange(
    const std::string& arguments,
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
