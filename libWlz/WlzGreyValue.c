#pragma ident "MRC HGU $Id$"
/***********************************************************************
* Project:      Woolz
* Title:        WlzGreyValue.c
* Date:         March 1999
* Author:       Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:      Provides functions for random access to the grey values
*		of 2D and 3D Woolz objects.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
* 02-10-00 bill	Use 3D transform for 3D object in WlzGreyValueGet().
* 05-06-00 bill Removed unused variables.
************************************************************************/
#include <stdlib.h>
#include <Wlz.h>

/************************************************************************
* Function:	WlzGreyValueSetBkdP					*
* Returns:	void							*
* Purpose:	Sets the WlzGreyValueWSpace grey value and pointer	*
*		to the background value.				*
* Global refs:	-							*
* Parameters:	WlzGreyV *gVP:		Ptr to grey values.		*
*		WlzGreyP *gPP:		Ptr to grey pointers.		*
*		WlzGreyType gType:	Grey type.			*
*		wlzGreyV val:		Background grey value to set.	*
************************************************************************/
static void	WlzGreyValueSetBkdP(WlzGreyV *gVP, WlzGreyP *gPP,
				    WlzGreyType gType,
				    WlzGreyV val)
{
  switch(gType)
  {
    case WLZ_GREY_INT:
      (*gVP).inv = val.inv;
      (*(gPP)).inp = &((*gVP).inv);
      break;
    case WLZ_GREY_SHORT:
      (*gVP).shv = val.shv;
      (*(gPP)).shp = &((*gVP).shv);
      break;
    case WLZ_GREY_UBYTE:
      (*gVP).ubv = val.ubv;
      (*(gPP)).ubp = &((*gVP).ubv);
      break;
    case WLZ_GREY_FLOAT:
      (*gVP).flv = val.flv;
      (*(gPP)).flp = &((*gVP).flv);
      break;
    case WLZ_GREY_DOUBLE:
      (*gVP).dbv = val.dbv;
      (*(gPP)).dbp = &((*gVP).dbv);
      break;
  }
}

/************************************************************************
* Function:	WlzGreyValueSetBkdPN					*
* Returns:	void							*
* Purpose:	Sets the WlzGreyValueWSpace grey values and pointers	*
*		to the background value.				*
* Global refs:	-							*
* Parameters:	WlzGreyV *gVP:		Ptr to grey values.		*
*		WlzGreyP *gPP:		Ptr to grey pointers.		*
*		WlzGreyType gType:	Grey type.			*
*		wlzGreyV val:		Background grey value to set.	*
*		int count:		number of values/pointers set.	*
************************************************************************/
static void	WlzGreyValueSetBkdPN(WlzGreyV *gVP, WlzGreyP *gPP,
				     WlzGreyType gType,
				     WlzGreyV val, int count)
{
  switch(gType)
  {
    case WLZ_GREY_INT:
      while(count-- > 0)
      {
        *gVP = val;
	(*(gPP++)).inp = &((*gVP++).inv);
      }
      break;
    case WLZ_GREY_SHORT:
      while(count-- > 0)
      {
        *gVP = val;
	(*(gPP++)).shp = &((*gVP++).shv);
      }
      break;
    case WLZ_GREY_UBYTE:
      while(count-- > 0)
      {
        *gVP = val;
	(*(gPP++)).ubp = &((*gVP++).ubv);
      }
      break;
    case WLZ_GREY_FLOAT:
      while(count-- > 0)
      {
        *gVP = val;
	(*(gPP++)).flp = &((*gVP++).flv);
      }
      break;
    case WLZ_GREY_DOUBLE:
      while(count-- > 0)
      {
        *gVP = val;
	(*(gPP++)).dbp = &((*gVP++).dbv);
      }
      break;
  }
}

/************************************************************************
* Function:	WlzGreyValueSetGreyP					*
* Returns:	void							*
* Purpose:	Sets the WlzGreyValueWSpace grey values and pointers	*
*		to the value obtained using the given base pointer and	*
*		offset.							*
* Global refs:	-							*
* Parameters:	WlzGreyV *gVP:		Ptr to grey values.		*
*		WlzGreyP *gPP:		Ptr to grey pointers.		*
*		WlzGreyType gType:	Grey type.			*
*		WlzGreyP baseGVP:	Base pointer.			*
*		int offset:		Offset from base pointer.	*
************************************************************************/
static void	WlzGreyValueSetGreyP(WlzGreyV *gVP, WlzGreyP *gPP,
				     WlzGreyType gType,
	  			     WlzGreyP baseGVP, int offset)
{
  WlzGreyP	gP;

  switch(gType)
  {
    case WLZ_GREY_INT:
      gP.inp = baseGVP.inp + offset;
      *(gPP) = gP;
      (*(gVP)).inv = *(gP.inp);
      break;
    case WLZ_GREY_SHORT:
      gP.shp = baseGVP.shp + offset;
      *(gPP) = gP;
      (*(gVP)).shv = *(gP.shp);
      break;
    case WLZ_GREY_UBYTE:
      gP.ubp = baseGVP.ubp + offset;
      *(gPP) = gP;
      (*(gVP)).ubv = *(gP.ubp);
      break;
    case WLZ_GREY_FLOAT:
      gP.flp = baseGVP.flp + offset;
      *(gPP) = gP;
      (*(gVP)).flv = *(gP.flp);
      break;
    case WLZ_GREY_DOUBLE:
      gP.dbp = baseGVP.dbp + offset;
      *(gPP) = gP;
      (*(gVP)).dbv = *(gP.dbp);
      break;
  }
}

