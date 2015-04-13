#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include "y.tab.h"
#include "turtle.h"

alias aliastab[MAXALIAS];
COMMAND comtab[MAXCMDS];
int currcmd;
int aliasno;
char *aliasroot;
int aliasDepth;
int ignoreEOF;
int yylex();
int yylineno;
char *yytext;
char **environ;

void shellinit(void) {
	currcmd = 0;
	aliasno = 0;
	aliasroot = NULL;
	aliasDepth = 0;
	ignoreEOF = 0;
	ofileredir = 0;
	ifileredir = 0;
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
  	if(ofileredir) {
  		fputs(environ[i++], comtab[curr].outfd);
  		fputs("\n", comtab[curr].outfd);
  	}
  	else
  		printf("%s\n", environ[i++]);
  }
}

void printalias(void) {
	int i = 0;
	for(i; i<aliasno; i++) {
		if(ofileredir) {
			fprintf(comtab[curr].outfd, "%s <=> %s\n", aliastab[i].alname, aliastab[i].alstring);
		}
		else
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


char *replace(char *input, char *target, char * replacement) {
    static char temp[4096];
    char *point;
    if( !(point = strstr(input, target) ) ) {
        return input; 
    }
    
    strncpy(temp, input, point-input);
    temp[point-input] = '\0';
    
    sprintf(temp+(point-input), "%s%s", replacement, point+strlen(target));
    
    return temp;
}

char * removeQuotes( char* in ) {
    char * out = in;
    if( out[0] == '"') {
        out++;
        out[strlen(out) - 1] = 0;
    }
    return out;
}

char * expandEnv( char* in ) {
    char * out = in;
    int i;
    int possibleEnv = 0;
    int envStart;
    for (i = 0; i < strlen(out); i++) {
        if( out[i] == '$' ) {
            envStart = i;
        }
        if( out[i] == '{' && i == envStart+1 ) {
            possibleEnv = 1;
        }
        if( out[i] == '}' && possibleEnv ) {
            char temp[5000];
            char * envVar;

            memcpy( temp, &out[envStart], i-envStart+1 );
            temp[i-envStart+1] = '\0';
            copystring( envVar, temp );
            envVar = envVar + 2;
            envVar[i-envStart-2] = '\0';
            
            out = replace( out, temp, getenv(envVar) );
        }
    
    }
    return out;
}

char * expandTilde( char * in ) {
    char * out = in;
    int i;
    if( strlen(out) == 1 ) {
        return replace( out, "~", getHOME );
    }
    if( out[0] == '~' && out[1] == '/' ) {
        return replace( out, "~", getHOME );
    }
    if( out[0] == '~' && strlen(out) > 1 ) {
        out++;
        if( getpwnam(out) != NULL ) {
        	struct passwd *pass = getpwnam(out);
            char temp[5000];
            strcpy(temp, "/home/");
            return strcat(temp, pass->pw_name);
        } else {
            turtErr("error with getpwnam()\n");
        }
    }
    return out;
}
char * cleanInput( char * in ) {
	return expandTilde( expandEnv( removeQuotes( in ) ) );
}
void execute_cmd(void) {
	currcmd = currcmd % MAXCMDS;
	int curr = currcmd;
	FILE *f;

	if(comtab[curr].external == 0) {
		// Built-in Command
		aliasroot = NULL;
                aliasDepth = 0;

		switch(comtab[curr].code) {
			case CHD : {
	            if( chdir(getHOME) ) {
	                printf("ERROR at line %d\n", __LINE__);
	                break;
	            }
	            setenv("PWD", getHOME, 1);
				break;
			}
			case CDX : {
				char* dest = cleanInput(comtab[curr].atptr[0]);
				if( chdir(dest) == -1 ) {
	                printf("ERROR: \n%s is not a directory\n", dest);
            	}
            	char pwd[5000];
            	getcwd( pwd, sizeof(pwd) );
            	setenv("PWD", pwd, 1);
				break;
			}
			case SETENV : {
				char* name = cleanInput(comtab[curr].atptr[0]);
	            char* word = cleanInput(comtab[curr].atptr[1]);
	            if( setenv( name, word, 1 ) == -1 ) {
	                printf("setenv failed, could not set %s to %s\n", name, word );
	            }
				break;
			}
			case UNSETENV : {
				char* name = cleanInput(comtab[curr].atptr[0]);
	            if( getenv(name) ){
	                unsetenv(name);
	            } else {
	                printf("unsetenv failed, could not find %s\n", name);
	            }
				break;
			}
			case PRINTENV : {
				if(ofileredir) {
					if(comtab[curr].append) {
						f = fopen(comtab[curr].outfd, "a");
					}
					else {
						f = fopen(comtab[curr].outfd, "w");
					}

					if(f == NULL)
						return SYSERR;
				}
				else
					print_env();

				if(ofileredir)
					fclose(f);
				break;
			}
			case SETALIAS : {
				char* name = cleanInput(comtab[curr].atptr[0]);
	            char* word = cleanInput(comtab[curr].atptr[1]);
				setalias(name, word);
				break;
			}
			case UNALIAS : {
				char* name = cleanInput(comtab[curr].atptr[0]);
				removealias(name);
				break;
			}
			case PRINTALIAS : {
				if(ofileredir) {
					if(comtab[curr].append) {
						f = fopen(comtab[curr].outfd, "a");
					}
					else {
						f = fopen(comtab[curr].outfd, "w");
					}
				}
				else
					printalias();

				if(ofileredir)
					fclose(f);
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
			if( aliasDepth > 30 ) {
				printf("ERR: Infinite aliasing detected. Exiting...\n");
				return;
			}
			else {
				ignoreEOF = 1;
				parse_string(aliastab[acurr].alstring);
				aliasDepth++;
				execute_cmd();
			}
		}
		else {
			// External Command
			aliasroot = NULL;
			aliasDepth = 0;
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

	// Disable interrupt signals, i.e., keyboard interrupts
	signal(SIGINT, SIG_IGN);

    while(1){
        // shell loop
        TURTLE_PROMPT;
        yyparse();
        execute_cmd();
    }
    // Exit gracefully
    return 0;
}
