
/*#########################################################################
# peer.c
# Author: Leah Brown
# Project Description and Socket Code: Dr. M. McAllister
# Class: CSCI 3171
# Due Date: Wednesday, February 22, 2012 (9:00p.m.)
#
# code to create a peer, manage incoming queries for the peer, as well as
# initialize requests to other peers
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
#else
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

#ifndef h_query
#define h_query
#include "handle_query.h"
#endif

#define PRTCL "3171_a3/1.0"
#define CRET "\r\n"
#define PORT 30460
#define MAX_STR 20
#define BUFLEN 100
#define peerobjs

int check_request (int *, int *, char *, char *);
int init_peer (peer_obj *, int, char **);
void print_peer (peer_obj * peer);
int get_int (char *);

int
main (int argc, char **argv)
{
    peer_obj *peer;
    peer = (peer_obj *) malloc (sizeof (peer_obj));
    int k;
    int lines;
    int outlines;
    int next_id;
    int flag;
    char inlinebuf[MAX_STR][BUFLEN];
    char outbuf[MAX_STR][BUFLEN];
    char input[BUFLEN];
    char request[BUFLEN];
    char next_host[BUFLEN];
    int bytes_written;
/* socket vars */
    int sock;
    struct sockaddr_in server;
    int msgsock;
    int rval;
    int val;
/* hold host and port number after a pull command and before a done command */
    char pull_host[BUFLEN];
    int pull_port;
    if (init_peer (peer, argc, argv) != 1)	// may end up being 10
    {				//initialize peer
	printf ("Peer initialization failed\n");
	free (peer);
	exit (1);
    }

//    strcpy (peer->next_host, "bluenose.cs.dal.ca");
//    peer->next_port = 30500;


    int next_port;
/*    printf ("requesting id");
    if (id_request
	(peer->next_host, peer->next_port, 13, next_host, &next_port) == 1)
	printf ("Correct peer\n");
    else
	printf ("Redirect: %s, port  %d\n", next_host, next_port);
    int rnext_id;
    next_request (peer, next_host, &next_port, &rnext_id);
    printf ("next host: %s, next port: %d, next id: %d\n", next_host,
	    next_port, rnext_id);

*/


/* adding strings for testing*/
    print_peer (peer);
/* Create socket */
    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
	perror ("opening stream socket");
	exit (1);
    }

/* Name socket using wildcards. */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons (peer->my_port);
    printf ("%d\n", peer->my_port);

    if (bind (sock, (struct sockaddr *) &server, sizeof (server))) {
	perror ("binding stream socket");
	exit (1);
    }

/* Allow the socket to be re-used immediately once the server ends. */
    val = 1;
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (void *) &val, sizeof (val));

    int j;
    int s;
