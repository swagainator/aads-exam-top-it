#ifndef PERSON_DATA_HPP
#define PERSON_DATA_HPP

#include <exam_array.hpp>

#include <cstddef>
#include <iosfwd>
#include <string>

namespace alekseev
{
  struct Person
  {
    size_t id;
    std::string info;
  };

  using PersonArray = Array< Person >;

  struct PersonReadStats
  {
    size_t success;
    size_t ignored;
    bool hasInput;
  };

  void initPersonArray(PersonArray& persons);
  void destroyPersonArray(PersonArray& persons);
  bool containsPersonId(const PersonArray& persons, size_t id);
  void pushPerson(PersonArray& persons, const Person& person);
  bool parsePersonLine(
      const std::string& line,
      const PersonArray& persons,
      Person& person);
  void readPersons(
      std::istream& input,
      PersonArray& persons,
      PersonReadStats& stats);
  void writePersons(std::ostream& output, const PersonArray& persons);
}

#endif
