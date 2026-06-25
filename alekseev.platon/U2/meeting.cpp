#include "meeting.hpp"

#include <istream>
#include <string>

#include <text_utils.hpp>

void alekseev::initMeetingArray(MeetingArray& meetings)
{
  initArray(meetings);
}

void alekseev::destroyMeetingArray(MeetingArray& meetings)
{
  destroyArray(meetings);
}

void alekseev::pushMeeting(MeetingArray& meetings, const Meeting& meeting)
{
  pushBack(meetings, meeting);
}

bool alekseev::parseMeetingLine(const std::string& line, Meeting& meeting)
{
  size_t position = 0;
  position = skipSpaces(line, position);
  if (position == line.size() || !parseSizeT(line, position, meeting.first))
  {
    return false;
  }

  size_t secondBegin = skipSpaces(line, position);
  if (secondBegin == position ||
      !parseSizeT(line, secondBegin, meeting.second))
  {
    return false;
  }
  position = secondBegin;

  size_t timeBegin = skipSpaces(line, position);
  if (timeBegin == position || !parseSizeT(line, timeBegin, meeting.time))
  {
    return false;
  }
  position = skipSpaces(line, timeBegin);
  return position == line.size();
}

bool alekseev::readMeetings(
    std::istream& input,
    MeetingArray& meetings,
    PersonArray& persons)
{
  std::string line;
  while (std::getline(input, line))
  {
    Meeting meeting = {0, 0, 0};
    if (!parseMeetingLine(line, meeting))
    {
      return false;
    }
    ensurePersonExists(persons, meeting.first);
    ensurePersonExists(persons, meeting.second);
    if (meeting.first != meeting.second)
    {
      pushMeeting(meetings, meeting);
    }
  }
  return input.eof();
}

size_t alekseev::findPersonIndex(const PersonArray& persons, size_t id)
{
  for (size_t i = 0; i < persons.size; ++i)
  {
    if (persons.data[i].id == id)
    {
      return i;
    }
  }
  return persons.size;
}

bool alekseev::hasPersonInfo(const Person& person)
{
  return !person.info.empty();
}

void alekseev::ensurePersonExists(PersonArray& persons, size_t id)
{
  if (findPersonIndex(persons, id) != persons.size)
  {
    return;
  }
  const Person person = {id, std::string()};
  pushPerson(persons, person);
}

void alekseev::collectMeetingViews(
    const MeetingArray& meetings,
    size_t id,
    MeetingViewArray& views)
{
  for (size_t i = 0; i < meetings.size; ++i)
  {
    const Meeting& meeting = meetings.data[i];
    if (meeting.first == id)
    {
      const MeetingView view = {meeting.second, meeting.time};
      pushBack(views, view);
    }
    else if (meeting.second == id)
    {
      const MeetingView view = {meeting.first, meeting.time};
      pushBack(views, view);
    }
  }
}

void alekseev::sortMeetingViews(MeetingViewArray& views)
{
  for (size_t i = 1; i < views.size; ++i)
  {
    const MeetingView value = views.data[i];
    size_t position = i;
    while (position > 0)
    {
      const MeetingView& previous = views.data[position - 1];
      if (previous.id < value.id ||
          (previous.id == value.id && previous.time <= value.time))
      {
        break;
      }
      views.data[position] = previous;
      --position;
    }
    views.data[position] = value;
  }
}

void alekseev::sortIds(Array< size_t >& ids)
{
  for (size_t i = 1; i < ids.size; ++i)
  {
    const size_t value = ids.data[i];
    size_t position = i;
    while (position > 0 && ids.data[position - 1] > value)
    {
      ids.data[position] = ids.data[position - 1];
      --position;
    }
    ids.data[position] = value;
  }
}