/************************************************************************
* Function:	WlzGreyValueComputeGreyP2D				*
* Returns:	void							*
* Purpose:	Knowing that the given point is within the value table	*
*		computes the base pointer and offset for the point.	*
* Global refs:	-							*
* Parameters:	WlzGreyP *baseGVP:	Destination pointer for the	*
*					base pointer.			*
*		int *offset:		Destination pointer for the	*
*					offset from base pointer.	*
*		WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		int line:		Line coordinate of point.	*
*		int kol:		Column coordinate of point.	*
************************************************************************/
static void	WlzGreyValueComputeGreyP2D(WlzGreyP *baseGVP, int *offset,
					   WlzGreyValueWSpace *gVWSp,
					   int line, int kol)
{
  int		itvCount,
  		kol0;
  WlzValueLine	*vLn;
  WlzValueIntervalLine *vILn;

  switch(gVWSp->gTabType2D)
  {
    case WLZ_GREY_TAB_RAGR:
      vLn = gVWSp->values2D.v->vtblines +
	    (line - gVWSp->values2D.v->line1);
      *baseGVP = vLn->values;
      *offset = kol - gVWSp->values2D.v->kol1 - vLn->vkol1;
      break;
    case WLZ_GREY_TAB_RECT:
      *baseGVP = gVWSp->values2D.r->values;
      *offset = (gVWSp->values2D.r->width *
                 (line - gVWSp->values2D.r->line1)) +
	        kol - gVWSp->values2D.r->kol1;
      break;
    case WLZ_GREY_TAB_INTL:
      kol0 = kol - gVWSp->values2D.i->kol1;
      vILn = gVWSp->values2D.i->vil + line - gVWSp->values2D.i->line1;
      itvCount = vILn->nintvs;
      vLn = vILn->vtbint;
      while(itvCount-- > 0)
      {
	if((kol0 >= vLn->vkol1 - 1) && (kol0 <= vLn->vlastkl))
	{
	  *baseGVP = vLn->values;
	  *offset = kol0 - vLn->vkol1;
	  itvCount = 0;				      /* Break from the loop */
	}
	++vLn;
      }
      break;
  }
}

/************************************************************************
* Function:	WlzGreyValueGet2D1					*
* Returns:	void							*
* Purpose:	Gets a single grey value/pointer for the given point	*
*		from the 2D values and domain in the work space.	*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		int line:		Line coordinate of point.	*
*		int kol:		Column coordinate of point.	*
************************************************************************/
static void	WlzGreyValueGet2D1(WlzGreyValueWSpace *gVWSp,
				   int line, int kol)
{
  int		kol1,
		kolRel,
		count,
  		offset,
		valSet = 0;
  WlzGreyP	baseGVP;
  WlzInterval	*itv;
  WlzIntervalLine *itvLn;

  kol1 = gVWSp->iDom2D->kol1;
  gVWSp->bkdFlag = 0;
  if((gVWSp->values2D.core) && 
     (kol >= kol1) && (kol <= gVWSp->iDom2D->lastkl))
  {
    if((line >= gVWSp->iDom2D->line1) && (line <= gVWSp->iDom2D->lastln))
    {
      if(gVWSp->iDom2D->type == WLZ_INTERVALDOMAIN_RECT)
      {
	valSet = 1;
	WlzGreyValueComputeGreyP2D(&baseGVP, &offset, gVWSp, line, kol);
	WlzGreyValueSetGreyP(gVWSp->gVal, gVWSp->gPtr, gVWSp->gType,
			     baseGVP, offset);
      }
      else	          /* gVWSp->iDom2D->type == WLZ_INTERVALDOMAIN_INTVL */
      {
	itvLn = gVWSp->iDom2D->intvlines + line - gVWSp->iDom2D->line1;
	count = itvLn->nintvs;
	itv = itvLn->intvs;
	kolRel = kol - kol1;
	while(count-- > 0)
	{
	  if(kolRel < itv->ileft)
	  {
	    count = 0;				      /* Break from the loop */
	  }
	  else if(kolRel <= itv->iright)
	  {
	    valSet = 1;
	    WlzGreyValueComputeGreyP2D(&baseGVP, &offset, gVWSp, line, kol);
	    WlzGreyValueSetGreyP(gVWSp->gVal, gVWSp->gPtr, gVWSp->gType,
	    			 baseGVP, offset);
	    count = 0;				      /* Break from the loop */
	  }
	  ++itv;
	}
      }
    }
  }
  if(valSet == 0)
  {
    WlzGreyValueSetBkdP(gVWSp->gVal, gVWSp->gPtr, gVWSp->gType, gVWSp->gBkd);
    gVWSp->bkdFlag = 1;
  }
}

