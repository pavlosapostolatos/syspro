#include "includes.h"
extern int nodesCopied;
extern bool links;
bool same(char *tempath, char *tempath2,bool change){
    struct stat stats,stats2;
    if (isDirectoryExists(tempath) && !isExists(tempath2))//!!!!!!!!!
    {
        printf(" found dir %s \n",tempath);
        if(change){//changes to the file system happen only if change=true.else just return whether the files are same or not
        mkdir(tempath2,S_IRWXU | S_IRWXG | S_IRWXO);
        nodesCopied++;
        copydir(tempath,tempath2);
        }
        return false;
    }
    else if (isDirectoryExists(tempath) && isDirectoryExists(tempath2)){
        printf(" recoursing to %s , %s \n",tempath,tempath2); 
        ExamineAndCopy(tempath,tempath2,change);//here the modification will bve to recurse to those 2 same dictionaries to examine their files
        return true;
    }
    else if (isDirectoryExists(tempath) && !isDirectoryExists(tempath2)){
        printf(" replacing file %s with directory %s \n",tempath,tempath2); 
        if(change){
        remove(tempath2);/////remove also calls unlink so no need to check for symbolic link in my IFs
        nodesCopied++;
        mkdir(tempath2,S_IRWXU | S_IRWXG | S_IRWXO);
        copydir(tempath,tempath2);//like the cp program again like in line 42-49
        }
        return false;
    }
    else if (isSlinkExists(tempath) && !isExists(tempath2) && links)//!!!!!!!!!
    {
        printf(" found symbolic link %s \n",tempath);
        if(change){
            makeSlink(tempath,tempath2);
        }
        return false;
    }
    else if (isSlinkExists(tempath) && isDirectoryExists(tempath2) && links )//!!!!!!!!!
    {
        printf(" found symbolic link %s \n",tempath);
        if(change){
            assert(removedirectoryrecursively(tempath2));
            makeSlink(tempath,tempath2);
        }
        return false;
    }
    else if (isSlinkExists(tempath) && isSlinkExists(tempath2) && links )//!!!!!!!!!
    {
        //struct stat stats,stats2;
        stat(tempath, &stats);
        stat(tempath2, &stats2);//notice only here i use stat instead of lstat because stat gives us information not about the link itself like lstat but the file it points too if arguement is a symbolic link
        printf(" found 2 symbolic links %s %s \n",tempath,tempath2);
        if(stats.st_ino==stats2.st_ino) return true;//if the symbolink links point to the same inode and have the same names they are the same. no need to change them
        else if(change){
            unlink(tempath2);
            makeSlink(tempath,tempath2);
        }
        return false;
    }
    else if (isSlinkExists(tempath) && links)//!!!!!!!!!
    {//copy a link while a regular file exists wth the same name
        printf(" found symbolic link %s \n",tempath);///tempath2 is file
        if(change){
            remove(tempath2);
            makeSlink(tempath,tempath2);
        }
        return false;
    }
    else if(!isExists(tempath2)){///from now on these cases are when tempath is a regular file and we examine the type tempath2
        printf(" making file %s from %s \n",tempath2,tempath);
        if (change)
        copyFiles(tempath,tempath2);
        return false;
    }
    else if(isSlinkExists(tempath2)){
        printf("deleting symbolic link %s to make file %s\n",tempath2,tempath);
        if (change){
        unlink(tempath2);
        copyFiles(tempath,tempath2);
        }
        return false;
    }
    else if(isDirectoryExists(tempath2)){
        printf("deleting directory %s to make file %s\n",tempath2,tempath);
        if (change){
        assert(removedirectoryrecursively(tempath2));
        copyFiles(tempath,tempath2);
        }
        return false;
    }
    else if( isExists(tempath2)){
        lstat(tempath, &stats);lstat(tempath2, &stats2);
        if(stats.st_size!=stats2.st_size || (stats.st_mtime> stats2.st_mtime && change==true)){
            printf("both files exist but they arent same\n");
            if (change)
            copyFiles(tempath,tempath2);
            return false;
        }
        return true;
    }
    else
    {
        printf("uknown datatype\n");
    }
    exit(-1);
}
