#ifndef DATED_MEETING_HPP
#define DATED_MEETING_HPP

#include <cstddef>
#include <iosfwd>
#include <string>

#include "date.hpp"
#include "range_state.hpp"

#include <exam_array.hpp>
#include <person_data.hpp>

namespace alekseev
{
  struct DatedMeeting
  {
    Date date;
    size_t first;
    size_t second;
    size_t time;
  };

  using DatedMeetingArray = Array< DatedMeeting >;

  struct MeetingView
  {
    size_t id;
    size_t time;
  };

  using MeetingViewArray = Array< MeetingView >;

  void initDatedMeetingArray(DatedMeetingArray& meetings);
  void destroyDatedMeetingArray(DatedMeetingArray& meetings);
  void pushDatedMeeting(DatedMeetingArray& meetings,
      const DatedMeeting& meeting);
  bool parseDatedMeetingLine(const std::string& line,
      DatedMeeting& meeting);
  bool readDatedMeetings(std::istream& input,
      DatedMeetingArray& meetings,
      PersonArray& persons,
      DateArray& dates);
  size_t findPersonIndex(const PersonArray& persons, size_t id);
  bool hasPersonInfo(const Person& person);
  void ensurePersonExists(PersonArray& persons, size_t id);
  void removePersonAt(PersonArray& persons, size_t index);
  void collectMeetingViews(const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range,
      size_t id,
      MeetingViewArray& views);
  void collectCommonPersons(const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range,
      size_t first,
      size_t second,
      Array< size_t >& ids);
  void sortMeetingViews(MeetingViewArray& views);
  void sortIds(Array< size_t >& ids);
  void replacePersonInMeetings(DatedMeetingArray& meetings,
      size_t oldId,
      size_t newId);
  void removeSelfMeetings(DatedMeetingArray& meetings);
}

#endif
