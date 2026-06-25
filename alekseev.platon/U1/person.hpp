#ifndef PERSON_HPP
#define PERSON_HPP

#include <exam_array.hpp>

#include <cstddef>
#include <string>

namespace alekseev
{
  struct Person
  {
    size_t id;
    std::string info;
  };

  using PersonArray = Array< Person >;

  void initPersonArray(PersonArray& persons);
  void destroyPersonArray(PersonArray& persons);
  void reservePersonArray(PersonArray& persons, size_t newCapacity);
  bool containsPersonId(const PersonArray& persons, size_t id);
  void pushPerson(PersonArray& persons, const Person& person);
}

#endif
