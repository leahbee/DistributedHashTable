/*btree.c
LEAH BROWN
ASSIGNMENT 5, CSCI 2132, DR. MCALLSITER
SEE PROBLEM: http://web.cs.dal.ca/~prof2132/
A program that implements a btree
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAXKEYS 4

/*DATA TYPE DEFINITIONS*/
//node datatype
typedef struct yada
{
  int numkeys;			//number of keys
  int numchildren;		//number of children
  struct yada *parent;
  char keys[MAXKEYS][50];
  struct yada *children[MAXKEYS + 1];
} Node;

//BTree datatype
typedef struct yadayada
{
  Node *root;
} BTree;

//Boolean datatype
typedef enum
{ FALSE, TRUE } Boolean;


/*FUNCTION HEADERS*/
BTree *btree_initialize (void);
Node *newNode (void);
Boolean btree_add (BTree *, char *);
BTree *btree_add_recur (BTree *, Node *, char *, Node **, Node **);
void btree_print (BTree *);
void printB (Node *, int);
Boolean btree_delete (BTree *, char *);
void btree_destroy (BTree *);
void btree_recur_destroy (Node *, BTree *);
/*MAIN METHOD*/
int
main ()
{

  BTree *btree = btree_initialize ();
  char word[51];
  char str[BUFSIZ];
  printf
    ("COMMANDS:\n--add string\t\tadds string to btree\n--delete string\t\tdeletes string from btree\n--print\t\t\tprints tree\n--quit\t\t\texits program\n\n");
  int i = 0,j;
  FILE *fp;
  fp = stdin;
  char ch;
  while (fgets (str, BUFSIZ, fp))//scanning for an entire line in the input
    {
      if (strncmp (str, "add", 3) == 0 || strncmp (str, "delete", 6) == 0)
	{

	  if (strncmp (str, "add", 3) == 0)
	    {
	      j = 4;
	      i = 4;
	    }
	  else
	    {
	      j = 7;
	      i = 7;
	    }
	  while (i < BUFSIZ && str[i] != '\n')//copying the rest of the line as the word to be inputted/deleted
	    {
	      word[i - j] = str[i];
	      i++;
	    }
	}
      word[i] = '\0';

      if (strncmp (str, "add", 3) == 0)
	btree_add (btree, word);
      else if (strncmp (str, "delete", 6) == 0)
	btree_delete (btree, word);
      else if (strncmp (str, "print", 5) == 0)
	btree_print (btree);
      else if (strncmp (str, "quit", 4) == 0)
	break;
      else
	printf ("Invalid command.\n");
      for (i = 0; i < 51; i++)
	word[i] = '\0';

    }


  printf ("QUITTING...\n");



// TEST CASE -> ADDS LETTERS A-Z IN ALPHABETICAL ORDER TO TREE
/*
	char str1[50], str2[50], str3[50], str4[50],str5[50], str6[50], str7[50], str8[50], str9[50], str10[50], str11[50], str12[50], str13[50], str14[50], str15[50], str16[50], str17[50], str18[50], str19[50], str21[50], str22[50], str23[50], str24[50], str25[50], str26[50], str20[50];
	strcpy(str1,"A");strcpy(str2,"B");strcpy(str3,"C");strcpy(str4,"D");strcpy(str5,"E");strcpy(str6,"F");strcpy(str7,"G");
	strcpy(str8,"H");strcpy(str9,"I");strcpy(str10,"J");strcpy(str11,"K");strcpy(str12,"L");strcpy(str13,"M");strcpy(str14,"N");strcpy(str15,"O");
	strcpy(str16,"P");strcpy(str17,"Q");strcpy(str18,"R");strcpy(str19,"S");strcpy(str20,"T");strcpy(str21,"U");strcpy(str22,"V");strcpy(str23,"W");
	strcpy(str24,"X");strcpy(str25,"Y");strcpy(str26,"Z");
	btree_add(btree, str1);	btree_add(btree, str2);	btree_add(btree, str3);	btree_add(btree, str4);	btree_add(btree, str5);	btree_add(btree, str6);
	btree_add(btree, str7);	btree_add(btree, str8);	btree_add(btree, str9);	btree_add(btree, str10);btree_add(btree, str11);btree_add(btree, str12);
	btree_add(btree, str13);btree_add(btree, str14);btree_add(btree, str15);btree_add(btree, str16);btree_add(btree, str17);
	btree_add(btree, str18);btree_add(btree, str19);btree_add(btree, str20);btree_add(btree, str21);btree_add(btree, str22);btree_add(btree, str23);
	btree_add(btree, str24);btree_add(btree, str25);btree_add(btree, str26);
	btree_print(btree);
	btree_destroy(btree);
*/
  return 0;
}