/************************************************************************
* Function:	WlzGreyValueGet3D1					*
* Returns:	void							*
* Purpose:	Gets a single grey value/pointer for the given point	*
*		from the 3D values and domain in the work space.	*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		int plane:		Plane coordinate of point.	*
*		int line:		Line coordinate of point.	*
*		int kol:		Column coordinate of point.	*
************************************************************************/
static void	WlzGreyValueGet3D1(WlzGreyValueWSpace *gVWSp,
				   int plane, int line, int kol)
{
  int		tI0,
  		valSet = 0;
  WlzDomain	*domP;
  WlzValues	*valP;

  if((plane >= gVWSp->domain.p->plane1) &&
     (plane <= gVWSp->domain.p->lastpl))
  {
    if(gVWSp->plane == plane)
    {
      WlzGreyValueGet2D1(gVWSp, line, kol);
      valSet = 1;
    }
    else
    {
      tI0 = plane - gVWSp->domain.p->plane1;
      domP = gVWSp->domain.p->domains + tI0;
      valP = gVWSp->values.vox->values + tI0;
      /* check for non-NULL domain and valuetable
	 pointers until empty obj consistently implemented */
      /*if(domP && valP)*/
      if(domP && valP && (*domP).core && (*valP).core)
      {
        gVWSp->plane = plane;
	gVWSp->iDom2D = (*domP).i;
	gVWSp->values2D = (*valP);
	gVWSp->gTabType2D = gVWSp->gTabTypes3D[tI0];
        WlzGreyValueGet2D1(gVWSp, line, kol);
        valSet = 1;
      }
    }
  }
  if(valSet == 0)
  {
    WlzGreyValueSetBkdP(gVWSp->gVal, gVWSp->gPtr, gVWSp->gType, gVWSp->gBkd);
    gVWSp->bkdFlag = 1;
  }
}

/************************************************************************
* Function:	WlzGreyValueGet2DCon					*
* Returns:	void							*
* Purpose:	Gets four grey values/pointers for the given point	*
*		from the 2D values and domain in the work space.	*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		int line:		Line coordinate of point.	*
*		int kol:		Column coordinate of point.	*
************************************************************************/
static void	WlzGreyValueGet2DCon(WlzGreyValueWSpace *gVWSp,
				     int line, int kol)
{
  int		kol1,
		kolRel,
		count,
  		pass = 2,
  		offset;
  WlzGreyP	baseGVP;
  WlzInterval	*itv;
  WlzIntervalLine *itvLn;
  WlzGreyV	*gVP;
  WlzGreyP	*gPP;

  kol1 = gVWSp->iDom2D->kol1;
  gVP = gVWSp->gVal;
  gPP = gVWSp->gPtr;
  WlzGreyValueSetBkdPN(gVP, gPP, gVWSp->gType, gVWSp->gBkd, 4);
  if((gVWSp->values2D.core) &&
     ((kol + 1) >= kol1) && (kol <= gVWSp->iDom2D->lastkl))
  {
    while(pass-- > 0)
    {
      if((line >= gVWSp->iDom2D->line1) && (line <= gVWSp->iDom2D->lastln))
      {
	if(gVWSp->iDom2D->type == WLZ_INTERVALDOMAIN_RECT)
	{
	  WlzGreyValueComputeGreyP2D(&baseGVP, &offset, gVWSp, line, kol);
	  if(kol >= kol1)
	  {
	    WlzGreyValueSetGreyP(gVP, gPP, gVWSp->gType, baseGVP, offset);
	  }
	  if(kol <= (gVWSp->iDom2D->lastkl - 1))
	  {
	    WlzGreyValueSetGreyP(gVP + 1, gPP + 1, gVWSp->gType,
	    			 baseGVP, offset + 1);
	  }
	}
	else		  /* gVWSp->iDom2D->type == WLZ_INTERVALDOMAIN_INTVL */
	{
	  itvLn = gVWSp->iDom2D->intvlines + line - gVWSp->iDom2D->line1;
	  count = itvLn->nintvs;
	  itv = itvLn->intvs;
	  kolRel = kol - kol1;
	  while(count-- > 0)
	  {
	    if((kolRel + 1) < itv->ileft)
	    {
	      count = 0;			      /* Break from the loop */
	    }
	    else if(kolRel <= itv->iright)
	    {
	      WlzGreyValueComputeGreyP2D(&baseGVP, &offset, gVWSp, line, kol);
	      if(kol >= (kol1 + itv->ileft))
	      {
		WlzGreyValueSetGreyP(gVP, gPP, gVWSp->gType, baseGVP, offset);
	      }
	      if(kol <= (itv->iright + kol1 - 1))
	      {
		WlzGreyValueSetGreyP(gVP + 1, gPP + 1, gVWSp->gType,
				     baseGVP, offset + 1);
	      }
	    }
	    ++itv;
	  }
	}
      }
      ++line;
      gVP += 2;
      gPP += 2;
    }
  }
}

