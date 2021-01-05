#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>


// MYSql
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "123123"
#define DB_NAME "4m"
#define CHOP(x) x[strlen(x) - 1] = ' '

typedef struct _Channel_struct{
  int index_id;
  char name[40];
  int activate;
  char com_type[30];
  char ip[17];
  int port;
  int query_period;
  int max_wait_time;
  //Frame* jmp;
}Channel;

typedef struct _Frame_struct{
  int index_id;
  int index_c;
  char frame_name[30];
  int func;
  int active_status;
  int device_addr;//slave id
  int start_addr;//address
  int point_count;//quantity
  int assigned_count;
}Frame;

typedef struct _Tcp_data_struct{
  unsigned char functioncode;
  unsigned char start_addr[2];
  unsigned char length[2];
  unsigned char count;
}TcpData;

MYSQL       *connection=NULL, conn;
MYSQL_RES   *sql_result;
MYSQL_ROW   sql_row;
int       query_stat;

Channel* channelheap;
Frame** frameheap;
TcpData * tcpheap;

int get_frame_number(int channelnumber){
  int totalnumber=0;
  // 사용 가능한 channelid를 확인해야함.
  printf("[+] frame heap allocate\n");
  printf("[+] channelnumber = %d\n", channelnumber);
  printf("[+] frameheap size = %d\n", sizeof(frameheap));
  printf("[+] frame size = %d\n", sizeof(Frame*));
  printf("[+] total size = %d\n", sizeof(Frame*)*channelnumber);
  frameheap = (Frame**)malloc(sizeof(Frame*) * channelnumber);

  for (int i = 0; i < channelnumber; i++) {
    //heap의 아이디를 순서대로 받아온다.
    int id = channelheap[i].index_id;
    int framenumber=0;
    //id를 토대로 sql 검색한다.
    char query[50];

    // id로 해당 쿼리의 개수를 확인한다.
    printf("select! id =%d\n", id);
    sprintf(query,"SELECT * FROM frame WHERE index_c=%d", id);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
      fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
      return 1;
    }
    sql_result = mysql_store_result(connection);//sql에 던진 쿼리를 받아온다.
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
    {framenumber++;}
    //개수 확인이 끝나면 heap을 할당한다.
    printf("%d ", i);
    frameheap[i] = (Frame*)malloc(sizeof(Frame)*framenumber);
    mysql_free_result(sql_result);//result에 대한 메모리 내용을 정리한다.


    //다시 쿼리를 날려서 해당 데이터를 토대로 값을 입력한다.
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
      fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
      return 1;
    }
    sql_result = mysql_store_result(connection);//sql에 던진 쿼리를 받아온다.
    int frameindex=0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
    {
      Frame tmpframe;
      tmpframe.index_id = atoi(sql_row[0]);
      tmpframe.index_c = atoi(sql_row[1]);
      memcpy(&tmpframe.frame_name, sql_row[2], sizeof(tmpframe.frame_name));
      tmpframe.func = atoi(sql_row[3]);
      tmpframe.active_status = atoi(sql_row[4]);
      tmpframe.device_addr = atoi(sql_row[5]);
      tmpframe.start_addr = atoi(sql_row[6]);
      tmpframe.point_count = atoi(sql_row[7]);
      tmpframe.assigned_count = atoi(sql_row[8]);
      printf("%d %d %s %d\n", tmpframe.index_id, tmpframe.index_c, tmpframe.frame_name, tmpframe.func);
      memcpy(&frameheap[i][frameindex++], &tmpframe, sizeof(tmpframe));
      totalnumber++;
    }
  }
  return totalnumber;//성공!
}


