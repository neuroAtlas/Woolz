#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzBibFileIOUtils.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Fri Jan  4 12:31:36 2002
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2001 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \brief        Woolz bibfile IO utility functions to write and
 parse records written in bibfile format
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <WlzIO.h>

/* function:     write_Wlz3DSectionViewParams_Record    */
/*! 
* \ingroup      WlzIO
* \brief        Write the section view parameters in the bibtex style
record using the bibFile library.
*
* \return       woolz error number
* \param    fp	FILE pointer opened for writing
* \param    wlzViewStr	woolz 3D view structure
* \par      Source:
*                WlzBibFileIOUtils.c
*/
WlzErrorNum write_Wlz3DSectionViewParams_Record(
  FILE			*fp,
  WlzThreeDViewStruct	*wlzViewStr)
{
  char	fixedPointStr[64];
  char	distanceStr[64];
  char	pitchStr[64];
  char	yawStr[64];
  char	rollStr[64];
  char	scaleStr[64];
  char	upVectorStr[64];
  char	viewModeStr[64];
  BibFileRecord	*bibfileRecord;
  BibFileError	bibFileErr;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* generate strings for the bibfile entry */
  sprintf(fixedPointStr, "%.1f, %.1f, %.1f", wlzViewStr->fixed.vtX,
	  wlzViewStr->fixed.vtY, wlzViewStr->fixed.vtZ);
  sprintf(distanceStr, "%.01f", wlzViewStr->dist);
  sprintf(pitchStr, "%.2f", wlzViewStr->phi * 180.0 / WLZ_M_PI);
  sprintf(yawStr, "%.2f", wlzViewStr->theta * 180.0 / WLZ_M_PI);
  sprintf(rollStr, "%.2f", wlzViewStr->zeta * 180.0 / WLZ_M_PI);
  sprintf(scaleStr, "%.2f", wlzViewStr->scale);
  sprintf(upVectorStr, "%g, %g, %g", wlzViewStr->up.vtX,
	  wlzViewStr->up.vtY, wlzViewStr->up.vtZ);

  /* view mode */
  switch( wlzViewStr->view_mode ){
  case WLZ_UP_IS_UP_MODE:
  default:
    sprintf(viewModeStr, "up-is-up");
    break;

  case WLZ_STATUE_MODE:
    sprintf(viewModeStr, "statue");
    break;

  case WLZ_ZETA_MODE:
    sprintf(viewModeStr, "absolute");
    break;
  }

  /* create the bibfile record */
  bibfileRecord = 
    BibFileRecordMake("Wlz3DSectionViewParams", "0",
		      BibFileFieldMakeVa("FixedPoint",	fixedPointStr,
					 "Distance",	distanceStr,
					 "Pitch",	pitchStr,
					 "Yaw",		yawStr,
					 "Roll",	rollStr,
					 "Scale",	scaleStr,
					 "UpVector",	upVectorStr,
					 "ViewMode",	viewModeStr,
					 NULL));

  /* write the bibfile and release resources */
  bibFileErr = BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);

  /* check the bibfile write error */
  if( bibFileErr != BIBFILE_ER_NONE ){
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }

  return errNum;
}

