#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include  <dirent.h>
#include  <stdbool.h>
#include <assert.h>
#define BUFFSIZE 1024
bool isDirectoryExists(const char *path);
void copydir(char *oldDir, char *newDir);
void copyFiles(char *oldDir, char *newDir);
void ExamineAndCopy(char *oldDir, char *newDir,bool change);
bool same(char *oldDir, char *newDir,bool change);
bool isExists(const char *path);
int removedirectoryrecursively(const char *dirname);
bool isSlinkExists(const char *path);
void makeSlink(char *oldlink, char *newlink);