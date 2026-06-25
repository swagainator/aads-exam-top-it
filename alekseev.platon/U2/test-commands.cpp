#include <boost/test/unit_test.hpp>

#include "commands.hpp"

#include <cstdio>
#include <fstream>
#include <string>

namespace alekseev
{
  namespace
  {
    void addPerson(PersonArray& persons, size_t id, const std::string& info)
    {
      const Person person = {id, info};
      pushPerson(persons, person);
    }

    void addMeeting(
        MeetingArray& meetings,
        size_t first,
        size_t second,
        size_t time)
    {
      const Meeting meeting = {first, second, time};
      pushMeeting(meetings, meeting);
    }

    std::string readFile(const char* filename)
    {
      std::ifstream input(filename);
      std::string result;
      char value = '\0';
      while (input.get(value))
      {
        result += value;
      }
      return result;
    }
  }
}

BOOST_AUTO_TEST_CASE(anons_are_sorted)
{
  const char* const filename = "/tmp/alekseev-u2-anons.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::addPerson(persons, 41, "");
  alekseev::addPerson(persons, 31, "The Agent");
  alekseev::addPerson(persons, 33, "");
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::handleAnons("", output, persons));
  }

  BOOST_TEST(alekseev::readFile(filename) == "33\n41\n");
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(desc_handles_described_and_anonymous)
{
  const char* const filename = "/tmp/alekseev-u2-desc.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::addPerson(persons, 31, "The Agent");
  alekseev::addPerson(persons, 33, "");
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::handleDesc(" 31", output, persons));
    BOOST_REQUIRE(alekseev::handleDesc(" 33", output, persons));
  }

  BOOST_TEST(alekseev::readFile(filename) == "The Agent\n<ANON>\n");
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(meets_are_sorted_by_id_and_time)
{
  const char* const filename = "/tmp/alekseev-u2-meets.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  alekseev::addPerson(persons, 31, "");
  alekseev::addPerson(persons, 32, "");
  alekseev::addPerson(persons, 33, "");
  alekseev::addPerson(persons, 41, "");
  alekseev::addMeeting(meetings, 33, 41, 10);
  alekseev::addMeeting(meetings, 33, 32, 11);
  alekseev::addMeeting(meetings, 33, 31, 10);
  alekseev::addMeeting(meetings, 32, 33, 99);
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::handleMeets(" 33", output, persons, meetings));
    BOOST_REQUIRE(alekseev::handleMeets(" 31", output, persons, meetings));
    BOOST_REQUIRE(alekseev::handleMeets(" 32", output, persons, meetings));
  }

  const std::string expected =
      "31 10\n32 11\n32 99\n41 10\n33 10\n33 11\n33 99\n";
  BOOST_TEST(alekseev::readFile(filename) == expected);
  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(commons_are_unique_and_sorted)
{
  const char* const filename = "/tmp/alekseev-u2-commons.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  for (size_t id = 1; id <= 4; ++id)
  {
    alekseev::addPerson(persons, id, "known");
  }
  alekseev::addMeeting(meetings, 1, 4, 10);
  alekseev::addMeeting(meetings, 2, 4, 20);
  alekseev::addMeeting(meetings, 1, 3, 30);
  alekseev::addMeeting(meetings, 2, 3, 40);
  alekseev::addMeeting(meetings, 1, 3, 50);
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(
        alekseev::handleCommons(" 1 2", output, persons, meetings));
  }

  BOOST_TEST(alekseev::readFile(filename) == "3\n4\n");
  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(less_and_greater_filter_meetings)
{
  const char* const filename = "/tmp/alekseev-u2-filters.txt";
  const char* const inputFilename = "/tmp/alekseev-u2-filter-commands.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  alekseev::addPerson(persons, 28, "");
  alekseev::addPerson(persons, 31, "");
  alekseev::addPerson(persons, 32, "");
  alekseev::addPerson(persons, 33, "");
  alekseev::addPerson(persons, 41, "");
  alekseev::addMeeting(meetings, 31, 33, 9);
  alekseev::addMeeting(meetings, 41, 28, 80);
  alekseev::addMeeting(meetings, 41, 32, 20);
  alekseev::addMeeting(meetings, 41, 33, 10);
  {
    std::ofstream commandOutput(inputFilename);
    commandOutput << "less 10 31\ngreater 9 41";
  }
  {
    std::ifstream commandInput(inputFilename);
    std::ofstream output(filename);
    alekseev::executeCommands(commandInput, output, persons, meetings);
  }

  BOOST_TEST(
      alekseev::readFile(filename) == "33 9\n28 80\n32 20\n33 10");
  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
  std::remove(inputFilename);
}

