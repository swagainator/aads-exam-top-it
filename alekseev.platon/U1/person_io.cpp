#include "person_io.hpp"

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>

#include <text_utils.hpp>

bool alekseev::parseProgramOptions(int argc, char** argv, ProgramOptions& options)
{
  options.inputName = nullptr;
  options.outputName = nullptr;
  options.hasInput = false;
  options.hasOutput = false;

  if (argc < 1 || argc > 3)
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
      if (options.hasInput || argument[3] == '\0')
      {
        return false;
      }
      options.inputName = argument + 3;
      options.hasInput = true;
    }
    else if (startsWith(argument, "out:"))
    {
      if (options.hasOutput || argument[4] == '\0')
      {
        return false;
      }
      options.outputName = argument + 4;
      options.hasOutput = true;
    }
    else
    {
      return false;
    }
  }
  return true;
}

bool alekseev::parsePersonLine(
    const std::string& line,
    const PersonArray& persons,
    Person& person)
{
  size_t position = 0;
  size_t id = 0;
  if (!parseSizeT(line, position, id))
  {
    return false;
  }

  position = skipSpaces(line, position);
  const size_t end = trimRight(line, position);
  if (position == end)
  {
    return false;
  }
  if (containsPersonId(persons, id))
  {
    return false;
  }

  person.id = id;
  person.info.assign(line, position, end - position);
  return true;
}

void alekseev::readPersons(
    std::istream& input,
    PersonArray& persons,
    size_t& successCount,
    size_t& ignoredCount)
{
  successCount = 0;
  ignoredCount = 0;
  std::string line;
  while (std::getline(input, line))
  {
    Person person = {0, std::string()};
    if (!parsePersonLine(line, persons, person))
    {
      ++ignoredCount;
      continue;
    }
    pushPerson(persons, person);
    ++successCount;
  }
  if (!input.eof())
  {
    throw std::runtime_error("input failure");
  }
}

void alekseev::writePersons(std::ostream& output, const PersonArray& persons)
{
  for (size_t i = 0; i < persons.size; ++i)
  {
    output << persons.data[i].id << ' ' << persons.data[i].info << '\n';
  }
  output.flush();
  if (!output)
  {
    throw std::runtime_error("output failure");
  }
}

int alekseev::runU1(
    int argc,
    char** argv,
    std::istream& input,
    std::ostream& output,
    std::ostream& error)
{
  ProgramOptions options = {nullptr, nullptr, false, false};
  if (!parseProgramOptions(argc, argv, options))
  {
    error << "invalid arguments\n";
    return 1;
  }

  PersonArray persons = {nullptr, 0, 0};
  initPersonArray(persons);
  try
  {
    std::ifstream inputFile;
    std::istream* inputSource = &input;
    if (options.hasInput)
    {
      inputFile.open(options.inputName);
      if (!inputFile)
      {
        throw std::runtime_error("cannot open input");
      }
      inputSource = &inputFile;
    }

    size_t successCount = 0;
    size_t ignoredCount = 0;
    readPersons(*inputSource, persons, successCount, ignoredCount);
    if (inputFile.is_open())
    {
      inputFile.close();
    }

    std::ofstream outputFile;
    std::ostream* outputTarget = &output;
    if (options.hasOutput)
    {
      outputFile.open(options.outputName);
      if (!outputFile)
      {
        throw std::runtime_error("cannot open output");
      }
      outputTarget = &outputFile;
    }

    writePersons(*outputTarget, persons);
    error << successCount << ' ' << ignoredCount << '\n';
    if (!error)
    {
      throw std::runtime_error("error output failure");
    }
  }
  catch (const std::exception&)
  {
    destroyPersonArray(persons);
    error << "file or internal error\n";
    return 2;
  }
  catch (...)
  {
    destroyPersonArray(persons);
    error << "internal error\n";
    return 2;
  }

  destroyPersonArray(persons);
  return 0;
}