/* Start accepting connections */
    listen (sock, 5);
    do {

	msgsock = accept (sock, 0, 0);
	if (msgsock == -1)
	    perror ("accept");
	else {
	// get request
	    bzero (input, sizeof (input));
	    j = 0;
	    s = 0;
	    rval = read (msgsock, input, BUFLEN);
	    if (rval < 0) {
		perror ("reading stream message");
	    }
	    else {
		printf ("Request: \"%s\" using %d bytes, getting %d\n", input, rval, strlen(input));
/*		if (strlen (input) + 3 < rval) {

		    printf ("actual request: \n");
		    int r;
		    printf ("%c\n", input[0]);
		    for (r = 0; r < rval; r++) {

			printf ("%c", input[r]);
			if (input[r] == '\n') {
			    printf ("NEW LINE CHARACTER\n");
			    if (j > 0) {
				inlinebuf[j][s] = '\n';
				inlinebuf[j][s + 1] = '\0';
				printf ("Extracted line: %s\n", inlinebuf[j]);
			    }
			    j++;
			    s = 0;
			}
			if (j > 0) {
			    inlinebuf[j][s] = input[r];
			    s++;
			}


		    }
		    j--;

		    printf ("\n");
		}
	    }
*/
	    flag = 0;
	    if (!check_request (&next_id, &lines, request, input))
		flag = 1;

	// get any extra lines from the request
	    for (k = j; k < lines; k++) {
		bzero (input, sizeof (input));
		rval = read (msgsock, input, BUFLEN);
		if (rval < 0) {
		    perror ("reading stream message");
		}
		else {

		    printf ("Request %d: \"%s\"\n", k, input);
		    input[strlen (input) - 2] = '\0';
		    strcpy (inlinebuf[k], input);
		}
	    }
	    if (flag == 1) {
		sprintf (outbuf[0], "%s %s 0 400 bad query\r\n", PRTCL,
			 request);
		outlines = 1;
	    }
	// not process
	    else if (strcmp (request, "ID") == 0) {
		outlines = query_id (peer, next_id, outbuf);
	    }
	    else if (strcmp (request, "NEXT") == 0) {
		printf ("Got  next request\n");
		outlines = query_next (peer, outbuf);
	    }
	    else if (strcmp (request, "ADD") == 0) {
	    // need to get next peer's id
		outlines = query_add (peer, inlinebuf[0], outbuf);
	    }
	    else if (strcmp (request, "QUERY") == 0) {
		if (lines != 1)
		    exit (1);
		outlines = query_string (peer, inlinebuf[0], outbuf);
	    }
	    else if (strcmp (request, "DELETE") == 0) {
		outlines = query_delete (peer, inlinebuf[0], outbuf);
	    }
	    else if (strcmp (request, "PULL") == 0) {
		outlines = query_pull (peer, next_id, outbuf);
		printf ("inline buf for pull: %s\n", inlinebuf[0]);
		sscanf (inlinebuf[0], "%s %d", pull_host, &pull_port);
	    }
	    else if (strcmp (request, "DONE") == 0) {
		outlines = query_done (peer, next_id, outbuf);
		strcpy (peer->next_host, pull_host);
		peer->next_port = pull_port;
	    }
	    else if (strcmp (request, "PUSH") == 0) {
		outlines = query_push (peer, inlinebuf, lines, outbuf);
		print_peer (peer);
	    }
	    else
		sprintf (outbuf[0], "%s %s 0 400 bad query\r\n", PRTCL,
			 request);
	    for (k = 0; k < outlines && strcmp (request, "DONE") != 0; k++) {
		if (bytes_written =
		    write (msgsock, outbuf[k], strlen (outbuf[k]) + 1) < 0) {
		    perror ("writing on stream socket");
		}
		printf ("Wrote message \"%s\"with %d bytes\n", outbuf[k],
			bytes_written);
		sleep (3);
	    }
	    print_peer (peer);
	}


	close (msgsock);
    }
    while (1);

    close (sock);

    return 1;

}

int
check_request (int *next_id, int *lines, char *request, char *input)
{
    int ntoks;
    char ipbuf[5][BUFLEN];
    input[strlen (input) - 2] = '\0';
    ntoks =
	sscanf (input, "%s %s %s %s %s", ipbuf[0], ipbuf[1], ipbuf[2],
		ipbuf[3], ipbuf[4]);
    strcpy (request, ipbuf[0]);
    if (strcmp (ipbuf[1], PRTCL) != 0) {	// f protocol is incorrect, error
	printf ("Protocol incorrect\n");
	printf ("Protocol given: \"%s\", protocol required: \"%s\"\n",
		ipbuf[1], PRTCL);
	printf ("At 0: \"%s\", protocol required: \"%s\"\n", ipbuf[0]);
	return 0;
    }
    if ((*lines = get_int (ipbuf[2])) < 0) {
	printf ("Line value not an integer\n");
	return 0;
    }
    if (ntoks == 4)
	if ((*next_id = get_int (ipbuf[3])) < 0) {	// if next port number not  anumber, error
	    printf ("Next peer's port not a number\n");
	    return 0;
	}


    return 1;


}

