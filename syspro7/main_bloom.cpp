
#include "includes.hpp"

extern int bloomsize, skipsize;

unsigned long djb2(string str)
{
    unsigned long hash = 5381;
    int c;
    std::string::iterator it = str.begin();
    while (it != str.end())
    {
        hash = ((hash << 5) + hash) + *it; /* hash * 33 + c */
        it++;
    }
    return hash;
}

unsigned long sdbm(string str)
{
    unsigned long hash = 0;
    int c;
    std::string::iterator it = str.begin();
    while (it != str.end())
    {
        hash = *it + (hash << 6) + (hash << 16) - hash;
        it++;
    }

    return hash;
}

unsigned long hash_i(string str, unsigned int i)
{
    return djb2(str) + i * sdbm(str) + i * i;
}

// makes the kth bit of an int 1. k numbering goes from 1 to 32 from right to left.
int setBit(int n, int k)
{
    return (n | (1 << (k - 1))); //number 1 has only the 1rst bit as 1. move the bits k positions left and the | operator will make only the k bit 1 and rest unchaged because of the 31 0s
}

bool isKthBitSet(int n, int k)
{
    if (n & (1 << (k - 1))) //now because & the 31 0s will create 31  0s in n. if k bit of n is 1 it will stay as such and the condition number will be >0
        return true;
    else
        return false;
}

BloomFilter::BloomFilter(int M) //bits asked in commandline
{
    this->M = M;
    this->size = M / (sizeof(int) * 8); //array size M/32
    this->array = new int[size];
    for (int i = 0; i < size; i++)
    {
        array[i] = 0; //set all bits to 0
    }
}
BloomFilter::BloomFilter(int *arr){
    this->M = bloomsize;
    this->size = bloomsize/ (sizeof(int) * 8); //array size M/32
    this->array = arr;
}
BloomFilter::~BloomFilter()
{
    delete[] array;
}

void BloomFilter::BloomInsert(int id)
{
    std::string s = std::to_string(id);
    int bitpositions[K];
    for (int i = 0; i < K; i++)
    {
        bitpositions[i] = hash_i(s, i) % M;
    }
    for (int i = 0; i < K; i++)
    {
        int cellposition = bitpositions[i] / (sizeof(int) * 8); //example if bit 33 must be changed we go to the 2nd int and change its 1rst bit(or 32th bit according to the numbering of line 38)
        int bitnumber = bitpositions[i] % (sizeof(int) * 8);
        array[cellposition] = setBit(array[cellposition], sizeof(int) * 8 - bitnumber);
    }
}

bool BloomFilter::BloomSearch(int id) //same principle as above
{
    std::string s = std::to_string(id);
    int bitpositions[K];
    for (int i = 0; i < K; i++)
    {
        bitpositions[i] = hash_i(s, i) % M;
    }
    for (int i = 0; i < K; i++)
    {
        int cellposition = bitpositions[i] / (sizeof(int) * 8);
        int bitnumber = bitpositions[i] % (sizeof(int) * 8);
        if (!isKthBitSet(array[cellposition], sizeof(int) * 8 - bitnumber))
            return false;
    }
    return true;
}
void BloomFilter::print_Bloom() //use if you want to see the bloom filter raw
{
    for (int i = 0; i < size; i++)
        for (int j = 32; j > 0; j--)
            if (isKthBitSet(array[i], j))
                cout << "1";
            else
                cout << "0";
    cout << endl;
}
//this function is used with dates too when i also use for the popStatus command since its a ready way to surely check if someone got vaccinated for a virus
int Viruses::vaccineStatus(string & message,int id, string virusname, struct date *date1 = NULL, struct date *date2 = NULL, bool print = false)
{
    Viruses *temp;
    citizen *person;
    //string message;
    for (temp = this; temp != NULL; temp = temp->next)
    {
        if (temp->name.compare(virusname) == 0)
        {                                                      //search all viruses
            person = temp->VaccinatedList->searchSkipList(id); //search the positive skip list of the virus that returns the citizen with the id
            if (person == NULL)
            {
                if (!print)
                    message +=( virusname + " NO\n");
                return 0;
            } //if i use it for popStatus i dont want it to make a million prints//could make print false again to check validity of results
            else
            {
                if (!print)
                {
                    message +=( virusname + " ");
                    message +=print_date(person->vaccine->dateVaccinated);
                }
                if (date1 != NULL && date2 != NULL && compare_dates(*(person->vaccine->dateVaccinated), *date1) >= 0 && compare_dates(*(person->vaccine->dateVaccinated), *date2) <= 0)
                    return 2; //if vaccinated within a date return 2 else 1
                return 1;
            }
        }
    }
    message +="virus name not found\n";
}
string Viruses::vaccineStatus(int id)
{
    Viruses *temp;
    string message;
    for (temp = this; temp != NULL; temp = temp->next)
    {
        temp->vaccineStatus(message,id, temp->name); //above
    }                                        //this will immediately make true the first if statement in the above function so the for only executes once
    return message;
}

