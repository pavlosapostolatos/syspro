#include "includes.hpp"
extern int bloomsize, skipsize;
vaccinedata::vaccinedata(/* args */)
{
}
citizenData::citizenData(/* args */)
{

}

citizenData::~citizenData()
{
}
citizen::~citizen()
{
    if(vaccine!=NULL){//only delete vaccine data becuase this meant to delete the virus instances whose pointers to citizenData work as references so no citizenData deletion here
    // if (vaccine->dateVaccinated != NULL)
    //     delete vaccine->dateVaccinated;
    delete vaccine;
    }
}
vaccinedata::~vaccinedata()
{
    if (dateVaccinated != NULL)
    delete dateVaccinated;
}
citizen::citizen() {}
citizen::citizen(int, std::string, std::string, std::string, short, std::string, bool, std::string) {}

Record::Record(){
    viruses=NULL;
    people=NULL; 
}

Countries::Countries(string name)
{//start list of countries
    this->name = name;
    DataList = initializeSkipList();
    next = NULL;
}

void citizen::printcitizen()
{
    cout << person->citizenID << " " << person->firstName << " " << person->country << " " << person->age << " " << vaccine->virusName;
    print_date(vaccine->dateVaccinated);
}

Viruses::Viruses(string name)
{
    this->name = name;
    bloom = new BloomFilter(bloomsize);
    VaccinatedList = initializeSkipList();
    UnVaccinatedList = initializeSkipList();
    next = NULL;
}
Viruses::Viruses(std::string name,int* blom, SkipListNode l1, SkipListNode l2, Viruses * v2){
    this->name = name;//printf("bgike3\n");
    this->bloom=new BloomFilter(blom);
    VaccinatedList = l1;//printf("bgike3\n");
    UnVaccinatedList = l2;
    next = v2;
}
Viruses::~Viruses()
{
    delete bloom;
    VaccinatedList->destroy_layer(false);
    UnVaccinatedList->destroy_layer(false);
    if(next!=NULL) delete next;
}
Countries::~Countries(){
    DataList->destroy_layer(true);
    if(next!=NULL) delete next;
}

Viruses* Viruses::addVirus(std::string name, int * arr){
    if (this==NULL) return new Viruses(name,arr,NULL,NULL,NULL);
    else{
        Viruses * temp=this;
        while(temp->next!=NULL) temp=temp->next;
        //printf("bgike3\n");
        temp->next= new Viruses(name,arr,NULL,NULL,NULL);
        //temp->next->bloom->print_Bloom();
        //printf("bgike3\n");
        return this;
    }

}
Viruses *Viruses::insert_in_virus(citizen *person)
{
    citizen* vac,*unvac;
    Viruses *temp, *prev;
    if (this == NULL)
        temp = new Viruses(person->vaccine->virusName);//initialise empty virus list
    else
    {

        for (temp = this; temp != NULL; temp = temp->next)
        {//search foir virus' node
            if (temp->name.compare(person->vaccine->virusName) == 0)
                break;
            prev = temp;
        }
        if (temp == NULL)//didnt find it. make new node for it
        { //reached the end
            temp = new Viruses(person->vaccine->virusName);
            prev->next = temp;
        }
    }
    if (temp->name.compare(person->vaccine->virusName) == 0)
        if ( (vac=temp->VaccinatedList->searchSkipList(person->person->citizenID)) !=NULL || temp->UnVaccinatedList->searchSkipList(person->person->citizenID)!=NULL )
        {//if we already have knowledge on this citizen for this virus we ignore the 2nd entry the inputFile may have or the user might have accidentaly 
            cout << "ERROR IN RECORD ";//given because the vaccinateNow command has the purpose to take the citizen from UnVaccinatedList to VaccinatedList
            person->printcitizen();
            if(vac){
            cout<<"ERROR: CITIZEN " <<person->person->citizenID<<" ALREADY VACCINATED ON ";
            print_date(person->vaccine->dateVaccinated);
            }
            else
                cout<<"ERROR: CITIZEN " <<person->person->citizenID<<" ALREADY NOT VACCINATED\n ";
            delete person;
            return this;
        }
        else if (person->vaccine->vaccinated){//data doesnt exist on citizen for this virus
            temp->VaccinatedList = temp->VaccinatedList->InsertToSkip(person);
            temp->bloom->BloomInsert(person->person->citizenID);
        }
        else
            temp->UnVaccinatedList = temp->UnVaccinatedList->InsertToSkip(person);
    if (this == NULL)
        return temp;
    return this;
}

