/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
/* portul folosit */
#define PORT 2908
#define streetsfile "Streets.xml"
#define speedfile "speed.xml"
#define preturifile "preturi.xml"
#define meteofile "vreme.xml"
#define evenfile "evenimente.xml"
struct price{
char text[200];
}preturi[100];
struct evenn{
  char text[200];
}even[100];
struct meteo{
  char zi[100];
  char noapte[100];

}evenmeteo[100];
int nr_pret,nr_even,nr_meteo;
int cur_pret,cur_even;
/* codul de eroare returnat de anumite apeluri */
extern int errno;
FILE *fp;
typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;
struct userpos{
    int x,y,offset,dir,vit;
}pos[100];
int nr_streets;
struct streets{
    int x,y,vit;
    char name[100];

}st[100];
int vremea_azi;
struct clients{
  int active,fd;
  int options[3];
}clienti[100];
int maxc;
static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
void readstreets ();
char* gettext(char * text);
int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}
char* analizetext(char * text,int fd,int id);
void updateposition(char *loc,int nr,int speed,int id);
void readspeed();
int legalspeed(char * name);
void readmeteo();
void readpreturi();
void readeven();
void checkaglomeration();
int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  time_t ttt;
   
   srand((unsigned) time(&ttt));


  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
    readstreets();
    readspeed();

readmeteo();
   vremea_azi=(rand()%nr_meteo);
 
readpreturi();
readeven();
  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      
      fflush (stdout);
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
	
	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

clienti[i-1].active=1;
clienti[i-1].fd=client;
if(maxc<i)maxc=i;
	pthread_create(&th[i], NULL, &treat, td);	      
				checkaglomeration();
	}//while    
};				
static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	

		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
    clienti[tdL.idThread].active=0;
    printf("S-a deconectat un client!\n");
		return(NULL);	
  		
};


