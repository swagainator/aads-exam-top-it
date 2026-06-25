#include "text_utils.hpp"

#include <limits>

bool alekseev::isSpace(char value)
{
  return (value == ' ') || (value == '\t') || (value == '\r') ||
      (value == '\v') || (value == '\f');
}

size_t alekseev::skipSpaces(const std::string& text, size_t position)
{
  while ((position < text.size()) && isSpace(text[position]))
  {
    ++position;
  }
  return position;
}

size_t alekseev::trimRight(const std::string& text, size_t begin)
{
  size_t end = text.size();
  while ((end > begin) && isSpace(text[end - 1]))
  {
    --end;
  }
  return end;
}

bool alekseev::startsWith(const char* text, const char* prefix)
{
  if ((text == nullptr) || (prefix == nullptr))
  {
    return false;
  }
  size_t position = 0;
  while (prefix[position] != '\0')
  {
    if (text[position] != prefix[position])
    {
      return false;
    }
    ++position;
  }
  return true;
}

bool alekseev::parseSizeT(const std::string& text,
    size_t& position,
    size_t& value)
{
  const size_t begin = position;
  size_t result = 0;
  const size_t maxValue = std::numeric_limits< size_t >::max();
  while ((position < text.size()) && (text[position] >= '0') &&
      (text[position] <= '9'))
  {
    const size_t digit = static_cast< size_t >(text[position] - '0');
    if (result > (maxValue - digit) / 10)
    {
      return false;
    }
    result = result * 10 + digit;
    ++position;
  }
  if (position == begin)
  {
    return false;
  }
  value = result;
  return true;
}