Countries *Countries::insert_in_data(citizen * person){//much simpler than above. remember the country list stores the citizenData of the person. vaccinedata is obviously NUll
    citizen* vac,*unvac;
    Countries *temp, *prev;
    if (this == NULL)
        temp = new Countries(person->person->country );
    else
    {

        for (temp = this; temp != NULL; temp = temp->next)
        {
            if (temp->name.compare(person->person->country) == 0)
                break;
            prev = temp;
        }
        if (temp == NULL)
        { //reached the end
            temp = new Countries(person->person->country);
            prev->next = temp;
        }
    }
    if (temp->name.compare(person->person->country) == 0)//always true
        // if ( (vac=temp->DataList->searchSkipList(person->citizenID)) !=NULL)
        // {
        //     //cout << "ERROR IN RECORD ";
        //     //person->printcitizen();
        //     //if(vac){
        //     //cout<<"ERROR: CITIZEN " <<person->citizenID<<" ALREADY VACCINATED ON ";
        //     //print_date(person->dateVaccinated);
        //     //}
        //     // else
        //     //     cout<<"ERROR: CITIZEN " <<person->citizenID<<" ALREADY NOT VACCINATED ";
        //     delete person;
        //     return this;
        // }
        //else if (person->vaccinated){
            temp->DataList = temp->DataList->InsertToSkip(person);
            //bloom->BloomInsert(person->citizenID);
        //}
        // else
        //     temp->UnVaccinatedList = temp->UnVaccinatedList->InsertToSkip(person);
    if (this == NULL)
        return temp;
    return this;  
}
citizen *Countries::search_in_data(citizen* person){//this also returns the data not only shows if it exists. 
 Countries *temp;//that way i can cross reference with the maybe Inconsistent data theuser or the random inputfile might have given and keep the original entry
    for (temp = this; temp != NULL; temp = temp->next)
        {
            if (temp->name.compare(person->person->country) == 0){
                return (temp->DataList->searchSkipList(person->person->citizenID));
            }
        }
        return NULL;
}
bool Countries::raw_search_in_data(int id){//this also returns the data not only shows if it exists. 
 Countries *temp;//that way i can cross reference with the maybe Inconsistent data theuser or the random inputfile might have given and keep the original entry
    for (temp = this; temp != NULL; temp = temp->next)
        {
            if (temp->DataList->searchSkipList(id)!=NULL) return true;
        }
    return false;
}
Record *Record::insert_in_record(citizen * person){
    citizen* data;
    citizen * person2;
if((data=this->people->search_in_data(person))==NULL){
    person2=new citizen();//data doesnt exist in record so add it to country list too
    person2->person=person->person;
    person2->vaccine=NULL;//this null is the difference between person1 and 2
    this->people=this->people->insert_in_data(person2);
    this->viruses=this->viruses->insert_in_virus(person);
}
else{
    delete person->person;//if the citizenData exists dump the just inputed data and keep the original
    person->person=data->person;
    this->viruses=this->viruses->insert_in_virus(person);
}
}

