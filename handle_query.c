/*#########################################################################
# handle_query.c
# Author: Leah Brown
# Project Description and Socket Code: Dr. M. McAllister
# Class: CSCI 3171
# Due Date: Wednesday, February 22, 2012 (9:00p.m.)
#
# code to handle all incoming queries from another peer.
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

#ifndef h_query
#define h_query
#include "handle_query.h"
#endif

//#ifndef peerobjs
#define PRTCL "3171_a3/1.0"
#define CRET "\r\n"
#define PORT 30460
#define MAX_STR 20
#define BUFLEN 100
#define peerobjs
//#endif




/* query_id
* queries another peer to see if it is responsible for a certain id
* if responsible, returns ok message, otherwise, a redirect host/port
* ARGUMENTS
* peer - the peer querying
* query_id - the id being queried for 
* outbuf - buffer to hold response
* RETURN 
* number of lines in outbuf
*/
int
query_id (peer_obj * peer, int query_id, char outbuf[][BUFLEN])
{

    int i;
    int tempi;
    char temp[BUFSIZ];
    next_request (peer, temp, &tempi, &i);
    if ((peer->id == i) || (query_id >= peer->id && query_id < i)) {
	sprintf (outbuf[0], "%s ID 0 200 ok\r\n", PRTCL);
	return 1;
    }
    else {
	sprintf (outbuf[0], "%s ID 1 301 redirect\r\n", PRTCL);
	sprintf (outbuf[1], "%s %d\r\n", peer->my_host, peer->id);
	return 2;
    }
}

/* query_next
* queries for information of next peer of the calling peer 
* ARGUMENTS
* peer - the peer querying
* outbuf - buffer to hold response
* RETURN 
* number of lines in outbuf
* in outbuf, an ok message and the hostname, port, and id number of
* 	the peer's next peer
*/
int
query_next (peer_obj * peer, char outbuf[][BUFLEN])
{
    int i;
    int tempi;
    char temp[BUFSIZ];
    if (strcmp (peer->my_host, peer->next_host) == 0
	&& peer->my_port == peer->next_port)
	i = peer->id;
    else
	next_request (peer, temp, &tempi, &i);
    sprintf (outbuf[0], "%s NEXT 1 200 ok\r\n", PRTCL);

    sprintf (outbuf[1], "%s %d %d\r\n", peer->next_host, peer->next_port, i);
    return 2;
}

/* query_pull
* queries for strings >= a certain id
* ARGUMENTS 
* peer - the peer to pull strings from
* oeer_id - any strings in peer with string id >=peer_id are pulled
* outbuf - buffer holding response
* RETURN 
* number of strings pulled
* in outbuf, an ok message, followed by the pulled strings
*/
int
query_pull (peer_obj * peer, int peer_id, char outbuf[][BUFLEN])
{
    int i = 1;			// MUST BE 1 - index 0 is for response!
    string_list *itr = peer->strings;
    while (itr != NULL) {
	if (itr->str_id >= peer_id) {
	    sprintf (outbuf[i], "%s\r\n", itr->str);
	    i++;
	}
	itr = itr->next;
    }
    sprintf (outbuf[0], "%s PULL %d 200 ok\r\n", PRTCL, i - 1);

    return i;

}


/* query_done
* lets called peer know that he can delete the previously pulled strings
* ARGUMENTS
* peer - peer whose strings were pulled
* peer_id - any strings in peer with string id >= peer_id are deleted
* outbuf - buffer holding response
* RETURN 
* 0 (insignificant)
* in outbuf, an ok message
*/
int
query_done (peer_obj * peer, int peer_id, char outbuf[][BUFLEN])
{
    int strid = 0;
    int i;
    string_list *itr;
    string_list *itrprev = NULL;
    itr = peer->strings;
    while (itr != NULL) {
	if (itr->str_id >= peer_id) {
	    if (itrprev == NULL) {
		itr = itr->next;
		peer->strings = peer->strings->next;
	    }
	    else {
		itrprev->next = itr->next;
		itr = itrprev->next;
	    }
	}
	else {
	    itrprev = itr;
	    itr = itr->next;
	}
    }
    print_strings (peer);
//other wise string has not been found but belongs to us, so we add it.
    return 0;

}



