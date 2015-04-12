#include <stdio.h>
#include "y.tab.h"
#include "turtle.h"

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

main(){
    // set environment variables
    // set main settings
	shellinit();

    while(1){
        // shell loop
        printf(TURTLE_PROMPT);
        yyparse();
        execute_cmd();
    }
    // Exit gracefully
    return 0;
}