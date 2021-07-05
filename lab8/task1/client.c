#include <sys/types.h>
#include <sys/socket.h> //Семейства адресов
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define SOCK_NAME "mysocket.s"

//SOCK_DGRAM - поддерживает двусторонний поток данных. Не гарантируется, что этот поток будет последовательным, надежным, и что данные не будут дублироваться. Важной характеристикой данного сокета является то, что границы записи данных предопределены.
//Функция socket создает конечную точку соединения и возвращает файловый дескриптор, ссылающийся на сокет, или -1 в случае ошибки. Данный дескриптор используется в дальнейшем для установления связи.
//В большинстве реализаций UNIX, доменные датаграмные сокеты UNIX всегда надёжны и не меняют порядок датаграмм)
//Семейство протоколов = семейство адресов (как правило) = домен соединения = тип соединения
//AF_UNIX - Домен, обозначенный константой AF_UNIX, соответствует сокетам в файловом пространстве имен UNIX
int main()
{
    //создаёт конечную точку соединения и возвращает файловый дескриптор.
    int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        perror("socket failed");
        return EXIT_FAILURE;
    }
    
    struct sockaddr srvr_name;
    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);
    
    char buf[100];
    scanf("%99s", buf);
    sendto(sock_fd, buf,strlen(buf), 0, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family));
    
    printf("Client sent: %s\n", buf);
    return 0;
}
