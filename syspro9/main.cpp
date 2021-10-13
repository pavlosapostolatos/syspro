#include "includes.hpp"
int bloomsize,skipsize=0;
bool sig_end=false;
long dead_child=0;

int main(int argc, char **argv)
{
    srand(time(NULL));
    int numMonitors,bufferSize,i,status,country_count=0,j,acc=0,rej=0,numThreads,cyclicBufferSize,hostname;
    string input_dir,command,countryFrom, countryTo,virusName;
    pid_t parent=getpid();
    struct hostent *rem;
    if (argc!=13){
        perror("wrong command line arguements");
        exit(-1) ;
    }
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-m") == 0)
        {
            numMonitors = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            bufferSize = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            cyclicBufferSize = atoi(argv[i + 1]);
        }
       else if (strcmp(argv[i], "-s") == 0)
        {
            bloomsize = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            input_dir=argv[i + 1];
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            numThreads=atoi(argv[i + 1]);
        }
    }
    char host[256];
    hostname = gethostname(host, sizeof(host));cout<<host<<endl;//get the adress of our machine
    if ((rem = gethostbyname(host)) == NULL) {
        herror("gethostbyname"); exit(1);
    }
    //cout<<numMonitors<<argv[argc-1]<<endl;
    struct sockaddr_in *server=new struct sockaddr_in[numMonitors];
    struct sockaddr **serverptr = new struct sockaddr*[numMonitors];
    int* sockets=new int[numMonitors];//one socket for each server
    pid_t* children=new pid_t[numMonitors];//store children pids
    for (i=0;i<numMonitors;i++){
        serverptr[i] = (struct sockaddr*)&(server[i]);
        children[i]=getpid()+1+i;
        int temp=1;
        if ((sockets[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            perror_exit("socket");
        setsockopt(sockets[i], SOL_SOCKET, SO_REUSEADDR,&temp, sizeof(int));//this allows us to reuse ports on consequtive executions and not hang on TIME_WAIT
        setsockopt(sockets[i], SOL_SOCKET, SO_REUSEPORT,&temp, sizeof(int));
        server[i].sin_family = AF_INET; /* Internet domain */
        memcpy(&(server[i]).sin_addr, rem->h_addr, rem->h_length);
        server[i].sin_port = htons(STARTPORT+i); /* Server port */ //change startpoint includes.hpp if you want
    }
    DIR * dir_ptr = opendir(input_dir.c_str());
    struct dirent *direntp ;
    while ((direntp = readdir(dir_ptr)) != NULL){
        if ( !strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..") ) continue;
        country_count++;
    }//find out how many folders/countries exist
    closedir(dir_ptr);
    if(numMonitors>country_count){
        perror("more monitors given than countries");
        exit(1);
    }
    string* countries= new string[country_count];
    int* monitor_country_count=new int[numMonitors];
    for (i=0;i<numMonitors;i++){
        monitor_country_count[i]=country_count/numMonitors +( (i+1)<=country_count%numMonitors);
        //cout<<monitor_country_count[i]<<endl;
    }//here we equally round robin share the available 
    dir_ptr = opendir(input_dir.c_str());
    for (i=0; (direntp = readdir(dir_ptr)) != NULL;i++){
        if ( !strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..") ){
            i--;
            continue;
        }
        countries[i]=string(direntp->d_name);
        // cout<<i<<countries[i];
    }//get names of all countries
    cout<<countries[0]<<endl;
    closedir(dir_ptr);
    bubbleSort(countries,country_count);//sort the name alphabetically
    string** monitor_countries=new string*[numMonitors];//with this array we know which countries a specific process takes care of
    int k=-1;
    for(i=0;i<numMonitors;i++){
        monitor_countries[i]=new  string[monitor_country_count[i]];//create as many columns for country names as the number of countries assigned to this process in monitor_country_count
        char **arguements=new char*[11+monitor_country_count[i]+1];//make string array for server commandline arguements
        for (int p=0;p<11+monitor_country_count[i]+1;p++) 
            arguements[p]=new char[20];
        strcpy(arguements[0],"./monitorServer");
        strcpy(arguements[1],"-p");
        strcpy(arguements[2],(char*)to_string(STARTPORT+i).c_str());//cout<<arguements[2]<<endl;
        strcpy(arguements[3],"-t");
        strcpy(arguements[4],(char*)to_string(numThreads).c_str());
        strcpy(arguements[5],"-b");
        strcpy(arguements[6],(char*)to_string(bufferSize).c_str());//cout<<arguements[6]<<endl;
        strcpy(arguements[7],"-c");
        strcpy(arguements[8],(char*)to_string(cyclicBufferSize).c_str());
        strcpy(arguements[9],"-s");
        strcpy(arguements[10],(char*)to_string(bloomsize).c_str());//cout<<arguements[10]<<endl;
        for(j=0;j<monitor_country_count[i];j++){
            monitor_countries[i][j]=countries[++k];//we assign countires from top to bottom alphabetically
            strcpy(arguements[11+j], (char*)string(input_dir + '/' + monitor_countries[i][j]).c_str());
        }
        arguements[11+j]=NULL;
        if(fork()==0)//make servers
        execvp("./monitorServer",arguements);  
        for (int p=0;p<11+monitor_country_count[i]+1;p++) 
            delete[] arguements[p];
        delete[] arguements;
        //close(fd[i][WRITE]);
    }
    cout<<"called server\n";
    int connectstatus;
    for (i=0;i<numMonitors;i++){
    do{
        connectstatus = connect(sockets[i], serverptr[i], sizeof(server[i]));
    }while (connectstatus<0);//wait for server to connect first in bind and accept
    }
    cout<<"connected\n";
    Viruses** monitor_bloom_list=new Viruses*[numMonitors];//the 2 programs use the same classes
    for (i=0;i<numMonitors;i++){//make numMonitors lists of bloom filters.((viruses have NULL skip lists here))
        monitor_bloom_list[i]=NULL;
        int viruscount;
        read(sockets[i],&viruscount,sizeof(int));//learn how many viruses to read blooms for
        for(j=0;j<viruscount;j++){
            int stringlen,bloombytes_read=0;
            read(sockets[i],&stringlen,sizeof(int));
            char *str=new char[stringlen];
            read(sockets[i],str,stringlen);//read the name of the virus assigned to this list
            int *array = new int[(bloomsize / (sizeof(int) * 8))];//convert bits to int by dividing by 32
            while( (bloombytes_read+=read(sockets[i],(uint8_t*)array+bloombytes_read,bufferSize)) <(bloomsize/8));// cout<<bloombytes_read<<endl;
            monitor_bloom_list[i]=monitor_bloom_list[i]->addVirus(str,array);
            delete[] str;//keep reading until we have read the expected number of bytes then add that array to the list
        }
    }
    cout<<"recieved bloom\n";
    TravelRequests * Request_List=NULL;
    int dummy=0;
    while (1)
    {//take commands until /exit
        dummy++;
        cout<<"give command\n";
        cin >> command;
        int id;
        //string virusname, line, country;
        struct date d1, d2;
        citizen *a;
        char* message;
        string countryFrom, countryTo,virusName;
        if (command.compare("/exit") == 0)
        {
            // delete d1;
            // delete d2;
            ofstream log_file(string("logfile."+ to_string((long) (getpid()) )+ ".txt").c_str());
            for ( int i = 0; i < country_count; i++){
                log_file<<countries[i]<<endl;
                int temp=command.length()+1;
                write(sockets[i],&(temp),sizeof(int));//tell servers to exit too
                write(sockets[i],command.data(),temp);
            }
            log_file<<acc+rej<<endl;
            log_file<<acc<<endl;
            log_file<<rej<<endl;
            log_file.close(); //make logfile with all countries and request results
            break;
        }
        else if (command.compare("/travelRequest") == 0)
        {
            cin>>id;
            read_date(cin,&d1,true);
            cin>>countryFrom;
            cin>>countryTo;
            cin>>virusName;
            int my_monitor=search_monitor_by_country( numMonitors,monitor_countries,monitor_country_count, countryFrom);//find which monitor handles this country
            int ans=monitor_bloom_list[my_monitor]->vaccineStatusBloom(id,virusName);//check local bloom
            if(ans!=-1){
                if(ans==0) printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
                else{
                        int temp=command.length()+1;
                        write(sockets[my_monitor],&(temp),sizeof(int));
                        write(sockets[my_monitor],command.data(),temp);
                        write(sockets[my_monitor],&id,sizeof(int));
                        temp=virusName.length()+1;
                        write(sockets[my_monitor],&(temp),sizeof(int));
                        write(sockets[my_monitor],virusName.data(),temp);
                        write(sockets[my_monitor],&d1,sizeof(struct date));//tell it what virus we check for which id and the date of the request
                        read(sockets[my_monitor],&(ans),sizeof(int));//read answer
                        if(ans==1) printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATIONBEFORE TRAVEL DATE\n");
                        else if(ans==2)  printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                        else{
                            perror("tsekare tis domes sou");
                        }
                }
            }
            else{
               perror("wrong country\n"); 
            }
            TravelRequests* add=new TravelRequests(virusName,countryTo,d1,NULL,ans==2);
            Request_List=Request_List->insert(add);
            if(ans==2) acc++;//addd request to a request stack and count how many are accepted for the purposes of the logfile
            else if(ans<2) rej++;
        }
        else if (command.compare("/travelStats") == 0)
        {
            string line;
            getline(cin, line);
            stringstream lineStream(line);
            lineStream >> virusName;
            read_date(lineStream,&d1,true);
            read_date(lineStream,&d2,true);
            if (count_words(line) == 4){
                lineStream >> countryTo;
                Request_List->TravelStats(virusName, d1, d2,&countryTo);//lib.cpp
            }
            else Request_List->TravelStats(virusName, d1, d2,NULL);
        }
        else if (command.compare("/searchVaccinationStatus") == 0)
        {
            cin>>id;
            for(i=0;i<numMonitors;i++){
                int temp=command.length()+1;
                write(sockets[i],&(temp),sizeof(int));
                write(sockets[i],command.data(),temp);
                write(sockets[i],&id,sizeof(int));
                read(sockets[i],&temp,sizeof(int));
                printf("%d\n",temp);
                if(temp!=0){
                    message=new char[temp];
                    read(sockets[i],message,temp);
                    printf("%s\n",message);
                    delete message;
                }//give id we search for to the monitor and print the given list of vaccine info
            }
        }
        else if (command.compare("/addVaccinationRecords") == 0)
        {
            cin>>countryTo;
            int my_monitor=search_monitor_by_country( numMonitors,monitor_countries,monitor_country_count, countryTo);
            delete monitor_bloom_list[my_monitor];
            monitor_bloom_list[my_monitor]=NULL;//we empty the bloom list of the monitor thathandles the given country because we will read changed one 
            int temp=command.length()+1;
            write(sockets[my_monitor],&(temp),sizeof(int));
            write(sockets[my_monitor],command.data(),temp);
            string path=input_dir+"/"+countryTo;//make full path to subdirectory. its not like in project2
            temp=path.length()+1;
            write(sockets[my_monitor],&(temp),sizeof(int));
            write(sockets[my_monitor],path.data(),temp);
            int viruscount;
            read(sockets[my_monitor],&viruscount,sizeof(int));
            for(j=0;j<viruscount;j++){//this proceess is the same as above but for just one monitor
                int stringlen,bloombytes_read=0;
                read(sockets[my_monitor],&stringlen,sizeof(int));
                char *str=new char[stringlen];
                read(sockets[my_monitor],str,stringlen);
                int *array = new int[(bloomsize / (sizeof(int) * 8))];
                while( (bloombytes_read+=read(sockets[my_monitor],(uint8_t*)array+bloombytes_read,bufferSize)) <(bloomsize/8));
                monitor_bloom_list[my_monitor]=monitor_bloom_list[my_monitor]->addVirus(str,array);
                delete[] str;
            }
            cout<<"new bloom transfer complete\n";
        }
        else
        {
            cout << "wrong input\n";
        }
    }
    //clean up
    if(Request_List!=NULL) delete Request_List;
    for (i=0;i<numMonitors;i++){
        shutdown(sockets[i], 2);//so other students and i can reuse the port on this machine
        close(sockets[i]);
        delete monitor_bloom_list[i];
        delete[] monitor_countries[i];
    }
    delete[] monitor_bloom_list;  delete[] monitor_countries; delete[] monitor_country_count; delete[] countries;delete[] children;delete[] serverptr; delete[] server;delete[] sockets;
    while ((wait(&status)) > 0);
    exit(0);
}
