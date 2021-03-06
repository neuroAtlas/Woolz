#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzStdStructElements_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlz/WlzStdStructElements.c
* \author       Richard Baldock
* \date         September 2003
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \brief	Standard structuring elements in 2D and 3D.
* \ingroup	WlzMorphologyOps
*/

#include <stdlib.h>
#include <math.h>

#include <Wlz.h>

/*
 * 	010
 *	111
 *	010
 */

static WlzObject *WlzSpecial_h4(WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  WlzInterval *intl;
  int 	i;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL, 
				       -1, 1, -1, 1, &errNum)) != NULL){
    values.core = NULL;
    if((obj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values,
			  NULL, NULL, &errNum)) != NULL){
      intl = (WlzInterval *) AlcMalloc(sizeof(WlzInterval)*3);
      domain.i->freeptr = AlcFreeStackPush(domain.i->freeptr, (void *)intl,
      					   NULL);
      for(i = -1; i < 2; i++){
	if(i == 0){
	  intl->ileft = 0;
	  intl->iright = 2;
	} else {
	  intl->ileft = 1;
	  intl->iright = 1;
	}
	WlzMakeInterval(i, domain.i, 1, intl);
	intl++;
      }
    }
    else {
      WlzFreeIntervalDomain(domain.i);
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}
/*
 *	111
 *	010
 *	111
 */
static WlzObject *WlzSpecial_ex4(WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  WlzInterval *intl;
  int 	i;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL, 
				       -1, 1, -1, 1, &errNum)) != NULL){
    values.core = NULL;
    if((obj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values,
			  NULL, NULL, &errNum)) != NULL){
      intl = (WlzInterval *) AlcMalloc(sizeof(WlzInterval)*3);
      domain.i->freeptr = AlcFreeStackPush(domain.i->freeptr, (void *)intl,
      					   NULL);
      for(i = -1; i < 2; i++){
	if(i == 0){
	  intl->ileft = 1;
	  intl->iright = 1;
	} else {
	  intl->ileft = 0;
	  intl->iright = 2;
	}
	WlzMakeInterval(i, domain.i, 1, intl);
	intl++;
      }
    }
    else {
      WlzFreeIntervalDomain(domain.i);
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}
/*
 *	111
 *	111
 *	111
 */	
static WlzObject *WlzSpecial_a8(WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  WlzInterval *intl;
  int 	i;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL, 
				       -1, 1, -1, 1, &errNum)) != NULL){
    values.core = NULL;
    if((obj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values,
			  NULL, NULL, &errNum)) != NULL){
      intl = (WlzInterval *) AlcMalloc(sizeof(WlzInterval)*3);
      domain.i->freeptr = AlcFreeStackPush(domain.i->freeptr, (void *)intl,
      					   NULL);
      for(i = -1; i < 2; i++){
	intl->ileft = 0;
	intl->iright = 2;
	WlzMakeInterval(i, domain.i, 1, intl);
	intl++;
      }
    }
    else {
      WlzFreeIntervalDomain(domain.i);
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}

/*
 *  0:		 1:     	2:	3:	4:	5:    
 *    011	   110  	  111     110     010     011       
 *    111          111         	  111     111     111     111
 *    110          011            010     110     111     011
 */
