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
#include "includes1.hpp"
using namespace std;
#define MGS_BUF 512
#define READ 0
#define WRITE 1

class TravelRequests
{
private:
    /* data */
public:
    string virusname;
    string countryTo;
    struct date travel_date;
    TravelRequests* next;
    bool accepted;
    TravelRequests(string,string, struct date, TravelRequests*,bool);
    ~TravelRequests();
    void TravelStats(string virusn,struct date d1,struct date d2,string* country);
    TravelRequests* insert(TravelRequests*);
};



void mklvl(int l,int n);
void bubbleSort(string arr[], int n);
void swap(string *xp, string *yp);
int search_monitor_by_country(int numMonitors,string** monitor_countries, int* monitor_country_count,string country);