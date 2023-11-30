#include "classes.hpp"
int main(int argc, char const *argv[])
{     
    int i,size=250,maxbuf=50;
    char a[50];
    char* inputfile=NULL,* configfile=NULL;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            inputfile = new char[strlen(argv[i + 1]) + 1];
            strcpy(inputfile, argv[i + 1]);
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            configfile = new char[strlen(argv[i + 1]) + 1];
            strcpy(configfile, argv[i + 1]);
        }
    }
    if(configfile!=NULL){
        ifstream Myconfig(configfile);
        while (Myconfig)
        {
        Myconfig>>a;
        if(strcmp(a,"SIZE")==0){
        Myconfig>>a;
        Myconfig>>a;
        size=atoi(a);
        }
        else if (strcmp(a,"MAXBUF")==0){
        Myconfig>>a;
        Myconfig>>a;
        maxbuf=atoi(a);
        }
        }
        Myconfig.close();
    }
    else if(inputfile!=NULL){
        // char* trash=new char[100];
        char c;
        ifstream Myinput(inputfile);
        int lines=0;
        while (Myinput){
            Myinput.get(c);
            if(c=='\n')
            lines++;
        }
        size=1.5*lines;
        Myinput.close();
    }
    Hash MyHash=new array(size);
    if(inputfile!=NULL){
        ifstream Myinput(inputfile);
        Student* a;
        char* studentid=new char[maxbuf],*lastname=new char[maxbuf],*firstname = new char[maxbuf];
        int zip,year;float gpa;
       while (Myinput){
            Myinput>>studentid;
            Myinput>>lastname;
            Myinput>>firstname;
            Myinput>>zip;
            Myinput>>year;
            Myinput>>gpa;
            a=new Student(studentid,lastname,firstname,zip,year,gpa);
            if((MyHash->HTInsert(a))==1)
            MyHash->years=MyHash->years->sortedInsert(a);
        }
        Myinput.close();
        delete[] studentid;
        delete[] lastname;
        delete[] firstname;
    }
    MyHash->prompt(maxbuf);
    delete MyHash;
    if(configfile!=NULL) delete[] configfile;
    if(inputfile!=NULL) delete[] inputfile;
    // // cin>>a;
    // cout<<a<<endl;
    // cin>>a;
    // cout<<a<<endl;
    // return 0;
}
