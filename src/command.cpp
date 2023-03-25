#include "command.h"
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>


int CommandHandler::parseint(const std::string &str)
{ return std::stoi(str); }

int CommandHandler::parseint(const std::string &str, const int default_value)
{
	try { return parseint(str); }
	catch (std::invalid_argument const &e) { return default_value; }
	catch (std::out_of_range     const &e) { return default_value; }
}

float CommandHandler::parsefloat(const std::string &str)
{ return std::stof(str); }

float CommandHandler::parsefloat(const std::string &str, const float default_value)
{
	try { return parsefloat(str); }
	catch (std::invalid_argument const &e) { return default_value; }
	catch (std::out_of_range     const &e) { return default_value; }
}

CommandHandler::CommandHandler(std::vector<Command> cmdv)
{
	this->cmdv = cmdv;
}

const char* CommandHandler::geterror()
{
	switch (error)
	{
	case NO_ERROR:        return "";
	case UNKNOWN_COMMAND: return "Unknown command";
	case UNEXPECTED_END:  return "Unexpected end of line";
	case UNEXPECTED_CHAR: return "Unexpected character";
	default:              return "Unknown error";
	}
}

bool CommandHandler::isargsep(char c)
{
	// whitespace and semi-colons mark the end of an argument
	return c == ';' || isspace(c);
}

std::string CommandHandler::escapestr(std::string str)
{
	std::string out = "[";
	int nest = 1;
	for (std::string::const_iterator c = str.begin(); c != str.end(); c++)
	{
		switch (*c)
		{
		case '[': nest++; break;
		case ']': nest--; break;
		case '\\': out += '\\'; break;
		}
		// prevent exiting nest level of 1
		if (!nest)
		{
			out += '\\';
			nest++;
		}
		out += *c;
	}
	// close the string
	out += "]";
	return out;
}

int CommandHandler::call(std::vector<std::string> argv) { return call(argv, resultvoid); }
int CommandHandler::call(std::vector<std::string> argv, std::string &result)
{
	result = "";
	for (const Command &cmd : cmdv)
		if (argv[0] == cmd.name)
			return cmd.call(argv, result);
	error = UNKNOWN_COMMAND;
	return -1;
}

void CommandHandler::copyinside(
	const std::string &from,
	std::string &to,
	std::string::const_iterator &c,
	const char open, const char close
)
{
	// read until matching bracket
	int nest = 1;
	while (++c != from.end())
	{
		if (*c == open) nest++;
		else if (*c == close) nest--;
		else if (*c == '\\' && c + 1 != from.end()) c++;
		// end loop on closing bracket without adding it
		if (!nest) break;
		to += *c;
	}
}

int CommandHandler::parse(const std::string &cmdstr, std::vector<std::vector<std::string>> &argvv)
{
	std::vector<std::string> argv;
	std::string arg = "";
	// arg started if no leading separater
	bool hasarg = !isargsep(cmdstr[0]);
	// iterate through characters
	for (std::string::const_iterator c = cmdstr.begin(); c != cmdstr.end(); c++)
	{
		if (isspace(*c))
		{
			// seek to end of space sequence
			while (c + 1 != cmdstr.end() && isspace(*(c + 1))) c++;
			// if there's a following argument, initialize it
			if (c + 1 != cmdstr.end())
			{
				if (hasarg) argv.push_back(arg);
				arg = "";
				hasarg = !isargsep(*(c + 1));
			}
			continue;
		}
		switch (*c)
		{
		case '\\':
			// escape next character
			if (c + 1 != cmdstr.end()) arg += *++c;
			break;
		case '"':
			// read until next unescaped quote or end of string
			while (++c != cmdstr.end() && *c != '"')
			{
				if (*c == '\\' && c + 1 != cmdstr.end()) arg += *++c;
				else arg += *c;
			}
			if (c == cmdstr.end()) return error = UNEXPECTED_END;
			break;
		case '[':
			copyinside(cmdstr, arg, c, '[', ']');
			if (c == cmdstr.end()) return error = UNEXPECTED_END;
			break;
		case '(':
			{
				std::string nestedcmdstr = "";
				copyinside(cmdstr, nestedcmdstr, c, '(', ')');
				if (c == cmdstr.end()) return error = UNEXPECTED_END;
				// substitute for the result of running the nested command
				std::string res;
				run(nestedcmdstr, res);
				arg += res;
			}
			break;
		case ']':
		case ')':
			return error = UNEXPECTED_CHAR;
		case ';':
			// add this arg to argv
			if (hasarg) argv.push_back(arg);
			// add this argv to argvv and start a new argv
			if (argv.size() > 0)
				argvv.push_back(argv);
			argv.clear();
			arg = "";
			// if there is a following command and the next isn't whitespace, an arg 
			// starts
			hasarg = (c + 1 != cmdstr.end() && !isargsep(*(c + 1)));
			break;
		default:
			// append character to current argument
			arg += *c;
			break;
		}
	}
	// add last arg and cmd
	if (hasarg) argv.push_back(arg);
	if (argv.size() > 0) argvv.push_back(argv);

	return error = NO_ERROR;
}

int CommandHandler::run(const std::string &cmdstr) { return run(cmdstr, resultvoid); }
int CommandHandler::run(const std::string &cmdstr, std::string &result)
{
	if (!cmdstr.size()) return 0;
	// get argv and check for error
	std::vector<std::vector<std::string>> argvv;
	if ((error = parse(cmdstr, argvv))) return -1;
	// find and call command
	for (const std::vector<std::string> &argv : argvv)
		if (call(argv, result) == -1)
			return -1;
	return 0;
}
