#ifndef TEXT_UTILS_HPP
#define TEXT_UTILS_HPP

#include <cstddef>
#include <string>

namespace alekseev
{
  bool isSpace(char value);
  size_t skipSpaces(const std::string& text, size_t position);
  size_t trimRight(const std::string& text, size_t begin);
  bool startsWith(const char* text, const char* prefix);
  bool parseSizeT(const std::string& text, size_t& position, size_t& value);
}

#endif
