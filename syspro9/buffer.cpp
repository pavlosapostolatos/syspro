#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <poll.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/select.h>
#include <sys/types.h> /* For sockets */
#include <sys/socket.h> /* For sockets */
#include <netinet/in.h> /* For Internet sockets */
#include <netdb.h> 
#include <sys/time.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

int main(){
    void* buf=malloc(12);
    char* a=(char*)malloc(6);
    memcpy(a,"hello",6);
    char *b="world";
    memcpy(buf,a,6);
    memcpy((uint8_t*)buf+6,b,6);
    cout<<(char*)buf<<endl;
    string c=(char*)buf;
    cout<<c<<endl;
    cout<<c.size()<<c.length()<<endl;
    string d=(char*)(buf+6);
    cout<<d<<endl;
    // memcpy(buf,&a,sizeof(char**));
    // memcpy((uint8_t*)buf+sizeof(char**),&b,sizeof(char**));
    // char *c=*(char**)buf;
    // char *d=*(char**)(buf+sizeof(char**));
    // cout<<c<<d<<endl;
}