#include "includes.hpp"
int bloomsize,skipsize=0;//in exercise 1 my skipsize was initialised to log10 of number of imputs 
bool sig_end=false;
short file_count=0;
int old_stdout = dup(1);
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

void initialize(pool_t **pool,int n) {
*pool=(pool_t*)malloc(sizeof(pool_t));
(*pool)->size=n;//size of this buffer is cyclicbuffersize
(*pool)->buffer=new string[n];
(*pool)->end = -1;//end tells the program the position of the end of the last string in the buffer
(*pool)->count = 0;
(*pool)->start = 0;
}

string obtain(pool_t * pool) {
string data;
pthread_mutex_lock(&mtx);//because i acess pool.count
while (pool->count <= 0) {
printf(">> Found Buffer Empty \n");
if(file_count <=0){//all data read. the job is done.exit
    printf("happened\n");
    pthread_mutex_unlock(&mtx);
    return data;
    //pthread_exit(0);
    
}
pthread_cond_wait(&cond_nonempty, &mtx);//now there is data
}
data = pool->buffer[pool->start];
pool->start = (pool->start + 1) % pool->size;
pool->count--;
pthread_mutex_unlock(&mtx);
return data;
}

void * consumer(void * ptr)
{
    pool_t* pool= *(pool_t**)ptr;
    Record *myrecord= *(Record**)((uint8_t*)ptr+sizeof(pool_t**));
    pthread_mutex_lock(&mtx);//because i access pool.count & file_count
    while (file_count > 0 || pool->count > 0) {
    pthread_mutex_unlock(&mtx);
    string file;
    printf("consumer %ld: %s\n",pthread_self(),( file= obtain(pool)).c_str());
    pthread_cond_broadcast(&cond_nonfull);
    //usleep(30);
    pthread_mutex_lock(&mtx);
    if(!file.empty()){//if consumer wasnt executing while all data were read before hand
        //fclose(stdout);
        ifstream Myinput(file.c_str());
        while (Myinput.peek() != EOF)
            {
                citizen *a = read_citizen(Myinput);
                if(a!=NULL){//empty line
                    myrecord->insert_in_record(a);//only this thread has access to this file because only it read it but this data structure pointer is shared so we must lock
                }
            }
        Myinput.close();
        //stdout = fdopen(old_stdout, "w");
    }
    }
    printf("consumer %ld exiting\n",pthread_self());
    pthread_mutex_unlock(&mtx);
    pthread_exit(0);
}


void place(pool_t * pool, string data) {
pthread_mutex_lock(&mtx);
// cout<<pool->size<<pool->count<<endl;
while (pool->count >=  pool->size) {//there isnt enough space
    printf(">> Found Buffer Full \n");
    pthread_cond_wait(&cond_nonfull, &mtx);
}
pool->end = (pool->end + 1) % pool->size;
pool->buffer[pool->end] = data;
pool->count++;
pthread_mutex_unlock(&mtx);
}

void * producer(void * ptr)
{
    int i,j;
    pool_t* pool= *(pool_t**)ptr;
    string* countries= *(string**)((uint8_t*)ptr+sizeof(pool_t**));//thread_create functions only accept a void* arguement so i put all my desired arguements inside a buffer and pass it here
    short *file_per_country= *(short**)((uint8_t*)ptr+sizeof(pool_t**)+sizeof(string**));
    int country_count= *(int*)((uint8_t*)ptr+sizeof(pool_t**)+sizeof(string**)+sizeof(short**));
    cout<<"producer has "<<country_count<<" countries"<<endl;
    //while (file_count > 0) {
    for(i=0;i<country_count;i++){
        string country=get_country(countries[i],"/");//str-tok basically with "/" delimeter
        ifstream counter(string(countries[i] + "/counter.txt").c_str());
        counter>>j;//we have read j-1 files in this subdirectory.continue from j in add Vaccinationrecords
        counter.close();//make it global var for multi-producer.j is synchronically wise equivelant to num_of_items while file_count is LITERALLY num_of_items in class example
        //pthread_mutex_lock(&mtx);
        for(;access(string( countries[i] + '/' + country + '-' + to_string(j) + ".txt").c_str(), F_OK ) == 0;j++ ){//increment j while the files keep existing
            //temp=j;//use temp from now on
            //file_count--;j++;//take out from for and file_count from below
            //pthread_mutex_unlock(&mtx);
            place(pool, string( countries[i] + '/' + country + '-' + to_string(j) + ".txt"));
            printf("producer: %s\n", string( countries[i] + '/' + country + '-' + to_string(j) + ".txt").c_str());
            file_count--;
            pthread_cond_broadcast(&cond_nonempty);  
            usleep(0);
            //pthread_mutex_lock(&mtx);
        }
        ofstream ofcounter(string( countries[i] + '/'  + "/counter.txt").c_str());
        ofcounter<<j;
        ofcounter.close();
        file_per_country[i]=j;//we have read j-1 files in this subdirectory.continue from j in add Vaccinationrecords
    }
    printf("producer exiting\n");
    pthread_exit(0);
}

