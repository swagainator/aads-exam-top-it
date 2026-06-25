#include <cstdio>
#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include "commands.hpp"

namespace alekseev
{
  namespace
  {
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

    void addCommandPerson(PersonArray& persons,
        size_t id,
        const std::string& info)
    {
      pushPerson(persons, Person{id, info});
    }

    void addCommandMeeting(DatedMeetingArray& meetings,
        const Date& date,
        size_t first,
        size_t second,
        size_t time)
    {
      pushDatedMeeting(meetings,
          DatedMeeting{date, first, second, time});
    }

    void initCommandData(PersonArray& persons,
        DatedMeetingArray& meetings,
        DateArray& dates,
        RangeState& range)
    {
      initPersonArray(persons);
      initDatedMeetingArray(meetings);
      initArray(dates);
      addCommandPerson(persons, 31, "The Agent");
      addCommandPerson(persons, 32, "Visible");
      addCommandPerson(persons, 33, "");
      addCommandPerson(persons, 41, "");
      pushBack(dates, Date{1, 1, 2026});
      pushBack(dates, Date{2, 1, 2026});
      pushBack(dates, Date{3, 1, 2026});
      pushBack(dates, Date{5, 1, 2026});
      addCommandMeeting(meetings, dates.data[0], 33, 41, 10);
      addCommandMeeting(meetings, dates.data[1], 33, 32, 11);
      addCommandMeeting(meetings, dates.data[2], 33, 31, 10);
      addCommandMeeting(meetings, dates.data[3], 32, 33, 99);
      initInitialRange(dates, range);
    }

    void destroyCommandData(PersonArray& persons,
        DatedMeetingArray& meetings,
        DateArray& dates)
    {
      destroyArray(dates);
      destroyDatedMeetingArray(meetings);
      destroyPersonArray(persons);
    }
  }
}

BOOST_AUTO_TEST_CASE(range_and_meets_respect_current_range)
{
  const char* const filename = "/tmp/alekseev-u3-range-command.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState range = {true, 0, 0};
  alekseev::RangeStateArray history = {nullptr, 0, 0};
  alekseev::initCommandData(persons, meetings, dates, range);
  alekseev::initArray(history);
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::handleRange("", output, dates, range));
    BOOST_REQUIRE(alekseev::handleAfter(" 3 1 2026",
            dates,
            range,
            history));
    BOOST_REQUIRE(alekseev::handleMeets(" 33",
            output,
            persons,
            meetings,
            dates,
            range));
  }

  const std::string expected =
      "1 1 2026 : 5 1 2026\n31 10\n32 99\n";
  BOOST_TEST(alekseev::readFile(filename) == expected);
  alekseev::destroyArray(history);
  alekseev::destroyCommandData(persons, meetings, dates);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(commons_and_filters_respect_range)
{
  const char* const filename = "/tmp/alekseev-u3-filter-command.txt";
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState range = {true, 0, 0};
  alekseev::initCommandData(persons, meetings, dates, range);
  alekseev::addCommandMeeting(meetings,
      dates.data[0],
      41,
      31,
      5);
  {
    std::ofstream output(filename);
    BOOST_REQUIRE(alekseev::handleCommons(" 33 31",
            output,
            persons,
            meetings,
            dates,
            range));
    BOOST_REQUIRE(alekseev::handleLess(" 20 33",
            output,
            persons,
            meetings,
            dates,
            range));
    BOOST_REQUIRE(alekseev::handleGreater(" 20 33",
            output,
            persons,
            meetings,
            dates,
            range));
  }

  const std::string expected =
      "41\n31 10\n32 11\n41 10\n32 99\n";
  BOOST_TEST(alekseev::readFile(filename) == expected);
  alekseev::destroyCommandData(persons, meetings, dates);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(redesc_and_deanon_update_all_data)
{
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::DatedMeetingArray meetings = {nullptr, 0, 0};
  alekseev::DateArray dates = {nullptr, 0, 0};
  alekseev::RangeState range = {true, 0, 0};
  alekseev::initCommandData(persons, meetings, dates, range);

  BOOST_REQUIRE(alekseev::handleRedesc(" 33 \"Mr. Bond\"", persons));
  BOOST_TEST(persons.data[alekseev::findPersonIndex(persons, 33)].info ==
      "Mr. Bond");
  BOOST_REQUIRE(alekseev::handleDeanon(" 41 31", persons, meetings));
  BOOST_TEST(alekseev::findPersonIndex(persons, 41) == persons.size);
  BOOST_TEST(meetings.data[0].second == 31);

  alekseev::destroyCommandData(persons, meetings, dates);
}

BOOST_AUTO_TEST_CASE(out_persons_ignores_anonymous_people)
{
  const char* const filename = "/tmp/alekseev-u3-out-persons.txt";
  std::remove(filename);
  alekseev::PersonArray persons = {nullptr, 0, 0};
  alekseev::initPersonArray(persons);
  alekseev::addCommandPerson(persons, 31, "The Agent");
  alekseev::addCommandPerson(persons, 33, "");

  const std::string arguments = std::string(" ") + filename;
  BOOST_REQUIRE(alekseev::handleOutPersons(arguments, persons));
  BOOST_TEST(alekseev::readFile(filename) == "31 The Agent\n");

  alekseev::destroyPersonArray(persons);
  std::remove(filename);
}
