#ifndef TREE_H
#define TREE_H
#include "functions.h"

typedef class wallet wallet;
typedef class treenode treenode;
typedef class transaction transaction;

class treenode
{
public:
    wallet* walletid;
    int value;
    treenode* childl;
    treenode* childr;
    transaction* associatedwith;
    treenode(wallet* id,int value);
    ~treenode();
    int getbalance(wallet* who);
    void transact(char* transid,wallet* send,wallet* receive,int amount,time_t when,int * remaining,transaction* trans);
    void printbottomlevel();
    int insidenodes(list* lst=NULL);
    int unspent();
    void printalltransactions(int level=0,list* lst=NULL);
};

class bitcoin
{
public:
    int bitcoinid;
    treenode* root;
    bitcoin(int coinid, wallet* initial,int coinvalue);
    int getbalance(wallet* who);
    void transact(char* transid,wallet* send,wallet* receive,int amount,time_t when,int* remaining,transaction* trans);
    ~bitcoin();
    void printcoinparts();
    int gettransactionsnumber();
    int unspent();
    void printalltransactions();

};


#endif
