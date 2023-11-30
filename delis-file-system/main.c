#include "includes.h"
int nodesFound = 0, nodesCopied = 0, bytesWritten = 0,nodesDeleted=0;
bool  links = false;
int main(int argc, char *argv[])
{
    int old_stdout = dup(1);
    bool verbose = false, deleted = false;// read_orgin = false;
    char *origindir=NULL, *destdir=NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = true;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            deleted = true; //d_ino
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            links = true;
        }
        else if (!origindir)
        {
            origindir = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
            strcpy(origindir, argv[i]);
            //read_orgin = true;
        }
        else
        {
            destdir = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
            strcpy(destdir, argv[i]);
        }
    }
    clock_t begin = clock();
    printf("%s %s \n",origindir,destdir);
    if (isDirectoryExists(origindir) == false)
    {
        printf("origin given isn't a directory\n");
        return -1;
    }
    if(!verbose) fclose(stdout);
    if (isDirectoryExists(destdir) == false)
    {
        printf("created directory %s\n", destdir);
        mkdir(destdir,S_IRWXU | S_IRWXG | S_IRWXO);//make it and put all contents of origindir into it
        nodesCopied++;
        nodesFound++;
        copydir(origindir, destdir);
    }
    else
    {
        ExamineAndCopy(origindir,destdir,true);//compare all contents of destdir with origindir's
    }
    if(deleted){
        ExamineAndCopy(destdir,origindir,false);
    }
    stdout = fdopen(old_stdout, "w"); 
    clock_t end = clock();
    printf("found %d nodes copied %d and wrote %d bytes in %lf seconds\n",nodesFound, nodesCopied, bytesWritten,(double)(end - begin) / CLOCKS_PER_SEC);
    printf("attention -d flag refinds the inodes.thats why in with -d you may see  nodesFound>nodesCopied\n");
    printf("deleted %d nodes\n",nodesDeleted);
}

bool isExists(const char *path){//inode with name=path exists or not
    struct stat stats;
    int err =lstat(path, &stats);
    if(-1 == err) {
    if(ENOENT == errno) {
        return false;
    }
    }
    return true;
}

bool isDirectoryExists(const char *path)
{
    struct stat stats;

    int err =lstat(path, &stats);
    if(-1 == err) {
    if(ENOENT == errno) {
        return false;
    }
    }
    // Check for file existence
    //printf("%d %d %d\n",stats.st_mode,stats.st_mode & S_IFMT,S_IFDIR);
    if ((stats.st_mode & S_IFMT) == S_IFDIR )
        return true;

    return false;
}

bool isSlinkExists(const char *path)
{
    struct stat stats;

    int err =lstat(path, &stats);
    if(-1 == err) {
    if(ENOENT == errno) {
        return false;
    }
    }
    // Check for file existence
    //printf("%d %d %d\n",stats.st_mode,stats.st_mode & S_IFMT,S_IFDIR);
    if ((stats.st_mode & S_IFMT) == S_IFLNK )
        return true;

    return false;
}