BOOST_AUTO_TEST_CASE(redesc_updates_description)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::addPerson(persons, 33, "");

  BOOST_REQUIRE(alekseev::handleRedesc(" 33 \"Mr. Bond\"", persons));
  BOOST_TEST(persons.data[0].info == "Mr. Bond");

  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(desc_with_description_updates_person)
{
  const char* const filename = "/tmp/alekseev-u2-desc-update.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  alekseev::addPerson(persons, 33, "");
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::executeCommandLine(
        "desc 33 \"Agent 007\"",
        output,
        persons,
        meetings));
  }

  BOOST_TEST(persons.data[0].info == "Agent 007");
  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(meet_alias_and_empty_results)
{
  const char* const filename = "/tmp/alekseev-u2-empty-results.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  alekseev::addPerson(persons, 33, "");
  alekseev::addPerson(persons, 41, "");
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::executeCommandLine(
        "meet 33",
        output,
        persons,
        meetings));
    BOOST_REQUIRE(alekseev::handleCommons(
        " 33 41",
        output,
        persons,
        meetings));
    BOOST_REQUIRE(alekseev::handleLess(
        " 1 33",
        output,
        persons,
        meetings));
  }

  BOOST_TEST(alekseev::readFile(filename) == "\n\n");
  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(deanon_replaces_person_and_removes_self_meetings)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  alekseev::addPerson(persons, 31, "The Agent");
  alekseev::addPerson(persons, 41, "");
  alekseev::addPerson(persons, 50, "Other");
  alekseev::addMeeting(meetings, 41, 31, 5);
  alekseev::addMeeting(meetings, 41, 50, 10);

  BOOST_REQUIRE(alekseev::handleDeanon(" 41 31", persons, meetings));
  BOOST_TEST(alekseev::findPersonIndex(persons, 41) == persons.size);
  BOOST_TEST(meetings.size == 1);
  BOOST_TEST(meetings.data[0].first == 31);
  BOOST_TEST(meetings.data[0].second == 50);

  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
}

BOOST_AUTO_TEST_CASE(out_persons_writes_only_described)
{
  const char* const filename = "/tmp/alekseev-u2-out-persons.txt";
  std::remove(filename);
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::addPerson(persons, 31, "The Agent");
  alekseev::addPerson(persons, 33, "");

  const std::string arguments = std::string(" ") + filename;
  BOOST_REQUIRE(alekseev::handleOutPersons(arguments, persons));
  BOOST_REQUIRE(alekseev::handleRedesc(" 33 \"Agent 007\"", persons));
  BOOST_REQUIRE(alekseev::handleOutPersons(arguments, persons));
  const std::string expected =
      "31 The Agent\n31 The Agent\n33 Agent 007\n";
  BOOST_TEST(alekseev::readFile(filename) == expected);

  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(out_persons_writes_newline_for_empty_result)
{
  const char* const filename = "/tmp/alekseev-u2-out-empty.txt";
  std::remove(filename);
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::addPerson(persons, 33, "");

  const std::string arguments = std::string(" ") + filename;
  BOOST_REQUIRE(alekseev::handleOutPersons(arguments, persons));
  BOOST_TEST(alekseev::readFile(filename) == "\n");

  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}
