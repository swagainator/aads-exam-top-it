#include "meeting_io.hpp"

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>

#include "commands.hpp"
#include "dated_meeting.hpp"

#include <person_data.hpp>
#include <text_utils.hpp>

bool alekseev::parseProgramOptions(
    int argc,
    char** argv,
    ProgramOptions& options)
{
  options.personInputName = nullptr;
  options.hasPersonInput = false;
  initArray(options.dataInputNames);
  if (argc < 1)
  {
    return false;
  }

  for (int i = 1; i < argc; ++i)
  {
    const char* const argument = argv[i];
    if (argument == nullptr)
    {
      return false;
    }
    if (startsWith(argument, "in:"))
    {
      if (options.hasPersonInput || argument[3] == '\0')
      {
        return false;
      }
      options.personInputName = argument + 3;
      options.hasPersonInput = true;
    }
    else if (startsWith(argument, "data:"))
    {
      if (argument[5] == '\0')
      {
        return false;
      }
      pushBack(options.dataInputNames, argument + 5);
    }
    else
    {
      return false;
    }
  }
  return true;
}

int alekseev::runU3(
    int argc,
    char** argv,
    std::istream& input,
    std::ostream& output,
    std::ostream& error)
{
  ProgramOptions options = {nullptr, false, {nullptr, 0, 0}};
  bool validOptions = false;
  try
  {
    validOptions = parseProgramOptions(argc, argv, options);
  }
  catch (const std::exception&)
  {
    destroyArray(options.dataInputNames);
    error << "internal error\n";
    return 2;
  }
  if (!validOptions)
  {
    destroyArray(options.dataInputNames);
    error << "invalid arguments\n";
    return 1;
  }

  PersonArray persons = {nullptr, 0, 0};
  DatedMeetingArray meetings = {nullptr, 0, 0};
  DateArray dates = {nullptr, 0, 0};
  initPersonArray(persons);
  initDatedMeetingArray(meetings);
  initArray(dates);
  int result = 0;
  try
  {
    if (options.hasPersonInput)
    {
      std::ifstream personInput(options.personInputName);
      if (!personInput)
      {
        throw std::runtime_error("cannot open persons input");
      }
      PersonReadStats stats = {0, 0, false};
      readPersons(personInput, persons, stats);
    }

    for (size_t i = 0; i < options.dataInputNames.size; ++i)
    {
      std::ifstream meetingInput(options.dataInputNames.data[i]);
      if (!meetingInput)
      {
        throw std::runtime_error("cannot open meetings input");
      }
      if (!readDatedMeetings(meetingInput, meetings, persons, dates))
      {
        error << "invalid meeting data\n";
        result = 3;
        break;
      }
    }
    if (result == 0)
    {
      sortDates(dates);
      RangeState range = {true, 0, 0};
      RangeStateArray history = {nullptr, 0, 0};
      initInitialRange(dates, range);
      initArray(history);
      try
      {
        executeCommands(
            input,
            output,
            persons,
            meetings,
            dates,
            range,
            history);
      }
      catch (...)
      {
        destroyArray(history);
        throw;
      }
      destroyArray(history);
    }
  }
  catch (const std::exception&)
  {
    error << "file or internal error\n";
    result = 2;
  }
  catch (...)
  {
    error << "internal error\n";
    result = 2;
  }

  destroyArray(dates);
  destroyDatedMeetingArray(meetings);
  destroyPersonArray(persons);
  destroyArray(options.dataInputNames);
  return result;
}
