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

int runcommand(const std::vector<Command> &cmdv, const std::string &cmdstr, int &error)
{
	error = PARSER_FINE;
	if (!cmdstr.size()) return error;
	int argc = -1;
	std::vector<std::string> argv;
	if (cmdstr[0] != ' ')
	{
		argc++;
		argv.push_back("");
	}
	for (std::string::const_iterator c = cmdstr.begin(); c != cmdstr.end(); c++)
	{
		switch (*c)
		{
		case ' ':
			for (; c + 1 != cmdstr.end() && *(c + 1) == ' '; c++);
			if (c + 1 != cmdstr.end())
			{
				argc++;
				argv.push_back("");
			}
			break;
		case '\\':
			if (c + 1 != cmdstr.end()) argv[argc] += *++c;
			break;
		case '"':
			// read until next unescaped quote or end of string
			while (++c != cmdstr.end() && *c != '"')
			{
				if (*c == '\\' && c + 1 != cmdstr.end()) argv[argc] += *++c;
				else argv[argc] += *c;
			}
			if (c == cmdstr.end()) return error = PARSER_UNMATCHED_QUOTE;
			break;
		default:
			argv[argc] += *c;
			break;
		}
	}
	for (const Command &cmd : cmdv) if (argv[0] == cmd.name) return cmd.call(argv);
	return error = PARSER_UNKNOWN_COMMAND;
}
