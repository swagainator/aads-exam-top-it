#ifndef MEETING_IO_HPP
#define MEETING_IO_HPP

#include <iosfwd>

namespace alekseev
{
  struct ProgramOptions
  {
    const char* personInputName;
    const char* meetingInputName;
    bool hasPersonInput;
  };

  bool parseProgramOptions(int argc, char** argv, ProgramOptions& options);
  int runU2(int argc,
      char** argv,
      std::istream& input,
      std::ostream& output,
      std::ostream& error);
}

#endif
