#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  	int retval=0;

  	printf("I am process %d and I will execute an 'ls -l .; \n", \
			getpid());
	char** k=new char*[12];
	k={"./monitorServer","-p","9005","-t","9004","-p","9003","-p","9002","9001","9001",NULL};
	char* a="-a";
  	// retval=execl("/bin/ls", "ls", "-l", ".", NULL); 
  	retval=execvp("./monitorServer",k);  

  	if (retval==-1)
		perror("execl");
}