void log_file(int signo){
    sig_end=true;
    printf("LOG ME \n");
}
int main(int argc, char **argv){
    cout<<"server\n";
    bloomsize=atoi(argv[10]);
    srand(time(NULL));
    int i,bufferSize=atoi(argv[6]),status,j,country_count=0,acc=0,rej=0,cyclicBufferSize=atoi(argv[8]),numThreads=atoi(argv[4]),port=atoi(argv[2]);
    country_count=argc-11;
    string* countries= new string[country_count];
    short *file_per_country= new short[country_count];
    for(i=0;i<country_count;i++){//i made a decision for  skiplist in project 1 that it must know the expected number of records to initialise its number of levels to the log10 of that  
        countries[i]=argv[11+i];//split
        //so we parse the files to find that out before the listening thread passes them on to consumer. this a small compromise to project3 but its unavoidable
        //i still  materialize the threaded parsing later though with 100% authenticity
        string country=get_country(countries[i],"/");
        for(j=1;access(string( countries[i] + '/' + country + '-' + to_string(j) + ".txt").c_str(), F_OK ) == 0;j++ ){
            ifstream Myinput(string( countries[i] + '/' + country + '-' + to_string(j) + ".txt").c_str());
            while (Myinput)
            {
                char c;
                Myinput.get(c);
                if (c == '\n')
                    skipsize++;//find how many lines/entires we have//this is the purpose of the whole loop
            }
            Myinput.close();
            file_count++;
        }
        file_per_country[i]=j;//get othe useful info as well
        //cout<<countries[i]  + "/counter.txt"<<endl;
        ofstream counter(string( countries[i] + "/counter.txt").c_str());
        counter<<1;//we start from file "-1". remember producer()
        counter.close();//create the temporary counter txt that tells us how many txts have been read in this folder. useful for /addVaccinationRecords 
    }//my code from project 1 requires i get the total number of records so i can initalize the number of levels of the skiplist to the log(#records)
    skipsize = log10(skipsize);
    pool_t* mypool;
    initialize(&mypool,cyclicBufferSize);
    void *buf_prod=malloc(50);
    memcpy(buf_prod,&mypool,sizeof(pool_t**));
    memcpy((uint8_t*)buf_prod+sizeof(pool_t**),&countries,sizeof(string**));
    memcpy((uint8_t*)buf_prod+sizeof(pool_t**)+sizeof(string**),&file_per_country,sizeof(short**));
    memcpy((uint8_t*)buf_prod+sizeof(pool_t**)+sizeof(string**)+sizeof(short**),&country_count,sizeof(int));//batch all arguements
    //producer(buf_prod);
    Record *myrecord = new Record();
    void *buf_cons=malloc(50);
    memcpy(buf_cons,&mypool,sizeof(pool_t**));
    memcpy((uint8_t*)buf_cons+sizeof(pool_t**),&myrecord,sizeof(Record**));
    pthread_t cons, prod;
    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);
    pthread_create(&prod, 0, producer,buf_prod);//call producer and consumer with buf_cons and buf_prod arguements. we do this because we can only pass void* to these functions
    for(i=0;i<numThreads;i++){
    pthread_create(&cons, 0, consumer,buf_cons);
    pthread_join(cons, 0);
    }
    pthread_join(prod, 0);
    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_mutex_destroy(&mtx);


    int    sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;//one socket because we have 1 client 
    struct hostent *rem;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    perror_exit("socket");
    int k=1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&k, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,&k, sizeof(int));//this allows us to reuse ports on consequtive executions and not hang on TIME_WAIT
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
    perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", port);
    clientlen = sizeof(client);
    /* accept connection */
    if ((newsock = accept(sock, clientptr, &clientlen))
    < 0) perror_exit("accept");
    /* Find client's name */
    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,
    sizeof(client.sin_addr.s_addr), client.sin_family))
    == NULL) {
    herror("gethostbyaddr"); exit(1);}
    printf("Accepted connection from %s\n", rem->h_name);
    string a="work";
    //if(fork()==0){
    Viruses* viruslist;
    int viruscount=0;
    for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next){
        viruscount++;
    }//count the viruses detected. its not 100% certain all children will handle the same viruses
    printf("viruscount of monitor is %d\n",viruscount);
    write(newsock,&viruscount,sizeof(int));
    for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next){
        int temp=viruslist->name.length()+1;
        int bloombytes_written=0;
        write(newsock,&temp,sizeof(int));
        write(newsock,viruslist->name.data(),temp);
        int * array=viruslist->bloom->array;//this mirrors the read of bloom in main.cpp
        while( (bloombytes_written+=write(newsock,(uint8_t*)array+bloombytes_written,bufferSize)) <(bloomsize/8)); //printf("%d\n",bloombytes_written);
    }
    cout<<"transferred bloom\n";
    while (1)
    {

        struct date d1, d2;
        printf("waiting\n");
        // while(!pipe_ready){
        // }
        //printf("pipe_ready\n");
        int command_length,id,temp, stringlen;
        read(newsock,&command_length,sizeof(int));
        char *str=new char[command_length];
        read(newsock,str,command_length);
        if (strcmp(str,"/exit") == 0){
            ofstream log_file(string("logfile."+ to_string( (long) (getpid()) )+ ".txt").c_str());
            for ( int i = 0; i < country_count; i++){
                log_file<<countries[i]<<endl;
            }
            log_file<<acc+rej<<endl;
            log_file<<acc<<endl;
            log_file<<rej<<endl;
            log_file.close();
            delete[] str;//make logfile with all countries and request results
            break; 
        }
        else if (strcmp(str,"/travelRequest") == 0){
            read(newsock,&id,sizeof(int));
            read(newsock,&temp,sizeof(int));
            delete[] str;
            str =new char[temp];
            read(newsock,str,temp);
            read(newsock,&d1,sizeof(struct date));
            d2=d1;
            d2.month -=6;
            if(d2.month<=0){
                d2.month=12+d2.month;
                d2.year--;
            }//decrease d1 by 6 months to get a date range for adequate vaccination
            string dummy;
            printf("Monitor bloom says  %d\n",myrecord->viruses->vaccineStatusBloom(id,str));
            int ans=myrecord->viruses->vaccineStatus(dummy, id, str, &d2, &d1, true);//reuse my vaccineStatus() from project1
            if(ans==2) acc++;
            else if(ans<2) rej++;
            write(newsock,&ans,sizeof(int));
            //kill(getppid(),SIGUSR2);
        }
        else if (strcmp(str,"/searchVaccinationStatus") == 0){
            delete[] str;
            //printf("searchVaccinationStatus\n");
            read(newsock,&id,sizeof(int));
            if(myrecord->people->raw_search_in_data(id)){
            string report =myrecord->viruses->vaccineStatus(id);
            //printf("%s\n",report.data());
            temp=report.length()+1;
            write(newsock,&temp,sizeof(int));
            write(newsock,report.data(),temp);
            }
            else{
                temp=0;
                write(newsock,&temp,sizeof(int));
            }
            
            //kill(getppid(),SIGUSR2);
        }
        else if (strcmp(str,"/addVaccinationRecords") == 0){
            read(newsock,&temp,sizeof(int));
            delete[] str;
            str =new char[temp];//countryto
            read(newsock,str,temp);
            file_count=0;
            string path=str;
            string country=get_country(path,"/");
            cout<<country<<endl;
            int pos;
            for(i=0;i<country_count;i++){
                if(countries[i].compare(path)==0) pos=i;//get right subdirectory
            }
            for(j=file_per_country[pos];access(string( path + '/' + country + '-' + to_string(j) + ".txt").c_str(), F_OK ) == 0;j++ ){
            file_count++;//this is now how many NEW files exist because producer() made it 0
            }
            cout<<"got "<<file_count<<"new files\n";
            file_per_country[pos]=j;

            pthread_mutex_init(&mtx, 0);
            pthread_cond_init(&cond_nonempty, 0);
            pthread_cond_init(&cond_nonfull, 0);//restart threads. remember thanks to file_per_country and counter.txt producer knows from where to continue
            pthread_create(&prod, 0, producer,buf_prod);
            for(i=0;i<numThreads;i++){
            pthread_create(&cons, 0, consumer,buf_cons);
            pthread_join(cons, 0);
            }
            pthread_join(prod, 0);
            pthread_cond_destroy(&cond_nonempty);
            pthread_cond_destroy(&cond_nonfull);
            pthread_mutex_destroy(&mtx);


            int viruscount=0;
            for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next)
                viruscount++;
            printf("viruscount is %d\n",viruscount);
            write(newsock,&viruscount,sizeof(int));
            for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next){//pass updated bloom filter
                int temp=viruslist->name.length()+1;
                int bloombytes_written=0;
                write(newsock,&temp,sizeof(int));
                write(newsock,viruslist->name.data(),temp);
                int * array=viruslist->bloom->array;
                while( (bloombytes_written+=write(newsock,(uint8_t*)array+bloombytes_written,bufferSize)) <(bloomsize/8)); //printf("%d\n",bloombytes_written);
            }
        }
        else{
            perror("wrong command in pipe\n");
            exit(1);
        }
            //close(fd[i][WRITE]);
    }
    //no need for clean up because sigkill ends the monitors
    //sleep(1);
    shutdown(newsock, 2);//so other students and i can reuse the port on this machine
    close(newsock);
    delete[] countries;delete[] file_per_country;
    delete myrecord;
    delete[] mypool->buffer;free(mypool);free(buf_cons);free(buf_prod);
    exit(1);
}