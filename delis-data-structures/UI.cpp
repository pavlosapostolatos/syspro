#include "classes.hpp"
void array::prompt(int maxbuf)
{
    char buffer[8];
    Student* a;
    //list L;
    int zip,year;float gpa;
    char* studentid=new char[maxbuf],*lastname=new char[maxbuf],*firstname = new char[maxbuf];
    while (1)
    {
        cout<<"give command\n";
        cin >> buffer;
        if (strcmp(buffer, "i")==0 || strcmp(buffer, "insert")==0)
        {
            cin>>studentid;
            cin>>lastname;
            cin>>firstname;
            cin>>zip;
            cin>>year;
            cin>>gpa;
            a=new Student(studentid,lastname,firstname,zip,year,gpa);
            if((this->HTInsert(a))==1)
            this->years=this->years->sortedInsert(a);
        }
        else if (strcmp(buffer, "l")==0 || strcmp(buffer, "look-up")==0)
        {
            cin>>studentid;
            a=this->HTGet(studentid);
            if(a==NULL) cout<<"student "<<studentid<<" does not exist"<<endl;
            else {a->printstudent();cout<<endl;}
        }
        else if (strcmp(buffer, "d")==0 || strcmp(buffer, "delete")==0)
        {
            cin>>studentid;
            int h=hashcode(studentid,this->size),year=-1;
            this->table[h]=this->table[h]->deletenode(studentid, &a,year);
            if(year==-1) cout<<"Student "<<studentid<<"does not exist\n";
            else{
                II temp=this->years->getNthII(year);
                year=-1;
                temp->students=temp->students->deletenode(studentid,  &a,year);
                if(year!=-1){ 
                    cout<<"Student "<<studentid<<"deleted\n";
                    temp->scount--;
                    delete a;
            }
            }
        }
        else if (strcmp(buffer, "n")==0 || strcmp(buffer, "number")==0)
        {
            cin>>year;
            this->years->numberyear(year);
        }
        else if (strcmp(buffer, "t")==0 || strcmp(buffer, "top")==0)
        {
            int num;
            cin>>num;
            cin>>year;
            this->years->topnumyear(num,year);
        }
        else if (strcmp(buffer, "a") ==0|| strcmp(buffer, "average")==0)
        {
            cin>>year;
            this->years->averageyear(year);
        }
        else if (strcmp(buffer, "m")==0 || strcmp(buffer, "minimum")==0)
        {
            cin>>year;
            this->years->minimumyear(year);
        }
        else if (strcmp(buffer, "c") ==0|| strcmp(buffer, "count")==0)
        {
            this->years->count();
        }
        else if (strcmp(buffer, "p")==0 || strcmp(buffer, "postal")==0)
        {
            int rank;
            cin>>rank;
            this->postcode(rank);

        }
        else if (strcmp(buffer, "s") ==0|| strcmp(buffer, "show")==0)
        {
        this->showtable();
        }
        else if (strcmp(buffer, "e") ==0|| strcmp(buffer, "exit")==0)
        {
            delete[] studentid;
            delete[] lastname;
            delete[] firstname;
            return;
        }
        else
            cout << "wrong input" << endl;
    }
}
