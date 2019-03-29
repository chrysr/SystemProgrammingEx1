#include "functions.h"


using namespace std;

int main(int argc,char* argv[])
{

    char balancefile[100],transactfile[100];
    int coinvalue,hash1,hash2,bucketsize;//arguments given to the program
    int confirmation=0;//ask for confirmation after each transaction when reading a file
    if(argc<12)
    {
        cout<<red;
        cout<<"Not enough arguments given! Please provide the necessary arguments"<<endl<<endl;cout<<reset;
        cout<<"Usage: "<<argv[0]<<" [OPTIONS]..."<<endl<<endl;
        cout<<"Mandatory: "<<endl;
        cout<<"  -a  <BalancesFile>"<<endl;
        cout<<"  -t  <TransactionsFile>"<<endl;
        cout<<"  -v  <BitcoinValue>"<<endl;
        cout<<"  -h1 <senderHashtableEntries>"<<endl;
        cout<<"  -h2 <receiverHashtableEntries>"<<endl;
        cout<<"  -b  <BucketSize (in Bytes)>"<<endl;
        cout<<endl<<"Optional: "<<endl;
        cout<<"  -confirm   asks for confirmation after each transaction in file"<<endl;
        return -1;
    }
    for(int i=0;i<argc;i++)//if we have the required
    {
        if(strcmp(argv[i],"-a")==0)
            strcpy(balancefile,argv[i+1]);
        else if(strcmp(argv[i],"-t")==0)
            strcpy(transactfile,argv[i+1]);
        else if(strcmp(argv[i],"-v")==0)
            coinvalue=atoi(argv[i+1]);
        else if(strcmp(argv[i],"-h1")==0)
            hash1=atoi(argv[i+1]);
        else if(strcmp(argv[i],"-h2")==0)
            hash2=atoi(argv[i+1]);
        else if(strcmp(argv[i],"-b")==0)
            bucketsize=atoi(argv[i+1]);
        else if(strcmp(argv[i],"-confirm")==0)
            confirmation=1;
    }
    if(bucketsize<sizeof(bucketentry))
    {
        cout<<red;
        cout<<"Bucket Size is not a valid number. Min is "<<sizeof(bucketentry)<<". Use this? (y/n)";
        cout<<reset;
        char bs;
        bs=getchar();
        while(1)
        {
            if(bs=='y')
            {
                bucketsize=sizeof(bucketentry);
                break;
            }
            else if(bs=='n')
            {
                cout<<red;
                cout<<"Exiting..."<<endl;
                cout<<reset;
                return -1;
            }
            else
            {
                cout<<"Bucket Size is not a valid number. Min is "<<sizeof(bucketentry)<<". Use this? (y/n)";
                bs=getchar();
            }
        }

    }

    cout<<cyan;
    cout<<"Starting the Program with the following parameters:"<<endl;
    cout<<"bitCoinBalancesFile:      "<<balancefile<<endl;
    cout<<"transactionsFile:         "<<transactfile<<endl;
    cout<<"bitCoinValue:             "<<coinvalue<<endl;
    cout<<"senderHashtableEntries:   "<<hash1<<endl;
    cout<<"receiverHashtableEntries: "<<hash2<<endl;
    cout<<"bucketSize                "<<bucketsize<<endl;
    cout<<"ConfirmAfterFileTransact: ";
    if(confirmation==0)
        cout<<"False"<<endl;
    else cout<<"True"<<endl;
    cout<<reset<<endl<<endl<<endl;


    list* bitcoinlist=new list;//create a list that contains the bitcoins and one for wallets for quick access
    list* walletlist=new list;
    FILE* fp;
    fp=fopen(balancefile,"r");
    if(fp==NULL)
    {
        cout<<red;
        cout<<balancefile<<" does not exist "<<endl<<endl;
        cout<<reset;
        cout<<cyan<<"Exiting..."<<reset<<endl;
        return -1;
    }
    char *buffer=new char[500];
    char* buffer2=new char[500];
    char* buffer3=new char[500];    //temprary buffers for reading from files or stdin
    size_t n=500;       //size of buffers

    hashtable *rhash=new hashtable(hash2,bucketsize);//create the two hashtables
    hashtable *shash=new hashtable(hash1,bucketsize);
    char *tok;
    while(getline(&buffer,&n,fp)!=-1)//read the file containing the bitcoins and save it to the buffer
    {
        tok=strstr(buffer,"//");//if file contains // we consider it a comment
        if(tok!=NULL)
            continue;
        tok=strtok(buffer," \t\n\r");//tokenize string
        if(tok==NULL)
            continue;
        if(walletlist->findwal(tok))//if wallet exists print out error
        {
            cout<<red;
            cout<<"Wallet "<<tok<<" already exists! Skipping this entry..."<<endl<<endl<<endl;
            cout<<reset;
            continue;
        }
        wallet* wal=new wallet(tok);    //insert newly created wallet to the two hashtables and our list
        shash->insert(wal);
        rhash->insert(wal);
        walletlist->insert(wal);

        tok=strtok(NULL," \t\n\r");

        while(tok)//every next token we get should be a bitcoin id (int)
        {
            if(bitcoinlist->findbit(atoi(tok)))
            {
                cout<<red;
                cout<<"Bitcoin "<<tok<<" has been assigned to a different walletID"<<endl<<endl<<endl;//if bitcoinid already exists in data
                cout<<reset;
                tok=strtok(NULL," \t\n\r");
                continue;
            }
            bitcoin* btc=new bitcoin(atoi(tok),wal,coinvalue);  //else create new bitcoin object and insert it to wallet's list of bitcoins
            tok=strtok(NULL," \t\n\r");
            bitcoinlist->insert(btc);   //insert to bitcoin list
            wal->bitcoinlist->insert(btc);
        }
    }
    fclose(fp);//close file

    fp=fopen(transactfile,"r");//open file to read Transactions
    if(fp==NULL)
    {
        cout<<red;
        cout<<transactfile<<" does not exist "<<endl<<endl;
        cout<<reset;
        cout<<cyan<<"Exiting..."<<reset<<endl;
        return -1;
    }
    char args[10][100];
    while(getline(&buffer,&n,fp)!=-1)//read line by line and make the transaction
    {
        requesttransaction(walletlist,buffer,shash,rhash);
        if(confirmation)
            char ch=getchar();
    }
    fclose(fp);//close file


    int flag=1;
    int first=1;
    char* backup;
    int multiple=0;
    while(1)//after we are done with the files we loop forever to accept any commands from stdin
    {
        int cnt=0;
        if(flag==1&&multiple==0)
        {
            cout<<cyan;
            cout<<"Waiting for Command: "<<endl<<">";
            cout<<reset;

        }
        getline(&buffer,&n,stdin);      //read line from stdin
        if(buffer[0]!='/'&&multiple==0)     //if line doesn't start with / it's not a command so we present the error
        {
            if(first)
            {
                first=0;
                cout<<endl;
            }
            cout<<red;
            cout<<"Invalid Command! (Type /help for help menu)"<<endl;
            cout<<reset;
            continue;
        }
        strcpy(buffer2,buffer);
        if(multiple)//if we're in the multiple transactions mode
        {
            backup=strstr(buffer,";"); //we check for ;
            if(backup!=NULL)
                multiple=1;
            else multiple=0;
            if(multiple)
            {
                strcpy(buffer2,"/requestTransaction ");    //modify the command in this mode so that we can use the existing functions as is
                strcat(buffer2,buffer);
                strcpy(buffer,buffer2);
            }

        }
        tok=strtok(buffer2," \t\r\n");
        if(strcmp(tok,"/requestTransaction")==0)
        {
            if(multiple==0)
            {
                backup=strstr(buffer,";");
                if(backup!=NULL)
                    multiple=1;
                else multiple=0;
            }
            while(tok!=NULL)//count how many arguments were given
            {
                cnt++;
                backup=tok;
                tok=strtok(NULL," \t\r\n");
            }
            if(cnt==2)//second argument should by a file if cnt==2
            {
                fp=fopen(backup,"r");
                if(fp==NULL)
                {
                    cout<<red;
                    cout<<backup<<" does not exist "<<endl<<endl<<endl;  //if cannot open the file then it doesn't exist
                    cout<<reset;
                    continue;
                }
                while(getline(&buffer,&n,fp)!=-1)
                {
                    requesttransaction(walletlist,buffer,shash,rhash);//for each entry request a transaction
                    if(confirmation)
                        char ch=getchar();
                }
                fclose(fp);//close file
            }
            else//else just request the transaction (no file)
            {
                requesttransaction(walletlist,buffer,shash,rhash,1);
                continue;
            }
            flag=1;
        }
        else
        {
            strcpy(buffer3,buffer);
            char* c;
            c=strtok(buffer3," \t\r\n");
            while(c!=NULL)  //count given arguments
            {
                cnt++;
                c=strtok(NULL," \t\r\n");
            }
            if(strcmp(tok,"/exit")==0)//exit program
            {
                cout<<cyan<<"Exiting..."<<reset<<endl;
                break;
            }
            else if(strcmp(tok,"/printcoins")==0)   //prints all the coin and their owners
            {
                cout<<green;
                printcoinowner(bitcoinlist);
                cout<<reset;
                flag=1;
            }
            else if(strcmp(tok,"/printwallets")==0) //prints all the wallets and their bitcoins and how much of each bitcoin they have
            {
                cout<<green;
                printbalances(walletlist);
                cout<<reset;
                flag=1;
            }
            else if(strcmp(tok,"/help")==0)//displays a help menu so that one does not have to remember the commands by heart
            {
                cout<<yellow;
                cout<<"Supported Commands: "<<endl;
                cout<<"/requestTransaction  senderWalletID receiverWalletID amount date time"<<endl;
                cout<<"\tRequest New Single Transaction"<<endl;
                cout<<"/requestTransaction  (multiple seperated with <;>)"<<endl;
                cout<<"\tRequest Multiple New Transactions" <<endl;
                cout<<"/requestTransaction  inputFile"<<endl;
                cout<<"\tRequest New Transactions listed in inputFile"<<endl;
                cout<<"/findEarnings        walletID [time1] [year1] [time2] [year2]"<<endl;
                cout<<"\tDisplay transactions where walletID is receiving money. Other arguments are optional"<<endl;
                cout<<"/findPayments        walletID [time1] [year1] [time2] [year2]"<<endl;
                cout<<"\tDisplay transactions where walletID is sending money. Other arguments are optional"<<endl;
                cout<<"/walletStatus        walletID"<<endl;
                cout<<"\tDisplays current balance for walletID"<<endl;
                cout<<"/bitCoinStatus       bitCoinID"<<endl;
                cout<<"\tDisplays the total value, the number of transactions it takes part in and the value that has not been spent yet"<<endl;
                cout<<"/traceCoin           bitCoinID"<<endl;
                cout<<"\tDisplays the transactions the coin has taken part in"<<endl;
                cout<<"/printcoins"<<endl;
                cout<<"\tPrint whole database by coin"<<endl;                           //extra functions implemented
                cout<<"/printwallets"<<endl;
                cout<<"\tPrint whole database by wallet"<<endl;
                cout<<"/help"<<endl;
                cout<<"\tDisplays this help message"<<endl;                             //extra ends here
                cout<<"/exit"<<endl;
                cout<<"\tNormally exits the program"<<endl;

                cout<<reset;
                flag=1;
            }
            else if(strcmp(tok,"/findEarnings")==0)
            {
                if(cnt<2||cnt>6)//range of accepted arguments
                {
                    cout<<yellow;
                    cout<<"Please Check your arguments..."<<endl<<endl<<endl;
                    cout<<reset;
                    continue;
                }
                find(walletlist,rhash,buffer);
            }
            else if(strcmp(tok,"/findPayments")==0)
            {
                if(cnt<2||cnt>6)//same exact as above  just with different hashtable
                {
                    cout<<yellow;
                    cout<<"Please Check your arguments..."<<endl<<endl<<endl;
                    cout<<reset;
                    continue;

                }
                find(walletlist,shash,buffer);
            }
            else if(strcmp(tok,"/walletStatus")==0)
            {
                if(cnt!=2)      //must have exactly two arguments
                {
                    cout<<yellow;
                    cout<<"Please Check your arguments..."<<endl<<endl<<endl;
                    cout<<reset;
                    continue;

                }
                walletstatus(walletlist,buffer);
            }
            else if(strcmp(tok,"/bitCoinStatus")==0)
            {
                if(cnt!=2)  //must have exactly two arguments
                {
                    cout<<yellow;
                    cout<<"Please Check your arguments..."<<endl<<endl<<endl;
                    cout<<reset;
                    continue;

                }
                bitcoinstatus(bitcoinlist,buffer,coinvalue);
            }
            else if(strcmp(tok,"/traceCoin")==0)
            {
                if(cnt!=2)      //must have exactly two arguments
                {
                    cout<<yellow;
                    cout<<"Please Check your arguments..."<<endl<<endl<<endl;
                    cout<<reset;
                    continue;

                }
                tracecoin(bitcoinlist,buffer);
            }
            else    //if no match, then command is invalid
            {
                cout<<red;
                cout<<"Invalid Command! (Type /help for help menu)"<<endl;
                cout<<reset;
            }
            cout<<endl;
        }
    }
    delete[] buffer;    //delete dynamically allocated memory
    delete[] buffer2;
    delete[] buffer3;

    bitcoinlist->del(1);    //delete lists and hashtables (and trees)
    walletlist->del(2);
    delete shash;
    rhash->settransnull();  //do not delete same data twice
    delete rhash;

    return 0;
}
