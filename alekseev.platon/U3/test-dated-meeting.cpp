#include <boost/test/unit_test.hpp>

#include "dated_meeting.hpp"

#include <cstdio>
#include <fstream>

BOOST_AUTO_TEST_CASE(parse_valid_dated_meeting)
{
  alekseev::DatedMeeting meeting = {{0, 0, 0}, 0, 0, 0};

  BOOST_REQUIRE(
      alekseev::parseDatedMeetingLine(
          "1 2 2026 33 41 10",
          meeting));
  BOOST_TEST(meeting.date.day == 1);
  BOOST_TEST(meeting.date.month == 2);
  BOOST_TEST(meeting.date.year == 2026);
  BOOST_TEST(meeting.first == 33);
  BOOST_TEST(meeting.second == 41);
  BOOST_TEST(meeting.time == 10);
}

BOOST_AUTO_TEST_CASE(reject_invalid_dated_meetings)
{
  alekseev::DatedMeeting meeting = {{0, 0, 0}, 0, 0, 0};

  BOOST_TEST(!alekseev::parseDatedMeetingLine("", meeting));
  BOOST_TEST(
      !alekseev::parseDatedMeetingLine("1 2 2026 33 41", meeting));
  BOOST_TEST(
      !alekseev::parseDatedMeetingLine(
          "1 2 2026 33 41 10 extra",
          meeting));
  BOOST_TEST(
      !alekseev::parseDatedMeetingLine("-1 2 2026 33 41 10", meeting));
}

BOOST_AUTO_TEST_CASE(self_row_adds_date_but_not_meeting_or_person)
{
  const char* const filename = "/tmp/alekseev-u3-self-row.txt";
  {
    std::ofstream output(filename);
    output << "4 1 2026 33 33 100\n";
  }
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initDatedMeetingArray(meetings);
  alekseev::initArray(dates);
  {
    std::ifstream input(filename);
    BOOST_REQUIRE(
        alekseev::readDatedMeetings(
            input,
            meetings,
            persons,
            dates));
  }

  BOOST_TEST(meetings.size == 0);
  BOOST_TEST(persons.size == 0);
  BOOST_TEST(dates.size == 1);

  alekseev::destroyArray(dates);
  alekseev::destroyDatedMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(non_self_row_adds_anonymous_persons)
{
  const char* const filename = "/tmp/alekseev-u3-person-row.txt";
  {
    std::ofstream output(filename);
    output << "1 1 2026 33 41 10\n";
  }
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initDatedMeetingArray(meetings);
  alekseev::initArray(dates);
  {
    std::ifstream input(filename);
    BOOST_REQUIRE(
        alekseev::readDatedMeetings(
            input,
            meetings,
            persons,
            dates));
  }

  BOOST_TEST(meetings.size == 1);
  BOOST_TEST(persons.size == 2);
  BOOST_TEST(!alekseev::hasPersonInfo(persons.data[0]));
  BOOST_TEST(!alekseev::hasPersonInfo(persons.data[1]));

  alekseev::destroyArray(dates);
  alekseev::destroyDatedMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(mixed_file_keeps_date_from_incomplete_row)
{
  const char* const filename = "/tmp/alekseev-u3-mixed-row.txt";
  {
    std::ofstream output(filename);
    output << "10 10 1010 56 57 10\n";
    output << "5 5 1005 32 41\n";
  }
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initDatedMeetingArray(meetings);
  alekseev::initArray(dates);
  {
    std::ifstream input(filename);
    BOOST_REQUIRE(
        alekseev::readDatedMeetings(
            input,
            meetings,
            persons,
            dates));
  }

  BOOST_TEST(meetings.size == 1);
  BOOST_TEST(dates.size == 2);

  alekseev::destroyArray(dates);
  alekseev::destroyDatedMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(file_with_only_incomplete_row_is_invalid)
{
  const char* const filename = "/tmp/alekseev-u3-invalid-row.txt";
  {
    std::ofstream output(filename);
    output << "1 1 2026 33 41\n";
  }
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::initDatedMeetingArray(meetings);
  alekseev::initArray(dates);
  {
    std::ifstream input(filename);
    BOOST_TEST(
        !alekseev::readDatedMeetings(
            input,
            meetings,
            persons,
            dates));
  }

  alekseev::destroyArray(dates);
  alekseev::destroyDatedMeetingArray(meetings);
  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}
