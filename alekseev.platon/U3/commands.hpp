#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>

#include "dated_meeting.hpp"

namespace alekseev
{
  void executeCommands(std::istream& input,
      std::ostream& output,
      PersonArray& persons,
      DatedMeetingArray& meetings,
      const DateArray& dates,
      RangeState& range,
      RangeStateArray& history);
  bool executeCommandLine(const std::string& line,
      std::ostream& output,
      PersonArray& persons,
      DatedMeetingArray& meetings,
      const DateArray& dates,
      RangeState& range,
      RangeStateArray& history);
  bool handleAnons(const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons);
  bool handleDesc(const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons);
  bool handleMeets(const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range);
  bool handleCommons(const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range);
  bool handleLess(const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range);
  bool handleGreater(const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range);
  bool handleRedesc(const std::string& arguments,
      PersonArray& persons);
  bool handleDeanon(const std::string& arguments,
      PersonArray& persons,
      DatedMeetingArray& meetings);
  bool handleOutPersons(const std::string& arguments,
      const PersonArray& persons);
  bool handleRange(const std::string& arguments,
      std::ostream& output,
      const DateArray& dates,
      const RangeState& range);
  bool handleAfter(const std::string& arguments,
      const DateArray& dates,
      RangeState& range,
      RangeStateArray& history);
  bool handleBefore(const std::string& arguments,
      const DateArray& dates,
      RangeState& range,
      RangeStateArray& history);
  bool handlePopRange(const std::string& arguments,
      RangeState& range,
      RangeStateArray& history);
}

#endif
