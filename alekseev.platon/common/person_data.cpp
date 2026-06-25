#include "person_data.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>

#include <text_utils.hpp>

void alekseev::initPersonArray(PersonArray& persons)
{
  initArray(persons);
}

void alekseev::destroyPersonArray(PersonArray& persons)
{
  destroyArray(persons);
}

bool alekseev::containsPersonId(const PersonArray& persons, size_t id)
{
  for (size_t i = 0; i < persons.size; ++i)
  {
    if (persons.data[i].id == id)
    {
      return true;
    }
  }
  return false;
}

void alekseev::pushPerson(PersonArray& persons, const Person& person)
{
  pushBack(persons, person);
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
  if (position == end || containsPersonId(persons, id))
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
