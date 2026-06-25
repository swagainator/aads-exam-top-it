#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>

#include "meeting.hpp"

#include <person_data.hpp>

namespace alekseev
{
  void executeCommands(
      std::istream& input,
      std::ostream& output,
      PersonArray& persons,
      MeetingArray& meetings);
  bool executeCommandLine(
      const std::string& line,
      std::ostream& output,
      PersonArray& persons,
      MeetingArray& meetings);
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
      const MeetingArray& meetings);
  bool handleCommons(
      const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const MeetingArray& meetings);
  bool handleLess(
      const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const MeetingArray& meetings);
  bool handleGreater(
      const std::string& arguments,
      std::ostream& output,
      const PersonArray& persons,
      const MeetingArray& meetings);
  bool handleRedesc(
      const std::string& arguments,
      PersonArray& persons);
  bool handleDeanon(
      const std::string& arguments,
      PersonArray& persons,
      MeetingArray& meetings);
  bool handleOutPersons(
      const std::string& arguments,
      const PersonArray& persons);
}

#endif
