#include "person.hpp"

void alekseev::initPersonArray(PersonArray& persons)
{
  initArray(persons);
}

void alekseev::destroyPersonArray(PersonArray& persons)
{
  destroyArray(persons);
}

void alekseev::reservePersonArray(PersonArray& persons, size_t newCapacity)
{
  reserveArray(persons, newCapacity);
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
