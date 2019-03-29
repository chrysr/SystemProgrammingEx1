#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <iostream>
#include <cstdio>
#include "list.h"
#include "hashtable.h"
#include "tree.h"
#include <cstring>
#include <cstdlib>
#include <ctime>


#define red "\033[1;31m"
#define yellow "\033[01;33m"
#define cyan "\033[1;36m"
#define reset "\033[0m"
#define green "\033[1;32m"



typedef class hashtable hashtable;
typedef class bitcoin bitcoin;
typedef class transaction transaction;

time_t stringtodate(char* date,char* tim);
void printtime(time_t t);
int readdel(char* dest,const char del1,const char del2);
void printcoinowner(list* btc);
void printbalances(list* wallets);
int validwallet(char* str,list* wallets);
void getvalidtransid(hashtable* ht,char *id,int size);
wallet* walletfromstr(list* lst, char* str);
bitcoin* bitcoinfromstr(list* lst, char* str);
transaction* maketransaction(char* transid,wallet* send,wallet* receive, int amount,time_t when);
void newtransaction(list* walletlist,char args[10][100],hashtable* shash,hashtable* rhash,int mode=0);
int requesttransaction(list* walletlist,char* buffer,hashtable* shash,hashtable* rhash,int mode=0);
int find(list* wlletlist,hashtable* rhash,char* buffer);
void compareandprinttrans(list* lst,struct tm timeinfo1,struct tm timeinfo2,int fields);
void walletstatus(list* walletlist, char* buffer);
void bitcoinstatus(list* bitcoinlist, char* buffer,int value);
void tracecoin(list* bitcoinlist,char* buffer);
class wallet
{
public:
    char* walletid;
    list* bitcoinlist;
    wallet(char* str);
    ~wallet();
    int getbalance();
};

class transaction
{
public:
    char* transactionid;
    wallet* sender;
    wallet* receiver;
    int amount;
    time_t when;
    list* parts; //treenode list with parts of the same transaction
    transaction(wallet* send,wallet* receive,int usd, char* id, time_t w=0);
    ~transaction();
    void readout();
    void simplereadout();

};


#endif
