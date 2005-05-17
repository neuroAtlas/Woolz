#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzSAToWlz.c
* \author       richard <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Tue Mar 29 12:23:04 2005
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2002 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \ingroup      WlzTransform
* \brief        Convert the Smart Atlas polygons to a woolz boundaries.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Wlz.h>

/* externals required by getopt  - not in ANSI C standard */
#ifdef __STDC__ /* [ */
extern int      getopt(int argc, char * const *argv, const char *optstring);
 
extern int 	optind, opterr, optopt;
extern char     *optarg;
#endif /* __STDC__ ] */

typedef struct _NamedBndItem {
  char		*name;
  WlzBoundList	*bnd;
} NamedBndItem;

void usage(char *proc_str)
{
  fprintf(stderr,
	  "Usage:\t%s -h -v"
	  "[<input file>]\n"
	  "\tRead the Smart Atlas xml file and convert to woolz\n"
	  "\tOptions are:\n"
	  "\t  -h        Help - prints this usage message\n"
	  "\t  -v        Verbose operation\n"
	  "",
	  proc_str);
  return;
}

char *nextStartTag(
  FILE	*inFile)
{
  char 	*tag=NULL;
  char 	buf[256];
  char	c;
  int	i;

  while( (c = fgetc(inFile)) != EOF ){
    if( c == '<' ){
      i = 0;
      c = fgetc(inFile);
      if( c == EOF ){
	break;
      }
      else if( c != '/' ){
	buf[i++] = c;
	while( (c = fgetc(inFile)) != '>' ){
	  if( c == EOF ){
	    break;
	  }
	  buf[i++] = c;
	}
	if( c == '>' ){
	  buf[i] = '\0';
	  break;
	}
      }
    }
  }

  if( i ){
    tag = strdup(buf);
  }
  return tag;
}
  

char *getNextName(
  FILE	*inFile)
{
  char	*name=NULL;
  char	*tag;
  char	c;
  char	buffer[128];
  int	i;

  /* check file pointer */
  if( inFile == NULL ){
    return NULL;
  }

  /* search for name tag */
  while( tag = nextStartTag(inFile) ){
    if(strcmp(tag, "name") == 0){
      break;
    }
  }

  /* read until end tag */
  if( tag ){
    i = 0;
    while( (c = fgetc(inFile)) != '<' ){
      if( c == EOF ){
	break;
      }
      buffer[i++] = c;
    }
    buffer[i] = '\0';
    while( (c = fgetc(inFile)) != '>' ){
      if( c == EOF ){
	break;
      }
    }

    /* extract name */
    name = strdup(buffer);
  }

  return name;
}

WlzBoundList *getNextBoundary(
  FILE	*inFile)
{
  WlzBoundList	*bnd=NULL, *bnd1=NULL;
  WlzPolygonDomain	*poly;
  char	c, *buf, *str, *tag;
  long	start, end;
  int	i, nVtxs;
  WlzDVertex2	*vtxs;

  /* find the polyline start tag */
  while( tag = nextStartTag(inFile) ){
    if(strcmp(tag, "c") == 0){
      break;
    }
  }

  /* count characters and vertices */
  start = ftell(inFile);
  nVtxs = 0;
  while( (c = fgetc(inFile)) != '<' ){
    if( c == ',' ){
      nVtxs++;
    }
  }
  end = ftell(inFile);

  /* read vertex values and consume end tag */
  fseek(inFile, start, SEEK_SET);
  buf = (char *) AlcMalloc(sizeof(char) * (end-start+1));
  fread(buf, end-start-1, sizeof(char), inFile);
  while( (c = fgetc(inFile)) != '>' ){}

  /* now read the vertices */
  vtxs = (WlzDVertex2 *) AlcMalloc(sizeof(WlzDVertex2) * (nVtxs + 1));
  str = buf;
  for(i=0; i < (nVtxs-1); i++){
    sscanf(str, "%lf,%lf", &(vtxs[i].vtX), &(vtxs[i].vtY));
    while( *str != ' ' ){
      str++;
    }
    str++;
  }
  sscanf(str, "%lf,%lf", &(vtxs[i].vtX), &(vtxs[i].vtY));
  i++;
  /* close the boundary */
  vtxs[i] = vtxs[0];
  nVtxs++;

  /* create a boundlist and polyline structure */
  poly = WlzMakePolygonDomain(WLZ_POLYGON_DOUBLE,
			      nVtxs, (WlzIVertex2 *) vtxs,
			      nVtxs, 0, NULL);
  bnd = WlzMakeBoundList(WLZ_BOUNDLIST_PIECE, 1, poly, NULL);

  /* need to check if there is another boundary */
  start = ftell(inFile);
  fread(buf, 3, sizeof(char), inFile);
  fseek(inFile, start, SEEK_SET);
  if( strncmp(buf, "<c>", 3) == 0 ){
    if( bnd1 = getNextBoundary(inFile) ){
      /* check for down or next */
      vtxs = (WlzDVertex2 *) bnd1->poly;
      if( WlzInsidePolyEOD(*vtxs, poly, NULL) ){
	bnd1->type = WLZ_BOUNDLIST_HOLE;
	bnd->down = WlzAssignBoundList(bnd1, NULL);
      }
      else {
	bnd->next = WlzAssignBoundList(bnd1, NULL);
      }
    }
  }

  /* clear space */
  AlcFree(buf);

  return bnd;
}