/************************************************************************
* Function:	WlzGreyValueGet3DCon					*
* Returns:	void							*
* Purpose:	Gets eight grey value/pointers for the given point	*
*		from the 3D values and domain in the work space.	*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		int plane:		Plane coordinate of point.	*
*		int line:		Line coordinate of point.	*
*		int kol:		Column coordinate of point.	*
************************************************************************/
static void	WlzGreyValueGet3DCon(WlzGreyValueWSpace *gVWSp,
				   int plane, int line, int kol)
{
  int		tI0,
  		planeOff,
		planeRel,
		savePlane;
  WlzDomain	*domP;
  WlzValues	*valP;
  WlzObjectType	saveGTabType2D;
  WlzIntervalDomain *saveIDom2D;
  WlzValues	saveValues2D;
  int		planeSet[2];
  WlzGreyP	saveGPtr[4];
  WlzGreyV	saveGVal[4];

  planeOff = 0;
  while(planeOff < 2)
  {
    planeSet[planeOff] = 0;
    if((plane > gVWSp->domain.p->plane1) && (plane < gVWSp->domain.p->lastpl))
    {
      if(plane == gVWSp->plane)
      {
	WlzGreyValueGet2DCon(gVWSp, line, kol);
	planeSet[planeOff] = 1;
      }
      else
      {
	planeRel = plane - gVWSp->domain.p->plane1;
	domP = gVWSp->domain.p->domains + planeRel;
	valP = gVWSp->values.vox->values + planeRel;
	if(domP && valP)
	{
          if(planeSet[0])
	  {
	    savePlane = gVWSp->plane;
	    saveIDom2D = gVWSp->iDom2D;
	    saveValues2D = gVWSp->values2D;
	    saveGTabType2D = gVWSp->gTabType2D;
	  }
	  gVWSp->plane = plane;
	  gVWSp->iDom2D = (*domP).i;
	  gVWSp->values2D = (*valP);
	  gVWSp->gTabType2D = gVWSp->gTabTypes3D[planeRel];
	  WlzGreyValueGet2D1(gVWSp, line, kol);
	  planeSet[planeOff] = 1;
	}
      }
    }
    if(planeOff == 0)
    {
      if(planeSet[0])
      {
	tI0 = 4;
	while(--tI0 >= 0)
	{
	  saveGPtr[tI0] = gVWSp->gPtr[tI0];
	  saveGVal[tI0] = gVWSp->gVal[tI0];
	}
      }
      else
      {
        WlzGreyValueSetBkdPN(saveGVal, saveGPtr,
			     gVWSp->gType, gVWSp->gBkd, 4);
      }
    }
    ++plane;
    ++planeOff;
  }
  if(planeSet[0] && planeSet[1])
  {
    gVWSp->plane = savePlane;
    gVWSp->iDom2D = saveIDom2D;
    gVWSp->values2D = saveValues2D;
    gVWSp->gTabType2D = saveGTabType2D;
  }
  tI0 = 4;
  while(--tI0 >= 0)
  {
    gVWSp->gPtr[tI0 + 4] = gVWSp->gPtr[tI0];
    gVWSp->gPtr[tI0] = saveGPtr[tI0];
    gVWSp->gVal[tI0 + 4] = gVWSp->gVal[tI0];
    gVWSp->gVal[tI0] = saveGVal[tI0];
  }
}

