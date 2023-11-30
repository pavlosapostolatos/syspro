#include "includes.h"
void attach_time(FILE *,char);
int main(int argc, char *argv[])
{
    srand(time(NULL));
    FILE *fp,*eniaio = fopen("eniaio.txt", "a");
    double t1, t2, random_sleep;
    long shmid;
    char yliko;
    void *mem;
    int stop = -1,val;
    if (argc != 9)
    {
        printf("wrong arguement number\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-t1") == 0)
        {
            t1 = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-t2") == 0)
        {
            t2 = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            shmid = atol(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-y") == 0)
        {
            yliko = argv[i + 1][0];
        }
    }
    random_sleep = (double)rand() / RAND_MAX * (t2 - t1) + t1;//pick double between in the given space(t1-t2)
    sem_t *chefsem = sem_open("chef", 0);
    char semname[7];
    strcpy(semname, "salad");
    semname[5] = yliko;
    semname[6] = '\0';
    char filename[11];
    strcpy(filename, semname);
    strcat(filename, ".txt");
    remove(filename);//remove log from previous execution
    fp = fopen(filename, "a");
    sem_t *sem = sem_open(semname, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);// each saladmakers creates anbd closes his own semaphore and txt log file
    if (sem != SEM_FAILED)
        fprintf(fp, "created new semaphore!\n");
    else if (errno == EEXIST)
    {
        fprintf(fp, "semaphore appears to exist already!\n");
        sem = sem_open(semname, 0);
    }
    sem_wait(sem);//wait for sem to announce the start of production in line 47
    fprintf(fp, "ready to produce\n");
    attach_time(eniaio,yliko);fprintf(eniaio, "ready to produce\n");
    mem = (int *)shmat(shmid, (void *)0, 0);//attach the shared memeory to your process
    while (1)
    {
        attach_time(eniaio,yliko);fprintf(eniaio, "Waiting for ingredients\n");
        sem_wait(sem);//wait for chef to give me ingredients in line  52 of chef.c
        fprintf(fp, "i am back\n");
        attach_time(eniaio,yliko);fprintf(eniaio, "Got ingredients so i Start making salad\n");
        sem_getvalue(sem, &val);
        if (memcmp(mem, &stop, sizeof(int)) == 0 && val == 0)// the saladmakers only need to read the value of the shared memory and the chef changes it 
            break;//stop production when the chef has given all ingredients to make the required salads and this specific saladmakers doesnt owe any pending salads
        sleep(random_sleep);//cut the ingredients
        attach_time(eniaio,yliko);fprintf(eniaio, "End making salad\n");
        fprintf(fp, "salad made\n");
        if (yliko == 'n') (*(int*)(mem + sizeof(int)) )=0;
        else if (yliko == 'k') (*(int*)(mem + 2*sizeof(int)) )=0;
        else if (yliko == 'p') (*(int*)(mem + 3*sizeof(int)) )=0;
    }
    if (shmdt((void *)mem) == -1)//and detach the shared memeory segment from this process
        printf("Removal\n");
    sem_close(sem);      // close the sem
    sem_unlink(semname); // remove it from system
    fclose(fp);
    fclose(eniaio);
    sem_post(chefsem);
    sem_close(chefsem);
}

void attach_time(FILE *fp, char yliko){
    time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  fprintf (fp, "%d:%d[saladmaker%c]", timeinfo->tm_min,timeinfo->tm_sec,yliko );
}