static WlzObject *WlzSpecial_h6(int num, WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzInterval	*intl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj = WlzSpecial_a8(&errNum)) != NULL){
    intl = obj->domain.i->intvlines->intvs;
    switch(num){
    case 0:
      intl->iright = 1;
      intl += 2;
      intl->ileft = 1;
      break;
    case 1:
      intl->ileft = 1;
      intl += 2;
      intl->iright = 1;
      break;
    case 2:
      intl->ileft = 1;
      intl->iright = 1;
      break;
    case 3:
      intl->iright = 1;
      intl += 2;
      intl->iright = 1;
      break;
    case 4:
      intl += 2;
      intl->ileft = 1;
      intl->iright = 1;
      break;
    case 5:
      intl->ileft += 1;
      intl += 2;
      intl->ileft += 1;
      break;
    default:
      errNum=WLZ_ERR_PARAM_DATA;
      WlzFreeObj(obj);
      obj = NULL;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}

/*
 *	0:	1:	2:	3:
 *	  010	  010	  011	  110
 * 	  111	  111	  111 	  111
 *	  110	  011	  010	  010
 */
static WlzObject *WlzSpecial_h5(int num, WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzInterval *intl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj = WlzSpecial_h4(&errNum)) != NULL){
    intl = obj->domain.i->intvlines->intvs;
    switch(num){
    case 0:
      intl->ileft = 0;
      break;
    case 1:
      intl->iright = 2;
      break;
    case 2:
      intl += 2;
      intl->iright = 2;
      break;
    case 3:
      intl += 2;
      intl->ileft = 0;
      break;
    default:
      errNum=WLZ_ERR_PARAM_DATA;
      WlzFreeObj(obj);
      obj = NULL;
      break;
    }
  }


  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}

/*
 *	0:	1:	2:	3:
 *	  111	  111	  110	  011
 * 	  111	  111	  111 	  111
 *	  011	  110	  111	  111
 */
static WlzObject *WlzSpecial_h7(int num, WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzInterval	*intl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj = WlzSpecial_a8(&errNum)) != NULL){
    intl = obj->domain.i->intvlines->intvs;
    switch(num){
    case 0:
      intl->ileft = 1;
      break;
    case 1:
      intl->iright = 1;
      break;
    case 2:
      intl += 2;
      intl->iright = 1;
      break;
    case 3:
      intl += 2;
      intl->ileft = 1;
      break;
    default:
      errNum=WLZ_ERR_PARAM_DATA;
      WlzFreeObj(obj);
      obj = NULL;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}
/*
 * num = 0:	1:	2:	3:
 *	   000	  010	  010	  010
 *	   111    011	  111	  110
 *	   010	  010	  000	  010
 */
static WlzObject *WlzSpecial_a3(int num, WlzErrorNum *dstErr)  /*-*/
{
  WlzObject 	*obj=NULL;
  WlzInterval *intl;
  int 	i;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj = WlzSpecial_a8(&errNum)) != NULL){
    switch(num){
    case 0:
      intl = obj->domain.i->intvlines[0].intvs;
      intl->ileft = 1;
      intl->iright = 1;
      obj->domain.i->intvlines[2].nintvs = 0;
      WlzStandardIntervalDomain(obj->domain.i);
      break;
    case 1:
      intl = obj->domain.i->intvlines[0].intvs;
      for(i = -1; i < 2; i++){
	intl->ileft = 1;
	if(i == 0){
	  intl->iright = 2;
	}
	else intl->iright = 1;
	intl++;
      }
      WlzStandardIntervalDomain(obj->domain.i);
      break;
    case 2:
      intl = obj->domain.i->intvlines[2].intvs;
      intl->ileft = 1;
      intl->iright = 1;
      obj->domain.i->intvlines[0].nintvs = 0;
      WlzStandardIntervalDomain(obj->domain.i);
      break;
    case 3:
      intl = obj->domain.i->intvlines[0].intvs;
      for(i = -1; i < 2; i++){
	if(i == 0)
	  intl->ileft = 0;
	else intl->ileft = 1;
	intl->iright = 1;
	intl++;
      }
      WlzStandardIntervalDomain(obj->domain.i);
      break;
    default:
      errNum=WLZ_ERR_PARAM_DATA;
      WlzFreeObj(obj);
      obj = NULL;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return(obj);
}
/*
 * 11
 */
static WlzObject *WlzSpecial_e1(WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzInterval	*intl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj = WlzMakeSinglePixelObject(WLZ_2D_DOMAINOBJ, 0, 0, 0,
  			             &errNum)) != NULL){
    intl = obj->domain.i->intvlines->intvs;
    intl->ileft = 0;
    intl->iright = 1;
    obj->domain.i->lastkl = 1;
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return obj;
}
/*
 * 111
 */
