
#include "classes.hpp"
int array::HTSize()
{
	int i, nodes = 0;
	list *Table = this->table;
	list cell;
	for (i = 0; i < this->size; i++)
		for (cell = Table[i]; cell != NULL; cell = cell->next)
			nodes++;
	return nodes;
}
int hashcode(char *K, int n)
{
	int h = 0, a = 33, i = 0;
	for (; K[i] != '\0'; i++)
		h = (a * h + K[i]) % n;
	return h;
}

Student *array::HTGet(char *keyid)
{
	int h;
	list  T;
	h = hashcode(keyid, this->size); //hashcode tells us where to put a new key and where we can find it
	T = this->table[h];
	return T->searchlist(keyid); //print later
}
bool array::HTInsert(Student *newstudent)
{
	Hash temphash = this;
	int h;
	list  T;
	h = hashcode(newstudent->getid(), temphash->size);
	T = (temphash)->table[h];
	if (T->searchlist(newstudent->getid()) != NULL)
	{
		cout << "student " << newstudent->getid() << " exists" << endl;
		delete newstudent;
		return 0;
	}
	else
	{
		temphash->table[h] = T->Insertfirst(newstudent); //else add it
		cout << "student " << newstudent->getid() << " inserted" << endl;
		return 1;
	}
}

Student *TableEntry::searchlist(char *id)
{
	list cell;
	for (cell = this; cell != NULL; cell = cell->next)
		if (strcmp(cell->student->getid(), id) == 0)
			return student;
	return NULL;
}

list TableEntry::deletenode(char *id, Student**s , int &year)
{
	list newlist, templist, prev_list;
	if (this == NULL)
		return this;
	if (strcmp(this->student->getid(), id) == 0)
	{
		year = this->student->year;
		return this->Deletefirst(s);
	}
	else
	{
		templist = this;
		newlist = this;
	}
	while (templist != NULL)
	{
		if (strcmp(templist->student->getid(), id) == 0)
		{
			year = templist->student->year;
			prev_list->next = templist->next;
			*s=templist->student;
			delete templist;
			return newlist;
		}
		prev_list = templist;
		templist = templist->next;
	}
	return newlist;
}

list TableEntry::Deletefirst( Student**s )
{
	list newlist, templist;
	templist = this;
	newlist = this->next;
	*s=templist->student;
	delete templist;
	return newlist;
}
list TableEntry::Insertfirst(Student *s)
{
	return new TableEntry(s, this);
}


II InvertedIndex::sortedInsert(Student *s)
{
	II current, newnode, front;
	if (this == NULL || this->year > s->year)
		return new InvertedIndex(s->year, s, this);
	else
	{
		current = this;
/////
		if (current->year == s->year)
		{
			current->students = current->students->Insertfirst(s);
			current->scount++;
			return this;
		}
///
		while (current->next != NULL && current->next->year < s->year)
			current = current->next;

		front = current->next;
		if (front == NULL || front->year > s->year)
		{
			newnode = new InvertedIndex(s->year, s, front);
			current->next = newnode;
		}
		else if (front->year == s->year)
		{
			front->students = front->students->Insertfirst(s);
			front->scount++;
			return this;
		}
	}
return this;
}


list InvertedIndex::getNthyear(int n)
{
	II current = this->getNthII(n);
	if (current != NULL)
		return current->students;
	else
	{
		cout << "this year doesnt have students" << endl;
		return NULL;
	}
}

void InvertedIndex::count()
{
	II temp;
	for (temp = this; temp != NULL; temp = temp->next)
	{
		cout << "year " << temp->year << " has " << temp->scount << " students\n";
	}
}

void InvertedIndex::minimumyear(int n)
{
	list temp,people = this->getNthyear(n);
	float min = 11.0;
	// Student *worst;
	temp=people;
	if (people == NULL)
	{
		cout << "no students enrolled this year" << endl;
		return;
	}
	for (; people != NULL; people = people->next)
		if (people->student->gpa < min)
			min = people->student->gpa;
			//worst = people->student;
	for (; temp != NULL; temp = temp->next)
		if (temp->student->gpa == min){
			temp->student->printstudent();cout<<" -- ";
			}
	//worst->printstudent();
	cout << endl;
}

void InvertedIndex::averageyear(int n)
{
	list people = this->getNthyear(n);
	float avg = 0.0;
	// count = 0;
	if (people == NULL)
		return;
	for (; people != NULL; people = people->next /*,count++*/)
		avg += people->student->gpa;
	avg = avg / (float)scount;
	cout << "average of" << n << " year is " << avg << endl;
}

void InvertedIndex::topnumyear(int num, int y)
{

	II index = this->getNthII(y);
	if (index == NULL)
	{
		cout << "No students enrolled in" << y << endl;
		return;
	}
	//int* grades=new int[scount];
	int j = 0, k,i;
	i=0;
	Student **s = new Student* [index->scount];
	list people;
	for (people = index->students; people != NULL; people=people->next)
	{
		s[i] = people->student;
		i++;
	}
	quickSort(s, 0, i - 1);
	for (k = 0; k < num && j + 1 < i ; k++)
	{
		while (j + 1 < i && s[j]->gpa == s[j + 1]->gpa)
		{
			cout << s[j]->getid() << " ";
			j++;
		}
		cout << s[j]->getid() << " ";
		j++;
	}
	cout << endl;
	delete[] s;
}

