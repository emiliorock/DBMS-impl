// tuple.c ... functions on tuples
// part of Multi-attribute Linear-hashed Files
// Written by John Shepherd, April 2016

#include "defs.h"
#include "tuple.h"
#include "reln.h"
#include "hash.h"
#include "chvec.h"
#include "bits.h"
#include "math.h"

// return number of bytes/chars in a tuple

int tupLength(Tuple t)
{
	return strlen(t);
}

// reads/parses next tuple in input

Tuple readTuple(Reln r, FILE *in)
{
	char line[MAXTUPLEN];
	fgets(line, MAXTUPLEN-1, in);
	line[strlen(line)-1] = '\0';
	// count fields
	// cheap'n'nasty parsing
	char *c; int nf = 1;
	for (c = line; *c != '\0'; c++)
		if (*c == ',') nf++;
	// invalid tuple
	if (nf != nattrs(r)) return NULL;
	return strdup(line); // needs to be free'd sometime
}

// extract values into an array of strings

void tupleVals(Tuple t, char **vals)
{
	char *c = t, *c0 = t;
	int i = 0;
	for (;;) {
		while (*c != ',' && *c != '\0') c++;
		if (*c == '\0') {
			// end of tuple; add last field to vals
			vals[i++] = strdup(c0);
			break;
		}
		else {
			// end of next field; add to vals
			*c = '\0';
			vals[i++] = strdup(c0);
			*c = ',';
			c++; c0 = c;
		}
	}
}

// release memory used for separate attirubte values

void freeVals(char **vals, int nattrs)
{
	int i;
	for (i = 0; i < nattrs; i++) free(vals[i]);
}

// hash a tuple using the choice vector

Bits tupleHash(Reln r, Tuple t)
{
	char buf[40];

	Count nvals = nattrs(r);
	char **vals = malloc(nvals*sizeof(char *));
	assert(vals != NULL);
	tupleVals(t, vals);

	Bits hash[10] = {0};
	Bits result = 0;

	int i;
	for (i = 0;i < nvals;i++) {
		hash[i] = hash_any((unsigned char *)vals[i], strlen(vals[i]));
	}
	for (i = 0;i < nvals;i++) {
		showBits(hash[i], buf);
		printf("hash(%s) = %s\n", vals[i], buf);
	}
	
	// get the CV
	ChVecItem *cv = chvec(r);
	i = 0;
	int k = 1;
	while (i < 32) {
		int b = cv->bit;
		unsigned int a = hash[cv->att];
		int j = (a & (1 << b)) >> b;
		//result = result + j * pow(2,i);
		result = result + j * k;
		i++;
		k = k * 2;
		cv++;
 	}
 	showBits(result, buf);
 	printf("result = %s", buf);
	return result;
}

// compare two tuples (allowing for "unknown" values)

Bool tupleMatch(Tuple t1, Tuple t2, int nattrs)
{
	char **v1 = malloc(nattrs*sizeof(char *));
	tupleVals(t1, v1);
	char **v2 = malloc(nattrs*sizeof(char *));
	tupleVals(t2, v2);
	Bool match = TRUE;
	int i;
	for (i = 0; i < nattrs; i++) {
		if (v1[i][0] == '?' || v2[i][0] == '?') continue;
		if (strcmp(v1[i],v2[i]) == 0) continue;
		match = FALSE;
	}
	freeVals(v1,nattrs); freeVals(v2,nattrs);
	return match;
}

// puts printable version of tuple in user-supplied buffer

void showTuple(Tuple t, char *buf)
{
	strcpy(buf,t);
}