/* query_push
* called peer receives strings pushed by peer
* if a parameter is missing, we error
* ARGUMENTS
* peer - peer to push strings to
* inbuf - buffer holding calling peer's next peer's information (so
*	appropriate pointers can be changed), as well as the strings
* 	to be pushed
* lines - number of lines in inbuf
* outbuf - buffer to hold response
* RETURN 
* number of strings in outbuf
* in outbuf, an ok or an error message 
*/
int
query_push (peer_obj * peer, char inbuf[][BUFLEN], int lines,
	    char outbuf[][BUFLEN])
{
    int i;
    char buf[3][BUFLEN];
    int temp1, temp2;
    if (sscanf (inbuf[0], "%s %s %s", buf[0], buf[1], buf[2]) == 3) {
	if ((temp1 = to_int (buf[1])) != -1
	    && (temp2 = to_int (buf[2])) != -1) {
	    strcpy (peer->next_host, buf[0]);
	    peer->next_port = temp1;
	    for (i = 1; i < lines; i++) {
		query_add (peer, inbuf[i], buf);
	    }
	    sprintf (outbuf[0], "%s PUSH 0 200 ok\r\n", PRTCL);

	    return 1;
	}
    }
    sprintf (outbuf[0], "%s PUSH 0 400 missing info\r\n", PRTCL);

    return 1;

}

int
to_int (char *str)
{
    if (strcmp (str, "0") == 0)
	return 0;
    else if (atoi (str))
	return atoi (str);

    else
	return -1;
}



/* query_string
checks to see if a peer contains a specified string
* returns appropriate errors if the string's id is out of it's range,
*	string is a duplicate, or if not found 
* ARGUMENTS 
* peer - peer to be queried
* string - string to search for
* outbuf - buffer holding response
* RETURN
* number of strings in outbuf
* in outbuf, appropriate ok/error response
*/
int
query_string (peer_obj * peer, char *string, char outbuf[][BUFLEN])
{
    int i;
    int next_peer_id;
    int tempi;
    char temp[BUFSIZ];
    next_request (peer, temp, &tempi, &next_peer_id);
    int strid = 0;
    string_list *itr;
    itr = peer->strings;
    for (i = 0; i < strlen (string); i++)
	strid += (int) string[i];
    strid = strid % peer->max_id;
    if ((strid < peer->id || strid >= next_peer_id)
	&& peer->id != next_peer_id) {
	sprintf (outbuf[0], "%s QUERY 0 400 notmine\r\n", PRTCL);
	return 1;
    }
    else if ((itr != NULL))	//if list is not empty, we look for it
    {
	while (itr != NULL) {
	    if (strcmp (itr->str, string) == 0) {
		sprintf (outbuf[0], "%s QUERY 0 200 ok\r\n", PRTCL);
		return 1;
	    }
	    else
		itr = itr->next;
	}
    }
    sprintf (outbuf[0], "%s QUERY 0 201 not present\r\n", PRTCL);
//other wise string has not been found but belongs to us, so we add it.
    print_strings (peer);
    return 1;
}


