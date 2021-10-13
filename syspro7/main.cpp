#include "includes.hpp"
int bloomsize,skipsize=0;
bool sig_end=false;
long dead_child=0;
void proc_exit(int signo)
{
    int wstat;
    cout<<"into handler\n";
    dead_child = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
        if(dead_child ==-1){
            perror("no idea what happened with child\n");
            exit(-1);
        }
        else if (dead_child ==0){//nobody died
            printf("nobody died\n");
            return;
        }
        else
            printf ("%ld died \n", (long)dead_child);//this is it's pid_t
}

void log_file(int signo){
    sig_end=true;
    printf("LOG ME dad\n");
}//this will now enable the signal's function in the command loop
int main(int argc, char **argv)
{
    srand(time(NULL));
    //bool pipe_ready=false;
    sigset_t set1;
    sigfillset (&set1);
    sigdelset (&set1, SIGINT);sigdelset (&set1, SIGQUIT);sigdelset (&set1, SIGCHLD);//only allow the isgnals i care for
    sigprocmask ( SIG_SETMASK , & set1 , NULL );

    static struct sigaction act2 ;
    act2 . sa_handler = log_file ;
    sigfillset (&(act2.sa_mask));
    sigaction ( SIGINT , & act2 , NULL );
    sigaction ( SIGQUIT , & act2 , NULL );

    static struct sigaction act3 ;
    act3 . sa_handler = proc_exit ;
    sigfillset (&(act3.sa_mask));
    //act3.sa_flags=SA_NOCLDWAIT;
    sigaction ( SIGCHLD , & act3 , NULL );//set behavior for our signals of interest

    int numMonitors,bufferSize,i,status,country_count=0,j,acc=0,rej=0;
    string input_dir,command,countryFrom, countryTo,virusName;
    pid_t parent=getpid();
    if (argc!=9){
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
       else if (strcmp(argv[i], "-s") == 0)
        {
            bloomsize = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            input_dir=argv[i + 1];
        }
    }
    int **fd= new int*[numMonitors];
    pid_t* children=new pid_t[numMonitors];//store children pids
    for (i=0;i<numMonitors;i++){
        children[i]=getpid()+1+i;
        fd[i]=new int[2];//make 2 FIFOs for each child and create array carrying the file descriptors 
        if (( mkfifo ( string("fifo" + to_string(i)+ "in").c_str()  , 0666) < 0 )||( mkfifo (string("fifo" + to_string(i)+ "out").c_str() , 0666) < 0 )){
            if(EEXIST == errno) {
                perror ( "FIFOs exist already\n");
            }
            else
                perror ( " Error creating the named pipe " );
            exit (1);
        }
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
        cout<<monitor_country_count[i]<<endl;
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
    closedir(dir_ptr);
    bubbleSort(countries,country_count);//sort the name alphabetically
    string** monitor_countries=new string*[numMonitors];//with this array we know which countries a specific process takes care of
    int k=-1;
    mklvl(1,numMonitors);//does the forks.go to lib.cpp
    if (getpid()!=parent)//childs do th exec* while parent opens the FIFOs. childrent hen open their end as well
    execlp("./Monitor","./Monitor",(to_string(bufferSize)).c_str(),(to_string(bloomsize)).c_str(),input_dir.c_str(),"0",NULL);
    for (i=0;i<numMonitors;i++){
        if((fd[i][READ]=open (string("fifo" + to_string(i)+ "in").c_str(), O_RDONLY  , 0 )) <0 ||
        (fd[i][WRITE]=open (string("fifo" + to_string(i)+ "out").c_str(), O_WRONLY  , 0))<0 ){
            perror ( "FIFOs won't open");
            exit(1);
        }
    }
    for(i=0;i<numMonitors;i++){
        monitor_countries[i]=new  string[monitor_country_count[i]];//create as many columns for country names as the number of countries assigned to this process in monitor_country_count
        write(fd[i][WRITE],&(monitor_country_count[i]),sizeof(int));//inform the monitor how many countires it will read
        for(j=0;j<monitor_country_count[i];j++){
            monitor_countries[i][j]=countries[++k];//we assign countires from top to bottom alphabetically
            int temp=monitor_countries[i][j].length()+1;
            write(fd[i][WRITE],&(temp),sizeof(int));
            write(fd[i][WRITE],monitor_countries[i][j].data(),temp);//pass #monitor_country_count[i] country names
            //cout<<monitor_countries[i][j]<< temp <<endl;
        }
        //close(fd[i][WRITE]);
    }
    Viruses** monitor_bloom_list=new Viruses*[numMonitors];//the 2 programs use the same classes
    //sleep(1);
    for (i=0;i<numMonitors;i++){//make numMonitors lists of bloom filters.((viruses have NULL skip lists here))
        monitor_bloom_list[i]=NULL;
        int viruscount;
        read(fd[i][READ],&viruscount,sizeof(int));//learn how many viruses to read blooms for
        for(j=0;j<viruscount;j++){
            int stringlen,bloombytes_read=0;
            read(fd[i][READ],&stringlen,sizeof(int));
            char *str=new char[stringlen];
            read(fd[i][READ],str,stringlen);//read the name of the virus assigned to this list
            int *array = new int[(bloomsize / (sizeof(int) * 8))];//convert bits to int by dividing by 32
            while( (bloombytes_read+=read(fd[i][READ],(void*)array+bloombytes_read,bufferSize)) <(bloomsize/8));// cout<<bloombytes_read<<endl;
            monitor_bloom_list[i]=monitor_bloom_list[i]->addVirus(str,array);
            delete[] str;//keep reading until we have read the expected number of bytes then add that array to the list
        }
    }
    TravelRequests * Request_List=NULL;
    int dummy=0;
    while (1)
    {//take commands until /exit
        dummy++;
        if(dead_child!=0){
            cout<<"making new child\n";
            int my_monitor;
            for (int i=0;i<numMonitors;i++)
                if((long)children[i] == dead_child)
                    my_monitor=i;//my_monitor is its place in my arrays
            close(fd[my_monitor][0]);
            close(fd[my_monitor][1]);
            remove(string("fifo" + to_string(children[my_monitor]-(long)getpid()-1)+ "in").c_str());
            remove(string("fifo" + to_string(children[my_monitor]-(long)getpid()-1)+ "out").c_str());//removing the pipes to the dead monitor and open new ones
            long newchild;
            if((newchild=fork())==0){
                sleep(1);
                execlp("./Monitor","./Monitor",(to_string(bufferSize)).c_str(),(to_string(bloomsize)).c_str(),input_dir.c_str(),"1",NULL);
            }//"1" arguement means this monitor is a duplicate
            mkfifo ( string("fifo" + to_string(newchild-(long)getpid()-1)+ "in").c_str()  , 0666);
            mkfifo ( string("fifo" + to_string(newchild-(long)getpid()-1)+ "out").c_str()  , 0666);
            fd[my_monitor][READ]=open (string("fifo" + to_string(newchild-(long)getpid()-1)+ "in").c_str(), O_RDONLY  , 0 );
            fd[my_monitor][WRITE]=open (string("fifo" + to_string(newchild-(long)getpid()-1)+ "out").c_str(), O_WRONLY  , 0 );//replace the dead file desctiptors in the array with the new ones so we can keep using the same code
            write(fd[my_monitor][WRITE],&(monitor_country_count[my_monitor]),sizeof(int));
            for(j=0;j<monitor_country_count[my_monitor];j++){
                int temp=monitor_countries[my_monitor][j].length()+1;
                write(fd[my_monitor][WRITE],&(temp),sizeof(int));
                write(fd[my_monitor][WRITE],monitor_countries[my_monitor][j].data(),temp);
            }//once again inform it what countries it handles
            children[my_monitor]=newchild;
            dead_child=0;
        }//notice we dont transfer blooms again. this is because we already have them and the new monitor needs to reproduce them
//the "1" arguement tells the monitor to not rewrite the bloom to the pipe and break the command communication protocol of this hile (1) loop
        if(!sig_end){
        cout<<"give command\n";
        cin >> command;
        }
        else{
            ofstream log_file(string("logfile."+ to_string((long) (getpid()) )+ ".txt").c_str());
            for ( int i = 0; i < country_count; i++){
                log_file<<countries[i]<<endl;
            }
            log_file<<acc+rej<<endl;
            log_file<<acc<<endl;
            log_file<<rej<<endl;
            log_file.close(); 
            sig_end=false;//!!!!put at end of loop for sigchild
            command="/exit";  //create the logfile and terminate the program normally as if /exit was given by user      
        }
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
            sigfillset (&set1);
            sigprocmask ( SIG_SETMASK , & set1 , NULL );//no need for sigchild handling.we are ending
            for(i=0;i<numMonitors;i++){
                kill(getpid()+1+i,SIGKILL);
            }
            //if(sig_end==false)
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
                        write(fd[my_monitor][WRITE],&(temp),sizeof(int));
                        write(fd[my_monitor][WRITE],command.data(),temp);
                        write(fd[my_monitor][WRITE],&id,sizeof(int));
                        temp=virusName.length()+1;
                        write(fd[my_monitor][WRITE],&(temp),sizeof(int));
                        write(fd[my_monitor][WRITE],virusName.data(),temp);
                        write(fd[my_monitor][WRITE],&d1,sizeof(struct date));//tell it what virus we check for which id and the dta eof the request
                        read(fd[my_monitor][READ],&(ans),sizeof(int));//read answer
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
                write(fd[i][WRITE],&(temp),sizeof(int));
                write(fd[i][WRITE],command.data(),temp);
                write(fd[i][WRITE],&id,sizeof(int));
                read(fd[i][READ],&temp,sizeof(int));
                printf("%d\n",temp);
                if(temp!=0){
                    message=new char[temp];
                    read(fd[i][READ],message,temp);
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
            write(fd[my_monitor][WRITE],&(temp),sizeof(int));
            write(fd[my_monitor][WRITE],command.data(),temp);
            temp=countryTo.length()+1;
            write(fd[my_monitor][WRITE],&(temp),sizeof(int));
            write(fd[my_monitor][WRITE],countryTo.data(),temp);
            int viruscount;
            read(fd[my_monitor][READ],&viruscount,sizeof(int));
            for(j=0;j<viruscount;j++){//this proceess is the same as above but for just one monitor
                int stringlen,bloombytes_read=0;
                read(fd[my_monitor][READ],&stringlen,sizeof(int));
                char *str=new char[stringlen];
                read(fd[my_monitor][READ],str,stringlen);
                int *array = new int[(bloomsize / (sizeof(int) * 8))];
                while( (bloombytes_read+=read(fd[my_monitor][READ],(void*)array+bloombytes_read,bufferSize)) <(bloomsize/8));
                monitor_bloom_list[my_monitor]=monitor_bloom_list[my_monitor]->addVirus(str,array);
                delete[] str;
            }
            cout<<"new bloom transfer complete\n";
        }
        else
        {
            cout << "wrong input\n";
        }
        // if(dummy==3){
        //     kill((long)getpid()+1 ,SIGKILL);
        //     sleep(1);
        // }
        // if(dummy==4) kill((long)getpid() ,SIGINT);
    }
    //clean up
    for (i=0;i<numMonitors;i++){
        close(fd[i][0]);
        close(fd[i][1]);
        remove(string("fifo" + to_string(i)+ "in").c_str());
        remove(string("fifo" + to_string(i)+ "out").c_str());// i also want to remove the FIFOs from the OS 
        delete[] fd[i];
        delete monitor_bloom_list[i];
        delete[] monitor_countries[i];
    }
    delete[] fd; delete[] monitor_bloom_list;  delete[] monitor_countries; delete[] monitor_country_count; delete[] countries;delete[] children;
    if(Request_List!=NULL) delete Request_List;
}
