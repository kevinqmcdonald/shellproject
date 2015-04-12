#include <stdio.h>
#include "y.tab.h"
#include "turtle.h"

alias aliastab[MAXALIAS];
COMMAND comtab[MAXCMDS];
int currcmd;
int aliasno;
char *aliasroot;
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

bool isalias(char *name) {
	bool ret = FALSE;
	int i = 0;
	for(i; i<aliasno; i++) {
		if(strcmp(name, aliastab[i].alname) == 0) {
			ret = TRUE;
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
