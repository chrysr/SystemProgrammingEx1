#include "functions.h"

using namespace std;

wallet::wallet(char* str)//wallet contructor
{
    int cnt=0;
    while(str[cnt]!='\0')
        cnt++;

    walletid=new char[cnt+1];//copy id and initialize bitcoin list
    bitcoinlist=new list;
    strcpy(walletid,str);
}
wallet::~wallet()
{
    delete[] walletid;
    bitcoinlist->del(0);    //delete id and list without its contents
}

int wallet::getbalance()
{
    if(this->bitcoinlist->counter==0)//if no bitcoins in list then balance is 0
        return 0;
    bitcoin** btc=(bitcoin**)this->bitcoinlist->getall();
    int balance=0;
    for(int i=0;i<this->bitcoinlist->counter;i++)//check all bitcoins and find balance
        balance+=btc[i]->getbalance(this);

    delete[] btc;
    return balance;
}

transaction::transaction(wallet* send,wallet* receive,int usd, char* id, time_t w)
{
    sender=send;
    receiver=receive;
    amount=usd;
    parts=new list; //initialize list of parts
    if(w==0)
    {
        when=time(NULL);
    }
    else when=w;
    int cnt=0;
    while(id[cnt]!='\0')
        cnt++;

    transactionid=new char[cnt+1];  //allocate space for transaction id
    strcpy(transactionid,id);
}
transaction::~transaction()
{
    parts->del(0);      //delete parts but not actual contents
    delete[] transactionid; //delete transaction id space
}

void transaction::readout()//detailed printing of transaction
{
    cout<<"Transaction: "<<sender->walletid<<" -> "<<receiver->walletid<<endl;
    cout<<"Id: "<<transactionid<<endl;
    cout<<"Amount: "<<amount<<endl;
    cout<<"When ";printtime(when);cout<<endl;
    cout<<"Parts:"<<endl;
    treenode** treeparts=(treenode**)parts->getall();
    for(int i=0;i<parts->counter;i++)
    {
        cout<<"\tSender: "<<treeparts[i]->childl->walletid->walletid<<endl;
        cout<<"\t\tValue: "<<treeparts[i]->childl->value<<endl;
        cout<<"\tReceiver: "<<treeparts[i]->childr->walletid->walletid<<endl;
        cout<<"\t\tValue: "<<treeparts[i]->childr->value<<endl;

    }
    delete[] treeparts;

}
void transaction::simplereadout()//simpler printing of transaction
{
    cout<<"Transaction("<<transactionid<<") "<<sender->walletid<<" -("<<amount<<")> "<<receiver->walletid<<" ";
    printtime(when);cout<<endl;
}

time_t stringtodate(char* date,char* tim)//converts a date string and time string to to time_t type variable
{
    if(date[0]=='\0'||tim[0]=='\0')
        return time(NULL);
    struct tm timeinfo;
    char args[5][10];
    int place=0;
    char str[30];
    strcpy(str,date);
    strcat(str," ");
    strcat(str,tim);
    char *tmp=strtok(str," \r\n\t-:");//tokenize strings
    while(tmp)
    {
        strcpy(args[place],tmp);
        tmp=strtok(NULL," \t\r\n-:");
        place++;
    }
    timeinfo.tm_mday=atoi(args[0]);
    timeinfo.tm_mon=atoi(args[1])-1;//input in struct
    timeinfo.tm_year=atoi(args[2])-1900;
    timeinfo.tm_hour=atoi(args[3]);
    timeinfo.tm_min=atoi(args[4]);
    timeinfo.tm_sec=0;
    timeinfo.tm_isdst=-1;

    return mktime(&timeinfo);       //maketime

}

void printtime(time_t t)//given a time_t it prints time
{
    struct tm* tt=localtime(&t);
    printf("%02d-%02d-%04d %02d:%02d",tt->tm_mday,tt->tm_mon+1,tt->tm_year+1900,tt->tm_hour,tt->tm_min);

}