int partition(Student **arr, int low, int high)
{
	Student *t;
	int pivot = arr[high]->gpa; // pivot
	int i = (low - 1);			// Index of smaller element

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than the pivot
		if (arr[j]->gpa > pivot)
		{
			i++; // increment index of smaller element
			t = arr[j];
			arr[j] = arr[i];
			arr[i] = t;
		}
	}
	t = arr[high];
	arr[high] = arr[i + 1];
	arr[i + 1] = t;
	return (i + 1);
}

void swap(int *a, int *b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void array::postcode(int rank)
{
	bool found=0;
	int max = this->HTSize();
	int *zips = new int[max];
	int *count = new int[max];
	int i, nodes = 0, v;
	list *Table = this->table;
	list cell;
	for (i = 0; i < this->size; i++)
		for (cell = Table[i]; cell != NULL; cell = cell->next, found = 0){
			for (int k = 0; k < nodes && found == 0; k++)
			{				
					if (zips[k] == cell->student->zip)
					{
						found = 1;
						count[k]++;
						v = k;
						while (v > 0 && count[v] > count[v - 1])
						{
							swap(&count[v], &count[v - 1]);	
							swap(&zips[v], &zips[v - 1]);	
						v--;
						}
					}
			}
				if (!found)
				{
					zips[nodes] = cell->student->zip;
					count[nodes] = 1;
					nodes++;
				}
			}
	if (rank > nodes)
		cout << "rank requested is greater than the individual zips\n";
	else
	{
		for (int k = 0; k < nodes && found == 0; k++){
			cout<<zips[k]<<"  "<<count[k]<<endl;
		}
		int k, j = 0;
		for (k = 0; k < rank; k++)
		{
			while (j + 1 < nodes && count[j] == count[j + 1])
			{
				if (k == rank - 1)
					cout << zips[j] << " ";
				j++;
			}
			if (j + 1 == nodes && k < rank - 1)
			{
				cout << "rank requested is greater than the zip ranks\n";
				delete[] zips;
				delete[] count;
				return;
			}
			if (k == rank - 1)
				cout << zips[j] << " ";
			j++;
		}
		cout << endl;
	}
	delete[] zips;
	delete[] count;
}

void quickSort(Student **arr, int low, int high)
{
	if (low < high)
	{
		int pi = partition(arr, low, high);
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

void Student::printstudent()
{
	cout << studentid << ',' << lastname << ',' << firstname << ',' << zip << ',' << year << ',' << gpa;
}
void InvertedIndex::numberyear(int year)
{
	II current = this->getNthII(year);
	if (current != NULL)
		cout << current->scount << " student(s) in year " << current->year << endl;
	else
	{
		cout << "No students enrolled in" << year << endl;
	}
}
II InvertedIndex::getNthII(int year)
{
	II current = this;
	while (current != NULL && current->year < year)
		current = current->next;
	if (current != NULL && current->year == year)
		return current;
	else
		return NULL;
}
void array::showtable(){
	int i;
	list *Table = this->table;
	list cell;
	for (i = 0; i < this->size; i++){
		for (cell = Table[i]; cell != NULL; cell = cell->next){
			cell->student->printstudent();
			cout<<"-";
		}
		cout<<endl;
}
}














Student::Student(char *sid, char *lname, char *fname, int zip, int year, float gpa)
{
    this->studentid=new char[strlen(sid)+1];
    this->lastname=new char[strlen(lname)+1];
    this->firstname=new char[strlen(fname)+1];
    strcpy(this->studentid, sid);
    strcpy(this->lastname, lname);
    strcpy(this->firstname, fname);
    this->zip=zip;
    this->year=year;
    this->gpa=gpa;
}
Student::~Student()
{
    delete[] studentid;
    delete[] lastname;
    delete[] firstname;
}
TableEntry::TableEntry(Student* s=NULL,list l=NULL)
{
    student=s;
    next=l;
}

TableEntry::~TableEntry()
{
}

InvertedIndex::InvertedIndex(int year, Student *s, II link)
{
	this->year = year;
	next = link;
	students = NULL;
	scount = 1;
	students = students->Insertfirst(s);
}

InvertedIndex::~InvertedIndex()
{
    list link;
    for(list cell=students;cell!=NULL;){
    link=cell->next;
    delete cell;
    cell= link;
    }
    delete next;
}

array::array(int n)
{
    this->size = n;
	this->table = new list[n];
	for (int i = 0; i < n; i++)
	{
		this->table[i] = NULL;
	}
    years=NULL;
}

array::~array()
{
    list link;
    for (int i = 0; i < this->size; i++)
    for(list cell=this->table[i];cell!=NULL;){
    link=cell->next;
    delete cell->student;
    delete cell;
    cell= link;
    }
    delete years;
    delete[] table;
}
