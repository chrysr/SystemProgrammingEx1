#ifndef HASTABLE_H
#define HASTABLE_H
#include "functions.h"

typedef class list list;
typedef class wallet wallet;
typedef class transaction transaction;

class bucketentry
{
public:
    list* transactions;
    wallet* wal;
    bucketentry(wallet* id);
    ~bucketentry();
    void settransnull();
};

class bucket
{
public:
    list* entries;
    bucket* next;
    int cursize; //in bytes
    bucket();
    ~bucket();
    void insertwallet(wallet* id);
    int inserttransaction(wallet* id,transaction* trans);
    void settransnull();
    list* gettransactions(wallet* id);

};

class hashtable
{
public:
    bucket* buckets;
    int bucketsnum;
    hashtable(int num,int max);
    ~hashtable();
    int shash(char* a);
    void insert(wallet* id);
    int transidexists(int id);
    int transidexists(char* id);
    int inserttransaction(wallet* id,transaction* trans);
    void settransnull();
    list* gettransactions(wallet* id);
};

#endif
