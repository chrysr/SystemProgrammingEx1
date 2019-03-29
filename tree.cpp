#include "tree.h"

using namespace std;

treenode::treenode(wallet* id,int value)//contructor
{
    walletid=id;
    this->value=value;
    childl=NULL;
    childr=NULL;
    associatedwith=NULL;
}
treenode::~treenode()//if not null delete children
{
    if(childl!=NULL)
        delete childl;
    if(childr!=NULL)
        delete childr;
}
int treenode::getbalance(wallet* who)
{
    int balance=0;

    if(childr!=NULL&&childl!=NULL)//serach the tree recursively
    {
        balance+=childr->getbalance(who);
        balance+=childl->getbalance(who);

    }
    else
    {
        if(walletid==who)
            balance+=value; //if it is who we are looking for ad the value to the current balance
    }

    return balance;

}
void treenode::transact(char* transid,wallet* send,wallet* receive,int amount,time_t when,int * remaining,transaction* trans)
{
    if(*remaining<=0)
        return;
    if(childl!=NULL&&childr!=NULL)
    {
        childl->transact(transid,send,receive,amount,when,remaining,trans);
        childr->transact(transid,send,receive,amount,when,remaining,trans);//go to the bottom of the tree recursively
    }
    else
    {
        if(walletid==send)
        {
            if(*remaining<=0)
                return;
            if(value>0)//subtruct as much as we can if there is money to do it from
            {
                int news,newr;
                news=value-*remaining;
                newr=*remaining;
                if(news<0)
                {
                    news=0;
                    newr=value;
                }
                *remaining-=newr;
                childl=new treenode(send,news);//carry out the (possibly part) of the transaction
                childr=new treenode(receive,newr);
                associatedwith=trans;
                trans->parts->insert(this);
            }
        }
    }
}
void treenode::printbottomlevel()//go to the bottom recursively and print what we have
{
    if(childl!=NULL&&childr!=NULL)
    {
        childl->printbottomlevel();
        childr->printbottomlevel();
    }
    else
    {
        cout<<"Owner: "<<walletid->walletid<<" Amount: "<<value<<endl;
    }
}
int treenode::insidenodes(list* lst)//inserts all the transaction ids to the list provided (recursively)
{
    int flag=1;
    if(lst==NULL)
    {
        flag=0;
        lst=new list;
    }
    if(childl!=NULL&&childr!=NULL)
    {
        if(lst->find(associatedwith)==0&&lst!=NULL)
            lst->insert(associatedwith);


        childl->insidenodes(lst);
        childr->insidenodes(lst);
    }
    if(flag==0)
    {
        flag=lst->counter;
        lst->del(0);
        return flag;
    }
}
int treenode::unspent()
{
    if(childl==NULL)
        return value;
    else return childl->unspent();//tracing left child (should be the initial owner)

}
void treenode::printalltransactions(int level,list* lst)//go through the tree recursively and print out all the transactions(each treenode that has children represents a part of a transaction)
{
    if(level==0)
    {
        if(childl==NULL&&childr==NULL)
        {
            cout<<"No Transactions Found"<<endl;
            return;
        }
        lst=new list;//put all transactions in a list(multiple treenodes could be part of the same transaction)

    }
    if(associatedwith!=NULL)
    {
        if(lst->find(associatedwith)==0)
            lst->insert(associatedwith);
    }
    if(childl!=NULL&&childr!=NULL)
    {
        childl->printalltransactions(1,lst);//recursive
        childr->printalltransactions(1,lst);
    }
    if(level==0)
    {
        transaction** t=(transaction**)lst->getall();
        for(int i=0;i<lst->counter;i++)
        {
            t[i]->simplereadout();//printeverything at root level
        }
        delete[] t;
        delete lst;
    }
}

bitcoin::bitcoin(int coinid, wallet* initial,int coinvalue)//a bitcoin contains the root of a tree
{
    bitcoinid=coinid;
    root=new treenode(initial,coinvalue);
}
bitcoin::~bitcoin()
{
    delete root;//will delete tree recursively
}
int bitcoin::getbalance(wallet* who)
{
    return this->root->getbalance(who);//assigns job to lower levels
}
void bitcoin::transact(char* transid,wallet* send,wallet* receive,int amount,time_t when,int* remaining,transaction* trans)
{
    int tmp=*remaining;
    this->root->transact(transid,send,receive,amount,when,remaining,trans);//try to make the transaction
    if(tmp!=*remaining)//if remaining is less then an amount was used
    {
        if(receive->bitcoinlist->find(this)==0)
            receive->bitcoinlist->insert(this);//add bitcoin entry to the new wallet
        if(this->getbalance(send)==0)
            send->bitcoinlist->findandremove(this);
    }
}
void bitcoin::printcoinparts()//prints bitcoin id and who owns what of it
{
    cout<<"Coin: "<<bitcoinid<<endl;
    root->printbottomlevel();
}
int bitcoin::gettransactionsnumber()//all the treenodes without the leafs should be the number of transactions
{
    int cnt=0;
    cnt=root->insidenodes();
    return cnt;

}
int bitcoin::unspent()//how much is unspent from the original amount
{
    return root->unspent();
}
void bitcoin::printalltransactions()
{
    root->printalltransactions();
}
