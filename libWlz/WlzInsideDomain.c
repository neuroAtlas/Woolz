#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzInsideDomain.c
* \author       Bill Hill
* \date         March 1999
* \version      $Id$
* \note
*               Copyright
*               2002 Medical Research Council, UK.
*               All rights reserved.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \brief	Functions which check whether a given vertex lies within an
* 		object's domain.
* \ingroup	WlzAccess
* \todo         -
* \bug          None known.
*/
#include <stdlib.h>
#include <Wlz.h>

/*!
* \return	Non zero if point is inside.
* \ingroup      WlzAccess
* \brief	Looks to see if the given point is within the given
*               interval domain.
* \param	iDom			Given interval domain.
* \param	line			Line (y) position.
* \param	kol			Column (x) position.
* \param	dstErr			Destination error pointer, may
*                                       be NOT NULL.
*/
static int	WlzInsideDomain2D(WlzIntervalDomain *iDom,
				  int line,  int kol,
				  WlzErrorNum *dstErr)
{
  int		itvCount,
  		kolRel,
  		inside = 0;
  WlzInterval	*itv;
  WlzIntervalLine *itvLn; 

  if((line >= iDom->line1) &&
     (line <= iDom->lastln) &&
     (kol >= iDom->kol1) &&
     (kol <= iDom->lastkl))
  {
    switch(iDom->type)
    {
      case WLZ_INTERVALDOMAIN_RECT:
	inside = 1;
	break;
      case WLZ_INTERVALDOMAIN_INTVL:
	kolRel = kol - iDom->kol1;
	itvLn = iDom->intvlines + line - iDom->line1;
	itv = itvLn->intvs;
	itvCount = itvLn->nintvs;
	while(itvCount-- > 0)
	{
	  if(kolRel < itv->ileft)
	  {
	    itvCount = 0; 		          /* Break from loop */
	  }
	  else if(kolRel <= itv->iright)
	  {
	    itvCount = 0;                           /* Break from loop */
	    inside = 1;
	  }
	  ++itv;
	}
	break;
      default:
	*dstErr = WLZ_ERR_DOMAIN_TYPE;
	break;
    }
  }
  return(inside);
}

/*!
* \return	Non zero if point is inside.
* \ingroup      WlzAccess
* \brief	Looks to see if the given point is within the given
*               3D domain.
* \param	pDom			Given plane domain.
* \param	plane			Plane (z) position.
* \param	line			Line (y) position.
* \param	kol			Column (x) position.
* \param	dstErr			Destination error pointer, may
*                                       be NOT NULL.
*/
static int	WlzInsideDomain3D(WlzPlaneDomain *pDom,
				  int plane, int line,  int kol,
				  WlzErrorNum *dstErr)
{
  int		planeRel,
  		inside = 0;
  WlzDomain	dom;

  if((plane >= pDom->plane1) &&
     (plane <= pDom->lastpl))
  {
    planeRel = plane - pDom->plane1;
    if((*(pDom->domains + planeRel)).core != NULL)
    {
      dom = *(pDom->domains + planeRel);
      inside = WlzInsideDomain2D(dom.i, line,  kol, dstErr);
    }
  }
  return(inside);
}

/*!
* \return	Non zero if point is inside.
* \ingroup      WlzAccess
* \brief	Looks to see if the given point is within the objects domain.
* \param	obj			Given object.
* \param	plane			Plane (z) position, only used
*                                       if the object has a 3D domain.
* \param	line			Line (y) position.
* \param	kol			Column (x) position.
* \param	dstErr			Destination error pointer, may
*                                       be NULL.
*/
int		WlzInsideDomain(WlzObject *obj,
				double plane, double line, double kol,
				WlzErrorNum *dstErr)
{
  int		inside = 0;
  WlzDVertex2	vtx2;
  WlzDVertex3	vtx3;
  WlzAffineTransform *invTrans = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  WLZ_DBG((WLZ_DBG_LVL_1),
  	  ("WlzInsideDomain FE 0x%lx %g %g %g 0x%lx\n",
	   (unsigned long )obj, plane, line, kol, (unsigned long )dstErr));
  if(obj == NULL)
  {
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else
  {
    /* switch on object type */
    switch( obj->type )
    {
      case WLZ_2D_DOMAINOBJ:
	if(obj->domain.core == NULL)
	{
	  errNum = WLZ_ERR_DOMAIN_NULL;
	}
	else
	{
	  inside = WlzInsideDomain2D(obj->domain.i, line,  kol,
	  			     &errNum);
	}
	break;
      case WLZ_3D_DOMAINOBJ:
	if(obj->domain.core == NULL)
	{
	  errNum = WLZ_ERR_DOMAIN_NULL;
	}
	else if(obj->domain.core->type != WLZ_PLANEDOMAIN_DOMAIN)
	{
          errNum = WLZ_ERR_PLANEDOMAIN_TYPE;
	}
	else
	{
	  inside = WlzInsideDomain3D(obj->domain.p, plane, line,  kol,
	  			     &errNum);
	}
	break;
      case WLZ_TRANS_OBJ:
	if(obj->domain.core == NULL)
	{
	  errNum = WLZ_ERR_DOMAIN_NULL;
	}
	else if(obj->values.core == NULL)
	{
	  errNum = WLZ_ERR_VALUES_NULL;
	}
	else if((invTrans = WlzAffineTransformInverse(obj->domain.t,
						      &errNum)) != NULL)
	{
	  switch(WlzAffineTransformDimension(obj->domain.t,NULL))
	  {
	    case 2:
	      vtx2.vtX = kol;
	      vtx2.vtY = line;
	      vtx2 = WlzAffineTransformVertexD2(invTrans, vtx2, &errNum);
	      if(errNum == WLZ_ERR_NONE)
	      {
		inside = WlzInsideDomain(obj->values.obj,
					 plane, vtx2.vtY, vtx2.vtX, &errNum);
	      }
	      break;
	    case 3:
	      vtx3.vtX = kol;
	      vtx3.vtY = line;
	      vtx3.vtZ = plane;
	      vtx3 = WlzAffineTransformVertexD3(invTrans, vtx3, &errNum);
	      if(errNum == WLZ_ERR_NONE)
	      {
		inside = WlzInsideDomain(obj->values.obj,
					 vtx3.vtZ, vtx3.vtY, vtx3.vtX, &errNum);
	      }
	    default:
	      errNum = WLZ_ERR_TRANSFORM_TYPE;
	      break;
	  }
	  (void )WlzFreeAffineTransform(invTrans);
	}
	break;
      default:
	errNum = WLZ_ERR_OBJECT_TYPE;
	break;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  WLZ_DBG((WLZ_DBG_LVL_FN|WLZ_DBG_LVL_1),
  	  ("WlzInsideDomain FX %d\n",
	   inside));
  return(inside);
}