/* query_add
* adds a string to a peer, if not a duplicate and if its string id is in the
* 	peer's acceptable range 
* ARGUMENTS
* peer - peer to add string in 
* string - string to add
* outbuf - buffer holding response
* RETURN
* number of strings in outbuf
* in outbuf, appropriate ok/error response
*/
int
query_add (peer_obj * peer, char *string, char outbuf[][BUFLEN])
{

    printf ("string to add: %s\n", string);
    int i;
    int next_peer_id;
    int strid = 0;
    int tempi;
    char temps[BUFSIZ];
    strcpy (temps, peer->next_host);
    tempi = peer->next_port;
    next_request (peer, temps, &tempi, &next_peer_id);
    string_list *itr;
    itr = peer->strings;
// get string id
    for (i = 0; i < strlen (string); i++)
	strid += (int) string[i];
    strid = strid % peer->max_id;
// if string id in wrong range, return 4xx error
    if (!((strid >= peer->id && strid < next_peer_id)
	  || peer->id == next_peer_id || (strid >= peer->id
					  && next_peer_id < peer->id))) {
	sprintf (outbuf[0], "%s ADD 0 400 notmine\r\n", PRTCL);
	printf ("%s\n", outbuf[0]);
	print_strings (peer);
	return 1;
    }
// otherwise, we check to see if we ready have it
    else if ((itr != NULL))	//if list is not empty, we look for it
    {
	while (itr != NULL) {
	    if (strcmp (itr->str, string) == 0) {
		sprintf (outbuf[0], "%s ADD 0 202 duplicate\r\n", PRTCL);
		printf ("%s\n", outbuf[0]);
		print_strings (peer);
		return 1;
	    }
	    else
		itr = itr->next;
	}
    }

//other wise string has not been found but belongs to us, so we add it.
    string_list *temp = (string_list *) malloc (sizeof (string_list));
    strcpy (temp->str, string);
    temp->str_id = strid;
    temp->next = peer->strings;
    peer->strings = temp;

// give a response message and return 
    sprintf (outbuf[0], "%s ADD 0 200 ok\r\n", PRTCL);
    print_strings (peer);
    return 1;
}


/* query_delete
* deletes specified string from a peer, if its string id is in the 
* 	same range as the peer's, and if it is in the peer's list of strings
* ARGUMENTS 
* peer - peer to delete string from
* string - string to delete
* outbuf - buffer to hold response
* RETURN
* number of strings in outbuf
* in outbuf, appropriate ok/error message 
*/
int
query_delete (peer_obj * peer, char *string, char outbuf[][BUFLEN])
{
    int i;
    int next_peer_id;
    int tempi;
    char temp[BUFSIZ];
    next_request (peer, temp, &tempi, &next_peer_id);
    int strid = 0;
    string_list *itr;
    string_list *itrprev = NULL;
    itr = peer->strings;
    for (i = 0; i < strlen (string); i++)
	strid += (int) string[i];
    strid = strid % peer->max_id;
    if ((strid < peer->id || strid >= next_peer_id)
	&& peer->id != next_peer_id) {
	sprintf (outbuf[0], "%s DELETE 0 400 notmine\r\n", PRTCL);
	return 1;
    }
    else if ((itr != NULL))	//if list is not empty, we look for it
    {
	while (itr != NULL) {
	    if (strcmp (itr->str, string) == 0) {
		if (itrprev == NULL)
		    peer->strings = peer->strings->next;
		else {
		    itrprev->next = itr->next;
		    itr = itrprev->next;
		}
		sprintf (outbuf[0], "%s DELETE 0 200 ok\r\n", PRTCL);
		return 1;
	    }
	    else {
		itrprev = itr;
		itr = itr->next;
	    }
	}
    }
    sprintf (outbuf[0], "%s DELETE 0 201 not present\r\n", PRTCL);
//other wise string has not been found but belongs to us, so we add it.
    print_strings (peer);
    return 1;

}

/* print_strings
handy function to print a peer's strings. (testing purposes only)
*/
void
print_strings (peer_obj * peer)
{
    if (peer->strings == NULL)
	printf ("Peer has no strings!\n");
    else {
	string_list *itr = peer->strings;
	printf ("Strings of peer %d:\n", peer->id);
	while (itr != NULL) {
	    printf ("\"%s\" (%d) ->", itr->str, itr->str_id);
	    itr = itr->next;
	}
	printf ("\n");
    }
    return;
}
