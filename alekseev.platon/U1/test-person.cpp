#define BOOST_TEST_MODULE U1_PERSON

#include <string>

#include <boost/test/included/unit_test.hpp>

#include "person_io.hpp"

#include <person_data.hpp>

BOOST_AUTO_TEST_CASE(parse_regular_person)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::Person person = {0, std::string()};

  BOOST_TEST(alekseev::parsePersonLine("100 Sneaky person", persons, person));
  BOOST_TEST(person.id == 100);
  BOOST_TEST(person.info == "Sneaky person");

  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(reject_duplicate_id)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  const alekseev::Person existing = {31, "The Agent"};
  alekseev::pushPerson(persons, existing);
  alekseev::Person duplicate = {0, std::string()};

  BOOST_TEST(!alekseev::parsePersonLine("31 Mr. Bond", persons, duplicate));
  BOOST_TEST(persons.size == 1);
  BOOST_TEST(persons.data[0].info == "The Agent");

  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(reject_invalid_lines)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::Person person = {0, std::string()};

  BOOST_TEST(!alekseev::parsePersonLine("99", persons, person));
  BOOST_TEST(!alekseev::parsePersonLine("abc Bad", persons, person));
  BOOST_TEST(!alekseev::parsePersonLine("", persons, person));
  BOOST_TEST(!alekseev::parsePersonLine("44 \t  ", persons, person));

  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(preserve_quotes_and_trim_right_spaces)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::Person person = {0, std::string()};

  BOOST_TEST(alekseev::parsePersonLine("56 \"Scarface\" \t ", persons, person));
  BOOST_TEST(person.info == "\"Scarface\"");

  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(preserve_input_order)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  const char* const lines[] = {"100 First", "23 Second", "31 Third"};

  for (size_t i = 0; i < 3; ++i)
  {
    alekseev::Person person = {0, std::string()};
    BOOST_REQUIRE(alekseev::parsePersonLine(lines[i], persons, person));
    alekseev::pushPerson(persons, person);
  }

  BOOST_TEST(persons.data[0].id == 100);
  BOOST_TEST(persons.data[1].id == 23);
  BOOST_TEST(persons.data[2].id == 31);

  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(accept_input_and_output_options)
{
  char program[] = "program";
  char input[] = "in:file";
  char output[] = "out:result";
  char* arguments[] = {program, input, output};
  alekseev::ProgramOptions options = {nullptr, nullptr, false, false};

  BOOST_TEST(alekseev::parseProgramOptions(3, arguments, options));
  BOOST_TEST(options.hasInput);
  BOOST_TEST(options.hasOutput);
  BOOST_TEST(std::string(options.inputName) == "file");
  BOOST_TEST(std::string(options.outputName) == "result");
}

BOOST_AUTO_TEST_CASE(reject_duplicate_options)
{
  char program[] = "program";
  char firstInput[] = "in:first";
  char secondInput[] = "in:second";
  char firstOutput[] = "out:first";
  char secondOutput[] = "out:second";
  char* inputArguments[] = {program, firstInput, secondInput};
  char* outputArguments[] = {program, firstOutput, secondOutput};
  alekseev::ProgramOptions options = {nullptr, nullptr, false, false};

  BOOST_TEST(!alekseev::parseProgramOptions(3, inputArguments, options));
  BOOST_TEST(!alekseev::parseProgramOptions(3, outputArguments, options));
}

BOOST_AUTO_TEST_CASE(reject_empty_option_names)
{
  char program[] = "program";
  char input[] = "in:";
  char output[] = "out:";
  char* inputArguments[] = {program, input};
  char* outputArguments[] = {program, output};
  alekseev::ProgramOptions options = {nullptr, nullptr, false, false};

  BOOST_TEST(!alekseev::parseProgramOptions(2, inputArguments, options));
  BOOST_TEST(!alekseev::parseProgramOptions(2, outputArguments, options));
}
