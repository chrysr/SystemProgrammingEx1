#include "list.h"

listnode::listnode()//double linked list lower level
{
    this->next=NULL;
    this->previous=NULL;
}




list::list()    //higher level
{
    this->counter=0;
    this->first=NULL;
    this->last=NULL;
}
void list::del(int mode)    //delete content with the appropriate casting (since list content is void*) or deletes the structure without the content
{
    listnode* tmp1=first;
    listnode* tmp2;
    while(tmp1!=NULL)
    {
        tmp2=tmp1->next;
        if(mode==1)
        {
            if(tmp1->content!=NULL)
                delete (bitcoin*)tmp1->content;

        }
        else if(mode==2)
        {

            if(tmp1->content!=NULL)
            delete (wallet*)tmp1->content;

        }
        else if(mode==3)
        {

            if(tmp1->content!=NULL)
            delete (bucketentry*)tmp1->content;

        }
        else if(mode==4)
        {

            if(tmp1->content!=NULL)
            delete (transaction*)tmp1->content;

        }
        else if(mode==5)
        {

            if(tmp1->content!=NULL)
            delete (treenode*)tmp1->content;


        }
        delete tmp1;
        tmp1=tmp2;
    }
    delete this;    //delete the list itself
}



void list::insert(void* item)
{
    if(this->counter==0)//if first item insert at first
    {
        this->first=new listnode;
        this->last=this->first;
    }
    else    //else insert at end
    {
        this->last->next=new listnode;
        this->last=this->last->next;
    }
    this->last->content=item;//increase counter after inserting item to list
    this->counter++;
}

int list::find(void* item)//returns one if pointer exists in list
{
    if(this->counter==0)
        return 0;
    listnode* tmp=this->first;
    while(tmp!=NULL)//go through the listnodes and look for the pointer
    {
        if(tmp->content==item)
            return 1;
        tmp=tmp->next;
    }
    return 0;
}
int list::findwal(char* id) //specifically for walletids
{
    if(this->counter==0)
        return 0;
    listnode* tmp=this->first;//go through the listnodes
    while(tmp!=NULL)
    {
        if(strcmp(((wallet*)tmp->content)->walletid,id)==0) //check if walletid exists
            return 1;
        tmp=tmp->next;
    }
    return 0;
}
int list::findbit(int id )//specifically for bitcoins
{
    if(this->counter==0)
        return 0;
    listnode* tmp=this->first;  //go through the listnodes
    while(tmp!=NULL)
    {
        if(((bitcoin*)tmp->content)->bitcoinid==id) //check if bitcoinid exists
            return 1;
        tmp=tmp->next;
    }
    return 0;
}
void list::findandremove(void* item)//looks for an item and if it finds it, it removes its listnodes
{
    if(this->counter==0)
        return;
    listnode* tmp=this->first;
    while(tmp!=NULL)
    {
        if(tmp->content==item)
        {
            if(tmp==this->first)
            {
                this->first=tmp->next;
            }
            if(tmp==this->last)
            {
                this->last=tmp->previous;
            }
            else
            {
                tmp->next=tmp->previous;
                tmp->previous=tmp->next;
            }
            tmp->content=NULL;
            delete tmp;
            this->counter--;
            if(this->counter==0)
                this->first=this->last=NULL;
            return;
        }
        tmp=tmp->next;
    }
}

void** list::getall()//returns an array of pointers with the objects that are contained in the list
{
    if(this->counter==0)
        return NULL;
    void** arr=new void*[counter];
    int i=0;
    listnode* node=this->first;
    while(node!=NULL)//go through the list and add each content entry
    {
        arr[i]=node->content;
        i++;
        node=node->next;
    }
    return arr;
}
