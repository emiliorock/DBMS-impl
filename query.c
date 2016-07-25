// query.c ... query scan functions
// part of Multi-attribute Linear-hashed Files
// Manage creating and using Query objects
// Written by John Shepherd, April 2016

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "hash.h"
#include "string.h"
#include "bits.h"

// A suggestion ... you can change however you like

struct QueryRep {
	Reln    rel;       // need to remember Relation info
	Bits    known;     // the hash value from MAH
	Bits    unknown;   // the unknown bits from MAH
	PageID  curpage;   // current page in scan
	int     is_ovflow; // are we in the overflow pages?
	Offset  curtup;    // offset of current tuple within page
	//TODO
};

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q)
{
	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);
	// TODO
	// Partial algorithm:
	// form known bits from known attributes
	// form unknown bits from '?' attributes
	// compute PageID of first page
	// using known bits and first "unknown" value
	// set all values in QueryRep object
	new->rel = r;
	Count num = nattrs(r);
	const char *attr[num];
	attr[0] = strtok(q,",");
	int i = 1;
	while (i < num) {
	 	attr[i] = strtok(NULL, ",");
	 	i++;
	}
	Bits hash[10] = {0};
	i = 0;
	char buf[40];
	while (i < num) {
		if (strcmp(attr[i], "?") != 0) {
			hash[i] = hash_any((unsigned char *)attr[i], strlen(attr[i]));
			
		}
		else {
			hash[i] = 0;
		}
		showBits(hash[i], buf);
		printf("hash(%s) = %s\n", attr[i], buf);
		i++;
	}

	// get the CV
	ChVecItem *cv = chvec(r);
	new->known = 0;
	new->unknown = 0;
	i = 0;
	int k = 1;
	while (i < 32) {
		int b = cv->bit;
		if(strcmp(attr[cv->att], "?")!=0){
			unsigned int a = hash[cv->att];
			int j = (a & (1 << b)) >> b;
			new->known = new->known + j * k;
			i++;
			k = k * 2;
			cv++;	
		}
		else{
			new->unknown = new->unknown + k;
			i++;
			k = k * 2;
			cv++;	
		}	
 	}
 	showBits(new->known, buf);
	printf("known = %s\n", buf);
	showBits(new->unknown, buf);
	printf("unknown = %s\n", buf);

	Count d = depth(r);
	Count rsp = splitp(r);

	Bits p = getLower(new->known, d);
	if (p < rsp) p = getLower(new->known, d);
	new->curpage = p;
	printf("current page is %d\n", new->curpage);

	//Page pg = getPage(r->data, p);

	return new;
}

// get next tuple during a scan

Tuple getNextTuple(Query q)
{
	// TODO
	// Partial algorithm:
	// if (more tuples in current page)
	//    get next matching tuple from current page
	// else if (current page has overflow)
	//    move to overflow page
	//    grab first matching tuple from page
	// else
	//    move to "next" bucket
	//    grab first matching tuple from data page
	// endif
	// if (current page has no matching tuples)
	//    go to next page (try again)
	// endif

	return NULL;
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	// TODO
}