/************************************************************************
* Function:	WlzGreyValueGetTransCon					*
* Returns:	void							*
* Purpose:	Gets four or eight grey value/pointers for the given	*
*		point from the 2D or 3D values and domain in the work	*
*		space.							*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		int plane:		Plane coordinate of point.	*
*		int line:		Line coordinate of point.	*
*		int kol:		Column coordinate of point.	*
************************************************************************/
static void	WlzGreyValueGetTransCon(WlzGreyValueWSpace *gVWSp,
				        int plane, int line, int kol)
{
  int		idN,
  		idX,
  		idY,
		idZ;
  WlzDVertex2	vtx2;
  WlzDVertex3	vtx3;
  WlzGreyP	gPtr[8];
  WlzGreyV	gVal[8];

  switch(gVWSp->objType)
  {
    case WLZ_2D_DOMAINOBJ:
      idN = 0;
      for(idY = 0; idY < 2; ++idY)
      {
	for(idX = 0; idX < 2; ++idX)
	{
	  vtx2.vtX = kol + idX;
	  vtx2.vtY = line + idY;
	  vtx2 = WlzAffineTransformVertexD2(gVWSp->invTrans, vtx2, NULL);
	  WlzGreyValueGet2D1(gVWSp, vtx2.vtY, vtx2.vtX);
	  gPtr[idN] = gVWSp->gPtr[0];
	  gVal[idN] = gVWSp->gVal[0];
	  ++idN;
	}
      }
      for(idN = 0; idN < 4; ++idN)
      {
        gVWSp->gPtr[idN] = gPtr[idN];
	gVWSp->gVal[idN] = gVal[idN];
      }
      break;
    case WLZ_3D_DOMAINOBJ:
      idN = 0;
      for(idZ = 0; idZ < 2; ++idZ)
      {
	for(idY = 0; idY < 2; ++idY)
	{
	  for(idX = 0; idX < 2; ++idX)
	  {
	    vtx3.vtX = kol + idX;
	    vtx3.vtY = line + idY;
	    vtx3.vtZ = plane + idZ;
	    vtx3 = WlzAffineTransformVertexD3(gVWSp->invTrans, vtx3, NULL);
	    WlzGreyValueGet3D1(gVWSp, vtx3.vtZ, vtx3.vtY, vtx3.vtX);
	    gPtr[idN] = gVWSp->gPtr[0];
	    gVal[idN] = gVWSp->gVal[0];
	    ++idN;
	  }
	}
      }
      for(idN = 0; idN < 8; ++idN)
      {
        gVWSp->gPtr[idN] = gPtr[idN];
	gVWSp->gVal[idN] = gVal[idN];
      }
      break;
  }
}

/************************************************************************
* Function:	WlzGreyValueFreeWSp					*
* Returns:	void							*
* Purpose:	Free's the given grey value work space.			*
*		object.							*
* Global refs:	-							*
* Parameters:	WlzObject *obj:		Given object.			*
*		WlzErrorNum *dstErrNum:	Destination error pointer.	*
************************************************************************/
void		WlzGreyValueFreeWSp(WlzGreyValueWSpace *gVWSp)
{
  WLZ_DBG((WLZ_DBG_LVL_1),
  	   ("WlzGreyValueFreeWSp FE 0x%lx\n",
	    (unsigned long )gVWSp));
  if(gVWSp)
  {
    if(gVWSp->invTrans)
    {
      WlzFreeAffineTransform(gVWSp->invTrans);
    }
    if(gVWSp->gTabTypes3D){
      AlcFree((void *) gVWSp->gTabTypes3D);
    }
    AlcFree(gVWSp);
  }
  WLZ_DBG((WLZ_DBG_LVL_FN|WLZ_DBG_LVL_1),
  	  ("WlzGreyValueFreeWSp FX\n"));
}