string print_date(struct date *d)
{
    if (d == NULL)
      return " NO\n";
    else
        return " YES " + to_string( d->day) + "-" + to_string(d->month) + "-" + to_string(d->year) + +"\n";
}

int compare_dates(struct date d1, struct date d2)
{
    if (d1.year < d2.year)
        return -1;

    else if (d1.year > d2.year)
        return 1;

    else
    {
        if (d1.month < d2.month)
            return -1;
        else if (d1.month > d2.month)
            return 1;
        else if (d1.day < d2.day)
            return -1;
        else if (d1.day > d2.day)
            return 1;
        else
            return 0;
    }
}

void read_date(std::istream &stream, struct date *d, bool space = false)
{ //this function take a stream arguememnt because the data can be read from standard input or a file
    string vacdate;
    getline(stream, vacdate, '-'); //split string(numbers) based on '-' character
    d->day = stoi(vacdate);
    getline(stream, vacdate, '-');
    d->month = stoi(vacdate);
    if (!space)
        getline(stream, vacdate); //structure of input changes in standard input. 1rst and 2nd date arent connected by '-' so it bugs
    else
        getline(stream, vacdate, ' ');
    d->year = stoi(vacdate);
}
citizen *read_citizen(std::istream &stream,string country, bool vac)
{
    string yesno;
    string id;
    stream >>id;
    if (/*id.empty()*/id=="" || id==" " || id=="\n" || id=="\r" || id=="\0") return NULL;//empty line
    citizen *a = new citizen();
    a->person = new citizenData();
    a->vaccine = new vaccinedata();
    a->person->citizenID=stoi(id);
    //cout<<a->citizenID<<endl;
    stream >> a->person->firstName;
    stream >> a->person->lastName;
    //a->person->country=country;
    stream >> a->person->country;
    stream >> a->person->age;
    stream >> a->vaccine->virusName;
    if (vac)
    {
        stream >> yesno;
        if (yesno.compare("YES") == 0)
        {
            // cout<<"yeeesss\n";
            a->vaccine->vaccinated = true;
            a->vaccine->dateVaccinated = new date();
            read_date(stream, a->vaccine->dateVaccinated);
        }
        else
        {
            a->vaccine->vaccinated = false;
            a->vaccine->dateVaccinated = NULL;
        }
    }
    else
        a->vaccine->dateVaccinated = NULL;
    return a;
}
int count_words(string line)//to error check the "/" command issuing i needed to change words given 
{
    string word;
    int wordCount = 0;
    stringstream lineStream(line);//copy string to streamsteam and take each word
    while (lineStream >> word)
        ++wordCount;
    return wordCount;
}

