#pragma ident "MRC HGU $Id$"
/***********************************************************************
* Project:      Woolz
* Title:        WlzLineArea.c
* Date:         March 1999
* Author:       Jim Piper
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:      Computes the line area of a Woolz object.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <Wlz.h>

/************************************************************************
*   Function   : WlzLineArea						*
*   Returns    :int: line area of the object, -1 on error		*
*   Parameters :WlzObject *obj: 2D domain object			*
*   Date       : Mon Oct 14 15:09:02 1996				*
*   Synopsis   :							*
* return the line-area of an object
* defined as the sum of the line segments bounded by the left hand
* end of the first interval in a line and the right hand end of the
* last interval in that line.
************************************************************************/

int WlzLineArea(
  WlzObject *obj,
  WlzErrorNum *dstErr)
{
  int 			size=0,l,r;
  WlzIntervalDomain	*idom;
  WlzIntervalWSpace	iwsp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  
  /* check the object */
  if( obj == NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
    size = -1;
  }

  if( errNum == WLZ_ERR_NONE ){
    switch( obj->type ){

    case WLZ_2D_DOMAINOBJ:
      if( obj->domain.core == NULL ){
	size = -1;
	errNum = WLZ_ERR_DOMAIN_NULL;
      }
      break;

    case WLZ_EMPTY_OBJ:
      if( dstErr ){
	*dstErr = WLZ_ERR_NONE;
      }
      return( 0 );

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      size = -1;

    }
  }

  if( errNum == WLZ_ERR_NONE ){
    idom = obj->domain.i;
    switch( idom->type ){

    case WLZ_INTERVALDOMAIN_INTVL:
      size = 0;
      if((errNum = WlzInitRasterScan(obj, &iwsp,
      				     WLZ_RASTERDIR_ILIC)) != WLZ_ERR_NONE ){
	size = -1;
	break;
      }
      while( (errNum = WlzNextInterval(&iwsp)) == WLZ_ERR_NONE ){
	l = iwsp.lftpos;
	while ((errNum == WLZ_ERR_NONE) && (iwsp.intrmn != 0) ){
	  errNum = WlzNextInterval(&iwsp);
	}
	r = iwsp.rgtpos;
	size += (r -l +1);
      }
      switch(errNum){
      case WLZ_ERR_NONE:
      case WLZ_ERR_EOO:
	errNum = WLZ_ERR_NONE;
	break;
      default:
	size = -1;
	break;
      }
      break;

    case WLZ_INTERVALDOMAIN_RECT:
      size = (idom->lastln - idom->line1 + 1) *
	     (idom->lastkl - idom->kol1 + 1);
      break;

    default:
      size = -1;
      errNum = WLZ_ERR_DOMAIN_TYPE;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return size;
}