/*INIT METHOD*/
BTree *
btree_initialize ()
{

  BTree *ptr = (BTree *) (malloc (sizeof (BTree)));
  (*ptr).root = NULL;
  return ptr;
}

BTree *
btree_add_recur (BTree * btree, Node * itr, char *strtoadd, Node ** lptr,
		 Node ** rptr)
{
/*templptr and temprptr are used to hold values of *lptr and *rptr (which hold pointers to new left and right nodes) if recursion is neccesary*/
  Node *templptr = *lptr;
  Node *temprptr = *rptr;
  *rptr = newNode ();
  *lptr = newNode ();
  int i, j = 0, k = 0, m = 0, ridx = 0, lidx = 0, rchidx = 0, lchidx = 0;

  char temp[MAXKEYS + 1][50];
  Node *childrentemp[MAXKEYS + 2];
  for (i = 0; i < MAXKEYS + 1; i++)
    childrentemp[i] = NULL;
	/*----------------------FINDING TEMP ARRAYS OF ORDERED VALUES.------------------
		temp is an ordered array of thenode keys plus the key to be added, so when the node splits, it is easier to assign keys to the new left node and the new right node. childrentemp is an ordered array of the node's children (if it has any); only used if the add_recur method becomes recursive (i.e. more than one split. in this case, childrentemp is an ordered array of the node's children, including the split node from the prvious add_recur call.*/
  if (strcmp (strtoadd, (*itr).keys[0]) < 0)	//if key to be added is the smaller than all keys in node
    {
      strcpy (temp[0], strtoadd);
      if ((*itr).numchildren != 0)	//if there are children, add the previously split node from last recursion
	{
	  childrentemp[0] = templptr;
	  childrentemp[1] = temprptr;
	}
      for (i = 1; i < MAXKEYS + 1; i++)
	{
	  strcpy (temp[i], (*itr).keys[i - 1]);
	  if ((*itr).numchildren != 0)	//if there are children, add them to childrentemp
	    childrentemp[i + 1] = (*itr).children[i];
	}
    }
  else if (strcmp (strtoadd, (*itr).keys[MAXKEYS - 1]) > 0)	//if key to be added is larger than all keys in node
    {
      for (i = 0; i < MAXKEYS; i++)
	{
	  strcpy (temp[i], (*itr).keys[i]);
	  if ((*itr).numchildren != 0)
	    childrentemp[i] = (*itr).children[i];
	}
      if ((*itr).numchildren != 0)	//if there are children, add the previously split node from last recursion.
	{
	  childrentemp[i] = templptr;
	  childrentemp[i + 1] = temprptr;
	}
      strcpy (temp[MAXKEYS], strtoadd);
    }
  else
    {				//if its in the middle
      for (i = 0; i < MAXKEYS - 1; i++)
	{
	  if (strcmp (strtoadd, (*itr).keys[i]) > 0
	      && strcmp (strtoadd, (*itr).keys[i + 1]) < 0)
	    {
	      strcpy (temp[j], (*itr).keys[i]);
	      j++;
	      strcpy (temp[j], strtoadd);
	      if ((*itr).numchildren != 0)	//if there are children, add the previously split node from last recursion to tempchildren
		{
		  childrentemp[k] = templptr;
		  k++;
		  childrentemp[k] = temprptr;
		  k++;
		}
	    }

	  else
	    {
	      strcpy (temp[j], (*itr).keys[i]);
	      if ((*itr).numchildren != 0)
		{
		  childrentemp[k] = (*itr).children[i];
		  k++;
		}
	    }
	  j++;
	}
    }

	/*-------------------- CREATING TWO NEW NODES-------------------*/
  for (i = 0; i < floor (MAXKEYS / 2); i++)	//copying first half of temp array into the new left node.
    {
      strcpy ((**lptr).keys[i], temp[i]);
      if (childrentemp[0] != NULL)	//if there are children, adds first half of tempchildren into children fields of new left node
	{
	  (**lptr).children[i] = childrentemp[i];
	  (**lptr).numchildren++;
	}
      (**lptr).numkeys++;

    }
  if (childrentemp[0] != NULL)	// adding rest of chidlren to new left node. 
    {
      (**lptr).children[i] = childrentemp[i];
      (**lptr).numchildren++;
    }

  strcpy (strtoadd, temp[(int) floor (MAXKEYS / 2)]);	//sets new strtoadd to middle value of temp
  for (i = ceil (MAXKEYS / 2) + 1; i < MAXKEYS + 1; i++)	//coping second half of temp array into the new right node.
    {
      strcpy ((**rptr).keys[ridx], temp[i]);
      (**rptr).numkeys++;
      ridx++;
      if (childrentemp[0] != NULL)	//if there are children, adds second half of tempchildren into children fields of new right node
	{
	  (**rptr).children[ridx - 1] = childrentemp[i];
	  (**rptr).numchildren++;
	}
    }
  if (childrentemp[0] != NULL)	//adding rest of children to new right node. 
    {
      (**rptr).children[ridx] = childrentemp[i];
      (**rptr).numchildren++;

    }
  if ((**lptr).numchildren != 0)	//setting parent of new left node's children to the new left node.
    for (m = 0; m < (**lptr).numchildren; m++)
      (*(**lptr).children[m]).parent = *lptr;
  if ((**rptr).numchildren != 0)	//setting parent of new right node's children to the new right ndoe
    for (m = 0; m < (**rptr).numchildren; m++)
      (*(**rptr).children[m]).parent = *rptr;
  Node *disposableitr = itr;
/*-------------------------------- ADDING NEW STRTOADD TO PARENT NODE-------------------------
now that we have two new split nodes and a string that is greater than the left and less than the right, want to scan the parent node for a place to add the new strtoadd and the two split nodes.*/

  itr = (*itr).parent;
  free (disposableitr);
  disposableitr = NULL;
  if (itr == NULL)		//if there is no parent, create a new node, call it the root, and add strtoadd and the right and left split nodes.
    {
      Node *newroot = newNode ();

      strcpy ((*newroot).keys[0], strtoadd);
      (*newroot).numkeys++;
      (*newroot).children[0] = *lptr;
      (**lptr).parent = newroot;
      (*newroot).numchildren++;
      (*newroot).children[1] = *rptr;
      (**rptr).parent = newroot;
      (*newroot).numchildren++;
      (*btree).root = newroot;

      return btree;
    }

  else if ((*itr).numkeys != MAXKEYS)	//if the parent is not full, find where strtoadd should go...
    {
      int i = 0;
      while ((*itr).keys[i][0] != '\0' && strcmp (strtoadd, (*itr).keys[i]) > 0)	//find index where strtoadd should go in the node's array of keys
	i++;
      int j = 0;
      (*itr).numkeys++;
      for (j = (*itr).numkeys - 1; j > i; j--)	//move all keys greater than than this one index to the right and the associated children
	{
	  strcpy ((*itr).keys[j], (*itr).keys[j - 1]);
	  (*itr).children[j + 1] = (*itr).children[j];
	}
      strcpy ((*itr).keys[i], strtoadd);	// add strtoadd to the "empty" spot
      (**rptr).parent = itr;	//set parent of the split nodes to the current node.
      (**lptr).parent = itr;
      (*itr).children[j + 1] = *rptr;	//add children to correct place.
      (*itr).children[j] = *lptr;
      (*itr).numchildren++;
    }
  else				//if the parent is full, the parent must also split --> recursion!!!!! :)
    {
      (**rptr).parent = itr;
      (**lptr).parent = itr;
      btree_add_recur (btree, itr, strtoadd, &*lptr, &*rptr);
    }

}


