/*#########################################################################
# make_query.c
# Author: Leah Brown
# Project Description and Socket Code: Dr. M. McAllister
# Class: CSCI 3171
# Due Date: Wednesday, February 22, 2012 (9:00p.m.)
#
# code handles all requests from the peer to another peer.
##########################################################################*/



#ifndef basics

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define basics
#endif

#ifndef peerobjh
#include "peer_obj.h"
#define peerobjh
#endif

#ifndef m_query
#define m_query
#include "make_query.h"
#endif

//#ifndef peerobjs
#define PRTCL "3171_a3/1.0"
#define CRET "\r\n"
//#define PORT 30462
#define MAX_STR 20
#define BUFLEN 100
#define peerobjs
//#endif


/*id request
* creates the id query and deals with its response
* ARGUMENTS
* peer - peer sending id query
* host, port- hostname, and port number to query for id
* id - id being queried for
* redir_host - queried peer's next host if not responsible
* redir_port - queried peer's next port if not responsible
* RETURN
* 1 the peer is responsible, 0 otherwise
* redir_host, redir_port, through argument pointers
*/

int
id_request (char *host, int port, int id, char *redir_host, int *redir_port)
{
    char outbuf[MAX_STR][BUFLEN];
    char inbuf[MAX_STR][BUFLEN];
    int val;
    char temp[BUFLEN];
    char temp2[BUFLEN];
    sprintf (outbuf[0], "ID %s 0 %d\r\n", PRTCL, id);
    char temp3[BUFLEN];
//sprintf (outbuf[0], "ID %s 0 %d%s" PRTCL, id, CRET);
    send_request (host, port, outbuf, 1, inbuf);
    printf ("inbuf: %s\n", inbuf[0]);
    sscanf (inbuf[0], "%s %s %s %d", temp, temp2, temp3, &val);

    printf ("returned value is: %d\n", val);
    if (val / 100 == 2) {

	(*redir_port) = port;
	strcpy (redir_host, host);
	return 1;
    }
    else {

	printf ("inbuf1: %s\n", inbuf[1]);
	sscanf (inbuf[1], "%s %s", temp, temp2);
	(*redir_port) = to_num (temp2);
	strcpy (redir_host, temp);
	return 0;
    }

}

/*pull request
* creates the pull query and deals with its response
* pull strings with id value >=calling peer's id from called peer 
* ARGUMENTS
* peer - peer sending pull request
* host, port - hostname and port number to pull strings from
* outbuf - buffer holding pulled strings
* RETURN
* lines - number of lines pulled
*/

//id val strings equal or over
int
pull_request (peer_obj * peer, char *host, int port, char out[][BUFLEN])
{

    char outbuf[MAX_STR][BUFLEN];
    char inbuf[MAX_STR][BUFLEN];
    int lines;
    char temp[BUFLEN];
    char temp2[BUFLEN];
    int i;
    sprintf (outbuf[0], "PULL %s 1 %d\r\n", PRTCL, peer->id);
    sprintf (outbuf[1], "%s %d\r\n", peer->my_host, peer->my_port);

//sprintf (outbuf[0], "ID %s 0 %d%s" PRTCL, id, CRET);
    send_request (host, port, outbuf, 2, inbuf);
    printf ("inbuf: %s\n", inbuf[0]);
    sscanf (inbuf[0], "%s %s %d", temp, temp2, &lines);
    for (i = 1; i < lines + 1; i++)
	strcpy (out[i - 1], inbuf[i]);
    return lines;
}


/*next request
* creates the next query and deals with its response
* ARGUMENTS
* peer - peer sending next request
* host, port, id - hostname, port &id  number of the peer's next peer
* RETURN
* nothing directly
* hostname, port, and id number are returned through argument  pointers
*/


void
next_request (peer_obj * peer, char *next_host, int *next_port, int *id)
{

    char outbuf[MAX_STR][BUFLEN];
    char inbuf[MAX_STR][BUFLEN];
    char val[BUFLEN];
    char temp[BUFLEN];
    int temp2;
    int temp3;
    char buf[BUFLEN];
    sprintf (outbuf[0], "NEXT %s 0\r\n", PRTCL);
//sprintf (outbuf[0], "ID %s 0 %d%s" PRTCL, id, CRET);
    if (strcmp (peer->next_host, peer->my_host) == 0
	&& peer->my_port == peer->next_port) {
	*next_port = peer->next_port;
	*id = peer->id;
	strcpy (next_host, peer->next_host);
    }
    else {
	send_request (peer->next_host, peer->next_port, outbuf, 1, inbuf);
	printf ("inbuf: %s\n", inbuf[0]);
	printf ("inbuf: %s\n", inbuf[1]);
	sscanf (inbuf[1], "%s %d %d", temp, &temp2, &temp3);

	(*next_port) = temp2;
	(*id) = temp3;
	strcpy (next_host, temp);
    }

    return;

}

/*done request 
* creates the done query (no response)
* ARGUMENTS
* peer - peer peer sending done request
* host, port - host & port number of peer receiving the query
* RETURN
* nothing 
*/
void
done_request (peer_obj * peer, char *host, int port)
{
    char outbuf[MAX_STR][BUFLEN];
    char inbuf[MAX_STR][BUFLEN];
    sprintf (outbuf[0], "DONE %s 0 %d\r\n", PRTCL, peer->id);
    send_request (host, port, outbuf, 1, inbuf);
    return;
}