SkipListNode initializeSkipList()
{//to create an "empty" skiplist make #skipsize "dummy" nodes are  the head of each of the #skipsize levels.these dummies have NUll citizen pointers and the dummy of level i leads to dummy of level i-1 with the down pointer
    SkipListNode templayer = new SkipList(NULL, NULL, NULL, NULL);
    SkipListNode layer0 = templayer;
    for (int i = 0; i < skipsize; i++)
    {
        SkipListNode uplayer = new SkipList(NULL, NULL, NULL, templayer);
        templayer = uplayer;
    }
    return templayer;//dummy of the max level is the head of the skip list 
}
int SkipList::count_levels()
{
    int n = 0;
    SkipListNode temp = this;
    while (temp != NULL)
    {
        n++;
        temp = temp->down;
    }
    return n;
}

SkipListNode SkipList::InsertToSkip(citizen *Person)
{
    int n = this->count_levels();
    SkipListNode *store = new SkipListNode[n];//keep track of the nodes i have to inserrt the netry in front of if the randomizer makes them to to that level
    SkipListNode temp = this;
    for (int layer = n - 1; layer >= 0; layer--)//search for th eright position starts from up to down
    {
        while (/*temp->person ==NULL && */ temp->next != NULL && temp->next->person->person->citizenID < Person->person->citizenID)
        {//ascending order so we go on until we find the next element has smaller id than the new entry's (ignore dummy)
            temp = temp->next;
        }
        if (layer > 0)
        {
            store[layer] = temp;//note thaat we insert after temp of each layer
            temp = temp->down;
        }
        else
        {
            temp->InsertAfterAbove(Person, NULL);//check line 225. this function inserts person in a node after temp(this) and above the 2nd arguement
            store[layer] = temp;
            SkipListNode ret=this->randomAscend(store, n, Person);//randomly decide how many levels this entry will ascend
            delete[] store;
            return ret;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
    }
    exit(-1);
}
SkipListNode SkipList::randomAscend(SkipListNode *level, int counter, citizen *Person)
{
    int i = 1;
    while (i < counter && myrandom())//while within the number of levels insert it after the stored positions and above its copy reference in the previous level
    {
        level[i]->InsertAfterAbove(Person, level[i - 1]->next);
        i++;
    }
    if (i == counter && myrandom())
    {                                                               //the max level must increases very slow. the low chance of exceeding covers that
        SkipListNode newtop = new SkipList(NULL, NULL, NULL, this); //and if it keeps returning true i increase the max level only once
        newtop->InsertAfterAbove(Person, level[i - 1]->next);             //so it keeps increasing slow
        return newtop;
    }
    else
        return this;
}

void SkipList::InsertAfterAbove(citizen *Person, SkipListNode down)
{
    SkipListNode added = new SkipList(Person, this->next, this, down);
    if(added->next!=NULL) added->next->prev = added;
    this->next = added;
}

citizen *SkipList::searchSkipList(int id)//samea sinsert
{
    int n = this->count_levels();
    SkipListNode temp = this;
    for (int layer = n - 1; layer >= 0; layer--)
    {
        while (/*temp->person ==NULL && */ temp->next != NULL && temp->next->person->person->citizenID < id)
        {
            temp = temp->next;
        }
        if (temp->next != NULL && temp->next->person->person->citizenID == id)
        {
            return temp->next->person;
        }
        if (layer > 0)
        {
            //store[layer]=temp;
            temp = temp->down;//if entry didnt ascend that much it may be below
        }
        else
        {
            return NULL;
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
    }
    exit(-1);
}
bool SkipList::DeleteFromSkip(int id)
{
    int n = this->count_levels();
    SkipListNode temp = this;
    for (int layer = n - 1; layer >= 0; layer--)
    {
        while (/*temp->person ==NULL && */ temp->next != NULL && temp->next->person->person->citizenID < id)
        {
            temp = temp->next;
        }
        if (temp->next != NULL && temp->next->person->person->citizenID == id)
        {
            temp->next->deletenode();//delete the node and i must also delete the copy references in below levels
            return true;
        }
        if (layer > 0)
        {
            //store[layer]=temp;
            temp = temp->down;
        }
        else
        {
            return false;
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
    }
    exit(-1);
}

void SkipList::deletenode()
{
    if (this == NULL)
        return;
    SkipListNode prv = this->prev, nxt = this->next;//the prev pointer is what allow the deletion to be O(1) not O(n) but searching the whole list
    //if(this->person->vaccine!=NULL &&  this->down==NULL) delete this->person->vaccine;
    if(this->down==NULL) delete this->person;//remember this destructor deletes only the vaccine data
    this->down->deletenode();
    prv->next = nxt;
    nxt->prev = prv;
    delete this;
}
bool myrandom()
{
    int n = rand() % 10000;
    if (n < 10000 * P)
        return true;
    else
        return false;
}



void SkipList::destroy_layer(bool data)//deletes a skiplist. the data boolean shows is it was called in country skip list(data or not)
{
    if (this == NULL)
        return;
    bool del = false;
    SkipListNode down = this->down, temp = this, next;
    if (down == NULL)//dont delete because if layer>0 pointers work as references
        del = true; //layer 0 found delete citizen data
    for (; temp != NULL; temp = next)
    {
        next = temp->next;
        if (del && data && temp->person!=NULL)
            delete temp->person->person; //layer 0 country skip list so delete citizenData
        if (del /*&& !data*/ && temp->person!=NULL /*dummy*/) delete temp->person; 
        delete temp;
    }//basically avoid double free
    down->destroy_layer(data); //will end when this* reaches layer0
}

SkipList::SkipList(citizen *person, SkipListNode next, SkipListNode prev, SkipListNode down)
{
    this->person = person;
    this->next = next;
    this->down = down;
    this->prev = prev;
}
SkipList::~SkipList() {}



int Viruses::vaccineStatusBloom(int id, string virusname){
 Viruses *temp;
        for (temp = this; temp != NULL; temp = temp->next)
        {
            if (temp->name.compare(virusname) == 0)
                if(temp->bloom->BloomSearch(id)){
                    cout<<virusname<< " MAYBE\n";
                    return 1;
                }
                else{
                cout<<virusname<< " NOT VACCINATED\n";
                return 0;
                }
        }
    cout<<"virus name not found. country does not test for this virus. please try another one\n";return -1;
}


SkipListNode SkipList::getall(){
    SkipListNode temp=this;
    while(temp->down!=NULL)
    temp=temp->down;
    return temp->next;  
}//get a list of all elements of a skip list like a linked list

// void Record::populationStatus(string virusName , struct date date1 , struct date date2 ,string country){
// Countries *temp;
// Viruses* vaclist=this->viruses;
// int vaccinated_count=0,citizen_count=0,in_time_count=0;
//     for (temp = this->people; temp != NULL; temp = temp->next)
//         if (temp->name.compare(country) == 0){
//             for(SkipListNode person=temp->DataList->getall(); person != NULL; person = person->next){//for each person in a country
//                 citizen_count++;
//                 int vac;
//                 if((vac=vaclist->vaccineStatus(person->person->person->citizenID ,virusName,&date1,&date2,true))==2 ){//vaccinated in time period
//                     vaccinated_count++;
//                     in_time_count++;
//                 }
//                 else if(vac==1)//just vaccinated
//                     vaccinated_count++;
//             }
//             break;
//         }
//         cout<<"for "<<virusName <<endl;
//         cout<<country<<" "<<vaccinated_count<<" vaccinated with percentage " << (float)vaccinated_count/citizen_count *100<<"%\n";
//         cout<<country<<" "<<in_time_count<<" vaccinated within date range with percentage " << (float)in_time_count/citizen_count*100<<"%\n";
// }

// void Record::populationStatus(string virusName , struct date date1 , struct date date2 ){
// Countries *temp;
//     for (temp = this->people; temp != NULL; temp = temp->next)
//         this->populationStatus(virusName,date1,date2,temp->name);
// }

// void Record::popStatusByAge(string virusName , struct date date1 , struct date date2 ,string country){
// Countries *temp;
// Viruses* vaclist=this->viruses;
// int vaccinated_count=0,citizen_count=0,in_time_count=0;
// int age_vaccinations[4]= {0,0,0,0},age_timed_vaccinations[4] = {0,0,0,0},age_group_population[4] = {0,0,0,0};
//     for (temp = this->people; temp != NULL; temp = temp->next)
//         if (temp->name.compare(country) == 0){
//             for(SkipListNode person=temp->DataList->getall(); person != NULL; person = person->next){//for each person in a country
//                 citizen_count++;
//                 int vac,age_group=person->person->person->age/20;
//                 if(age_group>=4) age_group=3;
//                 age_group_population[age_group]++;
//                 if((vac=vaclist->vaccineStatus(person->person->person->citizenID ,virusName,&date1,&date2,true))==2 ){//vaccinated in time period
//                     vaccinated_count++;
//                     in_time_count++;
//                     age_vaccinations[age_group]++;
//                     age_timed_vaccinations[age_group]++;
//                 }
//                 else if(vac==1){//just vaccinated
//                     vaccinated_count++;
//                     age_vaccinations[age_group]++;
//                 }
//             }
//             break;
//         }
//         cout<<"for "<<country<< " and " <<virusName<<endl;
//         cout<<"0-20 " <<age_vaccinations[0]<< " "<<(float)age_timed_vaccinations[0]/age_group_population[0]*100<<"%"<<endl;
//         cout<<"20-40 " <<age_vaccinations[1]<< " "<<(float)age_timed_vaccinations[1]/age_group_population[1]*100<<"%"<<endl;
//         cout<<"40-60 " <<age_vaccinations[2]<< " "<<(float)age_timed_vaccinations[2]/age_group_population[2]*100<<"%"<<endl;
//         cout<<"60+ " <<age_vaccinations[3]<< " "<<(float)age_timed_vaccinations[3]/age_group_population[3]*100<<"%"<<endl;
// }

// void Record::popStatusByAge(string virusName , struct date date1 , struct date date2 ){
// Countries *temp;
//     for (temp = this->people; temp != NULL; temp = temp->next)
//         this->popStatusByAge(virusName,date1,date2,temp->name);
// }

bool Viruses::delete_from_record(citizen *person){
Viruses *temp;
    for (temp = this; temp != NULL; temp = temp->next)
        if (temp->name.compare(person->vaccine->virusName) == 0){
            temp->UnVaccinatedList->DeleteFromSkip(person->person->citizenID);
            temp->VaccinatedList->DeleteFromSkip(person->person->citizenID);
        }
}
            

void Record::vaccinateNow(citizen * person){
person->vaccine->vaccinated=true;
time_t t = time(NULL);
struct tm tm = *localtime(&t);
person->vaccine->dateVaccinated = new date();
person->vaccine->dateVaccinated->day= tm.tm_mday;
person->vaccine->dateVaccinated->year=tm.tm_year + 1900;
person->vaccine->dateVaccinated->month= tm.tm_mon + 1;//give present time vaccination time
for (Viruses *temp = this->viruses; temp != NULL; temp = temp->next)
    if (temp->name.compare(person->vaccine->virusName) == 0)
        temp->UnVaccinatedList->DeleteFromSkip(person->person->citizenID);//delete from unvaccinated list if he exists there
this->insert_in_record(person);//insert him with the same function that inserts the entries from the input file and the /insertCitizenRecord command
}

void Viruses::list_nonVaccinated(string virusName){
Viruses *temp;
    for (temp = this; temp != NULL; temp = temp->next)
        if (temp->name.compare(virusName) == 0){
            SkipListNode list= temp->UnVaccinatedList;
            while(list->down!=NULL) list=list->down;//go to layer 0 where everybody is
            for (  list =list->next; list != NULL; list = list->next)
                cout << list->person->person-> citizenID << " " << list->person->person-> firstName << " " << list->person->person-> country << " " << list->person->person-> age <<endl;
        }    
}
Record::~Record(){
    delete people;
    delete viruses;
}