/* METHOD TO PRINT THE TREE*/
void
btree_print (BTree * tree)
{
//calls the recursive method printB
  printB (tree->root, 0);
  printf ("----------------------\n");
}

/*RECURSIVE METHOD TO PRINT THE TREE*/
void
printB (Node * root, int level)
{
  int i = 0, j = 0;
  for (i = 0; i < root->numkeys; i++)	//prints all keys of the node.
    {
      printf ("%s---", root->keys[i]);
    }
/*//information that can be printed along with each node - parent address, node address, and numchildren. handy for debugging.
	if((*root).parent==NULL)
		printf("Address: %d, numchildren: %d ",root, (*root).numchildren);
	else
		printf("Address: %d, ---Parent: %d, ---numchildren: %d ", root, (*root).parent, (*root).numchildren);*/
  printf ("\n");
  if ((*root).numchildren != 0)
    {				//if there are children, print the children (recursive)
      level++;
      for (i = 0; i < root->numchildren; i++)
	{
	  for (j = 0; j < level; j++)
	    printf ("/");	//to make it easier to visualize the tree, the number of "/" printed by each row indicates what "level" of the btree the node is on. no "/" is the root, one "\" is a child of the root, etc.
	  printB ((*root).children[i], level);

	}
    }
}

/*NON RECURSIVE ADD METHOD*/
Boolean
btree_add (BTree * tree, char *strtoadd)
{
  Node *itr;
  itr = (*tree).root;
  if (itr == NULL)		//if the root is empty, must create the root node
    {
      itr = newNode ();
      (*tree).root = itr;
    }
  int i;
		/*-----------TRAVERSE THE TREE ---------*/
  //traverses tree  until a place is found in a leaf node where strtoadd should go
  while ((*itr).numchildren != 0)
    {
      i = 0;
      while ((*itr).keys[i][0] != '\0'
	     && strcmp (strtoadd, (*itr).keys[i]) > 0 && i < MAXKEYS)
	i++;
      if (strcmp (strtoadd, (*itr).keys[i]) == 0)
	{
	  printf ("duplicates not permitted.\n");
	  return FALSE;
	}
      else
	itr = (*itr).children[i];
    }
  for (i = 0; i < (*itr).numkeys; i++)
    {
      if (strcmp (strtoadd, (*itr).keys[i]) < 0)
	break;			//finds index where strtoadd should go in the leafnode
      if (strcmp (strtoadd, (*itr).keys[i]) == 0)
	{
	  printf ("duplicates not permitted.\n");
	  return FALSE;
	}
    }
		/*-------------ADD STRTOADD TO LEAFNODEE----------*/
  if ((*itr).numkeys != MAXKEYS)	//if the leafnode is not full
    {
      int j;
      for (j = (*itr).numkeys; j > i; j--)	//move all keys greater than strtoadd right one spot
	strcpy ((*itr).keys[j], (*itr).keys[j - 1]);
      strcpy ((*itr).keys[j], strtoadd);	//add strtoadd to "empty" spot
      (*itr).numkeys++;
    }
  else				//if the leafnode is full, it must split to add strtoadd; recursive method btree_add_recur is called.
    {
      Node *lptr;
      Node *rptr;
      tree = btree_add_recur (tree, itr, strtoadd, &lptr, &rptr);
    }				//RECURSION!!!!!!! :D
  return TRUE;


}

