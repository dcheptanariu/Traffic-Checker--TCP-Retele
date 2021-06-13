
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define streetsfile "Streets.xml"

extern int errno;
typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;
#include <time.h>
int maxnode;

int port;
FILE *fp;

static void *treatinput(void *);
static void *treatsd(void *); 
void readinput(void *);
void readsd(void *);
 void analizeanswer(char * text);
int nr_streets;
struct strazi{
     char name[100];
     int dis;
 }st[50][50];
 struct streets2{
char name[100];
 int dist;
 int x,y;

 }st2[50];
 struct position{
     int x,y,offset,dir;
 }pos,des;
 struct route{
   int dis,next;
 }ruta[100];
 int my_speed;
 void calculateroute();
 int minDistance(long int dist[], int sptSet[]) ;
void readstreets ();
char* gettext(char * text);
int correctaddress(char *buff);
void sendstartlocations();
void sendlocation(int fd);
char * myposition();
void  readdestination();
int corectdestination();

thData *inp;
  thData *ser;
int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis
  int nr=0;
  char buf[300];
  
  pthread_t forinp,forsd;

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
    	inp=(struct thData*)malloc(sizeof(struct thData));	
      	ser=(struct thData*)malloc(sizeof(struct thData));	
    inp->cl=0;
    inp->idThread=0;
    ser->cl=sd;
    ser->idThread=2;
    int fwd,left2=0;
    readstreets();
sendstartlocations(sd);
readdestination();
 fd_set rfds;
    struct timeval tv;
    int retval;
struct timeval start;
struct timeval end;
int x=0;
   gettimeofday(&start,NULL);
    int left,leftback;
    leftback=0;
  /* citirea mesajului */
  while(1){
     tv.tv_sec = 10;
    tv.tv_usec = 0;
       FD_ZERO(&rfds);
       FD_SET(0,&rfds);
    FD_SET(sd ,&rfds);

 retval = select(sd+1, &rfds, NULL, NULL, &tv);
 if(retval<0){
   printf("Error at select \n");
   return;
 }else{
if(retval>0){

if(FD_ISSET(sd,&rfds)){
   
bzero(buf,sizeof(buf));
if(read(sd,buf,sizeof(buf))<=0){
  printf("Error at read \n");
  return;
}else{
  buf[strlen(buf)]='\0';
  

analizeanswer(buf);
}
}
if( FD_ISSET(0,&rfds)){
pthread_create(&forinp, NULL, &treatinput,inp);

}
}else{

}
}
 
  gettimeofday(&end,NULL);
 left=(end.tv_sec-start.tv_sec);
fwd=((my_speed*1000)/3600)*(left-left2);
left2=left;

if(pos.dir==1){
 
  pos.offset=pos.offset+fwd;
  

}else{
  pos.offset-=fwd;
  
}
if(pos.x==des.x||pos.x==des.y ){
    if(pos.x==des.x && pos.offset>=des.offset){
      printf("Ai ajuns la destinatie!\nDoriti sa ajungeti la alta destinatie?\n");
     bzero(buf,sizeof(buf));
      fflush(stdout);
      read(0,buf,100);
if(strncmp(buf,"Da",2)==0){
        readdestination();
      }else{
        if(strncmp(buf,"Nu",2)==0){
        bzero(buf,sizeof(buf));
        strcpy(buf,"exit");
        write(sd,buf,strlen(buf));
        return;
        }
      }
    }else{
      if(pos.x==des.y&&pos.offset<=des.offset){
          printf("Ai ajuns la destinatie!\nDoriti sa ajungeti la alta destinatie?\n");
         bzero(buf,sizeof(buf));
      fflush(stdout);
      read(0,buf,100);
      if(strncmp(buf,"Da",2)==0){
        readdestination();
      }else{
        if(strncmp(buf,"Nu",2)==0){
        bzero(buf,sizeof(buf));
        strcpy(buf,"exit");
        write(sd,buf,strlen(buf));
        return;
        }
      }
      }
    }
}

if(pos.offset>st[pos.x][pos.y].dis){

  if((pos.y==des.x)||(pos.y=des.y)){
    
    if(pos.y==des.x)pos.x=des.x,pos.y=des.y;
    else pos.x=des.y,pos.y=des.x;
    printf("Continua pe %s \n",st[pos.x][pos.y].name);
    sendlocation(sd);
  }else{ 
  pos.x=pos.y;
  pos.y=ruta[pos.x].next;
    printf("Continua pe %s \n",st[pos.x][pos.y].name);
        sendlocation(sd);
  }
  if(pos.x<pos.y){
    pos.dir=1;
    pos.offset=0;
  }else{
pos.dir=0;
pos.offset=st[pos.x][pos.y].dis;
  }
  }else{
if(pos.offset<0){
 
  if((pos.x==des.x)||(pos.x==des.y)){
    if(pos.x==des.x)pos.y=des.y;
    else pos.y=des.x;
       printf("Continua pe %s \n",st[pos.x][pos.y].name);
           sendlocation(sd);
  }else{
  pos.y=ruta[pos.x].next;
      printf("Continua pe %s \n",st[pos.x][pos.y].name);
          sendlocation(sd);
  }
  if(pos.x<pos.y){
    pos.dir=1;
    pos.offset=0;
  }else{
pos.dir=0;
pos.offset=st[pos.x][pos.y].dis;
  }
}else{
  
}

  }
  
   
 if(((leftback*60+60)<=left )){
   leftback++;
sendlocation(sd);
 }
 }


 
  
  }


