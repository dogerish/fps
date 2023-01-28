#include "command.h"
#include <vector>
#include <string>


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

int CommandHandler::parse(const std::string &cmdstr, std::vector<std::string> &argv)
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
			if (c == cmdstr.end()) return error = UNEXPECTED_END;
			break;
		case '[':
			{
				// read until matching bracket
				int nest = 1;
				while (++c != cmdstr.end())
				{
					switch (*c)
					{
					case '[': nest++; break;
					case ']': nest--; break;
					case '\\':
						  if (c + 1 != cmdstr.end()) c++;
						  break;
					}
					// end loop on closing bracket without adding it
					if (!nest) break;
					// add character
					argv[argc] += *c;
				}
				if (c == cmdstr.end()) return error = UNEXPECTED_END;
			}
			break;
		case ']':
			return error = UNEXPECTED_CHAR;
		default:
			// append character to current argument
			argv[argc] += *c;
			break;
		}
	}
	return error = NO_ERROR;
}

int CommandHandler::run(const std::string &cmdstr)
{
	if (!cmdstr.size()) return 0;
	// get argv and check for error
	std::vector<std::string> argv;
	if (error = parse(cmdstr, argv)) return -1;
	// find and call command
	for (const Command &cmd : cmdv) if (argv[0] == cmd.name) return cmd.call(argv);
	error = UNKNOWN_COMMAND;
	return -1;
}