/************************************************************************
* Function:	WlzGreyValueMakeWSp					*
* Returns:	WlzGreyValueWSpace *: Grey value work space or NULL	*
*					on error.			*
* Purpose:	Creates a grey value work space from the given object.	*
* Global refs:	-							*
* Parameters:	WlzObject *obj:		Given object.			*
*		WlzErrorNum *dstErrNum:	Destination error pointer.	*
************************************************************************/
WlzGreyValueWSpace *WlzGreyValueMakeWSp(WlzObject *obj,
					   WlzErrorNum *dstErrNum)
{
  int		planeIdx, numPlanes;
  WlzDomain	*planeDomains;
  WlzValues	*planeValues;
  WlzObjectType	gTabType0,
	        gTabType1 = WLZ_DUMMY_ENTRY;
  WlzGreyType	gType0,
		gType1 = WLZ_GREY_ERROR;
  WlzPixelV	bkdPix;
  WlzAffineTransform *trans0,
  		*trans1;
  WlzGreyValueWSpace *gVWSp = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  WLZ_DBG((WLZ_DBG_LVL_1),
  	   ("WlzGreyValueMakeWSp FE 0x%lx 0x%lx\n",
	    (unsigned long )obj, (unsigned long )dstErrNum));
  if(obj == NULL)
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else
  {
    switch(obj->type)
    {
      case WLZ_2D_DOMAINOBJ:
	if(obj->domain.core == NULL)
	{
	  errNum = WLZ_ERR_DOMAIN_NULL;
	}
	else if(obj->values.core == NULL)
	{
	  errNum = WLZ_ERR_VALUES_NULL;
	}
	else if((gVWSp = (WlzGreyValueWSpace *)AlcCalloc(1,
					  sizeof(WlzGreyValueWSpace))) == NULL)
	{
	  errNum = WLZ_ERR_MEM_ALLOC;
	}
	else
	{
	  gVWSp->objType = obj->type;
	  gVWSp->values = obj->values;
	  gVWSp->values2D = obj->values;
	  gVWSp->gType = WlzGreyTableTypeToGreyType(obj->values.core->type,
	  					    &errNum);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    gVWSp->gTabType2D = WlzGreyTableTypeToTableType(obj->values.
							    core->type,
							    &errNum);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    switch(gVWSp->gTabType2D)
	    {
	      case WLZ_GREY_TAB_RAGR:
		bkdPix = gVWSp->values2D.v->bckgrnd;
		break;
	      case WLZ_GREY_TAB_RECT:
		bkdPix = gVWSp->values2D.r->bckgrnd;
		break;
	      case WLZ_GREY_TAB_INTL:
		bkdPix = gVWSp->values2D.i->bckgrnd;
		break;
	      default:
		errNum = WLZ_ERR_VALUES_TYPE;
		break;
	    }
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    switch(gVWSp->gType)
	    {
	      case WLZ_GREY_INT:
	      case WLZ_GREY_SHORT:
	      case WLZ_GREY_UBYTE:
	      case WLZ_GREY_FLOAT:
	      case WLZ_GREY_DOUBLE:
		break;
	      default:
		errNum = WLZ_ERR_GREY_TYPE;
		break;
	    }
	  }
	  if((gVWSp->gType != bkdPix.type) && (errNum == WLZ_ERR_NONE))
	  {
	    errNum = WLZ_ERR_VALUES_DATA;
	  }
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  gVWSp->gBkd = bkdPix.v;
	  gVWSp->domain = obj->domain;
	  gVWSp->iDom2D = obj->domain.i;
	  if((gVWSp->iDom2D->type != WLZ_INTERVALDOMAIN_INTVL) &&
	     (gVWSp->iDom2D->type != WLZ_INTERVALDOMAIN_RECT))
	  {
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	  }
	}
	break;
      case WLZ_3D_DOMAINOBJ:
	if(obj->domain.core->type != WLZ_PLANEDOMAIN_DOMAIN)
	{
	  errNum = WLZ_ERR_DOMAIN_TYPE;
	}
	else if((gVWSp = (WlzGreyValueWSpace *)AlcCalloc(1,
					  sizeof(WlzGreyValueWSpace))) == NULL)
	{
	  errNum = WLZ_ERR_MEM_ALLOC;
	}
	else
	{
	  gVWSp->objType = obj->type;
	  gVWSp->domain = obj->domain;
	  gVWSp->values = obj->values;

	  /* put in a list of grey-table types - purely for efficiency
	     to avoid re-computing the type for each voxel request.
	     When table types are no longer computed this in principle
	     could go */
	  numPlanes = obj->domain.p->lastpl - obj->domain.p->plane1 + 1;
	  if( (gVWSp->gTabTypes3D = (WlzObjectType *)
	       AlcCalloc(numPlanes, sizeof(WlzObjectType))) == NULL )
	  {
	    errNum = WLZ_ERR_MEM_ALLOC;
	  }
	  else
	  {
	    WlzObjectType	*gTabTypes3D;

	    gVWSp->values2D.core = NULL;
	    planeIdx = obj->domain.p->plane1;
	    planeDomains = obj->domain.p->domains;
	    planeValues = obj->values.vox->values;
	    gTabTypes3D = gVWSp->gTabTypes3D;
	    while((planeIdx <= obj->domain.p->lastpl) &&
		  (errNum == WLZ_ERR_NONE))
	    {
	      if(((*planeValues).core) &&
		 ((*planeValues).core->type != WLZ_EMPTY_OBJ))
	      {
		gType0 = WlzGreyTableTypeToGreyType((*planeValues).core->type,
						    &errNum);
		if(errNum == WLZ_ERR_NONE)
		{
		  gTabType0 = WlzGreyTableTypeToTableType((*planeValues).
							  core->type,
							  &errNum);
		  *gTabTypes3D = gTabType0;
		}
		if(errNum == WLZ_ERR_NONE)
		{
		  if((gType0 != gType1) && (gType1 != WLZ_GREY_ERROR))
		  {
		    errNum = WLZ_ERR_VALUES_DATA;
		  }
		  else
		  {
		    gType1 = gType0;
		    gTabType1 = gTabType0;
		    if(gVWSp->values2D.core == NULL)
		    {
		      gVWSp->plane = planeIdx;
		      gVWSp->iDom2D = (*planeDomains).i;
		      gVWSp->values2D = *planeValues;
		    }
		  }
		}
	      }
	      ++planeIdx;
	      ++planeDomains;
	      ++planeValues;
	      ++gTabTypes3D;
	    }
	    if((gType1 == WLZ_GREY_ERROR) || (gTabType1 == WLZ_DUMMY_ENTRY))
	    {
	      errNum = WLZ_ERR_VALUES_DATA;
	    }
	  }
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  gVWSp->gType = gType0;
	  gVWSp->gTabType2D = gVWSp->gTabTypes3D[0];
	  switch(gVWSp->gTabType2D)
	  {
	    case WLZ_GREY_TAB_RAGR:
	      bkdPix = gVWSp->values2D.v->bckgrnd;
	      break;
	    case WLZ_GREY_TAB_RECT:
	      bkdPix = gVWSp->values2D.r->bckgrnd;
	      break;
	    case WLZ_GREY_TAB_INTL:
	      bkdPix = gVWSp->values2D.i->bckgrnd;
	      break;
	    default:
	      errNum = WLZ_ERR_VALUES_TYPE;
	      break;
	  }
	  if((gVWSp->gType != bkdPix.type) && (errNum == WLZ_ERR_NONE))
	  {
	    errNum = WLZ_ERR_VALUES_DATA;
	  }
	  else if((gVWSp->iDom2D->type != WLZ_INTERVALDOMAIN_INTVL) &&
		  (gVWSp->iDom2D->type != WLZ_INTERVALDOMAIN_RECT))
	  {
	    errNum = WLZ_ERR_DOMAIN_TYPE;
	  }
	  else
	  {
	    gVWSp->gBkd = bkdPix.v;
	  }
	}
	break;
      case WLZ_TRANS_OBJ:
	trans0 = NULL;
	while((errNum == WLZ_ERR_NONE) && (obj->type == WLZ_TRANS_OBJ))
	{
	  if(trans0 == NULL)
	  {
	    trans0 = WlzAffineTransformCopy(obj->domain.t, &errNum);
	  }
	  else
	  {
	    if((trans1 = WlzAffineTransformProduct(trans0, obj->domain.t,
	    				           &errNum)) != NULL)
	    {
	      WlzFreeAffineTransform(trans0);
	      trans0 = trans1;
	      trans1 = NULL;
	    }
	  }
	  if((obj = obj->values.obj) == NULL)
	  {
	    errNum = WLZ_ERR_OBJECT_NULL;
	  }
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  gVWSp = WlzGreyValueMakeWSp(obj, &errNum);
	}
	if(errNum == WLZ_ERR_NONE)
	{
	  gVWSp->invTrans = WlzAffineTransformInverse(trans0, &errNum);
	}
	if(trans0)
	{
	  WlzFreeAffineTransform(trans0);
	}
	break;
      default:
	errNum = WLZ_ERR_OBJECT_TYPE;
	break;
    }
  }
  if((errNum != WLZ_ERR_NONE) && (gVWSp != NULL))
  {
    WlzGreyValueFreeWSp(gVWSp);
    gVWSp = NULL;
  }
  if(dstErrNum)
  {
    *dstErrNum = errNum;
  }
  WLZ_DBG((WLZ_DBG_LVL_FN|WLZ_DBG_LVL_1),
  	  ("WlzGreyValueMakeWSp FX 0x%lx\n",
	   (unsigned long )gVWSp));
  return(gVWSp);
}

