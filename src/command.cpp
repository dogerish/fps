#include "command.h"
#include <vector>
#include <string>

const char* parser_errmsg(int error)
{
	switch (error)
	{
	case PARSER_FINE:            return "";
	case PARSER_UNKNOWN_COMMAND: return "Unknown command";
	case PARSER_UNMATCHED_QUOTE: return "Unmatched quote";
	default:                     return "Unknown error";
	}
}

int parsecommand(const std::string &cmdstr, std::vector<std::string> &argv)
{
	int argc = -1;
	// if it doesn't start with whitespace, initialize first argument
	if (cmdstr[0] != ' ')
	{
		argc++;
		argv.push_back("");
	}
	// iterate through characters
	for (std::string::const_iterator c = cmdstr.begin(); c != cmdstr.end(); c++)
	{
		switch (*c)
		{
		case ' ':
			// seek to end of space sequence
			for (; c + 1 != cmdstr.end() && *(c + 1) == ' '; c++);
			// if there's another argument, initialize it
			if (c + 1 != cmdstr.end())
			{
				argc++;
				argv.push_back("");
			}
			break;
		case '\\':
			// escape next character
			if (c + 1 != cmdstr.end()) argv[argc] += *++c;
			break;
		case '"':
			// read until next unescaped quote or end of string
			while (++c != cmdstr.end() && *c != '"')
			{
				if (*c == '\\' && c + 1 != cmdstr.end()) argv[argc] += *++c;
				else argv[argc] += *c;
			}
			if (c == cmdstr.end()) return PARSER_UNMATCHED_QUOTE;
			break;
		default:
			// append character to current argument
			argv[argc] += *c;
			break;
		}
	}
	return PARSER_FINE;
}

int runcommand(const std::vector<Command> &cmdv, const std::string &cmdstr)
{
	int error = PARSER_FINE;
	if (!cmdstr.size()) return error;
	// get argv and check for error
	std::vector<std::string> argv;
	if (error = parsecommand(cmdstr, argv)) return error;
	// find and call command
	for (const Command &cmd : cmdv) if (argv[0] == cmd.name) return cmd.call(argv);
	return error = PARSER_UNKNOWN_COMMAND;
}

int parseint(const std::string &str)
{ return std::stoi(str); }

int parseint(const std::string &str, const int default_value)
{
	try { return parseint(str); }
	catch (std::invalid_argument const &e) { return default_value; }
	catch (std::out_of_range     const &e) { return default_value; }
}

float parsefloat(const std::string &str)
{ return std::stof(str); }

float parsefloat(const std::string &str, const float default_value)
{
	try { return parsefloat(str); }
	catch (std::invalid_argument const &e) { return default_value; }
	catch (std::out_of_range     const &e) { return default_value; }
}
