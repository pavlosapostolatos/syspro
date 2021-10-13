#include "includes.hpp"
int bloomsize,skipsize=0;//in exercise 1 my skipsize was initialised to log10 of number of imputs 
bool sig_end=false;
void log_file(int signo){
    sig_end=true;
    printf("LOG ME \n");
}
int main(int argc, char **argv){
    // cout<<"in"<<endl;
    //bool pipe_ready=false;
    sigset_t set1;
    sigfillset (&set1);
    sigdelset (&set1, SIGINT);sigdelset (&set1, SIGQUIT);
    //sigprocmask (SIG_SETMASK,&set1, NULL );

    static struct sigaction act2 ;
    act2 . sa_handler = log_file ;
    sigfillset (&(act2.sa_mask));
    sigaction ( SIGINT , & act2 , NULL );
    sigaction ( SIGQUIT , & act2 , NULL );

    int old_stdout = dup(1);
    bloomsize=atoi(argv[2]);
    srand(time(NULL));
    int i,bufferSize=atoi(argv[1]),status,j,country_count=0,acc=0,rej=0;
    int fdout=open (string("fifo" + to_string(getpid()-getppid()-1 )+ "in").c_str(), O_WRONLY , 0);
    int fdin=open (string("fifo" + to_string(getpid()-getppid()-1 )+ "out").c_str(), O_RDONLY  , 0);//open opposite end of pipes
    string input_dir=argv[3];
    bool is_duplicate=(strcmp(argv[4],"1")==0);
    read(fdin,&country_count,sizeof(int));
    fclose(stdout);//close stdout so myproject 1 code doesnt print irrelevant stuff on insert()
    string* countries= new string[country_count];
    for ( i = 0; i < country_count; i++)
    {
        int stringlen;
        read(fdin,&stringlen,sizeof(int));
        char *str=new char[stringlen];
        read(fdin,str,stringlen);
        countries[i]=str;//read what countries it handles
    }
    cout<<string( input_dir + '/' + countries[0] + '/' + countries[0] + '-' + to_string(1) + ".txt").c_str()<<endl;
    for(i=0;i<country_count;i++){
        for(j=1;access(string( input_dir + '/' + countries[i] + '/' + countries[i] + '-' + to_string(j) + ".txt").c_str(), F_OK ) == 0;j++ ){
            ifstream Myinput(string( input_dir + '/' + countries[i] + '/'+ countries[i] + '-' + to_string(j) + ".txt").c_str());
            while (Myinput)
            {
                char c;
                Myinput.get(c);
                if (c == '\n')
                    skipsize++;//find how many lines/entires we have
            }
            Myinput.close();
        }
    }//my code from project 1 requires i get the total number of records so i can initalize the number of levels of the skiplist to the log(#records)
    cout<<skipsize<<" "<<bloomsize<<" "<<bufferSize<<endl;
    skipsize = log10(skipsize);
    Record *myrecord = new Record();
    for(i=0;i<country_count;i++){
        for(j=1;access( string( input_dir + '/' + countries[i] + '/' + countries[i] + '-' + to_string(j) + ".txt").c_str(), F_OK ) == 0;j++ ){
            cout<<i<<" "<<j<<endl;
            ifstream Myinput(string( input_dir + '/' + countries[i] + '/' +  countries[i] + '-' + to_string(j) + ".txt").c_str());
            while (Myinput.peek() != EOF)
            {
                citizen *a = read_citizen(Myinput,countries[i]);
                if(a!=NULL){//empty line
                myrecord->insert_in_record(a);
                }
            }
            Myinput.close();
        }
        ofstream counter(string( input_dir + '/' + countries[i] + "/counter.txt").c_str());
        counter<<j;
        counter.close();//cretae the temporary counter txt that tells us how many txts have been read in this folder. useful for /addVaccinationRecords 
    }
    stdout = fdopen(old_stdout, "w");//reope stdout
    Viruses* viruslist;
    int viruscount=0;
    for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next){
        viruscount++;
    }//count the viruses detected. its not 100% certain all children will handle the same viruses
    printf("viruscount of monitor is %d\n",viruscount);
    if(!is_duplicate){
    write(fdout,&viruscount,sizeof(int));
    for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next){
        int temp=viruslist->name.length()+1;
        int bloombytes_written=0;
        write(fdout,&temp,sizeof(int));
        write(fdout,viruslist->name.data(),temp);
        int * array=viruslist->bloom->array;//this mirrors the read of bloom in main.cpp
        while( (bloombytes_written+=write(fdout,(void*)array+bloombytes_written,bufferSize)) <(bloomsize/8)); //printf("%d\n",bloombytes_written);
    }
    }
    while (1)
    {
        if(sig_end){
            ofstream log_file(string("logfile."+ to_string( (long) (getpid()) )+ ".txt").c_str());
            for ( int i = 0; i < country_count; i++){
                log_file<<countries[i]<<endl;
            }
            log_file<<acc+rej<<endl;
            log_file<<acc<<endl;
            log_file<<rej<<endl;
            log_file.close(); 
            sig_end=false; 
        }
        struct date d1, d2;
        printf("waiting\n");
        // while(!pipe_ready){
        // }
        //printf("pipe_ready\n");
        int command_length,id,temp, stringlen;
        read(fdin,&command_length,sizeof(int));
        char *str=new char[command_length];
        read(fdin,str,command_length);
        if (strcmp(str,"/travelRequest") == 0){
            read(fdin,&id,sizeof(int));
            read(fdin,&temp,sizeof(int));
            delete str;
            str =new char[temp];
            read(fdin,str,temp);
            read(fdin,&d1,sizeof(struct date));
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
            write(fdout,&ans,sizeof(int));
            //kill(getppid(),SIGUSR2);
        }
        else if (strcmp(str,"/searchVaccinationStatus") == 0){
            //printf("searchVaccinationStatus\n");
            read(fdin,&id,sizeof(int));
            if(myrecord->people->raw_search_in_data(id)){
            string report =myrecord->viruses->vaccineStatus(id);
            //printf("%s\n",report.data());
            temp=report.length()+1;
            write(fdout,&temp,sizeof(int));
            write(fdout,report.data(),temp);
            }
            else{
                temp=0;
                write(fdout,&temp,sizeof(int));
            }
            
            //kill(getppid(),SIGUSR2);
        }
        else if (strcmp(str,"/addVaccinationRecords") == 0){
            read(fdin,&temp,sizeof(int));
            delete str;
            str =new char[temp];//countryto
            read(fdin,str,temp);
            ifstream counter(string(input_dir + '/' + str + "/counter.txt").c_str());
            int cont;
            counter>>cont;
            counter.close();
            for(j=cont;access( string( input_dir + '/' + str + '/' + str + '-' + to_string(j) + ".txt").c_str(), F_OK ) == 0;j++ ){
            cout<<i<<" "<<j<<endl;
            ifstream Myinput(string( input_dir + '/' + str + '/' +  str + '-' + to_string(j) + ".txt").c_str());
            while (Myinput.peek() != EOF)
            {
                //skipsize++;
                citizen *a = read_citizen(Myinput,str);
                if(a!=NULL){//empty line
                //a->printcitizen();
                myrecord->insert_in_record(a);
                }
            }//add new entries to record
            Myinput.close();
            }
            ofstream counterout(string( input_dir + '/' + str + "/counter.txt").c_str());//inform counter.txt in case of new addVaccinationRecords order
            counterout<<j;
            counterout.close();
            int viruscount=0;
            for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next)
                viruscount++;
            printf("viruscount is %d\n",viruscount);
            write(fdout,&viruscount,sizeof(int));
            for(viruslist=myrecord->viruses;viruslist!=NULL;viruslist=viruslist->next){//pass updated bloom filter
                int temp=viruslist->name.length()+1;
                int bloombytes_written=0;
                write(fdout,&temp,sizeof(int));
                write(fdout,viruslist->name.data(),temp);
                int * array=viruslist->bloom->array;
                while( (bloombytes_written+=write(fdout,(void*)array+bloombytes_written,bufferSize)) <(bloomsize/8)); //printf("%d\n",bloombytes_written);
            }
        }
        else{
            perror("wrong command in pipe\n");
            exit(1);
        }
            //close(fd[i][WRITE]);
        kill(getpid(),SIGQUIT);
    }
    //no need for clean up because sigkill ends the monitors
}