/************************************************************************
* Function:	WlzGreyValueGet						*
* Returns:	void							*
* Purpose:	Gets a single grey value/pointer for the given point	*
*		from the object with which the given work space was 	*
*		initialised.						*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		double plane:		Plane (z) coordinate of point.	*
*		double line:		Line (y) coordinate of point.	*
*		double kol:		Column (x) coordinate of point.	*
************************************************************************/
void		WlzGreyValueGet(WlzGreyValueWSpace *gVWSp,
			        double plane, double line, double kol)
{
  WlzDVertex2	vtx2;
  WlzDVertex3	vtx3;

  if(gVWSp)
  {
    switch(gVWSp->objType)
    {
      case WLZ_2D_DOMAINOBJ:
	if(gVWSp->invTrans)
	{
	  vtx2.vtX = kol;
	  vtx2.vtY = line;
	  vtx2 = WlzAffineTransformVertexD2(gVWSp->invTrans, vtx2, NULL);
	  kol = vtx2.vtX;
	  line = vtx2.vtY;
	}
	WlzGreyValueGet2D1(gVWSp, line, kol);
	break;
      case WLZ_3D_DOMAINOBJ:
	if(gVWSp->invTrans)
	{
	  vtx3.vtX = kol;
	  vtx3.vtY = line;
	  vtx3.vtZ = plane;
	  vtx3 = WlzAffineTransformVertexD3(gVWSp->invTrans, vtx3, NULL);
	  kol = vtx3.vtX;
	  line = vtx3.vtY;
	  plane = vtx3.vtZ;
	}
	WlzGreyValueGet3D1(gVWSp, plane, line, kol);
	break;
      default:
	break;
    }
  }
}