/* inititalizes the peer, using parameters from stdin */
/* lots of error checking */
int
init_peer (peer_obj * peer, int argnum, char **args)
{
    if (!(argnum == 10 || argnum == 13))	//may later be 10
    {
	printf ("Insufficient number of arguments: received %d\n", argnum);
	return 0;
    }
    int params_found = 0;
    int i;
    int temp;
    int first = 0;
    int port;
    char host[BUFSIZ];
    for (i = 1; i < argnum; i += 2) {
/* check to make sure we didn't forget a flag! */
	if (strcmp (args[i], "-f") == 0) {
	    printf ("F FLAG SEEN \n");
	    first = 1;
	}

	else if (args[i + 1][0] == '-') {
	    printf ("Missing argument for parameter %s\n", args[i]);
	    return 0;
	}
/* check for empty flat g "-" */
	else if (strcmp (args[i], "-") == 0) {
	    printf ("Unrecognized flag \"-\"\n");
	    return 0;
	}
/* id flag */
	else if (strcmp (args[i], "-i") == 0) {
	    if (get_int (args[i + 1]) >= 0)
		peer->id = atoi (args[i + 1]);
	    else {
		printf ("Invalid ID\n");
		return 0;
	    }
	}
/* port number */
	else if (strcmp (args[i], "-m") == 0) {
	    if (get_int (args[i + 1]) >= 0)
		peer->max_id = atoi (args[i + 1]);
	    else {
		printf ("Invalid max id\n");
		return 0;
	    }
	}
/* host */
	else if (strcmp (args[i], "-h") == 0)
	    strcpy (peer->my_host, args[i + 1]);
/* port number */
	else if (strcmp (args[i], "-p") == 0) {
	    if (get_int (args[i + 1]) >= 0)
		peer->my_port = atoi (args[i + 1]);
	    else {
		printf ("Invalid peer port number\n");
		return 0;
	    }
	}
/* host */
	else if (strcmp (args[i], "-r") == 0)
	    strcpy (host, args[i + 1]);
/* port number */
	else if (strcmp (args[i], "-s") == 0) {
	    if (get_int (args[i + 1]) >= 0)
		port = atoi (args[i + 1]);
	    else {
		printf ("Invalid peer port number\n");
		return 0;
	    }
	}
	else {
	    printf ("Unrecognized Flag\n");
	    return 0;
	}

    }


    char val[BUFLEN];
    if (first == 1) {
	strcpy (peer->next_host, peer->my_host);
	peer->next_port = peer->my_port;
    }
    else {
	strcpy (peer->next_host, host);
	peer->next_port = port;

	int peer_found = 0;
	char next_host[BUFLEN];
	int next_port;
	int tempid;
	char linebuffer[MAX_STR][BUFLEN];
	char linebuffer2[MAX_STR][BUFLEN];
	int linesout;
	printf ("Looking for peer\n");
	do {
	    peer_found = id_request
		(host, port, peer->id, next_host, &next_port);
	    strcpy (host, next_host);
	    port = next_port;

	} while (peer_found != 1);
    //DO NOT REVERSE ORDER OF THE NEXT THREE LINES, THEY ARE NEEDED FOR ADD.
	print_peer (peer);
	next_request (peer, next_host, &next_port, &tempid);
	strcpy (peer->next_host, next_host);
	peer->next_port = next_port;
	print_peer (peer);
	linesout = pull_request (peer, host, port, linebuffer);
	for (i = 0; i < linesout; i++)
	    query_add (peer, linebuffer[i], linebuffer2);
	done_request (peer, host, port);
    }

    printf ("Done Looking for peer\n");
/* next peer's port number - since we have to wait until we get the max_id, we wait til last to set this */
    if (peer->max_id <= peer->id) {
	printf ("Peer id must be less than max id\n");
	return 0;
    }
    return 1;
}

int
get_int (char *str)
{
    if (strcmp (str, "0") == 0)
	return 0;
    else if (atoi (str))
	return atoi (str);
    else
	return -1;
}

void
print_peer (peer_obj * peer)
{
    printf ("\nCurrent peer:---------------------------------\n");
    printf ("Peer ID:\t%d\n", peer->id);
    printf ("my_host: \t%s\n", peer->my_host);
    printf ("my_port: \t%d\n", peer->my_port);
    printf ("next_host: \t%s\n", peer->next_host);
    printf ("next_port: \t%d\n", peer->next_port);
    printf ("max id: \t%d\n", peer->max_id);
    printf ("----------------------------------------------\n\n");
}