/* function:     parse_Wlz3DSectionViewParams_Record    */
/*! 
* \ingroup      WlzIO
* \brief        Parse the bibfile record for a woolz 3D view structure
setting paramters in the given view structure.
*
* \return       woolz error number
* \param    bibfileRecord	bibfile record as read using libbibFile
* \param    wlzViewStr	woolz 3D view structure already allocated
* \par      Source:
*                WlzBibFileIOUtils.c
*/
WlzErrorNum parse_Wlz3DSectionViewParams_Record(
  BibFileRecord		*bibfileRecord,
  WlzThreeDViewStruct	*wlzViewStr)
{
  WlzDVertex3	fixedPointVtx;
  double	distance, pitch, yaw, roll, scale;
  WlzDVertex3	upVectorVtx;
  WlzThreeDViewMode	viewMode;
  char		viewModeStr[64];
  int		numParsedFields=0;
  BibFileField	*bibfileField;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check inputs */
  if((bibfileRecord == NULL) ||
     (wlzViewStr == NULL) ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* parse the record */
  if( errNum == WLZ_ERR_NONE ){
    numParsedFields = BibFileFieldParseFmt
      (bibfileRecord->field,
       (void *) &(fixedPointVtx.vtX), "%lg ,%*lg ,%*lg", "FixedPoint",
       (void *) &(fixedPointVtx.vtY), "%*lg ,%lg ,%*lg", "FixedPoint",
       (void *) &(fixedPointVtx.vtZ), "%*lg ,%*lg ,%lg", "FixedPoint",
       (void *) &distance, "%lg", "Distance",
       (void *) &pitch, "%lg", "Pitch",
       (void *) &yaw, "%lg", "Yaw",
       (void *) &roll, "%lg", "Roll",
       (void *) &scale, "%lg", "Scale",
       (void *) &(upVectorVtx.vtX), "%lg ,%*lg ,%*lg", "UpVector",
       (void *) &(upVectorVtx.vtY), "%*lg ,%lg ,%*lg", "UpVector",
       (void *) &(upVectorVtx.vtZ), "%*lg ,%*lg ,%lg", "UpVector",
       (void *) &(viewModeStr[0]), "%s", "ViewMode",
       NULL);
    if( numParsedFields < 12 ){
      errNum = WLZ_ERR_READ_INCOMPLETE;
    }
  }

  /* doesn't read the view mode correctly - ask Bill */
  if( errNum == WLZ_ERR_NONE ){
    bibfileField = bibfileRecord->field;
    errNum = WLZ_ERR_READ_INCOMPLETE;
    while( bibfileField ){
      if( strncmp(bibfileField->name, "ViewMode", 8) == 0 ){
	strcpy(viewModeStr, bibfileField->value);
	errNum = WLZ_ERR_NONE;
	break;
      }
      bibfileField = bibfileField->next;
    }
  }

  /* set the view structure */
  if( errNum == WLZ_ERR_NONE ){
    pitch *= (WLZ_M_PI/180.0);
    yaw *= (WLZ_M_PI/180.0);
    roll *= (WLZ_M_PI/180.0);
    if( !strncmp(viewModeStr, "up-is-up", 8) ){
      viewMode = WLZ_UP_IS_UP_MODE;
    }
    else if( !strncmp(viewModeStr, "statue", 8) ){
      viewMode = WLZ_STATUE_MODE;
    }
    else {
      viewMode = WLZ_ZETA_MODE;
    }
    wlzViewStr->fixed = fixedPointVtx;
    wlzViewStr->dist = distance;
    wlzViewStr->theta = yaw;
    wlzViewStr->phi = pitch;
    wlzViewStr->zeta = roll;
    wlzViewStr->up = upVectorVtx;
    wlzViewStr->scale = scale;
    wlzViewStr->view_mode = viewMode;
  }

  return errNum;
}

/* function:     write_WlzWarpTransformParams_Record    */
/*! 
* \ingroup      WlzIO
* \brief        Write the details of a warp transformation parameters.
Note this does not write the warp transformation itself.
*
* \return       woolz error number
* \param    fp	FILE pointer opened for writing
* \param    basisFnType	interpolation or basis function type
* \param    meshMthd	method used to define the mesh
* \param    meshMinDst	mesh minimum distance
* \param    meshMaxDst	mesh maximum distance
* \par      Source:
*                WlzBibFileIOUtils.c
*/
WlzErrorNum write_WlzWarpTransformParams_Record(
  FILE			*fp,
  WlzBasisFnType	basisFnType,
  WlzMeshGenMethod	meshMthd,
  int			meshMinDst,
  int	 		meshMaxDst)
{
  char	basisFnTypeStr[32];
  char	meshMthdStr[32];
  char	meshMinDstStr[16];
  char	meshMaxDstStr[16];
  BibFileRecord	*bibfileRecord;
  BibFileError	bibFileErr;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* generate strings for the bibfile entry */
  sprintf(meshMinDstStr, "%d", meshMinDst);
  sprintf(meshMaxDstStr, "%d", meshMaxDst);

  /*  types */
  switch( basisFnType ){
  case WLZ_BASISFN_TPS:
  default:
    sprintf(basisFnTypeStr, "Thin-plate-spline");
    break;

  case WLZ_BASISFN_GAUSS:
    sprintf(basisFnTypeStr, "Gaussian");
    break;

  case WLZ_BASISFN_POLY:
    sprintf(basisFnTypeStr, "Polynomial");
    break;

  case WLZ_BASISFN_MQ:
    sprintf(basisFnTypeStr, "Multiquadric");
    break;

  case WLZ_BASISFN_CONF_POLY:
    sprintf(basisFnTypeStr, "Conformal-polynomial");
    break;
  }

  switch( meshMthd ){
  case WLZ_MESH_GENMETHOD_BLOCK:
  default:
    sprintf(meshMthdStr, "Block");
    break;

  case WLZ_MESH_GENMETHOD_GRADIENT:
    sprintf(meshMthdStr, "Gradient");
    break;
  }

  /* create the bibfile record */
  bibfileRecord = 
    BibFileRecordMake("WlzWarpTransformParams", "0",
		      BibFileFieldMakeVa("BasisFnType",		basisFnTypeStr,
					 "MeshGenMethod",	meshMthdStr,
					 "MeshMinDist",		meshMinDstStr,
					 "MeshMaxDist",		meshMaxDstStr,
					 NULL));

  /* write the bibfile and release resources */
  bibFileErr = BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);

  /* check the bibfile write error */
  if( bibFileErr != BIBFILE_ER_NONE ){
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }

  return errNum;
}