void addToBndList(
  AlcDLPList	*list,
  char		*name,
  WlzBoundList	*bnd)
{
  AlcDLPItem	*bndItem;
  NamedBndItem	*namedBndItem;
  WlzBoundList	*tmpBnd;

  /* check if name exists */
  bndItem = list->head;
  namedBndItem = NULL;
  while( bndItem ){
    namedBndItem = (NamedBndItem *) bndItem->entry;
    if( strcmp(name, namedBndItem->name) == 0 ){
      break;
    }
    namedBndItem = NULL;
    bndItem = bndItem->next;
    if( bndItem == list->head ){
      break;
    }
  }

  if( namedBndItem ){
    tmpBnd = namedBndItem->bnd;
    while(tmpBnd->next){
      tmpBnd = tmpBnd->next;
    }
    tmpBnd->next = WlzAssignBoundList(bnd, NULL);
  }
  else {
    /* create a NamedBndItem */
    namedBndItem = (NamedBndItem *) AlcMalloc(sizeof(NamedBndItem));
    namedBndItem->name = name;
    namedBndItem->bnd = bnd;

    /* add to the list */
    AlcDLPListEntryAppend(list, NULL, (void *) namedBndItem, NULL);
  }

  return;
}

main(argc, argv)
  int         argc;
  char       *argv[];
{
  FILE		*inFile, *outFile;
  char 		optList[] = "hv";
  int		option;
  int		verboseFlg=0;
  WlzObject	*obj;
  WlzDomain	domain;
  WlzValues	values;
  WlzBoundList	*tmpBnd;
  char		*name;
  AlcDLPList	*bndDLPList;
  AlcDLPItem	*bndItem;
  NamedBndItem	*namedBndItem;
  int		noNameCntr=0;
  char		noNameBuf[32];
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* read the argument list and check for an input file */
  opterr = 0;
  while( (option = getopt(argc, argv, optList)) != EOF ){
    switch( option ){

    case 'v':
      verboseFlg = 1;
      break;

    case 'h':
      usage(argv[0]);
      return WLZ_ERR_NONE;

    default:
      usage(argv[0]);
      return WLZ_ERR_UNSPECIFIED;
    }
  }

  inFile = stdin;
  if( optind < argc ){
    if( (inFile = fopen(*(argv+optind), "r")) == NULL ){
      fprintf(stderr, "%s: can't open file %s\n", argv[0], *(argv+optind));
      usage(argv[0]);
      return( 1 );
    }
  }

  /* read xml file building boundary lists */
  /* quick fix read  - read until no new names */
  bndDLPList = AlcDLPListNew(NULL);
  name = NULL;
  while( name = getNextName(inFile) ){
    /* check for empty name */
    if( strcmp(name, " ") == 0 ){
      noNameCntr++;
      sprintf(noNameBuf, "NoName_%03d", noNameCntr);
      name = strdup(noNameBuf);
    }
    /* convert comma to underscore */
    tmpBnd = getNextBoundary(inFile);
    addToBndList(bndDLPList, name, tmpBnd);
  }

  /* write boundary lists */
  bndItem = bndDLPList->head;
  while( bndItem ){
    namedBndItem = (NamedBndItem *) bndItem->entry;
    name = (char *) AlcMalloc(sizeof(char)*(strlen(namedBndItem->name)+16));
    sprintf(name, "%s.wlz", namedBndItem->name);
    if( outFile = fopen(name, "w") ){
      domain.b = namedBndItem->bnd;
      values.core = NULL;
      obj = WlzMakeMain(WLZ_BOUNDLIST, domain, values, NULL, NULL, NULL);
      WlzWriteObj(outFile, obj);
      WlzFreeObj(obj);
      fclose(outFile);
    }
    AlcFree(name);
    bndItem = bndItem->next;
    if( bndItem == bndDLPList->head ){
      break;
    }
  }

  exit(0);
}