#ifndef MEETING_HPP
#define MEETING_HPP

#include <exam_array.hpp>
#include <person_data.hpp>

#include <cstddef>
#include <iosfwd>
#include <string>

namespace alekseev
{
  struct Meeting
  {
    size_t first;
    size_t second;
    size_t time;
  };

  using MeetingArray = Array< Meeting >;

  struct MeetingView
  {
    size_t id;
    size_t time;
  };

  using MeetingViewArray = Array< MeetingView >;

  void initMeetingArray(MeetingArray& meetings);
  void destroyMeetingArray(MeetingArray& meetings);
  void pushMeeting(MeetingArray& meetings, const Meeting& meeting);
  bool parseMeetingLine(const std::string& line, Meeting& meeting);
  bool readMeetings(
      std::istream& input,
      MeetingArray& meetings,
      PersonArray& persons);
  size_t findPersonIndex(const PersonArray& persons, size_t id);
  bool hasPersonInfo(const Person& person);
  void ensurePersonExists(PersonArray& persons, size_t id);
  void collectMeetingViews(
      const MeetingArray& meetings,
      size_t id,
      MeetingViewArray& views);
  void sortMeetingViews(MeetingViewArray& views);
  void sortIds(Array< size_t >& ids);
}

#endif
