#include "includes.h"
void attach_time(FILE *);
int main(int argc, char *argv[])
{
    //sleep(5);
    remove("eniaio.txt");//remove log from previous execution
    FILE *eniaio = fopen("eniaio.txt", "a");
    srand(time(NULL));
    char ylika[4] = "nkp"; //list of ingredients
    int saladcounter[3]={0,0,0};
    void *mem;              //the shared memory value. in the shared memory segment i only store the number of salads left to made so it's int.
    int numofSlds, mantime, val;
    if (argc != 5)
    {
        printf("wrong arguement number\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-n") == 0)
        {
            numofSlds = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            mantime = atof(argv[i + 1]);
        }
    } //read command line arguements
    sem_t *mysem = sem_open("chef", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (mysem != SEM_FAILED)
        printf("created new chef semaphore!\n");
    else if (errno == EEXIST)
    {
        printf("semaphore appears to exist already!\n");
        mysem = sem_open("chef", 0);
    }//chef creates his own semaphore or just opens it in case the ./chef didnt close normally  thus the semaphore didnt get closed as well. its for debugging
    long memid = shmget(IPC_PRIVATE, 10, 0666); //get shared memory key
    mem = (void *)shmat(memid, (void *)0, 0);    //attach value to it
    memcpy(mem, &numofSlds, sizeof(int));       //make it be the number of salads left to make
    memset(mem+ sizeof(int), 0, 3* sizeof(int));
    printf("press me when you have finished calling the saladmakers from another tty. memory id is: %ld \n", memid);
    getchar();                           //press any letter and then enter when the salamakers have been called
    sem_t *semn = sem_open("saladn", 0); // open the saladmaker semaphores so the chef can wake them up depending on when they have work
    sem_t *semk = sem_open("saladk", 0);
    sem_t *semp = sem_open("saladp", 0);
    sem_post(semn);//wake up the saladmakers from line 56 of their .c
    sem_post(semk);
    sem_post(semp);//they fall back asleep to begin production.check their .c
    while (numofSlds)//while we still have salads to make
    {
        char random_yliko = ylika[rand() % 3];//pick a random saladmakers you are gona give the appropriate ingerdients to
        if (random_yliko == 'n' && (*(int*)(mem + sizeof(int)) )==0 ){//wake him up to make him work
            attach_time(eniaio);fprintf(eniaio,"selecting ingredients p and k\n");
            attach_time(eniaio);fprintf(eniaio,"Notify saladmakerN\n");
            saladcounter[0]++;
            (*(int*)(mem + sizeof(int)) )=1;
            sem_post(semn);//saladmakers can be sem_posted multiple times even though he isnt asleep so that their sem value is increased by 1 
        }
        else if (random_yliko == 'k' && (*(int*)(mem + 2*sizeof(int)) )==0){//meaning sem_value is equal to the number of salads the chef has appointed to them 
            attach_time(eniaio);fprintf(eniaio,"selecting ingredients p and n\n");
            attach_time(eniaio);fprintf(eniaio,"Notify saladmakerK\n");
            saladcounter[1]++;
            (*(int*)(mem + 2*sizeof(int)) )=1;
            sem_post(semk);//as a result when line 63 in their .c executes they dont fall asleep if they have pending salads.
        }
        else if (random_yliko == 'p' && (*(int*)(mem + 3*sizeof(int)) )==0){
            attach_time(eniaio);fprintf(eniaio,"selecting ingredients k and n\n");
            attach_time(eniaio);fprintf(eniaio,"Notify saladmakerP\n");
            saladcounter[2]++;
            (*(int*)(mem + 3*sizeof(int)) )=1;
            sem_post(semp);
        }
        else
            continue;
        //sem_wait(mysem);//with this policy the ./chef doesnt have to sleep and waste time. he can continue to randomly pick ingredients to give
        numofSlds--;//thinking they will be used later. thus maximum concurency and parallelism is achieved
        memcpy(mem, &numofSlds, sizeof(int));//only the chef needs to write to the shared memory for this exercise
        fprintf(eniaio,"Man time for resting\n");
        sleep(mantime);//takes a brake
    }
    numofSlds--;
    memcpy(mem, &numofSlds, sizeof(int));//-1 value indicates we have finished production. terminate saladmakers when their pending salads(aka sem value) is 0
    sem_post(semn);//wake everyone up to close their semaphores and detach the shared memeory segment before the id gets relieved in line 90
    sem_post(semk);
    sem_post(semp);
    sem_wait(mysem);//wait until saladmakers 3 are complete and they wake you
    sem_wait(mysem);
    sem_wait(mysem);
    if (shmctl(memid, IPC_RMID, 0) == -1)
        printf("Removal\n");
    sem_close(mysem);
    sem_close(semn);
    sem_close(semk);
    sem_close(semp);
    sem_unlink("chef"); // remove it from system
    printf("saladmakerN made %d salads saladmakerK made %d saladmakerP made %d\n",saladcounter[0],saladcounter[1],saladcounter[2]);
    fclose(eniaio);
}

void attach_time(FILE *fp){
    time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  fprintf (fp, "%d:%d", timeinfo->tm_min,timeinfo->tm_sec );
}
