#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cstring>
#include <sstream>


#define K 16
#define P 0.5

using namespace std;



struct date{
    int year;
    int month;
    short day;
};

class citizenData
{
private:
    /* data */
public:
    int citizenID;
    string firstName;
    string lastName;
    string country;
    short age;
    citizenData(/* args */);
    ~citizenData();
};



class vaccinedata
{
private:
    /* data */
public:
    string virusName;
    bool vaccinated;
    struct date *dateVaccinated;
    vaccinedata(/* args */);
    ~vaccinedata();
};





class citizen
{
public:
    citizenData* person;
    vaccinedata* vaccine;
    citizen();
    citizen(int , string , string , string, short, string,bool,string);
    ~citizen();
    void printcitizen();
};



typedef class SkipList *SkipListNode;
class SkipList
{
private:
    /* data */
public:
    citizen* person;
    SkipListNode next;
    SkipListNode down;
    SkipListNode prev;
    SkipList(citizen* ,SkipListNode,SkipListNode,SkipListNode);
    ~SkipList();
    int count_levels();
    void destroy_layer(bool);
    void deletenode();
    bool DeleteFromSkip(int  );
    SkipListNode getall();
    SkipListNode InsertToSkip(citizen *);
    citizen*  searchSkipList(int  );
    void InsertAfterAbove(citizen*,SkipListNode);
    SkipListNode randomAscend(SkipListNode*,int,citizen* );
};

class BloomFilter
{
private:


public:
    int *array;
    int M;
    int size;
    BloomFilter(int);
    BloomFilter(int*);
    ~BloomFilter();
    void BloomInsert(int);
    bool BloomSearch(int);
    void print_Bloom();
};

class Viruses
{
private:
    /* data */
public:
    string name;
    BloomFilter* bloom;
    SkipListNode VaccinatedList;
    SkipListNode UnVaccinatedList;
    Viruses* next;
    Viruses(string);
    Viruses(string,int*,SkipListNode,SkipListNode,Viruses*);
    ~Viruses();
    Viruses* addVirus(string,int *);
    Viruses* insert_in_virus(citizen*);
    bool delete_from_record(citizen *);
    int vaccineStatusBloom(int,string);
    int vaccineStatus(string&,int,string,struct date* , struct date* ,bool);
    string vaccineStatus(int);
    void list_nonVaccinated(string);
};


class Countries
{
private:
    /* data */
public:
    string name;
    SkipListNode DataList;
    Countries* next;
    Countries(string);
    ~Countries();
    Countries* insert_in_data(citizen*);
    citizen* search_in_data(citizen* person);
    bool raw_search_in_data(int );
    //void delete_data(citizen *);
};

class Record
{
private:
    /* data */
public:
    Viruses* viruses;
    Countries* people;
    Record();
    ~Record();
    Record* insert_in_record(citizen*);
    bool delete_from_record(citizen *);
    void vaccineStatusBloom(int,string);
    void vaccineStatus(int,string);
    void vaccineStatus(int);
    void vaccinateNow(citizen*);
    void list_nonVaccinated(string);
    void populationStatus(string  , struct date , struct date  ,string);
    void populationStatus(string  , struct date , struct date);
    void popStatusByAge(string  , struct date , struct date  ,string);
    void popStatusByAge(string  , struct date , struct date);
};

bool myrandom();
SkipListNode initializeSkipList();
string print_date(struct date *);
int compare_dates(struct date , struct date );
SkipListNode initializeSkipList();
citizen *read_citizen(std::istream &,string, bool vac = true);
void read_date(std::istream &stream, struct date *d, bool space );
int count_words(std::string line);