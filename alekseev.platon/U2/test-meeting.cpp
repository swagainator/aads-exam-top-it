#define BOOST_TEST_MODULE U2_MEETING

#include <cstdio>
#include <fstream>
#include <string>

#include <boost/test/included/unit_test.hpp>

#include "meeting.hpp"

BOOST_AUTO_TEST_CASE(parse_valid_meeting_line)
{
  alekseev::Meeting meeting = {0, 0, 0};

  BOOST_TEST(alekseev::parseMeetingLine("33 32 11", meeting));
  BOOST_TEST(meeting.first == 33);
  BOOST_TEST(meeting.second == 32);
  BOOST_TEST(meeting.time == 11);
}

BOOST_AUTO_TEST_CASE(reject_invalid_meeting_lines)
{
  alekseev::Meeting meeting = {0, 0, 0};

  BOOST_TEST(!alekseev::parseMeetingLine("", meeting));
  BOOST_TEST(!alekseev::parseMeetingLine("33 32", meeting));
  BOOST_TEST(!alekseev::parseMeetingLine("33 32 11 extra", meeting));
  BOOST_TEST(!alekseev::parseMeetingLine("-33 32 11", meeting));
  BOOST_TEST(!alekseev::parseMeetingLine("33 bad 11", meeting));
}

BOOST_AUTO_TEST_CASE(ignore_self_and_add_unknown_persons)
{
  const char* const filename = "/tmp/alekseev-u2-meeting-test.txt";
  {
    std::ofstream output(filename);
    output << "33 33 100\n33 41 10\n";
  }

  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  {
    std::ifstream input(filename);
    BOOST_REQUIRE(alekseev::readMeetings(input, meetings, persons));
  }

  BOOST_TEST(meetings.size == 1);
  BOOST_TEST(persons.size == 2);
  const size_t firstIndex = alekseev::findPersonIndex(persons, 33);
  const size_t secondIndex = alekseev::findPersonIndex(persons, 41);
  BOOST_REQUIRE(firstIndex != persons.size);
  BOOST_REQUIRE(secondIndex != persons.size);
  BOOST_TEST(!alekseev::hasPersonInfo(persons.data[firstIndex]));
  BOOST_TEST(!alekseev::hasPersonInfo(persons.data[secondIndex]));

  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(store_each_undirected_meeting_once)
{
  const char* const filename = "/tmp/alekseev-u2-meeting-once.txt";
  {
    std::ofstream output(filename);
    output << "33 41 10\n";
    output << "33 32 11\n";
    output << "33 31 10\n";
    output << "33 33 100\n";
    output << "32 33 99\n";
  }

  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  {
    std::ifstream input(filename);
    BOOST_REQUIRE(alekseev::readMeetings(input, meetings, persons));
  }

  BOOST_TEST(meetings.size == 4);
  BOOST_TEST(meetings.data[2].first == 33);
  BOOST_TEST(meetings.data[2].second == 31);
  BOOST_TEST(meetings.data[2].time == 10);

  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(skip_empty_meeting_lines)
{
  const char* const filename = "/tmp/alekseev-u2-empty-meetings.txt";
  {
    std::ofstream output(filename);
    output << '\n';
    output << " \t \n";
  }

  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::MeetingArray meetings = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initMeetingArray(meetings);
  {
    std::ifstream input(filename);
    BOOST_REQUIRE(alekseev::readMeetings(input, meetings, persons));
  }
  BOOST_TEST(meetings.size == 0);
  BOOST_TEST(persons.size == 0);

  alekseev::destroyMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}