/************************************************************************
* Function:	WlzGreyValueGetCon					*
* Returns:	void							*
* Purpose:	Gets the four/eight connected grey values/pointers for	*
*		the given point which lie at:				*
*		  (p, k, l),          (p, k + 1, l),			*
*		  (p, k, l + 1),      (p, k + 1, l + 1),		*
*		  (p + 1, k, l),      (p + 1, k + 1, l),		*
*		  (p + 1, k, l + 1),  (p + 1, k + 1, l + 1).		*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		double plane:		Plane (z) coordinate of point.	*
*		double line:		Line (y) coordinate of point.	*
*		double kol:		Column (x) coordinate of point.	*
************************************************************************/
void		WlzGreyValueGetCon(WlzGreyValueWSpace *gVWSp,
			           double plane, double line, double kol)
{
  if(gVWSp)
  {
    if(gVWSp->invTrans)
    {
      WlzGreyValueGetTransCon(gVWSp, plane, line, kol);
    }
    else
    {
      int ikol, iline, iplane;
      ikol = (int) ((kol < 0.0) ? kol - 1.0 : kol);
      iline = (int) ((kol < 0.0) ? line - 1.0 : line);
      iplane = (int) ((kol < 0.0) ? plane - 1.0 : plane);
      switch(gVWSp->objType)
      {
	case WLZ_2D_DOMAINOBJ:
	  WlzGreyValueGet2DCon(gVWSp, iline, ikol);
	  break;
	case WLZ_3D_DOMAINOBJ:
	  WlzGreyValueGet3DCon(gVWSp, iplane, iline, ikol);
	  break;
	default:
	  break;
      }
    }
  }
}

/************************************************************************
* Function:	WlzGreyValueGetGreyType					*
* Returns:	WlzGreyType:		The workspace's object's grey	*
*					type.				*
* Purpose:	Access function to get the object's grey type.		*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		WlzErrorNum *dstErr:	Destination error pointer,	*
*					may be NULL.			*
************************************************************************/
WlzGreyType	WlzGreyValueGetGreyType(WlzGreyValueWSpace *gVWSp,
					WlzErrorNum *dstErr)
{
  WlzGreyType	gType = WLZ_GREY_ERROR;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(gVWSp == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else
  {
    gType = gVWSp->gType;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(gType);
}

/************************************************************************
* Function:	WlzGreyValueGetI					*
* Returns:	int:			Gets the grey value as an int.	*
* Purpose:	Gets a single grey value for the given point from the	*
*		object with which the given work space was initialised.	*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		double plane:		Plane (z) coordinate of point.	*
*		double line:		Line (y) coordinate of point.	*
*		double kol:		Column (x) coordinate of point.	*
************************************************************************/
int		WlzGreyValueGetI(WlzGreyValueWSpace *gVWSp,
			         double plane, double line, double kol)
{
  int		val = 0;

  if(gVWSp)
  {
    WlzGreyValueGet(gVWSp, plane, line, kol);
    switch(gVWSp->gType)
    {
      case WLZ_GREY_INT:
        val = gVWSp->gVal[0].inv;
        break;
      case WLZ_GREY_SHORT:
        val = gVWSp->gVal[0].shv;
        break;
      case WLZ_GREY_UBYTE:
        val = gVWSp->gVal[0].ubv;
        break;
      case WLZ_GREY_FLOAT:
        val = WLZ_NINT(gVWSp->gVal[0].flv);
	break;
      case WLZ_GREY_DOUBLE:
        val = WLZ_NINT(gVWSp->gVal[0].dbv);
	break;
    }
  }
  return(val);
}

/************************************************************************
* Function:	WlzGreyValueGetD					*
* Returns:	double:			Gets the grey value as a	*
*					double.				*
* Purpose:	Gets a single grey value for the given point from the	*
*		object with which the given work space was initialised.	*
* Global refs:	-							*
* Parameters:	WlzGreyValueWSpace *gVWSp: Grey value work space.	*
*		double plane:		Plane (z) coordinate of point.	*
*		double line:		Line (y) coordinate of point.	*
*		double kol:		Column (x) coordinate of point.	*
************************************************************************/
double		WlzGreyValueGetD(WlzGreyValueWSpace *gVWSp,
				 double plane, double line, double kol)
{
  double		val = 0;

  if(gVWSp)
  {
    WlzGreyValueGet(gVWSp, plane, line, kol);
    switch(gVWSp->gType)
    {
      case WLZ_GREY_INT:
        val = gVWSp->gVal[0].inv;
        break;
      case WLZ_GREY_SHORT:
        val = gVWSp->gVal[0].shv;
        break;
      case WLZ_GREY_UBYTE:
        val = gVWSp->gVal[0].ubv;
        break;
      case WLZ_GREY_FLOAT:
        val = gVWSp->gVal[0].flv;
	break;
      case WLZ_GREY_DOUBLE:
        val = gVWSp->gVal[0].dbv;
	break;
    }
  }
  return(val);
}