/* function:     parse_WlzWarpTransformParams_Record    */
/*! 
* \ingroup      WlzIO
* \brief        Parse the bibfile record for the warp transformation
parameter settings.
*
* \return       Woolz error number
* \param    bibfileRecord	bibfile record as read by libbibFile
* \param    basisFnType	return for basis function type
* \param    meshMthd	return for mesh method type
* \param    meshMinDst	return for mesh minimum distance
* \param    meshMaxDst	return for mesh maximum distance
* \par      Source:
*                WlzBibFileIOUtils.c
*/
WlzErrorNum parse_WlzWarpTransformParams_Record(
  BibFileRecord		*bibfileRecord,
  WlzBasisFnType	*basisFnType,
  WlzMeshGenMethod	*meshMthd,
  int			*meshMinDst,
  int	 		*meshMaxDst)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  char		basisFnTypeStr[32];
  char		meshMthdStr[32];
  int		numParsedFields=0;

  /* check inputs */
  if((bibfileRecord == NULL) ||
     (basisFnType == NULL) ||
     (meshMthd == NULL) ||
     (meshMinDst == NULL) ||
     (meshMaxDst == NULL) ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* parse the record */
  if( errNum == WLZ_ERR_NONE ){
    numParsedFields = BibFileFieldParseFmt
      (bibfileRecord->field,
       (void *) &(basisFnTypeStr[0]), "%s", "BasisFnType",
       (void *) &(meshMthdStr[0]), "%s", "MeshGenMethod",
       (void *) meshMinDst, "%d", "MeshMinDist",
       (void *) meshMaxDst, "%d", "MeshMaxDist",
       NULL);
    if( numParsedFields < 4 ){
      errNum = WLZ_ERR_READ_INCOMPLETE;
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    if( strncmp(basisFnTypeStr, "Thin-plate-spline", 17) == 0 ){
      *basisFnType = WLZ_BASISFN_TPS;
    }
    else if( strncmp(basisFnTypeStr, "Gaussian", 8) == 0 ){
      *basisFnType = WLZ_BASISFN_GAUSS;
    }
    else if( strncmp(basisFnTypeStr, "Polynomial", 10) == 0 ){
      *basisFnType = WLZ_BASISFN_POLY;
    }
    else if( strncmp(basisFnTypeStr, "Multiquadric", 12) == 0 ){
      *basisFnType = WLZ_BASISFN_MQ;
    }
    else if( strncmp(basisFnTypeStr, "Conformal-polynomial", 20) == 0 ){
      *basisFnType = WLZ_BASISFN_CONF_POLY;
    }
    else {
      *basisFnType = WLZ_BASISFN_TPS;
    }
  }

  if( errNum == WLZ_ERR_NONE ){
    if( strncmp(meshMthdStr, "Block", 5) == 0 ){
      *meshMthd = WLZ_MESH_GENMETHOD_BLOCK;
    }
    else if( strncmp(meshMthdStr, "Gradient", 8) == 0 ){
      *meshMthd = WLZ_MESH_GENMETHOD_GRADIENT;
    }
    else {
      *meshMthd = WLZ_MESH_GENMETHOD_BLOCK;
    }
  }

  return errNum;
}

/* function:     write_WlzTiePointVtxs_Record    */
/*! 
* \ingroup      WlzIO
* \brief        Write a tie-point record in bibtex style.
*
* \return       Woolz error number
* \param    fp	FILE pointer opened for writing
* \param    index	index of the given tie points
* \param    dstVtx	destination vertex
* \param    srcVtx	source vertex
* \par      Source:
*                WlzBibFileIOUtils.c
*/
WlzErrorNum write_WlzTiePointVtxs_Record(
  FILE		*fp,
  int		index,
  WlzDVertex3	dstVtx,
  WlzDVertex3	srcVtx)
{
  char	indexStr[16];
  char	dstVtxStr[64];
  char	srcVtxStr[64];
  BibFileRecord	*bibfileRecord;
  BibFileError	bibFileErr;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* generate strings for the bibfile entry */
  sprintf(indexStr, "%d", index);
  sprintf(dstVtxStr, "%f, %f, %f", dstVtx.vtX,
	  dstVtx.vtY, dstVtx.vtZ);
  sprintf(srcVtxStr, "%f, %f, %f", srcVtx.vtX,
	  srcVtx.vtY, srcVtx.vtZ);

  /* create the bibfile record */
  bibfileRecord = 
    BibFileRecordMake("WlzTiePointVtxs", "0",
		      BibFileFieldMakeVa("Index",	indexStr,
					 "DstVtx",	dstVtxStr,
					 "SrcVtx",	srcVtxStr,
					 NULL));

  /* write the bibfile and release resources */
  bibFileErr = BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);

  /* check the bibfile write error */
  if( bibFileErr != BIBFILE_ER_NONE ){
    errNum = WLZ_ERR_WRITE_INCOMPLETE;
  }

  return errNum;
}