int readdel(char* dest,const char del1,const char del2)//used for more than one delimeters
{
    int cnt=0;
    while(1)
    {
        dest[cnt]=getchar();
        if(dest[cnt]==del1||dest[cnt]==del2||dest[cnt]==EOF)
        {
            int flag=0;
            if(dest[cnt]==del1)
                flag=1;
            dest[cnt]='\0';
            return flag;
        }
        cnt++;
    }
}
void printcoinowner(list* btc)//prints coin and history of owners
{
    bitcoin** coin=(bitcoin**)btc->getall();
    for(int i=0;i<btc->counter;i++)
    {
        coin[i]->printcoinparts();
    }
    delete[] coin;
}
void printbalances(list* wallets)//for every wallet we print which bitcoins it owns and how much of it
{
    wallet** wal=(wallet**)wallets->getall();
    for(int i=0;i<wallets->counter;i++)
    {
        cout<<wal[i]->walletid<<": "<<wal[i]->getbalance()<<" (";
        bitcoin** btc=(bitcoin**) wal[i]->bitcoinlist->getall();
        for(int j=0;j<wal[i]->bitcoinlist->counter;j++)
        {
            if(j+1!=wal[i]->bitcoinlist->counter)
                cout<<btc[j]->bitcoinid<<":"<<btc[j]->getbalance(wal[i])<<", ";
            else cout<<btc[j]->bitcoinid<<":"<<btc[j]->getbalance(wal[i]);
        }
        cout<<")"<<endl;
        delete[] btc;
    }
    delete[] wal;
}
int validwallet(char* str, list* wallets)//checks if a wallet exists (is valid)
{
    wallet** l=(wallet**)wallets->getall();
    if(l==NULL)
        return 0;

    for(int i=0;i<wallets->counter;i++)
    {
        if(strcmp(l[i]->walletid,str)==0)
        {
            delete[] l;
            return 1;
        }
    }
    delete[] l;
    return 0;
}