static WlzObject *WlzSpecial_e2(WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj=NULL;
  WlzInterval	*intl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj = WlzMakeSinglePixelObject(WLZ_2D_DOMAINOBJ, 0, 0, 0,
                                     &errNum)) != NULL){
    intl = obj->domain.i->intvlines->intvs;
    intl->ileft = 0;
    intl->iright = 2;
    obj->domain.i->kol1 = -1;
    obj->domain.i->lastkl = 1;
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return obj;
}

/*
 * 1
 * 1
 * 1
 */
static WlzObject *WlzSpecial_v2(WlzErrorNum *dstErr)  /*-*/
{
  WlzObject	*obj1, *obj2, *obj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((obj1 = WlzSpecial_a3(1, &errNum)) != NULL){
    if((obj2 = WlzSpecial_a3(3, &errNum)) != NULL){
      obj = WlzIntersect2(obj1, obj2, &errNum);
      WlzFreeObj(obj2);
    }
    WlzFreeObj(obj1);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return obj;
}

/*! 
* \return       Structuring element object pointer.
* \ingroup      WlzMorphologyOps
* \brief        Return a pointer to a specific requested "special"
*		structuring element. These are inherited from the
*		original woolz library with rather cryptic names. 
*		See detail for more explanation of the elements and 
*		the additional parameters.
* \par	    Special element types:
<table border="1">
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_H4</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
    <td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_EX4</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
    </table></td>
    <td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_A8</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
    </table></td>
    <td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_H6
    elmIndex = 0,1,2</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>1</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_H6
    elmIndex = 3,4,5</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>1</td></tr>
    </table></td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_H5
    elmIndex = 0,1,2,3</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>1</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_H7
    elmIndex = 0,1,2,3</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>1</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
    </table></td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_A3
    elmIndex = 0,1,2,3</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>0</td><td>0</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>0</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>1</td></tr>
      <tr><td>0</td><td>0</td><td>0</td></tr>
    </table></td>
    <td><table border="0">
      <tr><td>0</td><td>1</td><td>0</td></tr>
      <tr><td>1</td><td>1</td><td>0</td></tr>
      <tr><td>0</td><td>1</td><td>0</td></tr>
    </table></td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_E1</tt></td>
    <td>2D SE,
    origin - left:/td>
    <td><table border="0">
      <tr><td>1</td><td>1</td></tr>
    </table></td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_E2</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>1</td><td>1</td><td>1</td></tr>
    </table></td>
  </tr>
  <tr>
    <td><tt>WLZ_SPEC_STRUCT_ELM_V2</tt></td>
    <td>2D SE,
    origin - centre:</td>
    <td><table border="0">
      <tr><td>1</td></tr>
      <tr><td>1</td></tr>
      <tr><td>1</td></tr>
    </table></td>
  </tr>
</table>
* \param    eType		Enumerated type of the requested special
*				element.
* \param    elmIndex		Additional parameter for a particular element
*				if required (see details).
* \param    dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeSpecialStructElement(
  WlzSpecialStructElmType	eType,
  int				elmIndex,
  WlzErrorNum			*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* switch on type calling each of Liang's specials as required */
  switch( eType ){
  case WLZ_SPEC_STRUCT_ELM_H4:
    rtnObj = WlzSpecial_h4(&errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_EX4:
    rtnObj = WlzSpecial_ex4(&errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_A8:
    rtnObj = WlzSpecial_a8(&errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_H6:
    rtnObj = WlzSpecial_h6(elmIndex, &errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_H5:
    rtnObj = WlzSpecial_h5(elmIndex, &errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_H7:
    rtnObj = WlzSpecial_h7(elmIndex, &errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_A3:
    rtnObj = WlzSpecial_a3(elmIndex, &errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_E1:
    rtnObj = WlzSpecial_e1(&errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_E2:
    rtnObj = WlzSpecial_e2(&errNum);
    break;

  case WLZ_SPEC_STRUCT_ELM_V2:
    rtnObj = WlzSpecial_v2(&errNum);
    break;

  default:
    errNum = WLZ_ERR_PARAM_TYPE;
    break;
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

/*! 
* \return       Single pixel object with coordinates (k,l,p).
* \ingroup      WlzMorphologyOps
* \brief        Make a single pixel/voxel object at the specified
*		coordinate position.
* \param    oType		Object type -
*                               <tt>WLZ_2D_DOMAINOBJ</tt> for a
*                               WLZ_INTERVALDOMAIN_INTVL or 
*				<tt>WLZ_3D_DOMAINOBJ</tt> for a
*				WLZ_PLANEDOMAIN_DOMAIN.
* \param    k			Column (x) coordinate.
* \param    l			Line (y) coordinate.
* \param    p			Plane (z) coordinate.
* \param    dstErr		Destination error pointer, may be NULL.
*/
static WlzDomain WlzMakeSinglePixelDomain(
  WlzObjectType	oType,
  int		k,
  int		l,
  int		p,
  WlzErrorNum	*dstErr)
{
  WlzInterval	*itv;
  WlzDomain	domain;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  domain.core = NULL;
  switch( oType ){
  case WLZ_2D_DOMAINOBJ:
    if((itv = (WlzInterval *)AlcMalloc(sizeof(WlzInterval))) == NULL) {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    else {
      itv->ileft = 0;
      itv->iright = 0;
      if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL,
                                           l, l, k, k, &errNum)) != NULL){
	domain.i->freeptr = AlcFreeStackPush(domain.i->freeptr, itv,
					     NULL);
	domain.i->intvlines->nintvs = 1;
	domain.i->intvlines->intvs = itv;
      }
    }
    break;
  case WLZ_3D_DOMAINOBJ:
    if((domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN,
                                      p, p, l, l, k, k, &errNum)) != NULL){
      domain.p->domains[0] = WlzAssignDomain(
			     WlzMakeSinglePixelDomain(WLZ_INTERVALDOMAIN_INTVL,
						      k, l, p, &errNum), NULL);
      if(errNum != WLZ_ERR_NONE) {
        (void )WlzFreePlaneDomain(domain.p);
	domain.core = NULL;
      }
    }
    break;
  default:
    errNum = WLZ_ERR_DOMAIN_TYPE;
    break;
  }
  if( dstErr ){
    *dstErr = errNum;
  }
  return domain;
}

/*! 
* \return       Single pixel object with coordinates (k,l,p).
* \ingroup      WlzMorphologyOps
* \brief        Make a single pixel/voxel object at the specified
*		coordinate position.
* \param    oType		Object type - <tt>WLZ_2D_DOMAINOBJ</tt>
*				or <tt>WLZ_3D_DOMAINOBJ</tt>.
* \param    k			Column (x) coordinate.
* \param    l			Line (y) coordinate.
* \param    p			Plane (z) coordinate.
* \param    dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeSinglePixelObject(
  WlzObjectType	oType,
  int		k,
  int		l,
  int		p,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  values.core = NULL;
  domain = WlzMakeSinglePixelDomain(oType, k, l, p, &errNum);
  if(errNum == WLZ_ERR_NONE) {
    rtnObj = WlzMakeMain(oType, domain, values, NULL, NULL, &errNum);
    if(errNum != WLZ_ERR_NONE) {
      (void )WlzFreeDomain(domain);
    }
  }
  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

/*! 
* \return       Object with circular domain.
* \ingroup      WlzMorphologyOps
* \brief        Generate a discretised circle domain centered at (x,y).
* \param    radius		Circle radius.
* \param    x			Column/x coordinate of the circle centre.
* \param    y			Row/y coordinate of the circle centre.
* \param    dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeCircleObject(
  double	radius,
  double	x,
  double	y,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  WlzInterval	*intvlPtr;
  int		line1, lastln, kol1, lastkl, width, idelta;
  int		l;
  double	delta;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* only error checked for is a negative radius */
  if( radius < 0.0 ){
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else if( radius < 0.5 ){
    rtnObj = WlzMakeSinglePixelObject(WLZ_2D_DOMAINOBJ,
				      WLZ_NINT(x), WLZ_NINT(y), 0,
				      &errNum);
  }
  else {
    line1 = WLZ_NINT(y-radius);
    lastln = WLZ_NINT(y+radius);
    kol1 = WLZ_NINT(x-radius);
    lastkl = WLZ_NINT(x+radius);
    width = lastkl - kol1 + 1;
    if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL,
					 line1, lastln, kol1, lastkl,
					 &errNum)) != NULL){
      if((intvlPtr = (WlzInterval *) AlcCalloc((lastln - line1 + 1),
					       sizeof(WlzInterval))) != NULL){
	domain.i->freeptr = AlcFreeStackPush(domain.i->freeptr,
					     (void *)intvlPtr, NULL);
	for(l=line1; l <= lastln; l++, intvlPtr++){
	  delta = radius*radius - (l-y)*(l-y);
	  if( delta < 0.5 ){
	    idelta = 0;
	  }
	  else {
	    idelta = (int) sqrt(delta);
	  }
	  intvlPtr->ileft = width/2 - idelta;
	  intvlPtr->iright = width/2 + idelta;
	  WlzMakeInterval(l, domain.i, 1, intvlPtr);
	}
	WlzStandardIntervalDomain(domain.i);
	values.core = NULL;
	if( (rtnObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values,
				  NULL, NULL, &errNum)) == NULL ){
	  WlzFreeIntervalDomain(domain.i);
	}
      }
      else {
	errNum = WLZ_ERR_MEM_ALLOC;
	WlzFreeIntervalDomain(domain.i);
      }
    }
  }
  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

/*! 
* \return       Rectangular object
* \ingroup      WlzMorphologyOps
* \brief        Generate a rectangular object centered at (x,y) with
*		half-width radiusX and half-height radius Y.
* \param    	radiusX		Column half-width of rectangle
* \param    	radiusY		Line (y) half-width of rectangle
* \param    	x		Column (x) rectangle centre.
* \param    	y		Line (y) rectangle centre.
* \param    	dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeRectangleObject(
  double	radiusX,
  double        radiusY,
  double	x,
  double	y,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzDomain	domain;
  WlzValues	values;
  int		line1, lastln, kol1, lastkl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* only error checked for is a negative radius */
  if( (radiusX < 0.0) || (radiusY < 0.0) ){
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else if( (radiusX < 0.5) || (radiusY < 0.5) ){
    rtnObj = WlzMakeSinglePixelObject(WLZ_2D_DOMAINOBJ,
				      WLZ_NINT(x), WLZ_NINT(y), 0,
				      &errNum);
  }
  else {
    line1 = WLZ_NINT(y-radiusY);
    lastln = WLZ_NINT(y+radiusY);
    kol1 = WLZ_NINT(x-radiusX);
    lastkl = WLZ_NINT(x+radiusX);
    if((domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_RECT,
					 line1, lastln,
					 kol1, lastkl, &errNum)) != NULL){
      values.core = NULL;
      rtnObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values,
			   NULL, NULL, &errNum);
    }
  }
  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

/*! 
* \return       Sphere object
* \ingroup      WlzMorphologyOps
* \brief        Make a spherical domain object.
* \param    	oType		Object type - make a circle if 2D.
* \param    	radius		Sphere radius
* \param    	x		Column (x) centre.
* \param    	y		Line (y) centre.
* \param    	z		Plane (x) centre.
* \param    	dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeSphereObject(
  WlzObjectType	oType,
  double	radius,
  double	x,
  double	y,
  double	z,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzObject	*obj1;
  WlzDomain	domain, *domains;
  WlzValues	values;
  double	tmpRadius;
  int		p, plane1, lastpl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check the radius - the only error is if it is negative */
  if( radius < 0.0 ){
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else {
    /* check type - 2D domain implies circular object */
    switch( oType ){
    case WLZ_2D_DOMAINOBJ:
      return WlzMakeCircleObject(radius, x, y, &errNum);

    case WLZ_3D_DOMAINOBJ:
      plane1 = WLZ_NINT(z-radius);
      lastpl = WLZ_NINT(z+radius);
      if((domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN,
					plane1, lastpl,
					WLZ_NINT(y-radius),
					WLZ_NINT(y+radius),
					WLZ_NINT(x-radius),
					WLZ_NINT(x+radius),
					&errNum)) != NULL){
	values.core = NULL;
	if( (rtnObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
				  NULL, NULL, &errNum)) == NULL ){
	  WlzFreePlaneDomain(domain.p);
	  break;
	}
	domains = domain.p->domains;
	for(p=plane1; p <= lastpl; p++, domains++){
	  tmpRadius = radius*radius - (p-z)*(p-z);
	  if( tmpRadius < 1.0 ){
	    tmpRadius = 1.0;
	  }
	  else {
	    tmpRadius = sqrt(tmpRadius);
	  }
	  if((obj1 = WlzMakeCircleObject(tmpRadius, x, y, &errNum)) != NULL){
	    *domains = WlzAssignDomain(obj1->domain, NULL);
	    WlzFreeObj(obj1);
	  }
	  else {
	    WlzFreeObj(rtnObj);
	    rtnObj = NULL;
	    break;
	  }
	}
      }
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  if( rtnObj ){
    WlzStandardPlaneDomain(rtnObj->domain.p, NULL);
  }
  return rtnObj;
}

/*! 
* \return       Cuboid domain object
* \ingroup      WlzMorphologyOps
* \brief        Generate a cuboid object centered at (x,y,z).
* \param    	oType		Object type use WlzMakerectangleObject()
*				if 2D.
* \param    	radiusX		Cuboid half-width (x)
* \param    	radiusY		Cuboid half-height (y).
* \param    	radiusZ		Cuboid half-depth (z).
* \param    	x		Column (x) centre.
* \param    	y		Line (y) centre.
* \param    	z		Plane (z) centre.
* \param    	dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeCuboidObject(
  WlzObjectType	oType,
  double	radiusX,
  double	radiusY,
  double	radiusZ,
  double	x,
  double	y,
  double	z,
  WlzErrorNum	*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzObject	*obj1;
  WlzDomain	domain, *domains;
  WlzValues	values;
  int		p, plane1, lastpl;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check the radius - the only error is if it is negative */
  if( (radiusX < 0.0) || (radiusY < 0.0 ) || (radiusZ < 0.0 ) ){
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else {
    /* check type - 2D domain implies circular object */
    switch( oType ){
    case WLZ_2D_DOMAINOBJ:
      return WlzMakeRectangleObject(radiusX, radiusY, x, y, &errNum);

    case WLZ_3D_DOMAINOBJ:
      plane1 = WLZ_NINT(z-radiusZ);
      lastpl = WLZ_NINT(z+radiusZ);
      if((domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN,
					plane1, lastpl,
					WLZ_NINT(y-radiusY),
					WLZ_NINT(y+radiusY),
					WLZ_NINT(x-radiusX),
					WLZ_NINT(x+radiusX),
					&errNum)) != NULL){
	values.core = NULL;
	if( (rtnObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
				  NULL, NULL, &errNum)) == NULL ){
	  WlzFreePlaneDomain(domain.p);
	  break;
	}
	domains = domain.p->domains;
	for(p=plane1; p <= lastpl; p++, domains++){
	  if((obj1 = WlzMakeRectangleObject(radiusX,radiusY, x, y,
	                                    &errNum)) != NULL){
	    *domains = WlzAssignDomain(obj1->domain, NULL);
	    WlzFreeObj(obj1);
	  }
	  else {
	    WlzFreeObj(rtnObj);
	    rtnObj = NULL;
	    break;
	  }
	}
      }
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  if( rtnObj ){
    WlzStandardPlaneDomain(rtnObj->domain.p, NULL);
  }
  return rtnObj;
}

/*!
* \return       Object with quadrilateral domain.
* \ingroup      WlzMorphologyOps
* \brief        Generate 2D interval domain object corresponding to the
*               arbitrarily oriented quadrilateral with the given ordered
*               vertex coordinates. The ordering is such that the vertices
*               allow the rectangle to be drawn by line segments conecting
*               v[i] to v[(i + 1)%4], with i \f$\in\f$ [0-3].
* \param    x0                  Column coordinate of the first vertex.
* \param    y0                  Row coordinate of the first vertex.
* \param    x1                  Column coordinate of the second vertex.
* \param    y1                  Row coordinate of the second vertex.
* \param    x2                  Column coordinate of the third vertex.
* \param    y2                  Row coordinate of the third vertex.
* \param    x3                  Column coordinate of the forth vertex.
* \param    y3                  Row coordinate of the forth vertex.
* \param    dstErr              Destination error pointer, may be NULL.
*/
WlzObject 	*WlzMakeQuadrilateral(double x0, double y0,
                                double x1, double y1,
                                double x2, double y2,
                                double x3, double y3,
                                WlzErrorNum *dstErr)
{
  int           ln,
                idL;
  int           tI[4];
  WlzIBox2      box;
  WlzIVertex2   vI[4];
  WlzDomain     dom;
  WlzValues     val;
  WlzInterval   *iPtr = NULL;
  WlzObject     *obj = NULL;
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  val.core = NULL;
  /* Quantize the coordinates and find the line range. */
  vI[0].vtX = (int )floor(x0 + 0.5);
  vI[0].vtY = (int )floor(y0 + 0.5);
  vI[1].vtX = (int )floor(x1 + 0.5);
  vI[1].vtY = (int )floor(y1 + 0.5);
  vI[2].vtX = (int )floor(x2 + 0.5);
  vI[2].vtY = (int )floor(y2 + 0.5);
  vI[3].vtX = (int )floor(x3 + 0.5);
  vI[3].vtY = (int )floor(y3 + 0.5);
  if(vI[0].vtX < vI[1].vtX)
  {
    tI[0] = vI[0].vtX;
    tI[1] = vI[1].vtX;
  }
  else
  {
    tI[0] = vI[1].vtX;
    tI[1] = vI[0].vtX;
  }
  if(vI[2].vtX < vI[3].vtX)
  {
    tI[2] = vI[2].vtX;
    tI[3] = vI[3].vtX;
  }
  else
  {
    tI[2] = vI[3].vtX;
    tI[3] = vI[2].vtX;
  }
  box.xMin = (tI[0] < tI[2])? tI[0]: tI[2];
  box.xMax = (tI[1] > tI[3])? tI[1]: tI[3];
  if(vI[0].vtY < vI[1].vtY)
  {
    tI[0] = vI[0].vtY;
    tI[1] = vI[1].vtY;
  }
  else
  {
    tI[0] = vI[1].vtY;
    tI[1] = vI[0].vtY;
  }
  if(vI[2].vtY < vI[3].vtY)
  {
    tI[2] = vI[2].vtY;
    tI[3] = vI[3].vtY;
  }
  else
  {
    tI[2] = vI[3].vtY;
    tI[3] = vI[2].vtY;
  }
  box.yMin = (tI[0] < tI[2])? tI[0]: tI[2];
  box.yMax = (tI[1] > tI[3])? tI[1]: tI[3];
  if((box.xMax - box.xMin == 0) && (box.yMax - box.yMin == 0))
  {
    obj = WlzMakeSinglePixelObject(WLZ_2D_DOMAINOBJ,
                                   box.xMin, box.yMin, 0,
                                   &errNum);
  }
  else
  {
    dom.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_INTVL,
                                  box.yMin, box.yMax, box.xMin, box.xMax,
                                  &errNum);
    if((iPtr = (WlzInterval *)AlcCalloc((box.yMax - box.yMin + 1),
                                         sizeof(WlzInterval))) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    else
    {
      dom.i->freeptr = AlcFreeStackPush(dom.i->freeptr, (void *)iPtr,
                                        NULL);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      for(ln = box.yMin; ln <= box.yMax; ++ln)
      {
        idL = ln - box.yMin;
        iPtr[idL].ileft = box.xMax - box.xMin;
        iPtr[idL].iright = 0;
        (void )WlzMakeInterval(ln, dom.i, 1, iPtr + idL);
      }
      for(idL = 0; idL < 4; ++idL)
      {
        vI[idL].vtX -= box.xMin;
        vI[idL].vtY -= box.yMin;
      }
      WlzRasterLineSetItv2D(dom.i, vI[0], vI[1]);
      WlzRasterLineSetItv2D(dom.i, vI[1], vI[2]);
      WlzRasterLineSetItv2D(dom.i, vI[2], vI[3]);
      WlzRasterLineSetItv2D(dom.i, vI[3], vI[0]);
      if((obj = WlzMakeMain(WLZ_2D_DOMAINOBJ, dom, val,
                            NULL, NULL, &errNum)) == NULL)
      {
        (void )WlzFreeDomain(dom);
      }
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(obj);
}

/*!
* \return
* \ingroup	WlzMorphologyOps
* \brief	Makes a standard structure element - basicaly a sphere
*		but with the appropriate connectivity or distance metric.
* \param    	oType		Object type - <tt>WLZ_2D_DOMAINOBJ</tt>
*				or <tt>WLZ_3D_DOMAINOBJ</tt>.
* \param	dType		Distance metric.
* \param	radius		Sphere radius.
* \param	dstErr		Destination error pointer, may be NULL.
*/
WlzObject *WlzMakeStdStructElement(
  WlzObjectType		oType,
  WlzDistanceType	dType,
  double		radius,
  WlzErrorNum		*dstErr)
{
  WlzObject	*structElm=NULL;
  WlzObject	*obj1, *obj2;
  int 		intRadius;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check the radius - convert to nearest integer for all except
     Euclidean. Even then r < 0.5 => an empty structuring element.
     -ve radius is an error */
  if( radius < 0.0 ){
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else if( radius < 0.5 ){
    return WlzMakeEmpty(dstErr);
  }
  else {
    intRadius = WLZ_NINT(radius);
  }

  /* now check the distance type */
  if( errNum == WLZ_ERR_NONE ){
    switch( dType ){
    case WLZ_8_DISTANCE:
    case WLZ_4_DISTANCE:
    case WLZ_6_DISTANCE:
    case WLZ_18_DISTANCE:
    case WLZ_26_DISTANCE:
      /* make a domain with a single pixel at the origin and dilate
	 to the required radius */
      if((obj1 = WlzMakeSinglePixelObject(oType, 0, 0, 0, &errNum)) != NULL){
	while( intRadius-- > 0 ){
	  if((obj2 = WlzDilation(obj1, (WlzConnectType) dType,
	  		         &errNum)) != NULL){
	    WlzFreeObj(obj1);
	    obj1 = obj2;
	  }
	  else {
	    WlzFreeObj(obj1);
	    obj1 = NULL;
	    break;
	  }
	}
	structElm = obj1;
      }
      break;
	
    case WLZ_OCTAGONAL_DISTANCE:
      /* make a domain with a single pixel at the origin and dilate
	 alternately 8-conn, 4-conn to required radius */
      if((obj1 = WlzMakeSinglePixelObject(oType, 0, 0, 0, &errNum)) != NULL){
	while( intRadius-- > 0 ){
	  if( intRadius & 0x01 ){
	    obj2 = WlzDilation(obj1, WLZ_4_CONNECTED, &errNum);
	  }
	  else {
	    obj2 = WlzDilation(obj1, WLZ_8_CONNECTED, &errNum);
	  }

	  if( obj2 ){
	    WlzFreeObj(obj1);
	    obj1 = obj2;
	  }
	  else {
	    WlzFreeObj(obj1);
	    obj1 = NULL;
	    break;
	  }
	}
	structElm = obj1;
      }
      break;
	
    case WLZ_EUCLIDEAN_DISTANCE:
      structElm = WlzMakeSphereObject(oType, radius,
				      0.0, 0.0, 0.0, &errNum);
      break;

    default:
      errNum = WLZ_ERR_PARAM_TYPE;
      break;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return structElm;
}
