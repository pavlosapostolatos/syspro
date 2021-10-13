#include "includes.hpp"
void mklvl(int l,int n){//makes process tree of l levels with n childs for each node
	int status;
	printf("process ID: %ld parent ID:%ld\n",(long)getpid(), (long)getppid());
	if (l==0) return;
	for (int i=1;i<=n; i++){
    	if ( (fork()) == 0 ){
        	mklvl(l-1,n);
        	break;
        }
        //else while ((wait(&status)) > 0);
    }
}

void swap(string *xp, string *yp)
{
    string temp = *xp;
    *xp = *yp;
    *yp = temp;
}
void bubbleSort(string arr[], int n)
{
    int i, j;
    for (i = 0; i < n-2; i++)    
    for (j = 0; j < n-i-1; j++)
        if (arr[j] > arr[j+1])
            swap(&arr[j], &arr[j+1]);
}

int search_monitor_by_country(int numMonitors,string** monitor_countries, int* monitor_country_count,string country){
for (int i=0;i<numMonitors;i++){
    for (int j=0;j<monitor_country_count[i];j++){
        if(monitor_countries[i][j].compare(country)==0) return i;
}
}
return -1;
}

TravelRequests::TravelRequests(string vn,string ct, struct date dt, TravelRequests* n,bool acc)
{
    virusname=vn;
    countryTo=ct;
    travel_date=dt;
    next=n;
    accepted=acc;
}
TravelRequests *TravelRequests::insert(TravelRequests * added){
    if (this!=NULL) 
    added->next=this;
    return added;
}

void TravelRequests::TravelStats(std::string virusn, date d1, date d2, std::string *country){
    int acc=0,rej=0;
    if(this==NULL){
        cout<<"make some requests first\n";
        return;
    }
    for (TravelRequests* temp=this; temp!=NULL;temp=temp->next){
        if(temp->virusname.compare(virusn)==0 && (country==NULL || (country!=NULL && country->compare(temp->countryTo)==0) ) 
        && compare_dates(temp->travel_date,d1  )>=0 && compare_dates(temp->travel_date,d2  )<=0  ){
            if(temp->accepted==true) acc++;//if the virusname is the same, the country name to if the user gave one and date is between the 2 provided this request node is relevant to us
            if(temp->accepted==false) rej++;
        }
    }
    printf("TOTAL REQUESTS %d\n ACCEPTED %d\nREJECTED %d\n\n",acc+rej,acc,rej);
}
TravelRequests::~TravelRequests()
{
    if(this->next!=NULL) delete this->next;
}