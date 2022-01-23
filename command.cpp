#include "command.h"
#include <cstring>

char* argcpy(const char* argstart, int len)
{
	char* arg = new char[len + 1];
	strncpy(arg, argstart, len);
	arg[len] = '\0';
	return arg;
}

int runcommand(int cmdc, const Command cmdv[], const char* cmdstr)
{
	int argc = 0;
	const char** argv = new const char*[MAXARGS];

	const char* argstart = NULL;
	const char* c = cmdstr;
	for (; *c && argc < MAXARGS; c++)
	{
		switch (*c)
		{
		case ' ':
			if (!argstart) for (; *(c + 1) == ' '; c++);
			// found end of argument - copy it
			else argv[argc++] = argcpy(argstart, c - argstart);
			// set start of an argument if the next character is not \0 and not a space
			argstart = (*(c + 1) && *(c + 1) != ' ') ? c + 1 : NULL;
			break;
		default:
			if (!argstart) argstart = c;
			break;
		}
	}
	// last argument ended with end of string - copy it
	if (argstart && argc < MAXARGS) argv[argc++] = argcpy(argstart, c - argstart);
	int r = -1;
	for (int i = 0; i < cmdc; i++)
	{
		if (strcmp(argv[0], cmdv[i].name) == 0)
		{
			r = cmdv[i].call(argc, argv);
			break;
		}
	}
	for (int i = 0; i < argc; i++) delete[] argv[i];
	delete[] argv;
	return r;
}