void readstreets(){
size_t read;
char *row=NULL;
int i,j;
int id,ok;
char name[100];
int dist,x,y;
size_t len=0;
 int len2;
 char buff[100];
fp=fopen(streetsfile,"r");
if(fp==NULL){
printf("Error opening file %s",streetsfile);
return;

}else{
 
while((read=getline(&row,&len,fp))!=-1){
   
len2=strlen(row);
row[len2-1]='\0';
if(strstr(row,"ROW")){
    ok=0;
    for(i=0;i<5;i++){
        read=getline(&row,&len,fp);
        if(strstr(row,"id")){
strcpy(buff,gettext(row));
id=atoi(buff);
ok++;

        }
          if(strstr(row,"name")){
strcpy(buff,gettext(row));
strcpy(name,buff);

ok++;
        }
          if(strstr(row,"p1")){
strcpy(buff,gettext(row));
x=atoi(buff);
ok++;

        }
          if(strstr(row,"p2")){
strcpy(buff,gettext(row));
y=atoi(buff);
ok++;

        }
          if(strstr(row,"dis")){
strcpy(buff,gettext(row));
dist=atoi(buff);

ok++;
        }
        
    }
    if(ok==5){
        strcpy(st[x][y].name,name);
            strcpy(st[y][x].name,name);
                st[x][y].dis=dist;
                st[y][x].dis=dist;
                if(x>maxnode)maxnode=x;
                if(y>maxnode)maxnode=y;
strcpy(st2[id].name,name);
st2[id].dist=dist;
st2[id].x=x;
st2[id].y=y;
if(nr_streets<id)nr_streets=id;
    }
    
read=getline(&row,&len,fp);
}
}
}

    


}
char * gettext(char* text){
char *buff=NULL;
buff=strstr(text,">");
buff=buff+1;
int j;
j=0;
while(buff[j]!='<'){
    j++;
}
buff[j]='\0';

return buff;

}
int correctaddress(char *buff){
    char buf[100];
    int nr=0;

for(int i=1;i<=nr_streets;i++){
if(strstr(buff,st2[i].name)){
   pos.x=st2[i].x;
   pos.y=st2[i].y;
   pos.dir=1;
   if(strstr(buff,"nr.")){
strcpy(buf,strstr(buff,"nr."));
int j=3;
while(buf[j]>='0'&&buf[j]<='9'){
    nr=nr*10+(buf[j]-'0');
    j++;
}
pos.offset=nr*50;
   }else{
       pos.offset=0;
   }
    return 1;
}

}
return 0;

}
void sendlocation(int fd){
char buff[200];
bzero(buff,sizeof(buff));
int nr;
nr=pos.offset/50;

snprintf(buff,200,"pos:%s nr.%dspeed:%d",st[pos.x][pos.y].name,nr,my_speed);
if(write(fd,buff,100)<=0){
  printf("Eroare la write\n");
}
}

