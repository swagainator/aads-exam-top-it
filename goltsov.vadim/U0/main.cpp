#include <iostream>

namespace goltsov
{
  std::ostream& printName(std::ostream& os)
  {
    os << "goltsov.vadim\n";
    return os;
  }
}

int main()
{
  goltsov::printName(std::cout);
}
