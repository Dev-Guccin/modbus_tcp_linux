#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <modbus.h>
#include "mysql.c"


int modbus_socket_tcp(int id, char*ip, int port, int channelindex, int framenumber){

  //socket 생성
  int   client_socket;
  struct sockaddr_in   server_addr;
  char   buf[8];

  printf("\n[+]start modbus connect to '%s:%d'\n", ip, port);
  client_socket  = socket( PF_INET, SOCK_STREAM, 0);
  if( -1 == client_socket)
  {
    printf( "socket 생성 실패\n");
    exit( 1);
  }

  memset( &server_addr, 0, sizeof( server_addr));
  server_addr.sin_family     = AF_INET;
  server_addr.sin_port       = htons(port);
  server_addr.sin_addr.s_addr= inet_addr(ip);
  if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
  {
    printf( "접속 실패\n");
    return -1;
  }
  //make buf




  printf("framenumber = %d\n", framenumber);
  //function판별을 하고 레즈스터를 보내야한다.
  for (size_t frameindex = 0; frameindex < framenumber; frameindex++) {//channel이 가지는 값중에 frame개수만큼 반복해서 통신한다.
    int functioncode = frameheap[channelindex][frameindex].func;
    buf[0] = frameheap[channelindex][frameindex].device_addr;
    printf("id : %d\n", buf[0]);
    //write( client_socket, argv[1], strlen( argv[1])+1);      // +1: NULL까지 포함해서 전송
    //read ( client_socket, buff, BUFF_SIZE);
    //printf( "%s\n", buff);
    //sql에 저장하던지 아님 데이터 전송하던지
  }
  //다 끝나면 소켓의 접속을 끝는다.
  close( client_socket);

}
int modbus_tcp(int id, char*ip, int port, int channelindex, int framenumber){
  modbus_t *ctx;
  int rc;
  int nb_fail;
  int nb_loop;
  int addr;
  int nb;
  uint16_t tab_reg[64];

  printf("MODBUS module\n");
  ctx = modbus_new_tcp(ip, port);//해당 ip와 port로 소켓 통신 열기
  modbus_set_debug(ctx, true);

  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n",
    modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
  }
  printf("socket complete!!\n");

  for (int frmaeindex = 0; frmaeindex < framenumber; frmaeindex++) {
    //set slave id = index_id
    modbus_set_slave(ctx, frameheap[channelindex][frmaeindex].device_addr);
    //rc = modbus_read_input_registers(ctx, 200, 8, tab_reg);//ctx, address, length, buf
    rc = modbus_read_registers(ctx, frameheap[channelindex][frmaeindex].start_addr, frameheap[channelindex][frmaeindex].point_count, tab_reg);//ctx, address, length, buf
    if (rc == -1) {
      fprintf(stderr, "%s\n", modbus_strerror(errno));
      return -1;
    }

    for (int i=0; i < rc; i++) {
      printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
    }

    modbus_close(ctx);
    modbus_free(ctx);
  }



}
int main(void) {
  //함수가 시작되면 mysql에 접근하여 각 sql의 값들을 정형화된 형태로 만든다.
  int available_number = init();
  printf("\n\n[+] init done");

  int channelnumber = get_channel_number(); // 채널의 개수를 구한다.
  printf("Channelnumber %d\n\n", channelnumber);

  for (int channelindex = 0; channelindex < channelnumber; channelindex++) {
    int framenumber = get_frame_number_from_channel(channelheap[channelindex].index_id);//frame의 개수를 찾아온다.
    modbus_tcp(channelheap[channelindex].index_id, channelheap[channelindex].ip, channelheap[channelindex].port, channelindex, framenumber);
    //modbus_tcp(channelheap[channelindex].index_id, channelheap[channelindex].ip, channelheap[channelindex].port, channelindex, framenumber);


  }

  for (size_t i = 0; i < available_number; i++) {
    //차례로 데이터를 전송하면 된다.

  }
}
