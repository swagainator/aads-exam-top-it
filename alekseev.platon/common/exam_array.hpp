#ifndef EXAM_ARRAY_HPP
#define EXAM_ARRAY_HPP

#include <cstddef>
#include <limits>
#include <stdexcept>

namespace alekseev
{
  template< class T >
  struct Array
  {
    T* data;
    size_t size;
    size_t capacity;
  };

  template< class T >
  void initArray(Array< T >& array)
  {
    array.data = nullptr;
    array.size = 0;
    array.capacity = 0;
  }

  template< class T >
  void destroyArray(Array< T >& array)
  {
    delete[] array.data;
    array.data = nullptr;
    array.size = 0;
    array.capacity = 0;
  }

  template< class T >
  void reserveArray(Array< T >& array, size_t newCapacity)
  {
    if (newCapacity <= array.capacity)
    {
      return;
    }

    T* const newData = new T[newCapacity];
    try
    {
      for (size_t i = 0; i < array.size; ++i)
      {
        newData[i] = array.data[i];
      }
    }
    catch (...)
    {
      delete[] newData;
      throw;
    }

    delete[] array.data;
    array.data = newData;
    array.capacity = newCapacity;
  }

  template< class T >
  void pushBack(Array< T >& array, const T& value)
  {
    if (array.size == array.capacity)
    {
      const size_t maxSize = std::numeric_limits< size_t >::max();
      if (array.capacity > maxSize / 2)
      {
        throw std::length_error("array capacity overflow");
      }
      const size_t newCapacity = array.capacity == 0 ? 1 : array.capacity * 2;
      reserveArray(array, newCapacity);
    }
    array.data[array.size] = value;
    ++array.size;
  }

  template< class T >
  void clearArray(Array< T >& array)
  {
    array.size = 0;
  }
}

#endif
