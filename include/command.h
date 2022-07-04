#ifndef COMMAND_PARSER
#define COMMAND_PARSER
#include <vector>
#include <string>
// command parser

struct Command {
	std::string name;
	int (*call)(std::vector<std::string> &argv);
};

enum ParserError {
	PARSER_FINE = 0,
	PARSER_UNKNOWN_COMMAND,
	PARSER_UNMATCHED_QUOTE,
};

const char* parser_errmsg(int error);

// runs a command. most basic parsing - just splits up arguments and calls the command
// returns the return of the command, or error if there is one
// puts any error into error if there is one. if not, error will be set to 0. call parser_errmsg
int runcommand(const std::vector<Command> &cmdv, const std::string &cmdstr, int &error);

#endif