/*push request 
* creates the push query, and deals with its response
* ARGUMENTS
* peer - peer peer pushing its strings to another peer
* host, port - host & port number of peer who will receive strings
* strings - buffer of strings to push to peer
* nstrings - number of strings in strings
* RETURN
* 1 if successful, 0 if not (i.e. received error)
*/

int
push_request (peer_obj * peer, char *host, int port, char strings[][BUFLEN],
	      int nstrings)
{


    char outbuf[MAX_STR][BUFLEN];
    char inbuf[MAX_STR][BUFLEN];
    int val;
    char temp[BUFLEN];
    char temp2[BUFLEN];
    char temp3[BUFLEN];
    char buf[BUFLEN];
    int i;
    sprintf (outbuf[0], "PUSH %s %d %d\r\n", PRTCL, nstrings, peer->id);
    for (i = 1; i < nstrings + 1; i++)
	sprintf (outbuf[i], "%s\r\n", strings[i - 1]);

    send_request (peer->next_host, peer->next_port, outbuf, 1, inbuf);
    sscanf (inbuf[1], "%s %s %s %d", temp, temp2, temp3, &val);

    return (val / 100 == 2);



}

/* send_request
* sends a request to the specified peer and gets a response 
* ARGUMENTS
* host, port - host & port number of peer who will receive the request
* outbuf - buffer holding queries to be send to receiving peer
* outlines - number of lines in the outbuf
* inbuf - buffer holding responses incoming from the receiving peer
* RETURN 
* number of lines in inbuf
*/
int
send_request (char *host, int port, char outbuf[][BUFLEN],
	      int outlines, char inbuf[][BUFLEN])
{
    int sock;
    struct sockaddr_in server;
    struct hostent *hp, *gethostbyname ();
    char buf[BUFLEN];
    int rval;
#ifdef use_senK
    int bytes_sent;
#endif

    printf ("\n\n------------SEND REQUEST----------------------\n");
/* Create socket */
    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
	perror ("opening stream socket");
	exit (1);
    }
    server.sin_family = AF_INET;
//hp = gethostbyname (host_to_contact);
    hp = gethostbyname (host);
    if (hp == 0) {
	fprintf (stderr, "localhost: unknown host\n");
	exit (2);
    }
    bcopy (hp->h_addr, &server.sin_addr, hp->h_length);
//server.sin_port = htons (MPORT);
    server.sin_port = htons (port);

/* Connect to the far end server. */

    if (connect (sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
	perror ("connecting stream socket");
	exit (1);
    }
    int bytes_sent;
    int i = 0;

/* I have a connection open.  Send all lines in outbuf */
    printf ("These are the lines to send awaY: \n");
    for (i = 0; i < outlines; i++)
	printf ("%s\n", outbuf[i]);
    for (i = 0; i < outlines; i++) {
#ifdef use_send
	bytes_sent = send (sock, outbuf[i], strlen (outbuf[i]) + 1, 0);
	printf ("send message on bytes sent: \"%s\"\n", outbuf[i]);
	printf ("bytes sent: %d\n", bytes_sent);
	if (bytes_sent < 0) {
	    perror ("writing on stream socket");
	}
#else
	bytes_sent = write (sock, outbuf[i], strlen (outbuf[i]) + 1);
	printf ("send message on not bytes sent: \"%s\"\n", outbuf[i]);
	printf ("bytes sent: %d\n", bytes_sent);
	if (bytes_sent < 0)
	    perror ("writing on stream socket");
#endif
	sleep (3);
    }

/* now that we sent, look for a reply */
    i = 0;
    bzero (inbuf[0], sizeof (inbuf[0]));
/* get first response, find out how many lines are coming after */
    rval = read (sock, inbuf[0], BUFLEN);
    inbuf[0][strlen (inbuf[0]) - 2] = '\0';
    if (rval < 0) {
	perror ("reading stream message");
    }
    else {
	printf ("Server says: \"%s\"using %d bytes, %d characters\n",
		inbuf[0], rval, strlen (inbuf[0]));
    }
    char temp[BUFSIZ];
    char temp2[BUFSIZ];
    int inlines;
    sscanf (inbuf[0], "%s %s %d", temp, temp2, &inlines);
    printf ("lines in : %d\n", inlines);
    printf ("inbuf: \"%s\"\n", inbuf[0]);
//original command is already in buffer
/* get rest of lines in buffer */
    for (i = 1; i < inlines + 1; i++) {
	bzero (inbuf[i], sizeof (inbuf[i]));
	rval = read (sock, inbuf[i], BUFLEN);
	inbuf[i][strlen (inbuf[i]) - 2] = '\0';
	if (rval < 0) {
	    perror ("reading stream message");
	}
	else {
	    printf ("Server replies: \"%s\" with %d bytes, %d chars\n",
		    inbuf[i], rval, strlen (inbuf[i]));
	}




/*	bzero (buf, sizeof (buf));
	rval = read (sock, buf, MAX_RESPONSE);
	if (rval < 0) {
	    perror ("reading stream message");
	}
	else {
	    printf ("Server says: \"%s\"\n", buf);
	}
*/
    }
    printf ("--------------------------------------------\n");
    close (sock);

    return inlines + 1;
}

int
to_num (char *str)
{
    if (strcmp (str, "0") == 0)
	return 0;
    else if (atoi (str))
	return atoi (str);

    else
	return -1;
}
