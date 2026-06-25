#include "meeting_io.hpp"

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>

#include "commands.hpp"
#include "meeting.hpp"

#include <person_data.hpp>
#include <text_utils.hpp>

bool alekseev::parseProgramOptions(int argc,
    char** argv,
    ProgramOptions& options)
{
  options.personInputName = nullptr;
  options.meetingInputName = nullptr;
  options.hasPersonInput = false;
  bool hasMeetingInput = false;

  if ((argc < 2) || (argc > 3))
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
    if (startsWith(argument, "data:"))
    {
      if (hasMeetingInput || (argument[5] == '\0'))
      {
        return false;
      }
      options.meetingInputName = argument + 5;
      hasMeetingInput = true;
    }
    else if (startsWith(argument, "in:"))
    {
      if (options.hasPersonInput || (argument[3] == '\0'))
      {
        return false;
      }
      options.personInputName = argument + 3;
      options.hasPersonInput = true;
    }
    else
    {
      return false;
    }
  }
  return hasMeetingInput;
}

int alekseev::runU2(int argc,
    char** argv,
    std::istream& input,
    std::ostream& output,
    std::ostream& error)
{
  if (argc > 3)
  {
    error << "Too many arguments\n";
    return 0;
  }
  ProgramOptions options = {nullptr, nullptr, false};
  if (!parseProgramOptions(argc, argv, options))
  {
    error << "invalid arguments\n";
    return 1;
  }

  PersonArray persons = {nullptr, 0, 0};
  MeetingArray meetings = {nullptr, 0, 0};
  initPersonArray(persons);
  initMeetingArray(meetings);
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

    std::ifstream meetingInput(options.meetingInputName);
    if (!meetingInput)
    {
      throw std::runtime_error("cannot open meetings input");
    }
    if (!readMeetings(meetingInput, meetings, persons))
    {
      error << "invalid meeting data\n";
      result = 3;
    }
    if (result == 0)
    {
      executeCommands(input, output, persons, meetings);
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

  destroyMeetingArray(meetings);
  destroyPersonArray(persons);
  return result;
}
