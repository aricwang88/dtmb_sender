//#ifndef __REED_SOLOMON_H__
//#define __REED_SOLOMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MM (rs->mm)
#define NN (rs->nn)
#define ALPHA_TO (rs->alpha_to) 
#define INDEX_OF (rs->index_of)
#define GENPOLY (rs->genpoly)
//#define NROOTS (rs->nroots)
#define NROOTS 	48
#define FCR (rs->fcr)
#define PRIM (rs->prim)
#define IPRIM (rs->iprim)
#define PAD (rs->pad)
#define A0 (NN)

#define DTYPE unsigned char
//#define NULL ((void *)0)
#define	min(a,b)	((a) < (b) ? (a) : (b))

/* Reed-Solomon codec control block */
typedef struct RS_TAG {
  int mm;              /* Bits per symbol */
  int nn;              /* Symbols per block (= (1<<mm)-1) */
  DTYPE *alpha_to;     /* log lookup table */
  DTYPE *index_of;     /* Antilog lookup table */
  DTYPE *genpoly;      /* Generator polynomial */
  int nroots;     /* Number of generator roots = number of parity symbols */
  int fcr;        /* First consecutive root, index form */
  int prim;       /* Primitive element, index form */
  int iprim;      /* prim-th root of 1, index form */
  int pad;        /* Padding bytes in shortened block */
  int gfpoly;
}RS_PARAM_t,*PRS_PARAM_t;

#define MODNN(x) modnn(rs,x)

void *init_rs_char(int symsize,int gfpoly,int fcr,int prim,int nroots,int pad);
void free_rs_char(void *handle);
void encode_rs_char(void *handle,unsigned char *data,unsigned char *parity);
int  decode_rs_char(void *handle,DTYPE *data, int *eras_pos, int no_eras);


//#endif
