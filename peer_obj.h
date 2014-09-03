#ifndef BUFLEN
#define BUFLEN 100
#endif
typedef struct yadayada{
struct yadayada *next;
char str[100];
int str_id;
} string_list;

typedef struct yada
{
    int id;			/* peer id */
    char my_host[BUFLEN];	/* this peer's host name */ 
    int my_port;		/* this peer's host port number */
    char next_host[BUFLEN];	/* next peer's host name */
    int next_port;		/* next peer's port number */
    int max_id;			/* peer's max id */
    string_list* strings;
} peer_obj;

