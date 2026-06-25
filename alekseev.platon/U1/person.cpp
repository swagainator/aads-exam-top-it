#include "person.hpp"

#include <limits>
#include <stdexcept>

void alekseev::initPersonArray(PersonArray& persons)
{
  persons.data = nullptr;
  persons.size = 0;
  persons.capacity = 0;
}

void alekseev::destroyPersonArray(PersonArray& persons)
{
  delete[] persons.data;
  persons.data = nullptr;
  persons.size = 0;
  persons.capacity = 0;
}

void alekseev::reservePersonArray(PersonArray& persons, size_t newCapacity)
{
  if (newCapacity <= persons.capacity)
  {
    return;
  }

  Person* const newData = new Person[newCapacity];
  try
  {
    for (size_t i = 0; i < persons.size; ++i)
    {
      newData[i] = persons.data[i];
    }
  }
  catch (...)
  {
    delete[] newData;
    throw;
  }

  delete[] persons.data;
  persons.data = newData;
  persons.capacity = newCapacity;
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
  if (persons.size == persons.capacity)
  {
    const size_t maxSize = std::numeric_limits< size_t >::max();
    if (persons.capacity > maxSize / 2)
    {
      throw std::length_error("person array capacity overflow");
    }
    const size_t newCapacity = persons.capacity == 0 ? 1 : persons.capacity * 2;
    reservePersonArray(persons, newCapacity);
  }
  persons.data[persons.size] = person;
  ++persons.size;
}