wallet* walletfromstr(list* lst, char* str)//tries to find the wallet* from a string
{
    wallet** wal=(wallet**)lst->getall();
    for(int i=0;i<lst->counter;i++)
    {
        if(strcmp(wal[i]->walletid,str)==0)
        {
            wallet* tmp=wal[i];
            delete[] wal;
            return tmp;
        }
    }
    delete[] wal;
    return NULL;
}
bitcoin* bitcoinfromstr(list* lst, char* str)//tries to find a bitcoin* from a string
{
    bitcoin** btc=(bitcoin**)lst->getall();
    for(int i=0;i<lst->counter;i++)
    {
        if(btc[i]->bitcoinid==atoi(str))
        {
            bitcoin* b=btc[i];
            delete[] btc;
            return b;
        }
    }
    delete[] btc;
    return NULL;
}
transaction* maketransaction(char* transid,wallet* send,wallet* receive, int amount,time_t when)//executes the actual transaction
{
    if(send->getbalance()<amount)//if there is enough money
        return NULL;
    bitcoin** btc=(bitcoin**)send->bitcoinlist->getall();
    int* remaining=new int;
    *remaining=amount;
    transaction* transfer=new transaction(send,receive,amount,transid,when);
    int counter=send->bitcoinlist->counter;
    for(int i=0;i<counter;i++)//for every bitcoin that the wallet has try to find funds to cover the required amount
    {
        btc[i]->transact(transid,send,receive,amount,when,remaining,transfer);
        if(*remaining==0)//if the amount has been transfered stop
            break;
    }
    delete[] btc;
    delete remaining;
    return transfer;
}
void getvalidtransid(hashtable* ht,char *id,int size)//checks if input id is valid and otherwize creates random string
{
    if(strcmp(id,"")!=0)
    {
        if(ht->transidexists(id)==0)//if given string is ok
            return;
    }
    size=7;
    srand(time(NULL));
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int backup=size;
    int cnt=0;
    while(1)
    {
        size=backup+cnt;
        if (size)
        {
            size--;
            for(int i=0;i<size;i++)
            {
                int key=rand() % (int) (sizeof(charset)-1);//create random string
                id[i]=charset[key];
            }
            id[size]='\0';
        }
        if(ht->transidexists(id)==0)//if it happens to exist, try again but with one more character
            return;
        cnt++;

    }
}
void newtransaction(list* walletlist,char args[10][100],hashtable* shash,hashtable* rhash,int mode)//primary checks
{
    cout<<"Trying to make the following transaction:"<<endl;
    cout<<"Transaction(";
    if(mode==0)
        cout<<args[0];
    cout<<") "<<args[1]<<" -("<<args[3]<<")> "<<args[2]<<" ";
    if(args[4][0]=='\0'||args[5][0]=='\0')
        cout<<endl;
    else
    {
        cout<<args[4]<<" "<<args[5];
        cout<<endl;
    }
    transaction* transfer=NULL;
    int w1=0,w2=0;

    if((w1=validwallet(args[1],walletlist))&&(w2=validwallet(args[2],walletlist)))//both wallets ok
    {
        if(strcmp(args[1],args[2])==0)
        {
            cout<<red;
            cout<<"Transaction Failed! Sender and Receiver wallets cannot be the same"<<endl;//different wallets
            if(mode==1)
                cout<<endl<<endl;
            cout<<reset;
            return;
        }
        if(atoi(args[3])==0)
        {
            cout<<red;
            cout<<"Transaction Failed! Amount cannot be 0"<<endl;       //valid amount given
            if(mode==1)
                cout<<endl<<endl;
            cout<<reset;
            return;
        }
        int flag=1;
        char tmpid[50];
        if(mode==0)
        {
            strcpy(tmpid,args[0]);
            getvalidtransid(shash,tmpid,50);
            if(strcmp(tmpid,args[0])!=0)//transaction id already exists
            {
                cout<<red;
                cout<<"Invalid Transaction Id. Do you want to continue with different Transaction Id? (y/n)";
                cout<<reset;
                int ch;
                while(ch=getchar())
                {
                    if(ch=='y')
                    {
                        ch=getchar();
                        cout<<green;
                        cout<<"Continuing Transaction with Transaction Id: "<<tmpid<<endl;      //offer to find a new transaction id
                        cout<<reset;
                        break;
                    }
                    else if(ch=='n')
                    {
                        ch=getchar();
                        cout<<red;
                        cout<<"Aborting Transaction"<<endl;
                        cout<<reset;
                        flag=0;
                        break;
                    }
                    else continue;
                }
            }
        }
        else
        {
            tmpid[0]='\0';
            getvalidtransid(shash,tmpid,50);
        }
        if(flag)//if all checks passed make the transaction
        {
            transfer=maketransaction(tmpid,walletfromstr(walletlist,args[1]),walletfromstr(walletlist,args[2]),atoi(args[3]),stringtodate(args[4],args[5]));
            if(transfer!=NULL)
            {
                cout<<green;
                cout<<"Transaction Completed"<<endl;//transaction successful
                //transfer->readout();
                transfer->simplereadout();
                cout<<reset;
                shash->inserttransaction(walletfromstr(walletlist,args[1]),transfer);
                rhash->inserttransaction(walletfromstr(walletlist,args[2]),transfer);
            }
            else
            {
                cout<<red;
                cout<<"Transaction "<<args[1]<<" -> "<<args[2]<<" failed due to insufficient funds"<<endl;//not enough money in wallet
                cout<<reset;
            }
        }

    }
    else
    {
        cout<<red;
        if(w1==0)
            cout<<"Invalid Wallet: "<<args[1]<<endl;//print invalid wallets if that is the case
        if(w2==0)
            cout<<"Invalid Wallet: "<<args[2]<<endl;
        cout<<"Transaction: "<<args[1]<<" -> "<<args[2]<<" failed due to invalid wallet"<<endl;
        cout<<reset;
    }
}
int requesttransaction(list* walletlist,char* buffer,hashtable* shash,hashtable* rhash,int mode)
{
    char* tok;
    char args[10][100];
    tok=strstr(buffer,"//");
    if(tok!=NULL)
        return 0;
    for(int i=0;i<10;i++)
        args[i][0]='\0';
    int place=0;
    char* tmp;
    tmp=strtok(buffer," \t\n\r;");//tokenize input string
    while(tmp)
    {
        strcpy(args[place],tmp);
        place++;
        tmp=strtok(NULL," \t\n\r;");
    }
    if((mode==0&&(place<3||place>6))||(mode==1&&(place<4||place>6)))//check for valid and correct number of parameters
    {
        cout<<yellow;
        cout<<"Please Check your arguments..."<<endl<<endl<<endl;
        cout<<reset;
        return 0;
    }

    newtransaction(walletlist,args,shash,rhash,mode);
    cout<<endl<<endl;
}
int find(list* walletlist,hashtable* rhash,char* buffer)//findearnings and findpayments
{
    char* tok;
    char args[15][100];
    tok=strstr(buffer,"//");
    if(tok!=NULL)
        return 0;
    for(int i=0;i<10;i++)
        args[i][0]='\0';
    int place=0;
    char* tmp;
    tmp=strtok(buffer," \t\n\r:-");//extract arguments
    while(tmp)
    {
        strcpy(args[place],tmp);
        place++;
        tmp=strtok(NULL," \t\n\r:-");
    }
    int fields=0;
    struct tm timeinfo1;
    struct tm timeinfo2;
    if(validwallet(args[1],walletlist)==0)//if wallet is valid or not
    {
        cout<<red;
        cout<<"Invalid Wallet: "<<args[1]<<endl;
        cout<<reset;
    }
    list* lst=rhash->gettransactions(walletfromstr(walletlist,args[1]));//get all the transactions by the specific walletid
    if(lst==NULL)
        return 0;
    if(place==2)//depending on the number of arguments transform the dates and times
    {
        fields=1;
        compareandprinttrans(lst,timeinfo1,timeinfo2,fields);//no dates or times
    }
    else if(place==6)//only times
    {
        fields=21;

        timeinfo1.tm_mday=0;
        timeinfo1.tm_mon=0;
        timeinfo1.tm_year=0;
        timeinfo1.tm_hour=atoi(args[2]);
        timeinfo1.tm_min=atoi(args[3]);
        timeinfo1.tm_sec=0;
        timeinfo1.tm_isdst=-1;

        timeinfo2.tm_mday=0;
        timeinfo2.tm_mon=0;
        timeinfo2.tm_year=0;
        timeinfo2.tm_hour=atoi(args[4]);
        timeinfo2.tm_min=atoi(args[5]);
        timeinfo2.tm_sec=0;
        timeinfo2.tm_isdst=-1;
        compareandprinttrans(lst,timeinfo1,timeinfo2,fields);
    }
    else if(place==8)//dates
    {

        fields=22;
        timeinfo1.tm_mday=atoi(args[2]);
        timeinfo1.tm_mon=atoi(args[3]);
        timeinfo1.tm_year=atoi(args[4]);
        timeinfo1.tm_hour=0;
        timeinfo1.tm_min=0;
        timeinfo1.tm_sec=0;
        timeinfo1.tm_isdst=-1;

        timeinfo2.tm_mday=atoi(args[5]);
        timeinfo2.tm_mon=atoi(args[6]);
        timeinfo2.tm_year=atoi(args[7]);
        timeinfo2.tm_hour=0;
        timeinfo2.tm_min=0;
        timeinfo2.tm_sec=0;
        timeinfo2.tm_isdst=-1;
        compareandprinttrans(lst,timeinfo1,timeinfo2,fields);

    }
    else if(place==12)//times and dates
    {
        fields=3;
        timeinfo1.tm_mday=atoi(args[4]);
        timeinfo1.tm_mon=atoi(args[5]);
        timeinfo1.tm_year=atoi(args[6]);
        timeinfo1.tm_hour=atoi(args[2]);
        timeinfo1.tm_min=atoi(args[3]);
        timeinfo1.tm_sec=0;
        timeinfo1.tm_isdst=-1;

        timeinfo2.tm_mday=atoi(args[9]);
        timeinfo2.tm_mon=atoi(args[10]);
        timeinfo2.tm_year=atoi(args[11]);
        timeinfo2.tm_hour=atoi(args[7]);
        timeinfo2.tm_min=atoi(args[8]);
        timeinfo2.tm_sec=0;
        timeinfo2.tm_isdst=-1;
        compareandprinttrans(lst,timeinfo1,timeinfo2,fields);

    }
    cout<<endl;
}
void compareandprinttrans(list* lst,struct tm timeinfo1,struct tm timeinfo2,int fields)
{
    int counter=0;
    transaction** trans=(transaction**)lst->getall();
    int total=0;
    if(fields==1)//depending on the field it compares different stuff
    {
        for(int i=0;i<lst->counter;i++)//here it compares nothing
        {
            cout<<green;
            trans[i]->simplereadout();
            total+=trans[i]->amount;
            cout<<reset;
            counter++;
        }
    }
    else if(fields==21)
    {
        for(int i=0;i<lst->counter;i++)
        {
            time_t tmp=trans[i]->when;
            struct tm timeinfo=*localtime(&tmp);//compares hours and minutes
            if((timeinfo.tm_hour*60+timeinfo.tm_min)>=(timeinfo1.tm_hour*60+timeinfo1.tm_min)&&(timeinfo.tm_hour*60+timeinfo.tm_min)<=(timeinfo2.tm_hour*60+timeinfo2.tm_min))
            {
                cout<<green;
                trans[i]->simplereadout();
                total+=trans[i]->amount;
                cout<<reset;
                counter++;
            }
        }
    }
    else if(fields==22)
    {
        for(int i=0;i<lst->counter;i++)//compares dates only
        {
            time_t tmp=trans[i]->when;
            struct tm timeinfo=*localtime(&tmp);
            timeinfo.tm_year+=1900;
            timeinfo.tm_mon+=1;
            long int t,t1,t2;
            t=timeinfo.tm_year*525600+timeinfo.tm_mon*43800+timeinfo.tm_mday*1440;
            t1=timeinfo1.tm_year*525600+timeinfo1.tm_mon*43800+timeinfo1.tm_mday*1440;
            t2=timeinfo2.tm_year*525600+timeinfo2.tm_mon*43800+timeinfo2.tm_mday*1440;
            if(t>=t1&&t<=t2)
            {
                cout<<green;
                trans[i]->simplereadout();
                total+=trans[i]->amount;
                cout<<reset;
                counter++;
            }
        }
    }
    else if(fields==3)//compares both dates and times
    {
        for(int i=0;i<lst->counter;i++)
        {
            time_t tmp=trans[i]->when;
            struct tm timeinfo=*localtime(&tmp);
            timeinfo.tm_year+=1900;
            timeinfo.tm_mon+=1;
            long int t,t1,t2;
            t=timeinfo.tm_year*525600+timeinfo.tm_mon*43800+timeinfo.tm_mday*1440+timeinfo.tm_hour*60+timeinfo.tm_min;
            t1=timeinfo1.tm_year*525600+timeinfo1.tm_mon*43800+timeinfo1.tm_mday*1440+timeinfo1.tm_hour*60+timeinfo1.tm_min;
            t2=timeinfo2.tm_year*525600+timeinfo2.tm_mon*43800+timeinfo2.tm_mday*1440+timeinfo2.tm_hour*60+timeinfo2.tm_min;
            if(t>=t1&&t<=t2)
            {
                    cout<<green;
                    trans[i]->simplereadout();
                    total+=trans[i]->amount;
                    cout<<reset;
                    counter++;
            }
        }
    }
    delete[] trans;
    if(counter==0)//if none was printed then print the message
    {
        cout<<yellow;
        cout<<"No Transactions match this query"<<endl;
        cout<<reset;
    }
    else
    {
        cout<<green;
        cout<<"Total amount: "<<total<<endl;
        cout<<reset;
    }
}
void walletstatus(list* walletlist, char* buffer)
{
    char* tok;
    tok=strtok(buffer," \t\n\r");
    tok=strtok(NULL," \t\n\r");

    wallet* tempwal=walletfromstr(walletlist,tok);
    if(tempwal!=NULL)//if wallet is valid
    {
        cout<<green;
        cout<<"Balance for "<<tok<<" is "<<tempwal->getbalance()<<endl;//print the balance
        cout<<reset;
    }
    else
    {
        cout<<red;//otherwise error message
        cout<<"Invalid Wallet: "<<tok<<endl;
        cout<<reset;
    }
}
void bitcoinstatus(list* bitcoinlist, char* buffer,int value)
{
    char* tok;
    tok=strtok(buffer," \t\n\r");
    tok=strtok(NULL," \t\n\r");

    bitcoin* tempbit=bitcoinfromstr(bitcoinlist,tok);//for the bitcoin specified
    if(tempbit!=NULL)
    {
        cout<<green;
        cout<<"Status for Bitcoin: "<<tok<<endl;//print initial value, num of transactions and unspent amount of the initial amount
        cout<<"  Initial value is: "<<value<<endl;
        cout<<"  Number of transactions: "<<tempbit->gettransactionsnumber()<<endl;
        cout<<"  Unspent: "<<tempbit->unspent()<<endl;
        cout<<red;
    }
    else
    {
        cout<<red;
        cout<<"Bitcoin "<<tok<<" does not exist"<<endl;//if it doesn't exist print error message
        cout<<reset;
    }
}
void tracecoin(list* bitcoinlist,char* buffer)
{
    char* tok;
    tok=strtok(buffer," \t\n\r");
    tok=strtok(NULL," \t\n\r");

    bitcoin* tempbit=bitcoinfromstr(bitcoinlist,tok);
    if(tempbit!=NULL)
    {
        cout<<green;
        cout<<"Trace Bitcoin: "<<tok<<endl;//print all the transactions that fall under this bitcoin's tree
        tempbit->printalltransactions();
        cout<<reset;
    }
    else
    {
        cout<<red;
        cout<<"Bitcoin "<<tok<<" does not exist"<<endl;//if bitcoin is invalid
        cout<<reset;
    }
}
