#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>

#include "dated_meeting.hpp"

namespace alekseev
{
  void executeCommands(
      std::istream& input,
      std::ostream& output,
      PersonArray& persons,
      DatedMeetingArray& meetings,
      const DateArray& dates,
      RangeState& range,
      RangeStateArray& history);
  bool executeCommandLine(
      const std::string& line,
      std::ostream& output,
      PersonArray& persons,
      DatedMeetingArray& meetings,
      const DateArray& dates,
      RangeState& range,
      RangeStateArray& history);
  bool handleAnons(
      const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons);
  bool handleDesc(
      const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons);
  bool handleMeets(
      const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const DatedMeetingArray& meetings,
      const DateArray& dates,
      const RangeState& range);
  bool handleRange(
      const std::string& arguments,
      std::ostream& output,
      const DateArray& dates,
      const RangeState& range);
}

#endif
