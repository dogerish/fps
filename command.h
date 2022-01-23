#ifndef COMMAND_PARSER
#define COMMAND_PARSER
// command parser
#define MAXARGS 32

struct Command {
	const char* name;
	int (*call)(int argc, const char* argv[]);
};

// returns a substring copy from argstart to argend, inclusive
char* argcpy(const char* argstart, int len);
// runs a command. most basic parsing - just splits up arguments and calls the command
// returns the return of the command
int runcommand(int cmdc, const Command cmdv[], const char* cmdstr);

#endif
