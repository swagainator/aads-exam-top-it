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

    bool parseTwoSizes(
        const std::string& arguments,
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

    bool parseDescription(
        const std::string& arguments,
        size_t& id,
        std::string& description)
    {
      size_t position = skipSpaces(arguments, 0);
      if (!parseSizeT(arguments, position, id))
      {
        return false;
      }
      const size_t quotePosition = skipSpaces(arguments, position);
      if (quotePosition == position ||
          quotePosition == arguments.size() ||
          arguments[quotePosition] != '"')
      {
        return false;
      }
      const size_t descriptionBegin = quotePosition + 1;
      size_t descriptionEnd = descriptionBegin;
      while (descriptionEnd < arguments.size() &&
          arguments[descriptionEnd] != '"')
      {
        ++descriptionEnd;
      }
      if (descriptionEnd == descriptionBegin ||
          descriptionEnd == arguments.size())
      {
        return false;
      }
      position = skipSpaces(arguments, descriptionEnd + 1);
      if (position != arguments.size())
      {
        return false;
      }
      description.assign(
          arguments,
          descriptionBegin,
          descriptionEnd - descriptionBegin);
      return true;
    }

    void writeMeetingViews(
        std::ostream& output,
        const MeetingViewArray& views)
    {
      if (views.size == 0)
      {
        output << '\n';
        return;
      }
      for (size_t i = 0; i < views.size; ++i)
      {
        output << views.data[i].id << ' ' << views.data[i].time << '\n';
      }
    }

    void writeFilteredMeetingViews(
        std::ostream& output,
        const MeetingViewArray& views)
    {
      if (views.size == 0)
      {
        output << '\n';
        return;
      }
      for (size_t i = 0; i < views.size; ++i)
      {
        output << views.data[i].id << ' ' << views.data[i].time << '\n';
      }
    }

    void collectFilteredViews(
        const MeetingArray& meetings,
        size_t id,
        size_t time,
        bool less,
        MeetingViewArray& filtered)
    {
      MeetingViewArray views = {nullptr, 0, 0};
      initArray(views);
      try
      {
        collectMeetingViews(meetings, id, views);
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
      }
      catch (...)
      {
        destroyArray(views);
        throw;
      }
      destroyArray(views);
    }

    bool handleTimeFilter(
        const std::string& arguments,
        std::ostream& output,
        const PersonArray& persons,
        const MeetingArray& meetings,
        bool less)
    {
      size_t time = 0;
      size_t id = 0;
      if (!parseTwoSizes(arguments, time, id) ||
          findPersonIndex(persons, id) == persons.size)
      {
        return false;
      }
      MeetingViewArray filtered = {nullptr, 0, 0};
      initArray(filtered);
      try
      {
        collectFilteredViews(meetings, id, time, less, filtered);
        sortMeetingViews(filtered);
        writeFilteredMeetingViews(output, filtered);
      }
      catch (...)
      {
        destroyArray(filtered);
        throw;
      }
      destroyArray(filtered);
      return true;
    }
  }
}

void alekseev::executeCommands(
    std::istream& input,
    std::ostream& output,
    PersonArray& persons,
    MeetingArray& meetings)
{
  std::string line;
  while (std::getline(input, line))
  {
    const bool valid = executeCommandLine(line, output, persons, meetings);
    if (!valid)
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
    MeetingArray& meetings)
{
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
    size_t id = 0;
    std::string description;
    if (parseDescription(arguments, id, description))
    {
      return handleRedesc(arguments, persons);
    }
    return handleDesc(arguments, output, persons);
  }
  if (command == "meet" || command == "meets")
  {
    return handleMeets(arguments, output, persons, meetings);
  }
  if (command == "commons")
  {
    return handleCommons(arguments, output, persons, meetings);
  }
  if (command == "less")
  {
    return handleLess(arguments, output, persons, meetings);
  }
  if (command == "greater")
  {
    return handleGreater(arguments, output, persons, meetings);
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
    if (ids.size == 0)
    {
      output << '\n';
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
    const MeetingArray& meetings)
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
    collectMeetingViews(meetings, id, views);
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

bool alekseev::handleCommons(
    const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const MeetingArray& meetings)
{
  size_t first = 0;
  size_t second = 0;
  if (!parseTwoSizes(arguments, first, second) ||
      findPersonIndex(persons, first) == persons.size ||
      findPersonIndex(persons, second) == persons.size)
  {
    return false;
  }

  Array< size_t > ids = {nullptr, 0, 0};
  initArray(ids);
  try
  {
    collectCommonPersons(meetings, first, second, ids);
    sortIds(ids);
    for (size_t i = 0; i < ids.size; ++i)
    {
      output << ids.data[i] << '\n';
    }
    if (ids.size == 0)
    {
      output << '\n';
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

bool alekseev::handleLess(
    const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const MeetingArray& meetings)
{
  return handleTimeFilter(arguments, output, persons, meetings, true);
}

bool alekseev::handleGreater(
    const std::string& arguments,
    std::ostream& output,
    const PersonArray& persons,
    const MeetingArray& meetings)
{
  return handleTimeFilter(arguments, output, persons, meetings, false);
}

bool alekseev::handleRedesc(
    const std::string& arguments,
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

bool alekseev::handleDeanon(
    const std::string& arguments,
    PersonArray& persons,
    MeetingArray& meetings)
{
  size_t anonymousId = 0;
  size_t describedId = 0;
  if (!parseTwoSizes(arguments, anonymousId, describedId) ||
      anonymousId == describedId)
  {
    return false;
  }
  const size_t anonymousIndex = findPersonIndex(persons, anonymousId);
  const size_t describedIndex = findPersonIndex(persons, describedId);
  if (anonymousIndex == persons.size ||
      describedIndex == persons.size ||
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

bool alekseev::handleOutPersons(
    const std::string& arguments,
    const PersonArray& persons)
{
  const size_t filenameBegin = skipSpaces(arguments, 0);
  const size_t filenameEnd = trimRight(arguments, filenameBegin);
  if (filenameBegin == filenameEnd)
  {
    return false;
  }
  for (size_t i = filenameBegin; i < filenameEnd; ++i)
  {
    if (isSpace(arguments[i]))
    {
      return false;
    }
  }
  const std::string filename(
      arguments,
      filenameBegin,
      filenameEnd - filenameBegin);
  std::ofstream output(filename.c_str(), std::ios::app);
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