/* function:     parse_WlzTiePointVtxs_Record    */
/*! 
* \ingroup      WlzIO
* \brief        Parse the bibfile record for a tie-point pair of vertices.
The record should also includ an index so that tie-point ordering is
preserved.
*
* \return       Woolz error number
* \param    bibfileRecord	bibfile record as read in by libbibFile
* \param    index	return for tie-point pair index within list
* \param    dstVtx	return for destination vertex
* \param    srcVtx	return for source vertex
* \par      Source:
*                WlzBibFileIOUtils.c
*/
WlzErrorNum parse_WlzTiePointVtxs_Record(
  BibFileRecord		*bibfileRecord,
  int		*index,
  WlzDVertex3	*dstVtx,
  WlzDVertex3	*srcVtx)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		numParsedFields=0;

  /* check inputs */
  if((bibfileRecord == NULL) ||
     (index == NULL) ||
     (dstVtx == NULL) ||
     (srcVtx == NULL) ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* parse the record */
  if( errNum == WLZ_ERR_NONE ){
    numParsedFields = BibFileFieldParseFmt
      (bibfileRecord->field,
       (void *) index, "%d", "Index",
       (void *) &(dstVtx->vtX), "%lg ,%*lg ,%*lg", "DstVtx",
       (void *) &(dstVtx->vtY), "%*lg ,%lg ,%*lg", "DstVtx",
       (void *) &(dstVtx->vtZ), "%*lg ,%*lg ,%lg", "DstVtx",
       (void *) &(srcVtx->vtX), "%lg ,%*lg ,%*lg", "SrcVtx",
       (void *) &(srcVtx->vtY), "%*lg ,%lg ,%*lg", "SrcVtx",
       (void *) &(srcVtx->vtZ), "%*lg ,%*lg ,%lg", "SrcVtx",
       NULL);
    if( numParsedFields < 7 ){
      errNum = WLZ_ERR_READ_INCOMPLETE;
    }
  }

  return errNum;
}