void sendstartlocations(int fd){
    char buf[100];
    char buf2[100];
    int ok;
    bzero(buf2,sizeof(buf2));
    ok=0;
while(1){
    bzero(buf,sizeof(buf));
 printf ("[client]Introduceti locatia dumneavoastra: ");
  fflush (stdout);
  read (0, buf, sizeof(buf));
 

  



if(correctaddress(buf)){
      buf[strlen(buf)-1]='\0';
    strcat(buf2,"pos:");
    strcat(buf2,buf);
  


ok=1;
break;
    }else{
        printf("Adresa este gresita \n");
    }


}
int speed;
if(ok==1){
while(1){
 printf ("[client]Introduceti viteza dumneavoastra: ");
  bzero(buf,sizeof(buf));
  fflush (stdout);
  read (0, buf, sizeof(buf));
 speed=atoi(buf);
 my_speed=speed;
  //scanf("%d",&nr);
  buf[strlen(buf)-1]='\0';
  strcat(buf2,"speed:");
  strcat(buf2,buf);

  /* trimiterea mesajului la server */
  if (write (fd,buf2,strlen(buf2)) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    
    
    
    }else{
      
        ok=1;
        break;
    }

   


}

}



}
void  readdestination(){
   char buf[100];
   while(1){
   bzero(buf,sizeof(buf));
 printf ("[client]Introduceti destinatia dumneavoastra : ");
  fflush (stdout);
  read (0, buf, sizeof(buf));
  buf[strlen(buf)-1]='\0';
  if(corectdestination(buf)){
    calculateroute();
    return ;
  }
   }
}
int corectdestination( char * text){
      char buf[100];
    int nr=0;

for(int i=1;i<=nr_streets;i++){
if(strstr(text,st2[i].name)){
   des.x=st2[i].x;
   des.y=st2[i].y;
des.dir=1;
   if(strstr(text,"nr.")){
strcpy(buf,strstr(text,"nr."));
int j=3;
while(buf[j]>='0'&&buf[j]<='9'){
    nr=nr*10+(buf[j]-'0');
    j++;
}
des.offset=nr*50;
   }else{
      des.offset=0;
   }
   
    return 1;
}

}
return 0;

}
void calculateroute(){
  int before[maxnode+1];

long dist[maxnode+1]; // The output array.  dist[i] will hold the shortest 
    // distance from src to i 
  
    int sptSet[maxnode+1]; // sptSet[i] will be true if vertex i is included in shortest 
    // path tree or shortest distance from src to i is finalized 
  
    // Initialize all distances as INFINITE and stpSet[] as false 
    for (int i = 1; i <=maxnode; i++) {
        dist[i] = 9999999; sptSet[i] = 0,before[i]=-1; 
}
    // Distance of source vertex from itself is always 0 
    dist[pos.x] = 0; 
  
    // Find shortest path for all vertices 
    for (int count = 1; count <=maxnode; count++) { 
        // Pick the minimum distance vertex from the set of vertices not 
        // yet processed. u is always equal to src in the first iteration. 
        int u = minDistance(dist, sptSet); 
  
        // Mark the picked vertex as processed 
        sptSet[u] = 1; 

        // Update dist value of the adjacent vertices of the picked vertex. 
        for (int v = 1; v <=maxnode; v++) 
  
            // Update dist[v] only if is not in sptSet, there is an edge from 
            // u to v, and total weight of path from src to  v through u is 
            // smaller than current value of dist[v] 
            if (!sptSet[v] && st[u][v].dis && dist[u] != 9999999
                && dist[u] + st[u][v].dis < dist[v]) {
                dist[v] = dist[u] + st[u][v].dis; 
              before[v]=u;
              
                }
    } 
    int x,y,z;
    long long  mindis=9999999;
    
    if((dist[des.x]+des.offset) <= (dist[des.y]+st[des.x][des.y].dis-des.offset)) {
      x=des.x;
      y=pos.x;
    }else{
      x=des.y;
      y=pos.x;
    }
   
    
    
   z=x;
    while(x!=y){

ruta[before[x]].next=x;
x=before[x];
   
    }
    while(x!=z){
      
      x=ruta[x].next;
    }

   if(ruta[pos.x].next!=pos.y)pos.dir=0;
   else pos.dir=1;


}
int minDistance(long int dist[], int sptSet[]) 
{ 
    // Initialize min value 
    int min = 99999, min_index; 
  
    for (int v =1; v <=maxnode; v++) 
        if (sptSet[v] == 0 && dist[v] <= min) 
            min = dist[v], min_index = v; 
  
    return min_index; 
} 
static void *treatinput(void *arg){
	struct thData tdL; 
		tdL= *((struct thData*)arg);	
	
		fflush (stdout);		 
		pthread_detach(pthread_self());		
readinput((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	

}
void readinput(void *arg){

  struct thData tdL; 
	tdL= *((struct thData*)arg);
  int fd;
  fd=tdL.cl;
  char buf[200];
  char acc[200];
  int bytes;
  bzero(buf,sizeof(buf));
  if(bytes=read(fd,buf,200)<=0){
    printf("Error at read\n");
    return;

  }
  buf[strlen(buf)-1]='\0';
  if(strcmp(buf,"up")==0){
    my_speed+=2;
    printf("Viteza ta de deplasare este acum :%d\n",my_speed);
  }
  if(strcmp(buf,"down")==0){
    my_speed-=2;
    printf("Viteza ta de deplare este acum :%d\n",my_speed);

  }
  if(strncmp(buf,"option:",7)==0){
    if(bytes=(write(ser->cl,buf,strlen(buf)))<=0){
      perror("Error at write to server\n");
    }
  }
  
  if(strncmp(buf,"accident",8)==0){
bzero(acc,sizeof(acc));
int nr;
nr=pos.offset/50;
snprintf(acc,sizeof(acc),"accidentatpos:%s nr.%d",st[pos.x][pos.y].name,nr);
if(bytes=(write(ser->cl,acc,200))<=0){
      perror("Error at write to server\n");
      return;
    }
   
  }
   if(strncmp(buf,"blocaj",6)==0){
bzero(acc,sizeof(acc));
int nr;
nr=pos.offset/50;
snprintf(acc,sizeof(acc),"blocajatpos:%s nr.%d",st[pos.x][pos.y].name,nr);
if(bytes=(write(ser->cl,acc,sizeof(acc)))<=0){
      perror("Error at write to server\n");
      return;
    }
   
  }
  


  }


void analizeanswer(char * text){
char buf[200];
int s=0;
int i,j;
if(strncmp(text,"speedlegal:",11)==0){
  i=11;
  while(text[i]>='0'&&text[i]<='9'){
    s=s*10+(text[i]-'0');
    i++;
  }
  if(my_speed>s){
    printf("Atentie! Depasesti viteza legala de %d km/ora cu aproximativ %d km/ora\n",s,my_speed-s);
  }else{
    printf("Bravo!Respecti limita de viteza de %d km/ora \n",s);
  }
}
if(strncmp(text,"accidentatpos:",14)==0){
  bzero(buf,sizeof(buf));
  strcpy(buf,text+14);
  printf("Atentie!Accident raportat pe:%s . Mergi cu grija!\n",buf);
}
if(strncmp(text,"pre:",4)==0){
bzero(buf,sizeof(buf));
strcpy(buf,text+4);
printf("[Statie de alimentare]:%s\n",buf);

}
if(strncmp(text,"met:",4)==0){
  bzero(buf,sizeof(buf));
  strcpy(buf,text+4);
  printf("[Starea vremei]-%s\n",buf);
}
if(strncmp(text,"even:",5)==0){
bzero(buf,sizeof(buf));
strcpy(buf,text+5);
printf("[Evenimente sportiv]-%s\n",buf);

}
if(strncmp(text,"blocajatpos:",12)==0){
  bzero(buf,sizeof(buf));
  strcpy(buf,text+12);
  printf("[Blocaj trafic]-Atentie este aglomeratie pe %s\n",buf);
}


}
