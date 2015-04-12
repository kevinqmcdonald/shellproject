#include <stdio.h>
#include "y.tab.h"
#include "turtle.h"

alias aliastab[MAXALIAS];
COMMAND comtab[MAXCMDS];
int currcmd;
int aliasno;
char *aliasroot;
int ignoreEOF;
int yylex();
int yylineno;
char *yytext;
char **environ;

void shellinit(void) {
	currcmd = 0;
	aliasno = 0;
	aliasroot = NULL;
}

int set_env(char *name, char *value) {
	return setenv(name, value, 1);
}

int unset_env(char *name) {
	return unsetenv(name);
}

void print_env(void) {
	int i = 0;
	while(environ[i]) {
		printf("%s\n", environ[i]);
		i++;
	}
}

void printalias(void) {
	int i = 0;
	for(i; i<aliasno; i++) {
		printf("%s <=> %s\n", aliastab[i].alname, aliastab[i].alstring);
	}
}

int isalias(char *name) {
	int ret = -1;
	int i = 0;
	for(i; i<aliasno; i++) {
		if(strcmp(name, aliastab[i].alname) == 0) {
			ret = i;
			break;
		}
	}
	return ret;
}

int setalias(char *name, char *value) {
	alias newalias;
	newalias.alname = name;
	newalias.alstring = value;

	bool exists = FALSE;
	int i = 0;
	for(i; i<aliasno; i++) {
		if(strcmp(name, aliastab[i].alname) == 0) {
			exists = TRUE;
			break;
		}
	}

	if(exists) {
		aliastab[i] = newalias;
		return;
	}
	else {
		if(aliasno < MAXALIAS) {
			aliastab[i] = newalias;
			aliasno++;
			return OK;
		}
	}
	return SYSERR;
}

int removealias(char *name) {
	int index = -1;
	int i = 0;
	for(i; i<aliasno; i++) {
		if(strcmp(name, aliastab[i].alname) == 0) {
			for(index = i; index < aliasno; index++) {
				aliastab[index] = aliastab[index+1];
			}
			aliasno--;
			return OK;
		}
	}
	return SYSERR;
}

void execute_cmd(void) {
	currcmd = currcmd % MAXCMDS;
	int curr = currcmd;

	if(comtab[curr].external == 0) {
		// Built-in Command
		aliasroot = NULL;

		switch(comtab[curr].code) {
			case CHD : {
				char *home = getenv("HOME");
				chdir(home);
				break;
			}
			case CDX : {
				if(chdir(comtab[curr].atptr[0]) == -1)
					printf("Invalid path or directory: %s\n", comtab[curr].atptr[0]);
				break;
			}
			case SETENV : {
				set_env(comtab[curr].atptr[0], comtab[curr].atptr[1]);
				break;
			}
			case UNSETENV : {
				unset_env(comtab[curr].atptr[0]);
				break;
			}
			case PRINTENV : {
				print_env();
				break;
			}
			case SETALIAS : {
				setalias(comtab[curr].atptr[0], comtab[curr].atptr[1]);
				break;
			}
			case UNALIAS : {
				removealias(comtab[curr].atptr[0]);
				break;
			}
			case PRINTALIAS : {
				printalias();
				break;
			}
			case PWD : {
				printf("%s\n", getPWD);
				break;
			}
		}
	}
	else {
		// Handle aliasing
		int acurr = isalias(comtab[curr].comname);

		if(acurr != -1) {
			comtab[curr].external = 0;
			if(aliasroot == NULL) {
				aliasroot = aliastab[acurr].alname;
			}
			// Check for infinite aliasing
			if(strcmp(aliasroot, aliastab[acurr].alstring)) {
				printf("ERR: Infinite aliasing detected. Exiting...\n");
				return;
			}
			else {
				ignoreEOF = 1;
				parse_string(aliastab[acurr].alstring);
				execute_cmd();
			}
		}
		else {
			// External Command
			aliasroot = NULL;
			pid_t child = fork();
			int stat;
			int success = -1;

			while(waitpid(child, &stat, 0) == -1) {
				if(errno != EINTR) {
					stat = -1;
					break;
				}
			}

			if(child < 0)
				exit(1);
			else if(child == 0) {
				// Prepare for execv call
				char tmp[256];
				char *paths = strcpy(tmp, getenv("PATH"));
				char *tok = strtok(paths, ":");
				char *cmp = "./";

				while(tok) {
					char place[255];
					if(comtab[curr].comname[0] == cmp[0] || comtab[curr].comname[0] == cmp[1]) {
						// If destination is specified 
						strcpy(place, comtab[curr].comname);
					}
					else {
						// If destination is not specified
						strcpy(place, tok);
						strcat(place, "/");
						// Append command name
						strcat(place, comtab[curr].comname);
					}

					char *cmds[comtab[curr].nargs + 2];
					cmds[0] = place;
					cmds[comtab[curr].nargs + 1] = (char *)NULL;

					int i = 0;
					for(i; i<comtab[curr].nargs; i++) {
						cmds[i+1] = comtab[curr].atptr[i];
					}

					if(execv(place, cmds) == -1) {
						tok = strtok(NULL, ":");
						continue;
					}
					else {
						_exit(0);
						success = 1;
						break;
					}
				}

				if(success == -1) {
					printf("ERR: Command not found: %s\n", comtab[curr].comname);
					_exit(1);
				}
			}
		}
	}

	currcmd += 1;
	comtab[currcmd].external = 0;
	ignoreEOF = 0;
}

void shellSplash(){
    // Intro Splash
    printf("                _,.---.---.---.--.._                \n");
    printf("            _.-' `--.`---.`---'-. _,`--.._          \n");
    printf("           /`--._ .'.     `.     `,`-.`-._\\        \n");
    printf("          ||   \\  `.`---.__`__..-`. ,'`-._/        \n");
    printf("     _  ,`\\ `-._\\   \\    `.    `_.-`-._,``-.     \n");
    printf("  ,`   `-_ \\/ `-.`--.\\    _\\_.-'\\__.-`-.`-._`.  \n");
    printf(" (_.o> ,--. `._/'--.-`,--`  \\_.-'       \\`-._ \\  \n");
    printf("  `---'    `._ `---._/__,----`           `-. `-\\   \n");
    printf("            /_, ,  _..-'                    `-._\\  \n");
    printf("            \\_, \\/ ._(     Welcome to the         \n");
    printf("             \\_, \\/ ._\\     Turtle Shell         \n");
    printf("              `._,\\/ ._\\       Made by Sean Thomas\n");
    printf("                `._// ./`-._     and Kevin McDonald \n");
    printf("                  `-._-_-_.-'                       \n");
}

main(){
    // set environment variables
    // set main settings
	shellinit();
	shellSplash();

    while(1){
        // shell loop
        TURTLE_PROMPT;
        yyparse();
        execute_cmd();
    }
    // Exit gracefully
    return 0;
}
