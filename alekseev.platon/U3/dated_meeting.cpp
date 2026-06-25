#include "dated_meeting.hpp"

#include <istream>
#include <string>

#include <text_utils.hpp>

namespace alekseev
{
  namespace
  {
    bool parseNextSize(const std::string& line,
        size_t& position,
        size_t& value)
    {
      size_t begin = skipSpaces(line, position);
      if ((begin == position) || !parseSizeT(line, begin, value))
      {
        return false;
      }
      position = begin;
      return true;
    }
  }
}

void alekseev::initDatedMeetingArray(DatedMeetingArray& meetings)
{
  initArray(meetings);
}

void alekseev::destroyDatedMeetingArray(DatedMeetingArray& meetings)
{
  destroyArray(meetings);
}

void alekseev::pushDatedMeeting(DatedMeetingArray& meetings,
    const DatedMeeting& meeting)
{
  pushBack(meetings, meeting);
}

bool alekseev::parseDatedMeetingLine(const std::string& line,
    DatedMeeting& meeting)
{
  size_t position = skipSpaces(line, 0);
  if ((position == line.size()) ||
      !parseSizeT(line, position, meeting.date.day) ||
      !parseNextSize(line, position, meeting.date.month) ||
      !parseNextSize(line, position, meeting.date.year) ||
      !parseNextSize(line, position, meeting.first) ||
      !parseNextSize(line, position, meeting.second) ||
      !parseNextSize(line, position, meeting.time))
  {
    return false;
  }
  position = skipSpaces(line, position);
  return position == line.size();
}

bool alekseev::readDatedMeetings(std::istream& input,
    DatedMeetingArray& meetings,
    PersonArray& persons,
    DateArray& dates)
{
  std::string line;
  bool hasMeetingRow = false;
  bool hasInvalidRow = false;
  while (std::getline(input, line))
  {
    DatedMeeting meeting = {{0, 0, 0}, 0, 0, 0};
    if (!parseDatedMeetingLine(line, meeting))
    {
      size_t position = 0;
      Date date = {0, 0, 0};
      if (parseDateFromCommand(line, position, date))
      {
        pushUniqueDate(dates, date);
      }
      hasInvalidRow = true;
      continue;
    }
    hasMeetingRow = true;
    pushUniqueDate(dates, meeting.date);
    if (meeting.first != meeting.second)
    {
      ensurePersonExists(persons, meeting.first);
      ensurePersonExists(persons, meeting.second);
      pushDatedMeeting(meetings, meeting);
    }
  }
  return input.eof() && (!hasInvalidRow || hasMeetingRow);
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

void alekseev::removePersonAt(PersonArray& persons, size_t index)
{
  if (index >= persons.size)
  {
    return;
  }
  for (size_t i = index + 1; i < persons.size; ++i)
  {
    persons.data[i - 1] = persons.data[i];
  }
  --persons.size;
}

void alekseev::collectMeetingViews(const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range,
    size_t id,
    MeetingViewArray& views)
{
  for (size_t i = 0; i < meetings.size; ++i)
  {
    const DatedMeeting& meeting = meetings.data[i];
    if (!isDateInRange(dates, range, meeting.date))
    {
      continue;
    }
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

void alekseev::collectCommonPersons(const DatedMeetingArray& meetings,
    const DateArray& dates,
    const RangeState& range,
    size_t first,
    size_t second,
    Array< size_t >& ids)
{
  MeetingViewArray firstViews = {nullptr, 0, 0};
  MeetingViewArray secondViews = {nullptr, 0, 0};
  initArray(firstViews);
  initArray(secondViews);
  try
  {
    collectMeetingViews(meetings, dates, range, first, firstViews);
    collectMeetingViews(meetings, dates, range, second, secondViews);
    for (size_t i = 0; i < firstViews.size; ++i)
    {
      const size_t candidate = firstViews.data[i].id;
      bool inSecond = false;
      for (size_t j = 0; j < secondViews.size; ++j)
      {
        if (secondViews.data[j].id == candidate)
        {
          inSecond = true;
          break;
        }
      }
      bool duplicate = false;
      for (size_t j = 0; j < ids.size; ++j)
      {
        if (ids.data[j] == candidate)
        {
          duplicate = true;
          break;
        }
      }
      if (inSecond && !duplicate)
      {
        pushBack(ids, candidate);
      }
    }
  }
  catch (...)
  {
    destroyArray(secondViews);
    destroyArray(firstViews);
    throw;
  }
  destroyArray(secondViews);
  destroyArray(firstViews);
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
      if ((previous.id < value.id) ||
          ((previous.id == value.id) && (previous.time <= value.time)))
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
    while ((position > 0) && (ids.data[position - 1] > value))
    {
      ids.data[position] = ids.data[position - 1];
      --position;
    }
    ids.data[position] = value;
  }
}

void alekseev::replacePersonInMeetings(DatedMeetingArray& meetings,
    size_t oldId,
    size_t newId)
{
  for (size_t i = 0; i < meetings.size; ++i)
  {
    if (meetings.data[i].first == oldId)
    {
      meetings.data[i].first = newId;
    }
    if (meetings.data[i].second == oldId)
    {
      meetings.data[i].second = newId;
    }
  }
}

void alekseev::removeSelfMeetings(DatedMeetingArray& meetings)
{
  size_t outputIndex = 0;
  for (size_t i = 0; i < meetings.size; ++i)
  {
    if (meetings.data[i].first != meetings.data[i].second)
    {
      meetings.data[outputIndex] = meetings.data[i];
      ++outputIndex;
    }
  }
  meetings.size = outputIndex;
}
