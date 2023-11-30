#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string.h>

using namespace std;
class Student
{

public:
    char *studentid;
    char *lastname;
    char *firstname;
    int zip;
    int year;
    float gpa;
    Student(char *, char *, char *, int, int year, float);
    ~Student();
    char *getid(){return studentid;}
    void printstudent();
};



typedef class TableEntry *list;
class TableEntry
{
private:
    /* data */
public:
    Student* student;
    list next;
    TableEntry(Student* ,list);
    ~TableEntry();
    void DeleteStudent(){delete this->student;}
    list deletenode(char* ,Student** ,int& );
    list Deletefirst(Student** );
    Student*  searchlist(char* );
    list Insertfirst(Student*);
};


typedef class InvertedIndex *II;
class InvertedIndex
{
private:

public:
    int year;
    list students;
    II next;
    int scount;
    InvertedIndex(int,Student*,II);
    ~InvertedIndex();
    II sortedInsert(Student*); 
    list getNthyear(int);
    void count();
    void minimumyear(int);
    void averageyear(int );
    void topnumyear(int,int);
    void numberyear(int);
    II getNthII(int);
};





typedef class array* Hash;
class array
{
private:
    /* data */
public:
    int size;
    list* table;
    II years;
    array(int);
    ~array();
    int HTSize();
    Student* HTGet(char*);
    bool HTInsert(Student* );
    void prompt(int);
    void postcode(int);
    void showtable();
};


int hashcode(char *, int );
void quickSort(Student** arr, int low, int high)  ;
int partition (Student** arr, int low, int high)  ;
