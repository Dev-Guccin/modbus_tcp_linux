
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>


enum {
    TCP,
    TCP_PI,
    RTU
};


int main(int argc, char *argv[]){
  int use_backend;
  modbus_t *ctx;
  uint8_t *query;
  int header_length;


	argv[1] = "tcp";
	if (argc > 1) {//인자를 통해서 어떤 통신인지 설정한다.
        if (strcmp(argv[1], "tcp") == 0) {
            use_backend = TCP;
        } else if (strcmp(argv[1], "tcppi") == 0) {
            use_backend = TCP_PI;
        } else if (strcmp(argv[1], "rtu") == 0) {
            use_backend = RTU;
        } else {
            printf("Usage:\n  %s [tcp|tcppi|rtu] - Modbus client for unit testing\n\n", argv[0]);
            exit(1);
        }
    } else {
        /* By default */
        use_backend = TCP;
    }
    // 결과적으로 어떤 통신으로 연결할지 정한다.
    if (use_backend == TCP) {
        ctx = modbus_new_tcp("127.0.0.1", 1502);
        query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    } else{
        ctx = modbus_new_tcp_pi("::0", "1502");
        query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    }

    header_length = modbus_get_header_length(ctx);
    modbus_set_debug(ctx, TRUE);
    printf("%d\n", header_length);

    if (modbus_connect(ctx) == -1) {//모드버스 연결을 설정한다.
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }







    modbus_close(ctx);
	modbus_free(ctx);
}
