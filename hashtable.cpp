#include "hashtable.h"
int hashtablebucketmaxsize;

using namespace std;

bucketentry::bucketentry(wallet* id)//each bucketentry has a list of transactions and a wallet
{
    transactions=new list;
    wal=id;
}
bucketentry::~bucketentry() //delete transactions list and the listnodes' contents
{
    if(transactions!=NULL)
        transactions->del(4);
}
void bucketentry::settransnull()//delete the list without the contents
{
    transactions->del(0);
    transactions=NULL;
}
bucket::bucket()
{
    entries=new list;//create new list of bucketentries
    next=NULL;
    cursize=0;

}
bucket::~bucket()//delete list and contents
{
    entries->del(3);
    if(next!=NULL)//delete next bucket if there is one
        delete next;
}
void bucket::insertwallet(wallet* id)
{
    bucket* b= this;
    while(b->next!=NULL)
        b=b->next;              //go to the end of the list of buckets

    if(hashtablebucketmaxsize-b->cursize<sizeof(bucketentry))//if there is no space create new bucket and go to it
    {
        b->next=new bucket;
        b=b->next;
    }

    bucketentry* bentry=new bucketentry(id);        //make new entry and add it to the bucket
    b->entries->insert(bentry);
    b->cursize+=sizeof(bucketentry);

}
int bucket::inserttransaction(wallet* id,transaction* trans)
{
    bucket* b=this;
    while(b!=NULL)
    {
        bucketentry** buck=(bucketentry**)b->entries->getall(); //go through the list and look for the wallet id
        for(int i=0;i<b->entries->counter;i++)
        {
            if(buck[i]->wal==id)        //if found
            {
                buck[i]->transactions->insert(trans);       //insert the transactions
                delete[] buck;
                return 1;
            }
        }
        delete[] buck;
        b=b->next;
    }
}
void bucket::settransnull()//this is used so that we can delete the structure without double deleting the actual data (ex. wallets) when deleting the two hash tables
{
    bucket* b=this;
    while(b!=NULL)
    {
        bucketentry** buck=(bucketentry**)b->entries->getall();
        for(int i=0;i<b->entries->counter;i++)
        {
            buck[i]->settransnull();
        }
        delete[] buck;
        b=b->next;
    }
}
list* bucket::gettransactions(wallet* id)//go through the bucket list and look for the wallet id that we are looking for
{
    if(id==NULL)
        return NULL;
    bucket* b=this;
    list* lst;
    while(b!=NULL)
    {
        bucketentry** buck=(bucketentry**)b->entries->getall();
        for(int i=0;i<b->entries->counter;i++)
        {
            if(buck[i]->wal==id)
            {
                lst=buck[i]->transactions;
                delete[] buck;
                return lst;
            }
        }
        delete[] buck;
        b=b->next;
    }
    return NULL;
}


hashtable::hashtable(int num,int max)
{

    buckets=new bucket[num];        //create an array of buckets
    bucketsnum=num;                 //size of array
    if(hashtablebucketmaxsize!=max) //setting max allowed size in bucket if it has not been set
        hashtablebucketmaxsize=max;
}
hashtable::~hashtable()
{
    delete[] buckets;       //just delete the arrays
}
void hashtable::insert(wallet* id)
{
    buckets[(shash(id->walletid))%bucketsnum].insertwallet(id); //insert to the bucket pointed by the hash function
}

int hashtable::transidexists(char* id)  //checks if transactionid exists in the hashtable
{
    if(id==NULL)
        return 0;
    for(int i=0;i<bucketsnum;i++)   //go through the array
    {
        bucket* b=this->buckets;    //go through the bucket list
        while(b!=NULL)
        {
            bucketentry** w=(bucketentry**)buckets[i].entries->getall();    //go through the bucketentry list
            for(int j=0;j<buckets[i].entries->counter;j++)
            {
                bucketentry* bentry=w[j];
                transaction** trns=(transaction**)bentry->transactions->getall();   //go through the transactions list
                if(trns==NULL)
                    continue;
                for(int k=0;k<bentry->transactions->counter;k++)
                {
                    if(strcmp(id,trns[k]->transactionid)==0)    //if found return found
                    {
                        delete[] trns;
                        delete[] w;
                        return 1;
                    }
                }
                delete[] trns;
            }
            delete[] w;
            b=b->next;
        }
    }
    return 0;       //return not found
}

int hashtable::inserttransaction(wallet* id,transaction* trans) //insert transaction to the bucket pointed by hash function
{
    buckets[(shash(id->walletid))%bucketsnum].inserttransaction(id,trans);
}
void hashtable::settransnull()//set content to null in lower levels
{
    for(int i=0;i<bucketsnum;i++)
        buckets[i].settransnull();
}
list* hashtable::gettransactions(wallet* id)
{
    if(id==NULL)
        return NULL;
    return buckets[(shash(id->walletid))%bucketsnum].gettransactions(id);//transfers the work to the appropriate bucket
}
int hashtable::shash(char* a)//universal hash function
{
    int sum=0;
    int i=0;
    int tmp=0;
    while(a[i]!='\0')
    {
        tmp=(int)a[i];
        sum+=tmp;
        i++;
    }
    int Q=10007;
    int result = 0;
    int n;
    for(n=0;n<sum;n++)
    {
        if(result<0)
            result=result*-1;
        result = result * Q + n * n;

    }
    result *= Q;
    if(result<0)
        result=result*-1;
    return result;
}
