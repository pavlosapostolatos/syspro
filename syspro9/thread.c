#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define POOL_SIZE 6
typedef struct {
int data[POOL_SIZE];
int start;
int end;
int count;
} pool_t;
int num_of_items = 15;
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
void initialize(pool_t * pool) {
pool->start = 0;
pool->end = -1;
pool->count = 0;
}

void place(pool_t * pool, int data) {
pthread_mutex_lock(&mtx);
while (pool->count >= POOL_SIZE) {
printf(">> Found Buffer Full \n");
pthread_cond_wait(&cond_nonfull, &mtx);
}
pool->end = (pool->end + 1) % POOL_SIZE;
pool->data[pool->end] = data;
pool->count++;
pthread_mutex_unlock(&mtx);
}

int obtain(pool_t * pool) {
int data = 0;
pthread_mutex_lock(&mtx);
while (pool->count <= 0) {
printf(">> Found Buffer Empty \n");
if(num_of_items <=0){
    printf("happened\n");
    pthread_mutex_unlock(&mtx);
    return 0;
    //pthread_exit(0);
    
}
pthread_cond_wait(&cond_nonempty, &mtx);
}
data = pool->data[pool->start];
pool->start = (pool->start + 1) % POOL_SIZE;
pool->count--;
pthread_mutex_unlock(&mtx);
return data;
}

void * producer(void * ptr)
{
    pool_t* pool= (pool_t*)ptr;
pthread_mutex_lock(&mtx);
while (num_of_items > 0) {
int temp=num_of_items;
num_of_items--;
pthread_mutex_unlock(&mtx);
place(pool, temp);
printf("producer: %ld: %d\n",pthread_self(), temp);
pthread_mutex_lock(&mtx);
pthread_mutex_unlock(&mtx);
pthread_cond_broadcast(&cond_nonempty);
usleep(0);
pthread_mutex_lock(&mtx);
}printf("producer exiting\n");
pthread_mutex_unlock(&mtx);
pthread_exit(0);
}

void * consumer(void * ptr)
{

pthread_mutex_lock(&mtx);
    pool_t* pool= (pool_t*)ptr;
while (num_of_items > 0 || pool->count > 0) {//must protecc.lock
pthread_mutex_unlock(&mtx);
printf("consumer %ld: %d\n",pthread_self(), obtain(pool));
pthread_cond_broadcast(&cond_nonfull);
//usleep(30);
pthread_mutex_lock(&mtx);
}
printf("consumer %ld exiting\n",pthread_self());
pthread_mutex_unlock(&mtx);
pthread_exit(0);
}

int main(){
pool_t pool;
pthread_t cons, prod;
initialize(&pool);
pthread_mutex_init(&mtx, 0);
pthread_cond_init(&cond_nonempty, 0);
pthread_cond_init(&cond_nonfull, 0);
pthread_create(&cons, 0, consumer,& pool);
pthread_create(&cons, 0, consumer,& pool);
pthread_create(&cons, 0, consumer,& pool);
pthread_create(&prod, 0, producer,& pool);
pthread_create(&prod, 0, producer,& pool);

printf("done\n");
pthread_join(prod, 0);printf("done\n");

pthread_join(prod, 0);printf("done\n");
pthread_join(cons, 0);printf("done\n");
pthread_join(cons, 0);printf("done\n");pthread_join(cons, 0);printf("done\n");
pthread_cond_destroy(&cond_nonempty);
pthread_cond_destroy(&cond_nonfull);
pthread_mutex_destroy(&mtx);
return 0;
}