int get_channel_allocate(){
  query_stat = mysql_query(connection, "select * from channel");
  if (query_stat != 0)
  {
    fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
    return 1;
  }
  sql_result = mysql_store_result(connection);//sql에 던진 쿼리를 받아온다.

  int Channelnumber = 0;
  while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
  {
    //activate의 개수만 확인한다.
    Channelnumber++;
  }
  printf("[+] channel number = %d\n", Channelnumber);
  printf("[+] channel size = %d\n", sizeof(Channel));
  mysql_free_result(sql_result);//result에 대한 메모리 내용을 정리한다.
  channelheap = malloc(sizeof(Channel) * Channelnumber);
  return Channelnumber;
}
int get_channel_number(){
  query_stat = mysql_query(connection, "select * from channel");
  if (query_stat != 0)
  {
    fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
    return 1;
  }
  sql_result = mysql_store_result(connection);//sql에 던진 쿼리를 받아온다.

  int Channelnumber = 0;
  while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
  {
    //activate의 개수만 확인한다.
    Channelnumber++;
  }
  printf("[+] channel number = %d\n", Channelnumber);
  printf("[+] channel size = %d\n", sizeof(Channel));
  mysql_free_result(sql_result);//result에 대한 메모리 내용을 정리한다.
  return Channelnumber;
}
int get_all_channel(){ // channel의 데이터를 받아서 구조체에 삽입한다.
  query_stat = mysql_query(connection, "select * from channel");
  if (query_stat != 0)
  {
    fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
    return 1;
  }
  sql_result = mysql_store_result(connection);//sql에 던진 쿼리를 받아온다.

  int channelindex =0;
  while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
  {
    Channel tmpchannel;
    tmpchannel.index_id = atoi(sql_row[0]);
    memcpy(&tmpchannel.name, sql_row[1], 40);
    tmpchannel.activate = atoi(sql_row[2]);
    memcpy(&tmpchannel.com_type, sql_row[3], 30);
    memcpy(&tmpchannel.ip, sql_row[4], 17);
    tmpchannel.port = atoi(sql_row[5]);
    tmpchannel.query_period = atoi(sql_row[6]);
    tmpchannel.max_wait_time = atoi(sql_row[6]);

    //printf("%d", tmpchannel.index_id);
    printf("%d %s %d %s %s\n", tmpchannel.index_id, tmpchannel.name, tmpchannel.activate, tmpchannel.com_type, tmpchannel.ip);
    memcpy(&channelheap[channelindex++], &tmpchannel, sizeof(tmpchannel));
    int tm=channelindex -1;
    printf("Channel !!!!\n%d %s %d %s %s\n", channelheap[tm].index_id, channelheap[tm].name, channelheap[tm].activate, channelheap[tm].com_type, channelheap[tm].ip);

    //if (tmpchannel.activate == 1){//실행중인 channel만 동적으로 할당된 배열에 넣어준다.
    //}
  }
  mysql_free_result(sql_result);//result에 대한 메모리 내용을 정리한다.
}

unsigned short ReverseByteOrder(unsigned short value) {
	unsigned short ret = 0;
	((char*)&ret)[0] = ((char*)&value)[1];
	((char*)&ret)[1] = ((char*)&value)[0];
	return ret;
}
int make_data(int channelnumber,int totalnumber){
  //total 값을 아니까 data로 그냥 다 만드렁버려?
  tcpheap = (TcpData*)malloc(sizeof(TcpData)*totalnumber);
  int totalindex = 0;
  //channel의 개수와 frame의 개수를 토대로 loop를 만든다.
  printf("channelnumber : %d \n", channelnumber);
  printf("frameheap : %p\n", frameheap);
  for (int i = 0; i < channelnumber; i++) {
    //frame의 개수를 알아야하는데 heap에서 그게 안되네 짜증나게
    //할수없이 직접 mysql에서 구해야댐;;
    // id로 해당 쿼리의 개수를 확인한다.
    if(channelheap[i].activate == 1){//동작허용된 channel만 골라낸다.
      int id = channelheap[i].index_id;
      int framenumber = get_frame_number_from_channel(id);
      printf("[+] frame number : %d\n", framenumber);
      for (size_t j = 0; j < framenumber; j++) {//frame heap에서 값가져와서 만들기
        if(frameheap[i][j].active_status == 1){//동작허용된 frame만 골라낸다.
          unsigned short tmpstart_addr = ReverseByteOrder((unsigned short)frameheap[i][j].start_addr);
          printf("start_addr :%4x\n", tmpstart_addr);
          unsigned short tmplength = ReverseByteOrder((unsigned short)frameheap[i][j].assigned_count);
          printf("length :%4x\n", tmplength);
          tcpheap[totalindex].functioncode = frameheap[i][j].func;
          memcpy(&tcpheap[totalindex].start_addr, &tmpstart_addr,2);
          memcpy(&tcpheap[totalindex].length, &tmplength,2);
          memcpy(&tcpheap[totalindex].count, &frameheap[i][j].point_count,1);
          totalindex++;
        }
      }
    }
  }
  printf("[+] available totalindex = %d\n", totalindex);
  return totalindex;
}

int get_frame_number_from_channel(int id){
  char query[50];
  int framenumber=0;
  sprintf(query,"SELECT * FROM frame WHERE index_c=%d", id);
  query_stat = mysql_query(connection, query);
  if (query_stat != 0)
  {fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
    return 1;}
  sql_result = mysql_store_result(connection);//sql에 던진 쿼리를 받아온다.
  while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
  {framenumber++;}
  //개수 확인이 끝나면 heap을 할당한다.
  mysql_free_result(sql_result);//result에 대한 메모리 내용을 정리한다.
  return framenumber;
}
int init(){
  char name[12];
  char address[80];
  char tel[12];
  char query[255];

  mysql_init(&conn);
  connection = mysql_real_connect(&conn, DB_HOST,DB_USER,DB_PASS,DB_NAME,3306,(char*)NULL,0);
  if (connection == NULL)
  {
    fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
    return 1;
  }
  int channelnumber = get_channel_allocate();
  get_all_channel();
  int totalnumber = get_frame_number(channelnumber);
  //사용가능한 애들만 패킷으로 만들어야한다.
  int available_number = make_data(channelnumber,totalnumber);

  printf("^^^^^^^^^^6 %d ^^^^^^^^^^\n ", channelheap[0].index_id);
  printf("%p\n", channelheap);
  return available_number;
}
