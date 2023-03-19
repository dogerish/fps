#ifndef COMMAND_PARSER
#define COMMAND_PARSER
#include <vector>
#include <string>
// command parser

struct Command {
	std::string name;
	unsigned int (*call)(const std::vector<std::string> &argv, std::string &result);
};

class CommandHandler
{
	private:
		int error = 0;
		int erroridx = 0;
		std::vector<Command> cmdv;
		std::string resultvoid;

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

		// returns a string describing the last error
		const char* geterror();

		// returns true if the given character separates arguments
		bool isargsep(char c);

		// escapes a string so it will be interpreted as plain text in a command string
		std::string escapestr(std::string str);

		// calls a command using the argv, assuming that there is at least one argument. 
		// returns its error code, or -1 on failure. call geterror() for more info
		int call(std::vector<std::string> argv);
		int call(std::vector<std::string> argv, std::string &result);

		// moves c to matching (unescaped) closing symbol for given pair in from, and 
		// appends the string between them to the to string. c is assumed to be at the 
		// opening symbol to match. if none is found, c will be str.end().
		void copyinside(
			const std::string &from,
			std::string &to,
			std::string::const_iterator &c,
			const char open, const char close
		);

		// appends argument list from command string to argv and returns an error code (0 = 
		// success)
		int parse(const std::string &cmdstr, std::vector<std::vector<std::string>> &argvv);

		// runs a command string, potentially including multiple commands. If all succeed, 
		// zero is returned. Returns -1 on failure, or the first non-zero return from 
		// call(). result is continuously updated, and thus holds the value of the last 
		// successful or unsuccessful command.
		int run(const std::string &cmdstr);
		int run(const std::string &cmdstr, std::string &result);
};

#endif