// int main(int argc, char **argv)
// {
//     srand(time(NULL));
//     int i;
//     char *inputfile = NULL;
//     for (i = 1; i < argc; i++)
//     {
//         if (strcmp(argv[i], "-c") == 0)
//         {
//             inputfile = new char[strlen(argv[i + 1]) + 1];
//             strcpy(inputfile, argv[i + 1]);
//         }
//         else if (strcmp(argv[i], "-b") == 0)
//         {
//             bloomsize = atoi(argv[i + 1]);
//         }
//     }
//     char c;
//     ifstream Myinput(inputfile);
//     skipsize = 0;
//     while (Myinput)
//     {
//         Myinput.get(c);
//         if (c == '\n')
//             skipsize++;//find how many lines/entires we have
//     }
//     skipsize = log10(skipsize);// skiplist maxsize initialized with number of expected number of entries
//     Myinput.clear();// go back to start of file
//     Myinput.seekg(0);
//     string yesno;
//     BloomFilter mybloom(bloomsize);
//     Record *myrecord = new Record();
//     while (Myinput.peek() != EOF)
//     {
//         citizen *a = read_citizen(Myinput);
//         a->printcitizen();
//         mybloom.BloomInsert(a->person->citizenID);
//         myrecord->insert_in_record(a);
//     }
//     string command;
//     while (1)
//     {//take commands until /exit
//         cin >> command;
//         int id;
//         string virusname, line, country;
//         struct date d1, d2;
//         citizen *a;
//         if (command.compare("/exit") == 0)
//         {
//             // delete d1;
//             // delete d2;
//             break;
//         }
//         else if (command.compare("/vaccineStatusBloom") == 0)
//         {
//             cin >> id;
//             cin >> virusname;
//             myrecord->viruses->vaccineStatusBloom(id, virusname);
//         }
//         else if (command.compare("/vaccineStatus") == 0)
//         {
//             getline(cin, line);
//             stringstream lineStream(line);
//             if (count_words(line) == 2)
//             {
//                 lineStream >> id;
//                 lineStream >> virusname;
//                 myrecord->viruses->vaccineStatus(id, virusname);
//             }
//             else
//             {
//                 lineStream >> id;
//                 myrecord->viruses->vaccineStatus(id);
//             }
//         }
//         else if (command.compare("/populationStatus") == 0)
//         {
//             getline(cin, line);
//             stringstream lineStream(line);
//             if (count_words(line) == 4)
//             {
//                 lineStream >> country;
//                 lineStream >> virusname;
//                 read_date(lineStream, &d1, true);
//                 read_date(lineStream, &d2, true);
//                 myrecord->populationStatus(virusname, d1, d2, country);
//             }
//             else if (count_words(line) == 3)
//             {
//                 lineStream >> virusname;
//                 read_date(lineStream, &d1, true);
//                 read_date(lineStream, &d2, true);
//                 myrecord->populationStatus(virusname, d1, d2);
//             }
//             else
//                 cout << "ERROR wrong number of inputs\n";
//         }
//         else if (command.compare("/popStatusByAge") == 0)
//         {
//             getline(cin, line);
//             stringstream lineStream(line);
//             if (count_words(line) == 4)
//             {
//                 lineStream >> country;
//                 lineStream >> virusname;
//                 read_date(lineStream, &d1, true);
//                 read_date(lineStream, &d2, true);
//                 myrecord->popStatusByAge(virusname, d1, d2, country);
//             }
//             else if (count_words(line) == 3)
//             {
//                 lineStream >> virusname;
//                 read_date(lineStream, &d1, true);
//                 read_date(lineStream, &d2, true);
//                 myrecord->popStatusByAge(virusname, d1, d2);
//             }
//             else
//                 cout << "ERROR wrong number of inputs\n";
//         }
//         else if (command.compare("/insertCitizenRecord") == 0)
//         { //!!!
//             a = read_citizen(cin);
//             myrecord->insert_in_record(a);
//         }
//         else if (command.compare("/vaccinateNow") == 0)
//         {
//             a = read_citizen(cin, false);
//             myrecord->vaccinateNow(a);
//         }
//         else if (command.compare("/list-nonVaccinated-Persons") == 0)
//         {
//             cin >> virusname;
//             myrecord->viruses->list_nonVaccinated(virusname);
//         }
//         else
//         {
//             cout << "wrong input\n";
//         }
//     }
//     Myinput.close();
//     delete[] inputfile;
//     delete myrecord;
// }
