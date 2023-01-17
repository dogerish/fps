#ifndef COMMAND_PARSER
#define COMMAND_PARSER
#include <vector>
#include <string>
// command parser

struct Command {
	std::string name;
	int (*call)(const std::vector<std::string> &argv);
};

// error codes 0 to 64 reserved for the parser
enum ParserError {
	PARSER_FINE = 0,
	PARSER_UNKNOWN_COMMAND,
	PARSER_UNMATCHED_QUOTE,
};

const char* parser_errmsg(int error);

// appends argument list from command string to argv and returns error code (0 = success)
int parsecommand(const std::string &cmdstr, std::vector<std::string> &argv);
// runs a command. returns an error code or the return of the command. call parser_errmsg to get a 
// message from the code. Error codes 0-64 are reserved for the parser, but 0 universally means 
// success.
int runcommand(const std::vector<Command> &cmdv, const std::string &cmdstr);

// standard method for turning an argument into an integer
int parseint(const std::string &str);
// above, but with error handling which spits back the default value in case of out-of-range or 
// invalid input
int parseint(const std::string &str, const int default_value);

float parsefloat(const std::string &str);
float parsefloat(const std::string &str, const float default_value);

#endif