void copydir(char *oldDir, char *newDir)
{
    char tempath[BUFFSIZE],tempath2[BUFFSIZE];
    struct stat stats;
    DIR * dir_ptr ;
    struct dirent *direntp ;
    if ((dir_ptr = opendir(oldDir)) == NULL)
        fprintf(stderr, " cannot open %s \n ", oldDir);
    else
    {
        printf("opened %s \n ", oldDir);
        while ((direntp = readdir(dir_ptr)) != NULL){
             if ( !strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..") ) continue;//this keeps from heading to present or the home directory again and again
            //printf(" inode %d of the entry %s \n",(int)(direntp->d_ino), direntp->d_name);
            nodesFound++;
            strcpy(tempath,oldDir);
            strcat(tempath,"/");//we are forming the unix filesystem path that ends with the name of the file we examining
            strcat(tempath, direntp->d_name);//strcat'ing is like doing cd
            strcpy(tempath2,newDir);
            strcat(tempath2,"/");
            strcat(tempath2, direntp->d_name);
            lstat(tempath, &stats);
            if (isDirectoryExists(tempath))
            {
            printf(" found dir %s \n",tempath);
            mkdir(tempath2,S_IRWXU | S_IRWXG | S_IRWXO);
            nodesCopied++;
            copydir(tempath,tempath2);//recursing to next directory
            }
            else if(isSlinkExists(tempath)){
               	makeSlink(tempath,tempath2);
            }
            else{
                printf(" found files %s %s \n",tempath,tempath2);
                copyFiles(tempath,tempath2);
            }
        }
        closedir(dir_ptr);
    }
}



void copyFiles(char *oldDir, char *newDir){//taken from recitation4sources
    nodesCopied++;
    printf("came in\n");
    int filedest,infile,nread;
    printf("got %s %s\n",oldDir ,newDir);
    assert ( (infile=open(oldDir,O_RDONLY)) != -1 );
    assert ( (filedest = open (newDir , O_WRONLY | O_CREAT | O_TRUNC , 0644)) != -1 );//create it if it doesnt exist trunicate it if it does
    void* buffer=malloc(BUFFSIZE);//bytes are transferred from 1 file to another. as raw data(void*)
    while ( (nread=read(infile, buffer, BUFFSIZE) ) > 0 ){
		if ( write(filedest,buffer,nread) < nread ){
			exit(-3);
		}
        bytesWritten+=nread;
	}
	close(infile); close(filedest);free(buffer);
    
}

void makeSlink(char *oldlink, char *newlink){
    nodesCopied++;
    char target[BUFFSIZE],temp[BUFFSIZE];
    int size =readlink(oldlink,target,BUFFSIZE);//read to which file the link points to and make a 2nd symbolic link to the same file
    target[size]='\0';
    // strcpy(temp,oldlink);
    // strcat(temp,"/");
    // strcat(temp,target);
    printf("!!!!!!!!!! %s !!!!!!!!!!\n",temp);
    symlink(target,newlink);
    struct stat stats;
    lstat(newlink,&stats);
    bytesWritten+=stats.st_size;   
}

void ExamineAndCopy(char *oldDir, char *newDir,bool change){
    char tempath[BUFFSIZE],tempath2[BUFFSIZE];
    struct stat stats;
    DIR * dir_ptr ;
    struct dirent *direntp ;
    if ((dir_ptr = opendir(oldDir)) == NULL)
        fprintf(stderr, " cannot open %s \n ", oldDir);
    else
    {
        printf("opened %s \n ", oldDir);
        while ((direntp = readdir(dir_ptr)) != NULL){
             if ( !strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..") ) continue;
            //printf(" inode %d of the entry %s \n",(int)(direntp->d_ino), direntp->d_name);
            nodesFound++;
            strcpy(tempath,oldDir);
            strcat(tempath,"/");
            strcat(tempath, direntp->d_name);
            strcpy(tempath2,newDir);
            strcat(tempath2,"/");
            strcat(tempath2, direntp->d_name);
            lstat(tempath, &stats);
            if (!same(tempath,tempath2,change) && !change){//for -d flag which makes change==false is something isnt the same in the old directory we delete it from the clone directory
                if (isDirectoryExists(tempath)){
                    assert(removedirectoryrecursively(tempath));
                }
                else if(isSlinkExists(tempath)){
               	    unlink(tempath);
                    nodesDeleted++;
                }
                else{             
                    remove(tempath);
                    nodesDeleted++;
                }
            }
        }
        closedir(dir_ptr);
    }
}




int removedirectoryrecursively(const char *dirname)
{
    DIR *dir;
    struct dirent *entry;
    char path[BUFFSIZE];

    if (path == NULL) {
        fprintf(stderr, "Out of memory error\n");
        return 0;
    }
    dir = opendir(dirname);
    if (dir == NULL) {
        perror("Error opendir()");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            nodesFound++;
            snprintf(path, (size_t) BUFFSIZE, "%s/%s", dirname, entry->d_name);
            if (entry->d_type == DT_DIR) {
                removedirectoryrecursively(path);
            }
            printf(" Deleting: %s\n", path);
            nodesDeleted++;
            remove(path);
        }

    }
    closedir(dir);
	rmdir(dirname);
    nodesDeleted++;
    printf(" Deleting: %s\n", dirname);

    return 1;
}
