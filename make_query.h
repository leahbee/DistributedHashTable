/*#########################################################################
# make_query.h
# Author: Leah Brown
# Project Description and Socket Code: Dr. M. McAllister
# Class: CSCI 3171
# Due Date: Wednesday, February 22, 2012 (9:00p.m.)
#
# head file for make_query.c
##########################################################################*/



#ifndef peerobjh
#include "peer_obj.h"
#define peerobjh
#endif

#ifndef peerobjs
#define PRTCL "3171_a3/1.0"
#define CRET "\r\n"
#define PORT 30460
#define MAX_STR 20
#define BUFLEN 100
#define peerobjs
#endif
int id_request(char *, int, int, char * , int *);
int pull_request (peer_obj *, char *, int ,char[][BUFLEN]);
void next_request(peer_obj *, char *, int *, int *);
void done_request(peer_obj *, char *, int);
int push_request(peer_obj *, char *, int, char[][BUFLEN], int);
int send_request(char *, int ,char[][BUFLEN], int, char[][BUFLEN]);
int to_num(char*);
