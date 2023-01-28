#ifndef COMMAND_PARSER
#define COMMAND_PARSER
#include <vector>
#include <string>
// command parser

struct Command {
	std::string name;
	int (*call)(const std::vector<std::string> &argv);
};

class CommandHandler
{
	private:
		int error = 0;
		int erroridx = 0;
		std::vector<Command> cmdv;

	public:
		enum ParserError {
			NO_ERROR = 0,
			UNKNOWN_COMMAND,
			UNEXPECTED_END,
			UNEXPECTED_CHAR
		};

		// standard method for turning an argument into an integer or float, with optional 
		// default value
		static int parseint(const std::string &str);
		static int parseint(const std::string &str, const int default_value);
		static float parsefloat(const std::string &str);
		static float parsefloat(const std::string &str, const float default_value);

		CommandHandler(const std::vector<Command> cmdv);

		const char* geterror();

		// appends argument list from command string to argv and returns an error code (0 = 
		// success)
		int parse(const std::string &cmdstr, std::vector<std::string> &argv);

		// runs a command. returns its error code or -1 on failure. call geterror() for 
		// more info
		int run(const std::string &cmdstr);
};

#endif