/*DELETE METHOD FOR BTREE
NOTE: method is only able to delete keys in leaf nodes*/
Boolean
btree_delete (BTree * tree, char *strtodel)
{
  int i, j;
  Node *itr = (*tree).root;
  char temp[50];
  temp[0] = '\0';
  while ((*itr).numchildren != 0)
    {
      i = 0;
      while ((*itr).keys[i][0] != '\0'
	     && strcmp (strtodel, (*itr).keys[i]) > 0 && i <= (*itr).numkeys)
	i++;
      itr = (*itr).children[i];
    }
  for (i = 0; i < (*itr).numkeys; i++)
    if (strcmp (strtodel, (*itr).keys[i]) == 0)
      break;			//finds index where strtoadd should go in the leafnode
  if (i == (*itr).numkeys)	//if strtodel was not found in a leaf node...
    {
      printf
	("Delete method is able to delete values from the leafnodes only.\n\"%s\" may not exist in the tree or it may exist in a node which is not a leaf node.\n",
	 strtodel);
      return FALSE;
    }
  else if (i == (*itr).numkeys - 1)
    {
      strcpy ((*itr).keys[i], temp);
      (*itr).numkeys--;
    }
  else
    {
      for (j = i; j < (*itr).numkeys; j++)	//moves all keys greater than strtodel one index to the left
	strcpy ((*itr).keys[j], (*itr).keys[j + 1]);
      strcpy ((*itr).keys[j], temp);	//overwrites last key with a null string
      (*itr).numkeys--;
    }
  return TRUE;
}

	/*NEW NODE METHOD */
Node *
newNode ()			//creates new node, setting all values to 0 or null
{
  int i;
  Node *ptr;
  ptr = (Node *) (malloc (sizeof (Node)));
  ptr->numkeys = 0;
  ptr->numchildren = 0;
  for (i = 0; i < MAXKEYS; i++)
    {
      (*ptr).keys[i][0] = '\0';
      (*ptr).children[i] = NULL;
    }
  (*ptr).children[MAXKEYS] = NULL;
  (*ptr).parent = NULL;
  return ptr;
}


void
btree_destroy (BTree * tree)
{
  btree_recur_destroy ((*tree).root, tree);
  tree->root = NULL;
  free (tree);
}

void
btree_recur_destroy (Node * root, BTree * tree)
{
  int i, j;
  if ((*root).numchildren != 0)
    {
      for (i = 0; i < (*root).numchildren; i++)
	btree_recur_destroy ((*root).children[i], tree);
    }
  free (root);
}
