/* See LICENSE file for copyright and license details. */
//MPU application menu codebase
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 32
#define MAX_LINE_LENGTH 256

//MPU application menu codebase
//2020-2023 evv42

char** filetotable(char* file, int* nbuttons){
	*nbuttons = -1;
	//prepare file
	FILE* fd = fopen(file,"r");
	if(fd == NULL){
		perror("fopen");
		return NULL;
	}
	//prepare table
	char** table=malloc(MAX_LINES*sizeof(char**));
	if(table == NULL){perror("malloc");exit(12);}
	for(int i=0; i<MAX_LINES; i++){
		table[i]=malloc(MAX_LINE_LENGTH);
		if(table[i] == NULL){perror("malloc");exit(13);}
		*table[i]=0;
	}

	char* r;
	char* line = malloc(MAX_LINE_LENGTH);
	if(line == NULL){perror("malloc");exit(14);}

	while( (r = fgets(line, MAX_LINE_LENGTH, fd)) != NULL ){
			*nbuttons +=1;
			//remove a newline, if any
			char* lf = strchr(r,'\n');
			if(lf != NULL)*lf=0;
			memcpy(table[*nbuttons], r, MAX_LINE_LENGTH);
	}
	free(line);
	return table;
}

void runcommand(char* tcmd){
	if(*tcmd == 0)return;
	/*convert char* to char** for exec*/
	/*count argv for malloc*/
	int ac=1;
	char* ptr = tcmd;
	while (*ptr != 0) {
		if(*ptr == ' ')ac++;
		ptr++;
	}
	char** argv = (char**)malloc(ac+10);
	/*feed char** */
	ptr = tcmd;
	int i=0;
	int c=0;
	argv[i]=malloc(MAX_LINE_LENGTH);
	while (*ptr != 0) {
		if(*ptr == ' '){
			argv[i][c]=0;
			i++;
			c=0;
			argv[i]=malloc(MAX_LINE_LENGTH);
		}else{
			argv[i][c]=*ptr;
			c++;
		}
		ptr++;
	}
	argv[i][c]=0;
	argv[i+1] = NULL;
	pid_t pid;
	if ((pid = fork()) < 0){
          perror("fork");
          exit(1);
     }
     else if (pid == 0){
          if (execvp(*argv, argv) < 0){
		perror("execvp");
          }
          exit(1);
     }

	free(argv);
}
