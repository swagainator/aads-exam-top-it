#ifndef MEETING_IO_HPP
#define MEETING_IO_HPP

#include <exam_array.hpp>

#include <iosfwd>

namespace alekseev
{
  struct ProgramOptions
  {
    const char* personInputName;
    bool hasPersonInput;
    Array< const char* > dataInputNames;
  };

  bool parseProgramOptions(int argc, char** argv, ProgramOptions& options);
  int runU3(
      int argc,
      char** argv,
      std::istream& input,
      std::ostream& output,
      std::ostream& error);
}

#endif
