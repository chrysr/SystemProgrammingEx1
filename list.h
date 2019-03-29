#ifndef LIST_H
#define LIST_H
#include "functions.h"

class listnode
{
public:
    listnode* next;
    listnode* previous;
    void* content;
    listnode();

};


class list
{
public:
    listnode* first;
    listnode* last;
    int counter;
    list();
    void insert(void* item);
    int find(void* item);
    int findwal(char* id);
    int findbit(int id );
    void findandremove(void* item);
    void** getall();
    void del(int mode);

};


#endif
