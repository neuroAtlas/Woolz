#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Woolz Library					*
*   File       :   Wlz3DViewTransformObj.c				*
*************************************************************************
* This module has been copied from the original woolz library and       *
* modified for the public domain distribution. The original authors of  *
* the code and the original file headers and comments are in the        *
* HISTORY file.                                                         *
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 22 13:24:21 1999				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdlib.h>
#include <float.h>
#include <Wlz.h>


static int compareVtxVal(
  const void *p1,
  const void *p2)
{
  WlzIVertex3  *vtx1 = (WlzIVertex3 *) p1;
  WlzIVertex3  *vtx2 = (WlzIVertex3 *) p2;

  if( vtx1->vtZ > vtx2->vtZ ){
    return 1;
  }
  if( vtx1->vtZ < vtx2->vtZ ){
    return -1;
  }
  if( vtx1->vtY > vtx2->vtY ){
    return 1;
  }
  if( vtx1->vtY < vtx2->vtY ){
    return -1;
  }
  if( vtx1->vtX > vtx2->vtX ){
    return 1;
  }
  if( vtx1->vtX < vtx2->vtX ){
    return -1;
  }
  return 0;
}

/************************************************************************
*   Function   : Wlz3DViewTransformObj					*
*   Date       : Fri Oct 22 13:25:10 1999				*
*************************************************************************
*   Synopsis   : Transform an object using the given view-transform.	*
*		Typically this is for mapping section data back into	*
*		the 3D space of the reference image/reconstruction.	*
*   Returns    :							*
*   Parameters :							*
*   Global refs:							*
************************************************************************/
WlzObject *Wlz3DViewTransformObj(
  WlzObject		*srcObj,
  WlzThreeDViewStruct	*viewStr,
  WlzErrorNum		*dstErr)
{
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  WlzObject		*dstObj=NULL;
  int			area;
  int			i, j, k, p, xp, yp, line;
  int			plane1, lastpl, line1, lastln, kol1, lastkl;
  WlzIVertex3		*vertices;
  int			numVtxs, vtxIdx;
  WlzIntervalWSpace	iwsp;
  WlzGreyWSpace		gwsp;
  WlzDomain		domain, tmpDomain;
  WlzValues		values;
  int			numInts, itvlFlg;
  WlzInterval		*itvl;


  /* check the object */
  if( srcObj == NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else {
    switch( srcObj->type ){

    case WLZ_2D_DOMAINOBJ:
      if( srcObj->domain.core == NULL ){
	errNum = WLZ_ERR_DOMAIN_NULL;
      }
      area = WlzArea(srcObj, &errNum);
      if( area == 0 ){
	dstObj = WlzMakeEmpty(&errNum);
      }
      break;

    case WLZ_2D_POLYGON: /* to be done at some time to 3D polyline */
    case WLZ_BOUNDLIST: /* convert to 3D polylines */
    case WLZ_TRANS_OBJ:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }

  /* create the voxel list */
  if( (errNum == WLZ_ERR_NONE) && (dstObj == NULL) ){
    numVtxs = sizeof(WlzIVertex3) * (area+4);

    vertices = AlcMalloc(sizeof(WlzIVertex3) * (area+4));
    numVtxs = 0;
    if( vertices ){
      errNum = WlzInitRasterScan(srcObj, &iwsp, WLZ_RASTERDIR_ILIC);
    }
    else {
      errNum = WLZ_ERR_MEM_ALLOC;
    }

    if( errNum == WLZ_ERR_NONE ){
      while( (errNum = WlzNextInterval(&iwsp)) == WLZ_ERR_NONE ){
	float x, y, z;

	if((iwsp.linpos < (int) viewStr->minvals.vtY) ||
	   (iwsp.linpos > (int) viewStr->maxvals.vtY)){
	  continue;
	}
	yp = iwsp.linpos - (int) viewStr->minvals.vtY;
	for(k=iwsp.lftpos; k <= iwsp.rgtpos; k++){
	  if((k < (int) viewStr->minvals.vtX) ||
	     (k > (int) viewStr->maxvals.vtX)){
	    continue;
	  }
	  xp = k - (int) viewStr->minvals.vtX;
	  x = viewStr->xp_to_x[xp] + viewStr->yp_to_x[yp];
	  y = viewStr->xp_to_y[xp] + viewStr->yp_to_y[yp];
	  z = viewStr->xp_to_z[xp] + viewStr->yp_to_z[yp];
	  vertices[numVtxs].vtX = WLZ_NINT(x);
	  vertices[numVtxs].vtY = WLZ_NINT(y);
	  vertices[numVtxs].vtZ = WLZ_NINT(z);
	  numVtxs++;
	}
      }

      if(errNum == WLZ_ERR_EOO) /* Reset error from end of object */ 
      {
	errNum = WLZ_ERR_NONE;
      }
    }
  }      

  /* sort wrt planes, lines, kols */
  if( (errNum == WLZ_ERR_NONE) && (dstObj == NULL) ){
    qsort((void *) vertices, (size_t) numVtxs, sizeof(WlzIVertex3),
	  compareVtxVal);

    /* create planedomain */
    plane1 = vertices[0].vtZ;
    lastpl = vertices[numVtxs - 1].vtZ;
    line1 = vertices[0].vtY;
    lastln = line1;
    kol1 = vertices[0].vtX;
    lastkl = kol1;
    for(i=1; i < numVtxs; i++){
      if( kol1 > vertices[i].vtX ){
	kol1 = vertices[i].vtX;
      }
      if( lastkl < vertices[i].vtX ){
	lastkl = vertices[i].vtX;
      }
      if( line1 > vertices[i].vtY ){
	line1 = vertices[i].vtY;
      }
      if( lastln < vertices[i].vtY ){
	lastln = vertices[i].vtY;
      }
    }
    if( (domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN,
				       plane1, lastpl,
				       line1, lastln,
				       kol1, lastkl,
				       &errNum)) == NULL ){
      AlcFree((void *) vertices);
    }
  }

  /* for each plane count intervals and make domain */
  if( (errNum == WLZ_ERR_NONE) && (dstObj == NULL) ){

    vtxIdx = 0;
    for(p=plane1; p <= lastpl; p++){

      /* increment vertex index to current plane */
      while( vertices[vtxIdx].vtZ < p ){
	vtxIdx++;
      }

      /* check for empty domain */
      if( vertices[vtxIdx].vtZ > p ){
	domain.p->domains[p - plane1].i = NULL;
	continue;
      }

      /* estimate intervals - foreach pixel add one,
	 foreach adjacent pixel on the same line subtract one */
      numInts = 1;
      kol1 = vertices[vtxIdx].vtX;
      lastkl = kol1;
      for(i=vtxIdx+1; i < numVtxs; i++){
	if( vertices[i].vtZ > p ){
	  break;
	}
	numInts++;
	if((vertices[i].vtY == vertices[i-1].vtY) &&
	   ((vertices[i].vtX == (vertices[i-1].vtX)) ||
	    (vertices[i].vtX == (vertices[i-1].vtX + 1))
	     )){
	  numInts--;
	}
	if(kol1 > vertices[i].vtX){
	  kol1 = vertices[i].vtX;
	}
	if(lastkl < vertices[i].vtX){
	  lastkl = vertices[i].vtX;
	}
      }
      line1 = vertices[vtxIdx].vtY;
      lastln = vertices[i-1].vtY;

      /* make the domain and add the intervals pointer */
      tmpDomain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL,
					  line1, lastln,
					  kol1, lastkl,
					  &errNum);
      itvl = (WlzInterval *) AlcMalloc(sizeof(WlzInterval)*numInts);
      tmpDomain.i->freeptr = WlzPushFreePtr(tmpDomain.i->freeptr,
					    (void *) itvl,
					    &errNum);
      
      /* one more loop to add the intervals */
      itvl->ileft = vertices[vtxIdx].vtX - kol1;
      line = vertices[vtxIdx].vtY;
      itvlFlg = 1;
      for(i=vtxIdx+1; i < numVtxs; i++){

	/* end of this plane */
	if( vertices[i].vtZ > p ){
	  if( itvlFlg ){
	    itvl->iright = vertices[i-1].vtX - kol1;
	    WlzMakeInterval(line, tmpDomain.i, 1, itvl);
	    itvl++;
	    itvlFlg = 0;
	  }
	  break;
	}

	/* check if new interval */
	if( !itvlFlg ){
	  itvl->ileft = vertices[i].vtX - kol1;
	  line = vertices[i].vtY;
	  itvlFlg = 1;
	  continue;
	}
	
	/* check for gap or new-line */
	if((line < vertices[i].vtY) ||
	   ((vertices[i].vtX - vertices[i-1].vtX) > 1)){
	  itvl->iright = vertices[i-1].vtX - kol1;
	  WlzMakeInterval(line, tmpDomain.i, 1, itvl);
	  itvl++;
	  itvl->ileft = vertices[i].vtX - kol1;
	  line = vertices[i].vtY;
	  itvlFlg = 1;
	}
      }

      /* complete the last interval */
      if( itvlFlg ){
	itvl->iright = vertices[i-1].vtX - kol1;
	WlzMakeInterval(line, tmpDomain.i, 1, itvl);
	itvl++;
      }

      /* add the domain to the planedomain */
      domain.p->domains[p - plane1] =
	WlzAssignDomain(tmpDomain, &errNum);
	    
    }
      
  }

  /* create the new object */
  if( (errNum == WLZ_ERR_NONE) && (dstObj == NULL) ){
    values.core = NULL;
    dstObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			 NULL, NULL, &errNum);
  }

  /* check for grey-level data */
  if((errNum == WLZ_ERR_NONE) && dstObj &&
     (dstObj->type != WLZ_EMPTY_OBJ) && srcObj->values.core ){
    WlzPixelV	bckgrnd = WlzGetBackground(srcObj, &errNum);
    WlzObject	*tmpObj;
    WlzValues	tmpValues;
    WlzDVertex3	vtx;
    WlzGreyValueWSpace	*gVWSp = NULL;
    WlzObjectType	valueTbType = WlzGreyTableType(WLZ_GREY_TAB_RAGR,
						       bckgrnd.type, NULL);
    

    /* make a voxel table */
    values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
				     plane1, lastpl, bckgrnd,
				     NULL, &errNum);
    dstObj->values = WlzAssignValues(values, &errNum);

    /* set up grey-value random access to original
       and loop through planes setting values */
    gVWSp = WlzGreyValueMakeWSp(srcObj, NULL);
    for(p=plane1; p <= lastpl; p++){

      /* check for empty domain */
      if( domain.p->domains[p-plane1].core == NULL ){
	continue;
      }

      /* make a value table */
      tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain.p->domains[p-plane1],
			   values.vox->values[p-plane1], NULL, NULL, &errNum);
      tmpValues.v = WlzNewValueTb(tmpObj, valueTbType, bckgrnd, &errNum);
      values.vox->values[p-plane1] = WlzAssignValues(tmpValues, &errNum);
      tmpObj->values = WlzAssignValues(tmpValues, &errNum);

      /* transfer values */
      errNum = WlzInitGreyScan(tmpObj, &iwsp, &gwsp);
      while((errNum == WLZ_ERR_NONE) && 
	    ((errNum = WlzNextGreyInterval(&iwsp)) == WLZ_ERR_NONE)){

	for(i=0;  i<iwsp.colrmn; i++){
	  vtx.vtX = iwsp.colpos + i;
	  vtx.vtY = iwsp.linpos;
	  vtx.vtZ = p;
	  Wlz3DSectionTransformVtx(&vtx, viewStr);
	  WlzGreyValueGet(gVWSp, 0.0,
			  WLZ_NINT(vtx.vtY), WLZ_NINT(vtx.vtX));
	  switch( gwsp.pixeltype ){
	  case WLZ_GREY_LONG:
	    *(gwsp.u_grintptr.lnp+i) = gVWSp->gVal[0].lnv;
	    break;
	  case WLZ_GREY_INT:
	    *(gwsp.u_grintptr.inp+i) = gVWSp->gVal[0].inv;
	    break;
	  case WLZ_GREY_SHORT:
	    *(gwsp.u_grintptr.shp+i) = gVWSp->gVal[0].shv;
	    break;
	  case WLZ_GREY_UBYTE:
	    *(gwsp.u_grintptr.ubp+i) = gVWSp->gVal[0].ubv;
	    break;
	  case WLZ_GREY_FLOAT:
	    *(gwsp.u_grintptr.flp+i) = gVWSp->gVal[0].flv;
	    break;
	  case WLZ_GREY_DOUBLE:
	    *(gwsp.u_grintptr.dbp+i) = gVWSp->gVal[0].dbv;
	    break;
	  case WLZ_GREY_BIT: /* not sure what to do with these */
	  default:
	    break;
	  }
	}
      }
      if(errNum == WLZ_ERR_EOO) /* Reset error from end of object */ 
      {
	errNum = WLZ_ERR_NONE;
      }
      WlzFreeObj(tmpObj);
    }

    WlzGreyValueFreeWSp(gVWSp);
  }

  /* clean temp allocation */
  if( vertices ){
    AlcFree((void *) vertices);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return dstObj;
}