void raspunde(void *arg)
{int bytes_read;
    fd_set rfds;
    struct timeval tv;
    int retval;
      char buff[300];
      char answer[300];
        int nr, i=0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);
    
   int c=0;

    while(1){
        
         tv.tv_sec = 15;
    tv.tv_usec = 0;
       FD_ZERO(&rfds);
    FD_SET(tdL.cl ,&rfds);
    

    retval = select(tdL.cl+1, &rfds, NULL, NULL, &tv);
     /* Don't rely on the value of tv now! */
if(!fd_is_valid(tdL.cl)){
 
    return;
}else{
    
}
     if (retval == -1)
        {perror("select()");}
     else if (retval){
      
        
     if(FD_ISSET(tdL.cl,&rfds)){
         bzero(buff,sizeof(buff));
         bzero(answer,sizeof(answer));
	if ((bytes_read=read (tdL.cl, buff,200) )<= 0)
			{
                if (bytes_read==0)
                
                {
                  return;}
                
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la read() de la client.\n");
			
			}
	

  buff[strlen(buff)]='\0';
  if(strncmp(buff,"accident",8)==0){
    for(int q=0;q<=maxc;q++){
      if(clienti[q].active){
        if(write(clienti[q].fd,buff,strlen(buff))<=0){
          perror("Error at write to all clients\n");
        }else{
        
        }
      }
    }
  }else{
    if(strncmp(buff,"exit",4)==0){
return;

    }else{
      if(strncmp(buff,"blocajatpos:",12)==0){
 for(int q=0;q<=maxc;q++){
      if(clienti[q].active){
        if(write(clienti[q].fd,buff,strlen(buff))<=0){
          perror("Error at write to all clients\n");
        }
      }}


      }else{

	strcpy(answer,	analizetext(buff,tdL.cl,tdL.idThread));   




		      
		    	     
	 if (write (tdL.cl, answer, strlen(answer)) <= 0)
		{
		 printf("[Thread %d] ",tdL.idThread);
		 perror ("[Thread]Eroare la write() catre client.\n");
		}
	
  }
  }
     }

     } }else{
       
       c++;
       if(c==4){
         
         if(clienti[tdL.idThread].options[1]){
           if(cur_pret==nr_pret)cur_pret=0;
           bzero(buff,strlen(buff));
           snprintf(buff,sizeof(buff),"pre:%s",preturi[cur_pret].text);
           if(write(tdL.cl,buff,strlen(buff))<=0){
             perror("Error at read()");
           }
           cur_pret++;
         }
       }
       if(c==8){
         c=0;
          if(clienti[tdL.idThread].options[2]){
           if(cur_even==nr_even)cur_even=0;
           bzero(buff,strlen(buff));
           snprintf(buff,sizeof(buff),"even:%s",even[cur_even].text);
           
           if(write(tdL.cl,buff,strlen(buff))<=0){
             perror("Error at read()");
           }
           cur_even++;
         }
       }
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
        strcpy(st[id].name,name);

st[id].x=x;
st[id].y=y;
if(nr_streets<id)nr_streets=id;
    }
    
read=getline(&row,&len,fp);
}
}
}

    
fclose(fp);

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
char *  analizetext(char * text,int fd,int id){
char buf[300];
char buf2[300];

char* answer=NULL;
int len;
int i;
int nr=0;
int speed=0;

len=strlen(text);
bzero(buf,sizeof(buf));
bzero(buf2,sizeof(buf2));
if(strstr(text,"pos:")){
   
strcpy(buf,(strstr(text,"speed:")));
for(i=4;i<len-strlen(buf);i++){
    buf2[i-4]=text[i];
}

buf2[i-4]='\0';
i=6;
while(buf[i]>='0'&&buf[i]<='9'){
    speed=speed*10+(buf[i]-'0');
    i++;
}
if(strstr(buf2,"nr.")){
bzero(buf,sizeof(buf));
strcpy(buf,strstr(buf2,"nr."));
len=strlen(buf2);
buf2[len-strlen(buf)-1]='\0';
i=3;
while(buf[i]>='0'&&buf[i]<='9'){
    nr=nr*10+ (buf[i]-'0');
    i++;
}

}

updateposition(buf2,nr,speed,id);
int l=legalspeed(buf2);

bzero(buf,sizeof(buf));
snprintf(buf,300,"speedlegal:%d",l);

answer=buf;
return answer;
}else{
if(strncmp(text,"option:",7)==0){
  
  
  bzero(buf,sizeof(buf));
  if(strstr(text,"meteo")){
   
    clienti[id].options[0]=1;
snprintf(buf,sizeof(buf),"met:%s\n%s\n",evenmeteo[vremea_azi].zi,evenmeteo[vremea_azi].noapte);

  }
  if(strstr(text,"preturi")){
      
    clienti[id].options[1]=1;
snprintf(buf,sizeof(buf),"pre:%s\n",preturi[cur_pret].text);
cur_pret++;
  }
  if(strstr(text,"evenimente")){
      
    clienti[id].options[2]=1;
snprintf(buf,sizeof(buf),"even:%s\n",even[cur_even].text);
cur_even++;
  }


answer=buf;

return answer;
}

}


return "invalid";
}
int legalspeed(char * name){
for(int i=1;i<=nr_streets;i++){
  if(strcmp(name,st[i].name)==0){
   
    return st[i].vit;

  }
}
    printf("NU Am gasit strada\n");
return 0;

}
void updateposition(char *loc,int nr,int speed,int id){
int i,j,ok;
ok=0;

for(i=1;i<nr_streets;i++){
if(strcmp(loc,st[i].name)==0){
    
    pos[id].x=st[i].x;
    pos[id].y=st[i].y;
    ok=1;
}

}
if(ok==0){printf("Eroare la updateposition\n");
return;}
pos[id].vit=speed;
pos[id].offset=nr*50;





}
void checkaglomeration(){
  int i,j;
  char buf[300];
  bzero(buf,sizeof(buf));
  int nrt,nr;
  char str[300];
  
  for(i=0;i<maxc;i++){
if(clienti[i].active){
  nrt=1;
  for(j=i+1;j<maxc;j++){
    if((pos[i].x==pos[j].x)&&(pos[i].y==pos[j].y)&&pos[i].x!=0&&((((pos[i].offset-pos[j].offset)<=50)||((pos[j].offset-pos[i].offset)<=50)))){
      nrt++;
      
    }
     if((pos[i].x==pos[j].y)&&pos[i].x &&(pos[i].y==pos[j].x)&&((((pos[i].offset-pos[j].offset)<=50)||((pos[j].offset-pos[i].offset)<=50)))){
      nrt++;
            
    }
    
  }
  if(nrt==3){
   
    nr=pos[i].offset/50;
    bzero(str,sizeof(str));
    for(j=0;j<nr_streets;j++){
      if(st[j].x==pos[i].x&&st[j].y==pos[i].y){
        strcpy(str,st[j].name);
        break;
      }
        if(st[j].x==pos[i].y&&st[j].y==pos[i].x){
        strcpy(str,st[j].name);
        break;
      }
    }
    snprintf(buf,sizeof(buf),"blocajatpos: %s nr.%d",str,nr);
    for(j=0;j<=maxc;j++){
      if(clienti[j].active){
        if(write(clienti[j].fd,buf,strlen(buf))<=0){
         perror ("Error at write()");
         return;
        }
      }
    }
  }
}
  }
}
void readspeed(){

  size_t read;
char *row=NULL;
int i,j;
int id,ok;
char name[100];
int speed;
size_t len=0;
 int len2;
 char buff[100];
fp=fopen(speedfile,"r");
if(fp==NULL){
printf("Error opening file %s",speedfile);
return;

}else{
 
while((read=getline(&row,&len,fp))!=-1){
   
len2=strlen(row);
row[len2-1]='\0';
if(strstr(row,"ROW")){
    ok=0;
    for(i=0;i<3;i++){
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
          if(strstr(row,"vit")){
strcpy(buff,gettext(row));
speed=atoi(buff);
ok++;

        }
       
        
    }
    if(ok==3){
      
      st[id].vit=speed;
    
      }

read=getline(&row,&len,fp);
}
}
}

    fclose(fp);

}
void readmeteo(){
size_t read;
char *row=NULL;
int i,j;

char zi[200];
char noapte[200];
int dist,x,y;
size_t len=0;
 int len2;
 char buff[100];
fp=fopen(meteofile,"r");
if(fp==NULL){
printf("Error opening file %s",meteofile);
return;

}else{
 
while((read=getline(&row,&len,fp))!=-1){
   
len2=strlen(row);
row[len2-1]='\0';
if(strstr(row,"ROW")){
   
    for(i=0;i<2;i++){
        read=getline(&row,&len,fp);
        if(strstr(row,"zi")){
strcpy(buff,gettext(row));
strcpy(evenmeteo[nr_meteo].zi,buff);

}

          if(strstr(row,"noapte")){
strcpy(buff,gettext(row));
strcpy(evenmeteo[nr_meteo].noapte,buff);

 }
         
    
    
   

}
nr_meteo++;
read=getline(&row,&len,fp);
}
}

}
  
fclose(fp);

}
void readpreturi(){

  size_t read;
char *row=NULL;
int i,j;

char zi[200];
char noapte[200];
int dist,x,y;
size_t len=0;
 int len2;
 char buff[200];
fp=fopen(preturifile,"r");
if(fp==NULL){
printf("Error opening file %s",preturifile);
return;

}else{
 
while((read=getline(&row,&len,fp))!=-1){
   
len2=strlen(row);
row[len2-1]='\0';
if(strstr(row,"ROW")){
   
  
        read=getline(&row,&len,fp);
        if(strstr(row,"info")){
strcpy(buff,gettext(row));

strcpy(preturi[nr_pret].text,buff);

}
          nr_pret++;
         
    
   
read=getline(&row,&len,fp);
}

}
}
     
fclose(fp);
}
void readeven(){
 size_t read;
char *row=NULL;
int i,j;

char zi[200];
char noapte[200];
int dist,x,y;
size_t len=0;
 int len2;
 char buff[100];
fp=fopen(evenfile,"r");
if(fp==NULL){
printf("Error opening file %s",evenfile);
return;

}else{
 
while((read=getline(&row,&len,fp))!=-1){
   
len2=strlen(row);
row[len2-1]='\0';
if(strstr(row,"ROW")){
   
  
        read=getline(&row,&len,fp);
        if(strstr(row,"even")){
strcpy(buff,gettext(row));
strcpy(even[nr_even].text,buff);

}
          nr_even++;
         
  
   
read=getline(&row,&len,fp);
}

}
}
  
fclose(fp);
}