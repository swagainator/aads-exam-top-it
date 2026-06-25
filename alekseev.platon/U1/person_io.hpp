#ifndef PERSON_IO_HPP
#define PERSON_IO_HPP

#include <iosfwd>

namespace alekseev
{
  struct ProgramOptions
  {
    const char* inputName;
    const char* outputName;
    bool hasInput;
    bool hasOutput;
  };

  bool parseProgramOptions(int argc, char** argv, ProgramOptions& options);
  int runU1(
      int argc,
      char** argv,
      std::istream& input,
      std::ostream& output,
      std::ostream& error);
}

#endif
