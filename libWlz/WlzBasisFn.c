#pragma ident "MRC HGU $Id$"
/*!
* \file         WlzBasisFn.c
* \author       Bill Hill
* \date         January 2003
* \version      $Id$
* \note
*               Copyright
*               2003 Medical Research Council, UK.
*               All rights reserved.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \brief	Woolz functions for which are themselves the sum of
*		radialy symetric functions.
* \ingroup	WlzFunction
* \todo         -
* \bug          None known.
*/
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <Wlz.h>

static void			WlzBasisFnVxExtent2D(
				  WlzDBox2 *extentDB,
				  WlzDVertex2 *vx0,
				  WlzDVertex2 *vx1,
				  int nPts);
static void			WlzBasisFnGauss2DCoef(
				  WlzBasisFn *basisFn,
				  double *vec,
				  int forX);
static void			WlzBasisFnMQ2DCoexff(
				  WlzBasisFn *basisFn,
				  double *vec,
				  WlzDBox2 *extentDB,
				  double range,
				  int forX);
static void			WlzBasisFnTPS2DCoef(
				  WlzBasisFn *basisFn,
				  double *vec,
				  WlzDBox2 *extentDB,
				  double range,
				  int forX);
static double   		WlzBasisFnValueMOSPhiPC(
				  double r,
				  double v,
				  double w,
				  double delta,
				  double rv,
				  double rw,
				  double norm);
static WlzDVertex2 		WlzBasisFnValueRedPoly2D(
				  WlzDVertex2 *poly,
				  WlzDVertex2 srcVx);
static WlzDVertex3 		WlzBasisFnValueRedPoly3D(
				  WlzDVertex3 *poly,
				  WlzDVertex3 srcVx);

/*!
* \return	Woolz error number.
* \ingroup	WlzFunction
* \brief	Free's the given basis function.
* \param	basisFn			Given basis function, may be NULL.
*/
WlzErrorNum	WlzBasisFnFree(WlzBasisFn *basisFn)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(basisFn)
  {
    if(basisFn->poly.v)
    {
      AlcFree(basisFn->poly.v);
    }
    if(basisFn->basis.v)
    {
      AlcFree(basisFn->basis.v);
    }
    if(basisFn->vertices.v)
    {
      AlcFree(basisFn->vertices.v);
    }
    if(basisFn->param)
    {
      AlcFree(basisFn->param);
    }
    AlcFree(basisFn);
  }
  return(errNum);
}

/*!
* \return       New vertex value.
* \ingroup      WlzFunction
* \brief        Calculates the value for the given vertex using
*               a 2D polynomial basis function. This is not a basis
*		function just a polynomial, but it makes sense for
*		it to be incuded in the basis functions since they
*		have a polynomial component.
* \param        basisFn                   Basis function.
* \param        srcVx                   Source vertex.
*/
WlzDVertex2 	WlzBasisFnValuePoly2D(WlzBasisFn *basisFn, WlzDVertex2 srcVx)
{
  int           idX,
                idY;
  double        tD0;
  WlzDVertex2   *polyP;
  WlzDVertex2   powVx,
                newVx;


  powVx.vtY = 1.0;
  polyP = basisFn->poly.d2;
  newVx.vtY = newVx.vtX = 0.0;
  for(idY = 0; idY <= basisFn->nPoly; ++idY)
  {
    powVx.vtX = 1.0;
    for(idX = 0; idX <= basisFn->nPoly; ++idX)
    {
      tD0 = powVx.vtX * powVx.vtY;
      newVx.vtX += polyP->vtX * tD0;
      newVx.vtY += polyP->vtY * tD0;
      powVx.vtX *= srcVx.vtX;
      ++polyP;
    }
    powVx.vtY *= srcVx.vtY;
  }
  return(newVx);
}

/*!
* \return	New vertex value.
* \ingroup	WlzFunction
* \brief	Calculates the value for the given vertex using
*		a 2D Gaussian basis function.
* \param	basisFn			Basis function.
* \param	srcVx			Source vertex.
*/
WlzDVertex2 	WlzBasisFnValueGauss2D(WlzBasisFn *basisFn, WlzDVertex2 srcVx)
{
  int           count;
  double        tD0,
		tD1,
		delta;
  WlzDVertex2    *basisCo,
		*cPts;
  WlzDVertex2    polyVx,
  		newVx;

  newVx.vtX = 0.0;
  newVx.vtY = 0.0;
  count = basisFn->nVtx;
  cPts = basisFn->vertices.d2;
  basisCo = basisFn->basis.d2;
  delta = *((double *)(basisFn->param));
  while(count-- > 0)
  {
    tD0 = srcVx.vtX - cPts->vtX;
    tD1 = srcVx.vtY - cPts->vtY;
    tD0 = (tD0 * tD0) + (tD1 * tD1);
    tD1 = (tD0 > DBL_EPSILON)? exp(tD0 * delta): 1.0;
    newVx.vtX += basisCo->vtX * tD1;
    newVx.vtY += basisCo->vtY * tD1;
    ++cPts;
    ++basisCo;
  }
  polyVx = WlzBasisFnValueRedPoly2D(basisFn->poly.d2, srcVx);
  newVx.vtX = newVx.vtX + polyVx.vtX;
  newVx.vtY = newVx.vtY + polyVx.vtY;
  return(newVx);
}

/*!
* \return	New vertex value.
* \ingroup	WlzFunction
* \brief	Calculates the value for the given vertex using
*		a 2D multiquadric basis function.
* \param	basisFn			Basis function.
* \param	srcVx			Source vertex.
*/
WlzDVertex2 	WlzBasisFnValueMQ2D(WlzBasisFn *basisFn, WlzDVertex2 srcVx)
{
  int           count;
  double        tD0,
		tD1,
		delta;
  WlzDVertex2    *basisCo,
		*cPts;
  WlzDVertex2    polyVx,
  		newVx;

  newVx.vtX = 0.0;
  newVx.vtY = 0.0;
  count = basisFn->nVtx;
  cPts = basisFn->vertices.d2;
  basisCo = basisFn->basis.d2;
  delta = *((double *)(basisFn->param));
  while(count-- > 0)
  {
    tD0 = srcVx.vtX - cPts->vtX;
    tD1 = srcVx.vtY - cPts->vtY;
    tD0 = (tD0 * tD0) + (tD1 * tD1);
    if(tD0 > DBL_EPSILON)
    {
      tD0 = sqrt(tD0 + delta);
      newVx.vtX += basisCo->vtX * tD0;
      newVx.vtY += basisCo->vtY * tD0;
    }
    ++cPts;
    ++basisCo;
  }
  polyVx = WlzBasisFnValueRedPoly2D(basisFn->poly.d2, srcVx);
  newVx.vtX = newVx.vtX + polyVx.vtX;
  newVx.vtY = newVx.vtY + polyVx.vtY;
  return(newVx);
}

/*!
* \return	New vertex value.
* \ingroup	WlzFunction
* \brief	Calculates the value for the given vertex using
*		a 2D thin plate spline basis function.
* \param	basisFn			Basis function.
* \param	srcVx			Source vertex.
*/
WlzDVertex2 	WlzBasisFnValueTPS2D(WlzBasisFn *basisFn, WlzDVertex2 srcVx)
{
  int           count;
  double        tD0,
		tD1;
  WlzDVertex2    *basisCo,
		*cPts;
  WlzDVertex2    polyVx,
  		newVx;

  newVx.vtX = 0.0;
  newVx.vtY = 0.0;
  count = basisFn->nVtx;
  cPts = basisFn->vertices.d2;
  basisCo = basisFn->basis.d2;
  while(count-- > 0)
  {
    tD0 = srcVx.vtX - cPts->vtX;
    tD1 = srcVx.vtY - cPts->vtY;
    tD0 = (tD0 * tD0) + (tD1 * tD1);
    if(tD0 > DBL_EPSILON)
    {
      tD0 *= log(tD0);
      newVx.vtX += basisCo->vtX * tD0;
      newVx.vtY += basisCo->vtY * tD0;
    }
    ++cPts;
    ++basisCo;
  }
  polyVx = WlzBasisFnValueRedPoly2D(basisFn->poly.d2, srcVx);
  newVx.vtX = (newVx.vtX / 2) + polyVx.vtX;
  newVx.vtY = (newVx.vtY / 2) + polyVx.vtY;
  return(newVx);
}

/*!
* \return	New vertex value.
* \ingroup	WlzFunction
* \brief	Calculates the value for the given vertex using
*		a 2D conformal polynomial basis function.
* \param	basisFn			Basis function.
* \param	srcVx			Source vertex.
*/
WlzDVertex2 	WlzBasisFnValueConf2D(WlzBasisFn *basisFn, WlzDVertex2 srcVx)
{
  int		i;
  ComplexD	z, w, powW, a, b;
  WlzDVertex2	newVx;
  WlzDVertex2	*polyP;

  polyP = basisFn->poly.d2;
  w.re = srcVx.vtX;
  w.im = srcVx.vtY;
  z.re = polyP[0].vtX;
  z.im = polyP[0].vtY;
  powW.re = 1.0;
  powW.im = 0.0;

  for(i=1; i <= basisFn->nPoly; i++){
    powW = AlgCMult(powW, w);
    a.re = polyP[i].vtX;
    a.im = polyP[i].vtY;
    b = AlgCMult(a, powW);
    z.re += b.re;
    z.im += b.im;
  }
  newVx.vtX = z.re;
  newVx.vtY = z.im;
  return(newVx);
}

/*!
* \return	New vertex value.
* \ingroup	WlzFunction
* \brief	Calculates the value for the given vertex using
*		a 3D multiorder basis function:
*		\f[
		\phi(r) = \frac{1}{4 \pi \delta^2 r}
			  (1 -
			   \frac{w}{w - v} e^{- \sqrt{v} r} +
			   \frac{v}{w - v} e^{- \sqrt{w} r})
		\f]
		\f[
		v = \frac{1 + \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
		\f[
		w = \frac{1 - \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
* \param	basisFn			Basis function.
* \param	srcVx			Source vertex.
*/
WlzDVertex3 	WlzBasisFnValueMOS3D(WlzBasisFn *basisFn, WlzDVertex3 srcVx)
{
  int           count;
  double        tD0,
		tD1,
		v,
		w,
		rad,
		phi,
		delta,
		tau,
		rv,
		rw,
		norm;
  WlzDVertex3   *basisCo,
		*cPts;
  WlzDVertex3   dispVx,
  		polyVx,
  		newVx;

  newVx.vtX = 0.0;
  newVx.vtY = 0.0;
  newVx.vtZ = 0.0;
  count = basisFn->nVtx;
  cPts = basisFn->vertices.d3;
  basisCo = basisFn->basis.d3;
  delta = *((double *)(basisFn->param) + 0);
  tau = *((double *)(basisFn->param) + 1);
  tD0 = 2.0 * tau * delta;
  tD1 = sqrt(1 - (tD0 * tD0));
  tD0 = 2.0 * tau;
  v = (1 + tD1) / tD0;
  w = (1 - tD1) / tD0;
  rv = sqrt(v);
  rw = sqrt(w);
  norm = 1.0 / (4.0 * ALG_M_PI * delta * delta);
  while(count-- > 0)
  {
    WLZ_VTX_3_SUB(dispVx, srcVx, *cPts);
    rad = WLZ_VTX_3_LENGTH(dispVx);
    phi = WlzBasisFnValueMOSPhiPC(rad, w, v, delta, rv, rw, norm);
    newVx.vtX += basisCo->vtX * phi;
    newVx.vtY += basisCo->vtY * phi;
    newVx.vtZ += basisCo->vtZ * phi;
    ++cPts;
    ++basisCo;
  }
  polyVx = WlzBasisFnValueRedPoly3D(basisFn->poly.d3, srcVx);
  WLZ_VTX_3_ADD(newVx, newVx, polyVx);
  return(newVx);
}

/*!
* \return	New scalar value.
* \ingroup	WlzFunction
* \brief	Calculates the value for the given vertex using
*		a scalar 3D multiorder basis function:
*		\f[
		\phi(r) = \frac{1}{4 \pi \delta^2 r}
			  (1 -
			   \frac{w}{w - v} e^{- \sqrt{v} r} +
			   \frac{v}{w - v} e^{- \sqrt{w} r})
		\f]
		\f[
		v = \frac{1 + \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
		\f[
		w = \frac{1 - \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
* \param	basisFn			Basis function.
* \param	srcVx			Source vertex.
*/
double 		WlzBasisFnValueScalarMOS3D(WlzBasisFn *basisFn,
					   WlzDVertex3 srcVx)
{
  int           count;
  double        tD0,
		tD1,
		v,
		w,
		phi,
		delta,
		tau,
		rv,
		rw,
		rad,
		norm,
		value;
  double	*basisCo;
  WlzDVertex3	*cPts;
  WlzDVertex3   dispVx;

  value = 0.0;
  count = basisFn->nVtx;
  cPts = basisFn->vertices.d3;
  basisCo = (double *)(basisFn->basis.v);
  delta = *((double *)(basisFn->param) + 0);
  tau = *((double *)(basisFn->param) + 1);
  tD0 = 2.0 * tau * delta;
  tD1 = sqrt(1 - (tD0 * tD0));
  tD0 = 2.0 * tau * tau;
  v = (1 + tD1) / tD0;
  w = (1 - tD1) / tD0;
  rv = sqrt(v);
  rw = sqrt(w);
  norm = 1.0 / (4.0 * ALG_M_PI * delta * delta);
  while(count-- > 0)
  {
    WLZ_VTX_3_SUB(dispVx, srcVx, *cPts);
    rad = WLZ_VTX_3_LENGTH(dispVx);
    phi = WlzBasisFnValueMOSPhiPC(rad, w, v, delta, rv, rw, norm);
    value += *basisCo * phi;
    ++basisCo;
    ++cPts;
  }
  value += *(double *)(basisFn->poly.v);
  return(value);
}

/*!
* \return	The value of a single multiorder radial basis function.
* \brief	Computes the value of a single multiorder radial basis
*		function:
*		\f[
		\phi(r) = \frac{1}{4 \pi \delta^2 r}
			  (1 -
			   \frac{w}{w - v} e^{- \sqrt{v} r} +
			   \frac{v}{w - v} e^{- \sqrt{w} r})
		\f]
		\f[
		v = \frac{1 + \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
		\f[
		w = \frac{1 - \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
* \param	r			Radial distance, \f$(r > 0)\f$.
* \param	delta			The 1st order smoothness parameter
*					\f$\delta\f$, \f$(\delta > 0)\f$.
* \param	tau			The 3rd order smoothness parameter
*					 \f$\tau\f$, \f$(\tau > 0)\f$.
*/
double          WlzBasisFnValueMOSPhi(double r, double delta, double tau)
{
  double        tD0,
                tD1,
                v,
                w,
                rv,
                rw,
                norm,
                phi;

  tD0 = 2.0 * tau * delta;
  tD1 = sqrt(1.0 - (tD0 * tD0));
  tD0 = 2.0 * tau * tau;
  v = (1 + tD1) / tD0;
  w = (1 - tD1) / tD0;
  rv = sqrt(v);
  rw = sqrt(w);
  norm = 1.0 / (4.0 * ALG_M_PI * delta * delta);
  phi = WlzBasisFnValueMOSPhiPC(r, v, w, delta, rv, rw, norm);
  return(phi);
}

/*!
* \return	The value of a single multiorder radial basis function.
* \brief	Computes the value of a single multiorder radial basis
*		function using either an approximation:
*		\f[
		phi'(r) = \frac{\frac{\sqrt{w} - \sqrt{v}}{w - v}
		                (\sqrt{w v} - \frac{w v}{6}) +
		                \frac{v w r^3}{24}}
		               {4 \pi \delta^2}
*		\f]
*		if the radial distance is near to zero or else the
*		function:
*		\f[
		\phi(r) = \frac{1}{4 \pi \delta^2 r}
			  (1 -
			   \frac{w}{w - v} e^{- \sqrt{v} r} +
			   \frac{v}{w - v} e^{- \sqrt{w} r})
		\f]
*		where
		\f[
		v = \frac{1 + \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
		\f[
		w = \frac{1 - \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
* \param	r			Radial distance, \f$(r > 0)\f$.
* \param	v			Precomputed parameter \f$ v \f$.
* \param	w			Precomputed parameter \f$ w \f$.
* \param	delta			The 1st order smoothness parameter
*					\f$ \delta \f$.
* \param	rv			Precomputed parameter \f$ \sqrt{v} \f$.
* \param	rw			Precomputed parameter \f$ \sqrt{w} \f$.
* \param	norm			Precomputed parameter,
*					\f$ 4 \pi \delta^2 \f$.
*/
static double   WlzBasisFnValueMOSPhiPC(double r, double v, double w,
                                        double delta, double rv, double rw,
                                        double norm)
{
  double        phi = 0.0,
                wv,
                rwv,
                rvr,
                rwr;

  wv = w - v;
  if(fabs(wv) > DBL_EPSILON)
  {
    rwv = rw - rv;
    if(((rvr = rv * r) < 0.05) || ((rwr = rw * r) < 0.05))
    {
      phi = (rwv * rw * rv) / wv;
      if(r > DBL_EPSILON)
      {
        phi += -(v * w * r * r) * ((rwv / (6.0 * wv)) + (r / 24.0));
      }
    }
    else
    {
      phi = (1.0  - (w * exp(-rvr) - v * exp(-rwr)) / wv) / r;
    }
    phi *= norm;
  }
  return(phi);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Creates a new Gaussian basis function.
* \param	nPts			Number of control point pairs.
* \param	dPts			Destination control points.
* \param	sPts			Source control points.
* \param	delta			Normalized delta value in range 
*					[> 0.0 , < 1.0 ].
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnGauss2DFromCPts(int nPts,
				    WlzDVertex2 *dPts, WlzDVertex2 *sPts,
				    double delta, WlzErrorNum *dstErr)
{
  int		idN,
  		idX,
  		idY,
		idX3,
		idY3,
		nSys;
  double	tD0,
		tD1,
		deltaRg,
		deltaSq,
		range,
		thresh,
		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzDVertex2	tDVx0;
  WlzDBox2	extentDB;
  WlzBasisFn 	*basisFn = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	tol = 1.0e-06;

  nSys = nPts + 3;
  deltaSq = delta * delta;
  if(((wMx = (double *)AlcCalloc(sizeof(double), nSys)) == NULL) ||
     ((bMx = (double *)AlcMalloc(sizeof(double) * nSys)) == NULL) ||
     (AlcDouble2Malloc(&vMx, nSys, nSys) !=  ALC_ER_NONE) ||
     (AlcDouble2Malloc(&aMx, nSys, nSys) !=  ALC_ER_NONE) ||
     ((basisFn = (WlzBasisFn *)AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL) ||
     ((basisFn->poly.v = AlcMalloc(sizeof(WlzDVertex2) * 3)) == NULL) ||
     ((basisFn->basis.v = AlcMalloc(sizeof(WlzDVertex2) * nPts)) == NULL) ||
     ((basisFn->vertices.v = AlcMalloc(sizeof(WlzDVertex2) * nPts)) == NULL) ||
     ((basisFn->param = AlcMalloc(sizeof(double))) == NULL))
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    WlzBasisFnVxExtent2D(&extentDB, dPts, sPts, nPts);
    tD0 = extentDB.xMax - extentDB.xMin;
    tD1 = extentDB.yMax - extentDB.yMin;
    range = (tD0 > tD1)? tD0: tD1;
    if(range <= 1.0)
    {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    basisFn->type = WLZ_FN_BASIS_2DGAUSS;
    basisFn->nPoly = 2;
    basisFn->nBasis = nPts;
    basisFn->nVtx = nPts;
    deltaRg = deltaSq / (range * range);
    *((double *)(basisFn->param)) = deltaRg;
    WlzValueCopyDVertexToDVertex(basisFn->vertices.d2, dPts, nPts);
    /* Fill matrix A and matrix b for the x component. */
    for(idY = 0; idY < 3; ++idY)
    {
      for(idX = 0; idX < 3; ++idX)
      {
	*(*(aMx + idY) + idX) = 0.0;
      }
      *(bMx + idY) = 0.0;
    }
    for(idY = 0; idY < nPts; ++idY)
    {
      idY3 = idY + 3;
      tD0 = (dPts + idY)->vtX;
      tD1 = (dPts + idY)->vtY;
      *(bMx + idY3) = (sPts + idY)->vtX - tD0;
      *(*(aMx + idY3) + 0) = 1.0;
      *(*(aMx + idY3) + 1) = tD0;
      *(*(aMx + idY3) + 2) = tD1;
      *(*(aMx + 0) + idY3) = 1.0;
      *(*(aMx + 1) + idY3) = tD0;
      *(*(aMx + 2) + idY3) = tD1;
      for(idX = 0; idX < idY; ++idX)
      {
	tDVx0.vtX = (dPts + idX)->vtX - (dPts + idY)->vtX;
	tDVx0.vtX *= tDVx0.vtX;
	tDVx0.vtY = (dPts + idX)->vtY - (dPts + idY)->vtY;
	tDVx0.vtY *= tDVx0.vtY;
	tD0 = (tDVx0.vtX + tDVx0.vtY) * deltaRg;
	tD1 = (tD0 > DBL_EPSILON)? exp(tD0): 1.0;
	idX3 = idX + 3;
	*(*(aMx + idY3) + idX3) = tD1;
	*(*(aMx + idX3) + idY3) = tD1;
      }
      *(*(aMx + idY3) + idY3) = 1.0;
    }
    /* Perform singular value decomposition of matrix A. */
    errNum = WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nSys, nSys, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Solve for lambda and the X polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
    						vMx, bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Recover lambda and the x polynomial coefficients, then set up for mu
       and the y polynomial coefficients. */
    WlzBasisFnGauss2DCoef(basisFn, bMx, 1);
    *(bMx + 0) = 0.0;
    *(bMx + 1) = 0.0;
    *(bMx + 2) = 0.0;
    for(idY = 0; idY < nPts; ++idY)
    {
      idY3 = idY + 3;
      *(bMx + idY3) = (sPts + idY)->vtY - (dPts + idY)->vtY;
    }
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
    						vMx, bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Recover mu and the y polynomial coefficients. */
    WlzBasisFnGauss2DCoef(basisFn, bMx, 0);
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    (void )WlzBasisFnFree(basisFn);
    basisFn = NULL;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Creates a new polynomial basis function.
* \param	nPts			Number of control point pairs.
* \param	order			Order of polynomial.
* \param	dPts			Destination control points.
* \param	sPts			Source control points.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnPoly2DFromCPts(int nPts, int order,
				   WlzDVertex2 *dPts, WlzDVertex2 *sPts,
				   WlzErrorNum *dstErr)
{
  int  		idM,
  		idN,
		idX,
  		idY,
		nCoef;
  double	thresh,
  		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzDVertex2	powVx,
  		sVx;
  WlzBasisFn *basisFn = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	tol = 1.0e-06;

  if((order < 0) || (nPts <= 0))
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else if((basisFn = (WlzBasisFn *)AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL)
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  else
  {
    basisFn->type = WLZ_FN_BASIS_2DPOLY;
    basisFn->nPoly = order;
    basisFn->nBasis = 0;
    basisFn->nVtx = 0;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    nCoef = (order + 1) * (order + 1);
    if(((wMx = (double *)AlcCalloc(sizeof(double), nCoef)) == NULL) ||
       ((bMx = (double *)AlcMalloc(sizeof(double) * nPts)) == NULL) ||
       (AlcDouble2Malloc(&vMx, nPts, nCoef) != ALC_ER_NONE) ||
       (AlcDouble2Malloc(&aMx, nPts, nCoef) !=  ALC_ER_NONE) ||
       ((basisFn->poly.v = AlcMalloc(sizeof(WlzDVertex2) * nCoef)) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Fill matrix A. */
    for(idM = 0; idM < nPts; ++idM)
    {
      idN = 0;
      powVx.vtY = 1.0;
      sVx = *(sPts + idM);
      for(idY = 0; idY <= basisFn->nPoly; ++idY)
      {
	powVx.vtX = 1.0;
	for(idX = 0; idX <= basisFn->nPoly; ++idX)
	{
	  *(*(aMx + idM) + idN++) = powVx.vtX * powVx.vtY;
	  powVx.vtX *= sVx.vtX;
	}
	powVx.vtY *= sVx.vtY;
      }
    }
    /* Perform singular value decomposition of matrix A. */
    errNum= WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nPts, nCoef, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nCoef; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nCoef; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Fill matrix b for x coordinate */
    for(idM = 0; idM < nPts; ++idM)
    {
      *(bMx + idM) = (sPts + idM)->vtX - (dPts + idM)->vtX;
    }
    /* Solve for x polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nPts, nCoef, wMx, vMx,
    					        bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Copy out the x polynomial coefficients, fill matrix b for
       y coordinate and re-solve. */
    for(idN = 0; idN < nCoef; ++idN)
    {
      (basisFn->poly.d2 + idN)->vtX = *(bMx + idN);
    }
    for(idM = 0; idM < nPts; ++idM)
    {
      *(bMx + idM) = (sPts + idM)->vtY - (dPts + idM)->vtY;
    }
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nPts, nCoef, wMx, vMx,
    						bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Copy out the ypolynomial coefficients. */
    for(idN = 0; idN < nCoef; ++idN)
    {
      (basisFn->poly.d2 + idN)->vtY = *(bMx + idN);
    }
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    (void )WlzBasisFnFree(basisFn);
    basisFn = NULL;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Creates a new conformal basis function.
* \param	nPts			Number of control point pairs.
* \param	order			Order of conformal poly.
* \param	dPts			Destination control points.
* \param	sPts			Source control points.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnConf2DFromCPts(int nPts, int order,
				WlzDVertex2 *dPts, WlzDVertex2 *sPts,
				WlzErrorNum *dstErr)
{
  int  		idM,
  		idN,
		idX,
  		idY,
		nCoef;
  double	thresh,
  		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzDVertex2	sVx;
  ComplexD	z, zPow;
  WlzBasisFn *basisFn = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	tol = 1.0e-06;

  if((order < 0) || (nPts <= 0))
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else if((basisFn = (WlzBasisFn *)
	           AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL)
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  else
  {
    basisFn->type = WLZ_FN_BASIS_2DCONF_POLY;
    basisFn->nPoly = order;
    basisFn->nBasis = 0;
    basisFn->nVtx = 0;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    nCoef = (order + 1) + (order + 1);
    if(((wMx = (double *)AlcCalloc(sizeof(double), nCoef)) == NULL) ||
       ((bMx = (double *)AlcMalloc(sizeof(double) * 2 * nPts)) == NULL) ||
       (AlcDouble2Malloc(&vMx, 2*nPts, nCoef) != ALC_ER_NONE) ||
       (AlcDouble2Malloc(&aMx, 2*nPts, nCoef) !=  ALC_ER_NONE) ||
       ((basisFn->poly.v = AlcMalloc(sizeof(WlzDVertex2) * nCoef)) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Fill matrix A. */
    for(idM = 0; idM < nPts; ++idM)
    {
      sVx = *(sPts + idM);
      z.re = sVx.vtX;
      z.im = sVx.vtY;
      zPow.re = 1.0;
      zPow.im = 0.0;
      for(idY = 0, idX = basisFn->nPoly + 1; idY <= basisFn->nPoly; ++idY, idX++)
      {
	aMx[idM][idY] = zPow.re;
	aMx[idM][idX] = -zPow.im;
	aMx[idM + nPts][idY] = zPow.im;
	aMx[idM + nPts][idX] = zPow.re;
	zPow = AlgCMult(zPow, z);
      }
   }
    /* Perform singular value decomposition of matrix A. */
    errNum= WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nPts, nCoef, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nCoef; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nCoef; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Fill matrix b for x coordinate */
    for(idM = 0; idM < nPts; ++idM)
    {
      *(bMx + idM) = (sPts + idM)->vtX - (dPts + idM)->vtX;
      *(bMx + idM + nPts) = (sPts + idM)->vtY - (dPts + idM)->vtY;
    }
    /* Solve for conformal polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nPts, nCoef, wMx, vMx,
    					        bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Copy out the conformal polynomial coefficients */
    for(idN = 0; idN < (order + 1); ++idN)
    {
      (basisFn->poly.d2 + idN)->vtX = *(bMx + idN);
      (basisFn->poly.d2 + idN)->vtY = *(bMx + idN + order + 1);
    }
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    (void )WlzBasisFnFree(basisFn);
    basisFn = NULL;
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Creates a new multiquadric basis function.
* \param	nPts			Number of control point pairs.
* \param	dPts			Destination control points.
* \param	sPts			Source control points.
* \param	delta			Normalized delta value in range
*					[> 0.0 , < 1.0 ].
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnMQ2DFromCPts(int nPts,
				WlzDVertex2 *dPts, WlzDVertex2 *sPts,
				double delta, WlzErrorNum *dstErr)
{
  int		idN,
  		idX,
  		idY,
		idX3,
		idY3,
		nSys;
  double	tD0,
		tD1,
		deltaRg,
		deltaSq,
		range,
		thresh,
		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzDVertex2	tDVx0;
  WlzDBox2	extentDB;
  WlzBasisFn *basisFn = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	tol = 1.0e-06;

  nSys = nPts + 3;
  deltaSq = delta * delta;
  if(((wMx = (double *)AlcCalloc(sizeof(double), nSys)) == NULL) ||
     ((bMx = (double *)AlcMalloc(sizeof(double) * nSys)) == NULL) ||
     (AlcDouble2Malloc(&vMx, nSys, nSys) !=  ALC_ER_NONE) ||
     (AlcDouble2Malloc(&aMx, nSys, nSys) !=  ALC_ER_NONE) ||
     ((basisFn = (WlzBasisFn *)
	       AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL) ||
     ((basisFn->poly.v = AlcMalloc(sizeof(WlzDVertex2) * 3)) == NULL) ||
     ((basisFn->basis.v = AlcMalloc(sizeof(WlzDVertex2) * nSys)) == NULL) ||
     ((basisFn->vertices.v = AlcMalloc(sizeof(WlzDVertex2) * nPts)) == NULL) ||
     ((basisFn->param = AlcMalloc(sizeof(double))) == NULL))
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    WlzBasisFnVxExtent2D(&extentDB, dPts, sPts, nPts);
    tD0 = extentDB.xMax - extentDB.xMin;
    tD1 = extentDB.yMax - extentDB.yMin;
    range = (tD0 > tD1)? tD0: tD1;
    if(range <= 1.0)
    {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    basisFn->type = WLZ_FN_BASIS_2DMQ;
    basisFn->nPoly = 2;
    basisFn->nBasis = nPts;
    basisFn->nVtx = nPts;
    deltaRg = deltaSq * range * range;
    *((double *)(basisFn->param)) = deltaRg;
    WlzValueCopyDVertexToDVertex(basisFn->vertices.d2, dPts, nPts);
    /* Fill matrix A and matrix b for the x component. */
    for(idY = 0; idY < 3; ++idY)
    {
      for(idX = 0; idX < 3; ++idX)
      {
	*(*(aMx + idY) + idX) = 0.0;
      }
      *(bMx + idY) = 0.0;
    }
    for(idY = 0; idY < nPts; ++idY)
    {
      idY3 = idY + 3;
      tD0 = (dPts + idY)->vtX;
      *(bMx + idY3) = (sPts + idY)->vtX - tD0;
      tD0 = (tD0 - extentDB.xMin) / range;
      tD1 = ((dPts + idY)->vtY - extentDB.yMin) / range;
      *(*(aMx + idY3) + 0) = 1.0;
      *(*(aMx + idY3) + 1) = tD0;
      *(*(aMx + idY3) + 2) = tD1;
      *(*(aMx + 0) + idY3) = 1.0;
      *(*(aMx + 1) + idY3) = tD0;
      *(*(aMx + 2) + idY3) = tD1;
      for(idX = 0; idX < idY; ++idX)
      {
	tDVx0.vtX = ((dPts + idX)->vtX - (dPts + idY)->vtX) / range;
	tDVx0.vtX *= tDVx0.vtX;
	tDVx0.vtY = ((dPts + idX)->vtY - (dPts + idY)->vtY) / range;
	tDVx0.vtY *= tDVx0.vtY;
	tD0 = tDVx0.vtX + tDVx0.vtY;
	tD1 = (tD0 > DBL_EPSILON)? sqrt(tD0 + deltaSq): delta;
	idX3 = idX + 3;
	*(*(aMx + idY3) + idX3) = tD1;
	*(*(aMx + idX3) + idY3) = tD1;
      }
      *(*(aMx + idY3) + idY3) = delta;
    }
    /* Perform singular value decomposition of matrix A. */
    errNum = WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nSys, nSys, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Solve for lambda and the X polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
    						vMx, bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Recover lambda and the x polynomial coefficients, then set up for mu
       and the y polynomial coefficients. */
    WlzBasisFnMQ2DCoexff(basisFn, bMx,  &extentDB, range, 1);
    *(bMx + 0) = 0.0;
    *(bMx + 1) = 0.0;
    *(bMx + 2) = 0.0;
    for(idY = 0; idY < nPts; ++idY)
    {
      idY3 = idY + 3;
      *(bMx + idY3) = (sPts + idY)->vtY - (dPts + idY)->vtY;
    }
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
    						vMx, bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Recover mu and the y polynomial coefficients. */
    WlzBasisFnMQ2DCoexff(basisFn, bMx,  &extentDB, range, 0);
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    if(basisFn)
    {
      (void )WlzBasisFnFree(basisFn);
      basisFn = NULL;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Creates a new thin plate spline basis function.
* \param	nPts			Number of control point pairs.
* \param	dPts			Destination control points.
* \param	sPts			Source control points.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnTPS2DFromCPts(int nPts,
				  WlzDVertex2 *dPts, WlzDVertex2 *sPts,
				  WlzErrorNum *dstErr)
{
  int		idN,
  		idX,
		idY,
		idX3,
		idY3,
		nSys;
  double	tD0,
		tD1,
		range,
		thresh,
		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzBasisFn *basisFn = NULL;
  WlzDVertex2	tDVx0;
  WlzDBox2	extentDB;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	tol = 1.0e-06;

  nSys = nPts + 3;
  if(((wMx = (double *)AlcCalloc(sizeof(double), nSys)) == NULL) ||
     ((bMx = (double *)AlcMalloc(sizeof(double) * nSys)) == NULL) ||
     (AlcDouble2Malloc(&vMx, nSys, nSys) !=  ALC_ER_NONE) ||
     (AlcDouble2Malloc(&aMx, nSys, nSys) !=  ALC_ER_NONE) ||
     ((basisFn = (WlzBasisFn *)
	       AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL) ||
     ((basisFn->poly.v = AlcMalloc(sizeof(WlzDVertex2) * 3)) == NULL) ||
     ((basisFn->basis.v = AlcMalloc(sizeof(WlzDVertex2) * nPts)) == NULL) ||
     ((basisFn->vertices.v = AlcMalloc(sizeof(WlzDVertex2) * nPts)) == NULL))
  {
    errNum = WLZ_ERR_MEM_ALLOC;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    WlzBasisFnVxExtent2D(&extentDB, dPts, sPts, nPts);
    tD0 = extentDB.xMax - extentDB.xMin;
    tD1 = extentDB.yMax - extentDB.yMin;
    range = WLZ_MAX(tD0, tD1);
    if(range <= 1.0)
    {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    basisFn->type = WLZ_FN_BASIS_2DTPS;
    basisFn->nPoly = 2;
    basisFn->nBasis = nPts;
    basisFn->nVtx = nPts;
    for(idY = 0; idY < 3; ++idY)
    {
      for(idX = 0; idX < 3; ++idX)
      {
	*(*(aMx + idY) + idX) = 0.0;
      }
      *(bMx + idY) = 0.0;
    }
    for(idY = 0; idY < nPts; ++idY)
    {
      idY3 = idY + 3;
      tD0 = (dPts + idY)->vtX;
      *(bMx + idY3) = (sPts + idY)->vtX - tD0;
      tD0 = (tD0 - extentDB.xMin) / range;
      tD1 = ((dPts + idY)->vtY - extentDB.yMin) / range;
      *(*(aMx + idY3) + 0) = 1.0;
      *(*(aMx + idY3) + 1) = tD0;
      *(*(aMx + idY3) + 2) = tD1;
      *(*(aMx + 0) + idY3) = 1.0;
      *(*(aMx + 1) + idY3) = tD0;
      *(*(aMx + 2) + idY3) = tD1;
      for(idX = 0; idX < idY; ++idX)
      {
	tDVx0.vtX = ((dPts + idX)->vtX - (dPts + idY)->vtX) / range;
	tDVx0.vtX *= tDVx0.vtX;
	tDVx0.vtY = ((dPts + idX)->vtY - (dPts + idY)->vtY) / range;
	tDVx0.vtY *= tDVx0.vtY;
	tD0 = tDVx0.vtX + tDVx0.vtY;
	tD1 = (tD0 > DBL_EPSILON)? tD0 * log(tD0): 0.0;
	idX3 = idX + 3;
	*(*(aMx + idY3) + idX3) = tD1;
	*(*(aMx + idX3) + idY3) = tD1;
      }
      *(*(aMx + idY3) + idY3) = 0.0;
    }
    /* Perform singular value decomposition of matrix A. */
    errNum = WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nSys, nSys, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Solve for lambda and the X polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
    						vMx, bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    WlzValueCopyDVertexToDVertex(basisFn->vertices.d2, dPts, nPts);
    WlzBasisFnTPS2DCoef(basisFn, bMx,  &extentDB, range, 1);
    *(bMx + 0) = 0.0;
    *(bMx + 1) = 0.0;
    *(bMx + 2) = 0.0;
    for(idY = 0; idY < nPts; ++idY)
    {
      *(bMx + idY + 3) = (sPts + idY)->vtY - (dPts + idY)->vtY;
    }
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
				    		vMx, bMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    WlzBasisFnTPS2DCoef(basisFn, bMx,  &extentDB, range, 0);
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    if(basisFn)
    {
      (void )WlzBasisFnFree(basisFn);
      basisFn = NULL;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Creates a new 3D multi order spline basis function:
*		\f[
		\phi(r) = \frac{1}{4 \pi \delta^2 r}
			  (1 -
			   \frac{w}{w - v} e^{- \sqrt{v} r} +
			   \frac{v}{w - v} e^{- \sqrt{w} r})
		\f]
		\f[
		v = \frac{1 + \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
		\f[
		w = \frac{1 - \sqrt{1 - 4 \pi \tau^2 \delta^2}}{2 \tau^2}
		\f]
		\f[
		f(\mathbf{x}) = P(\mathbf{x}) +
			        \sum_{i=1}^{n}{\lambda_i \phi(r_i)}
		\f]
		\f[
		r_i = |\mathbf{x} - \mathbf{x'_i}|
		\f]
*		The multi order spline may either be an exact interpolating
*		function or one which approximates the given control points.
*		In the approximating case values for the regularization
*		parameters should be given, with the regularization
*		parameters \f$\alpha_i\f$ given by:
*		\f[
		H[f] = \beta[f] +
		       \sum_{i=1}^{n}{\frac{(y_i - f(x))^2}{\alpha_i}}
		\f]
		Giving the design equation:
		\f[
		\left(
		\begin{array}{cccccccc}
		0 & 0 & 0 & 0 & 1 & 1 & \cdots & 1 \\
		0 & 0 & 0 & 0 & x_1 & x_2 & \cdots & x_n \\
		0 & 0 & 0 & 0 & y_1 & y_2 & \cdots & y_n \\
		0 & 0 & 0 & 0 & z_1 & z_2 & \cdots & z_n \\
		1 & x_1 & y_1 & z_1 & \phi(r_{11}) + \alpha_1 & \phi(r_{12}) &
		          \cdots & \phi(r_{1n}) \\
		\dotfill \\
		1 & x_n & y_n & z_n & \phi(r_{n1}) & \phi(r_{n2}) &
		          \cdots & \phi(r_{nn} + \alpha_n)
		\end{array}
		\right)

		\left(
		\begin{array}{c}
		a_o \\
		a_1 \\
		a_2 \\
		a_3 \\
		\lambda_1 \\
		\lambda_1 \\
		\vdots \\
		\lambda_n
		\end{array}
		\right)
		
		=

		\left(
		\begin{array}{c}
		0 \\
		0 \\
		0 \\
		0 \\
		f(\mathbf{x_1}) \\
		f(\mathbf{x_1}) \\
		\vdots \\
		f(\mathbf{x_n})
		\end{array}
		\right)
		\f]
*		The vertex values with components  \f$x\f$, \f$y \f$ and
*		\f$z\f$ are used to solve for the four polynomial
*		coefficients and the three basis function weights 
*		\f$\lambda\f$, \f$\mu\f$ and \f$\nu\f$.
*		The given values of \f$\lambda\f$ and \f$\tau\f$ are
*		constrained by \f$\lambda > 0, \tau > 0\f$ and
*		\f$\lambda \tau < 0.5\f$.
* \param	nPts			Number of control point pairs.
* \param	dPts			Destination control points.
* \param	sPts			Source control points.
* \param	alpha			Regularization parameter for the
*					control points. If NULL all
*					regularization parameters are set to
*					zero for exact interpolation.
* \param	param			Smoothness parameters \f$\delta\f$
*					and \f$\tau\f$ in that order,
*					but may be NULL in which case default
*					parameter values will be used.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnMOS3DFromCPts(int nPts,
				  WlzDVertex2 *dPts, WlzDVertex2 *sPts,
				  double *alpha, double *param,
				  WlzErrorNum *dstErr)
{
  WlzBasisFn	*basisFn = NULL;
  WlzErrorNum	errNum = WLZ_ERR_UNIMPLEMENTED;

  /* TODO */
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	New basis function.
* \ingroup	WlzFunction
* \brief	Computes a new 3D multi order spline basis function
*		which either interpolates or approximates the given
*		scalar values. See WlzBasisFnMOS3DFromCPts() for details
*		of the multi order spline.
*		The given values of \f$\lambda\f$ and \f$\tau\f$ are
*		constrained by \f$\lambda > 0, \tau > 0\f$ and
*		\f$\lambda \tau < 0.5\f$.
* \param	nPts			Number of values.
* \param	cPts			Positions of the control points.
* \param	cVal			Values at the control points.
* \param	alpha			Regularization parameter for the
*					control points. If NULL all
*					regularization parameters are set to
*					zero for exact interpolation.
* \param	param			Smoothness parameters \f$\delta\f$
*					and \f$\tau\f$ in that order,
*					but may be NULL in which case default
*					parameter values will be used.
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzBasisFn *WlzBasisFnScalarMOS3DFromCPts(int nPts,
				  WlzDVertex3 *cPts, double *cVal,
				  double *alpha, double *param,
				  WlzErrorNum *dstErr)
{
  int		idN,
  		idX,
		idY,
		idX1,
		idY1,
		nSys;
  double	tD0,
		rad,
		phi0,
		thresh,
		delta,
		tau,
		wMax;
  double	*bMx = NULL,
  		*wMx = NULL;
  double	**aMx = NULL,
  		**vMx = NULL;
  WlzBasisFn	*basisFn = NULL;
  WlzDVertex3	tV0,
  		tV1,
		tV2;
  WlzDBox2	extentDB;
  WlzErrorNum	errNum = WLZ_ERR_NONE;
  const double	tol = 1.0e-06;

  
  delta = *(param + 0);
  tau = *(param + 1);
  if((delta < DBL_EPSILON) || (tau < DBL_EPSILON) ||
     (delta * tau > (0.5 - DBL_EPSILON)))
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    nSys = nPts + 1;
    if(((wMx = (double *)AlcCalloc(sizeof(double), nSys)) == NULL) ||
	((bMx = (double *)AlcMalloc(sizeof(double) * nSys)) == NULL) ||
	(AlcDouble2Malloc(&vMx, nSys, nSys) !=  ALC_ER_NONE) ||
	(AlcDouble2Malloc(&aMx, nSys, nSys) !=  ALC_ER_NONE) ||
	((basisFn = (WlzBasisFn *)
	  AlcCalloc(sizeof(WlzBasisFn), 1)) == NULL) ||
	((basisFn->poly.v = AlcMalloc(sizeof(double) * 1)) == NULL) ||
	((basisFn->basis.v = AlcMalloc(sizeof(double) * nPts)) == NULL) ||
	((basisFn->vertices.v = AlcMalloc(sizeof(WlzDVertex3) *
	                                  nPts)) == NULL) ||
	((basisFn->param = AlcMalloc(sizeof(double) * 2)) == NULL))
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    *((double *)(basisFn->param) + 0) = delta;
    *((double *)(basisFn->param) + 1) = tau;
    basisFn->type = WLZ_FN_BASIS_SCALAR_3DMOS;
    basisFn->nPoly = 1;
    basisFn->nBasis = nPts;
    basisFn->nVtx = nPts;
    WlzValueCopyDVertexToDVertex3(basisFn->vertices.d3, cPts, nPts);
    *(*(aMx + 0) + 0) = 0.0;
    for(idX = 0; idX < nPts; ++idX)
    {
      idX1 = idX + 1;
      *(*(aMx + 0) + idX1) = 1.0;
    }
    *(bMx + 0) = 0.0;
    phi0 = WlzBasisFnValueMOSPhi(0.0, delta, tau);
    for(idY = 0; idY < nPts; ++idY)
    {
      idY1 = idY + 1;
      tV0 = *(cPts + idY);
      *(bMx + idY1) = *(cVal + idY);
      *(*(aMx + idY1) + 0) = 1.0;
      for(idX = 0; idX < idY; ++idX)
      {
	idX1 = idX + 1;
	tV1 = *(cPts + idX);
	WLZ_VTX_3_SUB(tV2, tV1, tV0);
	rad = WLZ_VTX_3_LENGTH(tV2);
	tD0 = WlzBasisFnValueMOSPhi(rad, delta, tau);
	*(*(aMx + idY1) + idX1) = *(*(aMx + idX1) + idY1) = tD0;
      }
      *(*(aMx + idY1) + idY1) = phi0 + *(alpha + idY);
    }
#ifdef WLZ_BASISFNSCALARMOS3DFROMCPTS_DEBUG
    {
      FILE	*fP;
      fP = fopen("DEBUG_aMx.num", "w");
      AlcDouble2WriteAsci(fP, aMx, nSys, nSys);
      fclose(fP);
      fP = fopen("DEBUG_bMx.num", "w");
      AlcDouble1WriteAsci(fP, bMx, nSys);
      fclose(fP);
    }
#endif
    /* Perform singular value decomposition of matrix A. */
    errNum = WlzErrorFromAlg(AlgMatrixSVDecomp(aMx, nSys, nSys, wMx, vMx));
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Edit the singular values. */
    wMax = 0.0;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) > wMax)
      {
	wMax = *(wMx + idN);
      }
    }
    thresh = tol * wMax;
    for(idN = 0; idN < nSys; ++idN)
    {
      if(*(wMx + idN) < thresh)
      {
	*(wMx + idN) = 0.0;
      }
    }
    /* Solve for lambda and the polynomial coefficients. */
    errNum = WlzErrorFromAlg(AlgMatrixSVBackSub(aMx, nSys, nSys, wMx,
    						vMx, bMx));
  }
#ifdef WLZ_BASISFNSCALARMOS3DFROMCPTS_DEBUG
    {
      FILE	*fP;
      fP = fopen("DEBUG_bMxBS.num", "w");
      AlcDouble1WriteAsci(fP, bMx, nSys);
      fclose(fP);
    }
#endif
  if(errNum == WLZ_ERR_NONE)
  {
    *(double *)(basisFn->poly.v) = *bMx;	      /* Only constant used. */
    for(idY = 0; idY < nPts; ++idY)
    {
      idY1 = idY + 1;
      *((double *)(basisFn->basis.v) + idY) = *(bMx + idY1);
    }
  }
  if(bMx)
  {
    AlcFree(bMx);
  }
  if(wMx)
  {
    AlcFree(wMx);
  }
  if(aMx)
  {
    (void )AlcDouble2Free(aMx);
  }
  if(vMx)
  {
    (void )AlcDouble2Free(vMx);
  }
  if(errNum != WLZ_ERR_NONE)
  {
    if(basisFn)
    {
      (void )WlzBasisFnFree(basisFn);
      basisFn = NULL;
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(basisFn);
}

/*!
* \return	<void>
* \ingroup	WlzFunction
* \brief	Computes the extent (bounding box) of two arrays of
*		2D vertices.
* \param	extentDB		Pointer for the extent of the
* 					vertices.
* \param	vx0			First vector of vertices.
* \param	vx1			Second vector of vertices.
* \param	nPts			Number of vertices in each vector.
*/
static void	WlzBasisFnVxExtent2D(WlzDBox2 *extentDB,
				   WlzDVertex2 *vx0, WlzDVertex2 *vx1,
				   int nPts)
{
  double	tD0,
		tD1,
		tD2;

  extentDB->xMin = extentDB->xMax = vx0->vtX;
  extentDB->yMin = extentDB->yMax = vx0->vtY;
  while(nPts-- > 0)
  {
    if((tD0 = vx0->vtX) > (tD1 = vx1->vtX))
    {
      tD2 = tD0;
      tD0 = tD1;
      tD1 = tD2;
    }
    if(tD0 < extentDB->xMin)
    {
      extentDB->xMin = tD0;
    }
    if(tD1 > extentDB->xMax)
    {
      extentDB->xMax = tD1;
    }
    if((tD0 = vx0->vtY) > (tD1 = vx1->vtY))
    {
      tD2 = tD0;
      tD0 = tD1;
      tD1 = tD2;
    }
    if(tD0 < extentDB->yMin)
    {
      extentDB->yMin = tD0;
    }
    if(tD1 < extentDB->yMax)
    {
      extentDB->yMax = tD1;
    }
    ++vx0;
    ++vx1;
  }
}

/*!
* \return	<void>
* \ingroup	WlzFunction
* \brief	Extracts the Gaussian coefficients from the given column
* 		vector using the given extent and range for function.
* \param	basisFn			Allocated basis function to
* 					be filled in.
* \param	vec			Given column vector.
* \param	forX			True if the coefficients are for the x
* 					coordinate.
*/
static void	WlzBasisFnGauss2DCoef(WlzBasisFn *basisFn, double *vec, int forX)
{
  int		idN;
  double	*vecP;
  WlzDVertex2	*basisVxP,
  		*polyVxP;

  vecP = vec;
  basisVxP = basisFn->basis.d2;
  polyVxP = basisFn->poly.d2;
  if(forX)
  {
    polyVxP++->vtX = *vecP++;
    polyVxP++->vtX = *vecP++;
    polyVxP->vtX = *vecP++;
    for(idN = 0; idN < basisFn->nBasis; ++idN)
    {
      basisVxP++->vtX = *vecP++;
    }
  }
  else
  {
    polyVxP++->vtY = *vecP++;
    polyVxP++->vtY = *vecP++;
    polyVxP->vtY = *vecP++;
    for(idN = 0; idN < basisFn->nBasis; ++idN)
    {
      basisVxP++->vtY = *vecP++;
    }
  }
}

/*!
* \return	<void>
* \ingroup	WlzFunction
* \brief	Extracts the multiquadric coefficients from the given
*		column vector using the given extent and range for
*		function.
* \param	basisFn			Allocated basis function
*					to be filled in.
* \param	vec			Given column vector.
* \param	extentDB		Extent of the vertices.
* \param	range			Range of the vertices.
* \param	forX		 	True if the coefficients are for
*					the x coordinate.
*/
static void	WlzBasisFnMQ2DCoexff(WlzBasisFn *basisFn,
				  double *vec, WlzDBox2 *extentDB,
				  double range, int forX)
{
  int		idN;
  double 	vec0,
  		vec1,
  		vec2;
  double	*vecP;
  WlzDVertex2	*basisVxP,
  		*polyVxP;

  vec0 = *vec;
  vec1 = *(vec + 1);
  vec2 = *(vec + 2);
  vecP = vec + 3;
  basisVxP = basisFn->basis.d2;
  polyVxP = basisFn->poly.d2;
  if(forX)
  {
    polyVxP++->vtX = vec0 -
    		     (((vec1 * extentDB->xMin) +
    		       (vec2 * extentDB->yMin)) / range);
    polyVxP++->vtX = vec1 / range;
    polyVxP->vtX = vec2 / range;
    for(idN = 0; idN < basisFn->nBasis; ++idN)
    {
      basisVxP++->vtX = *vecP++ / range;
    }
  }
  else
  {
    polyVxP++->vtY = vec0 -
    		     (((vec1 * extentDB->xMin) +
    		       (vec2 * extentDB->yMin)) / range);
    polyVxP++->vtY = vec1 / range;
    polyVxP->vtY = vec2 / range;
    for(idN = 0; idN < basisFn->nBasis; ++idN)
    {
      basisVxP++->vtY = *vecP++ / range;
    }
  }
}

/*!
* \return	<void>
* \ingroup	WlzFunction
* \brief	Extracts the thin plate spline coefficients from the given
*		column vector using the given extent and range for
*		function.
* \param	basisFn			Allocated basis function to
* 					be filled in.
* \param	vec			Given column vector.
* \param	extentDB		Extent of the vertices.
* \param	range			Range of the vertices.
* \param	forX			True if the coefficients are for the x
* 					coordinate.
*/
static void	WlzBasisFnTPS2DCoef(WlzBasisFn *basisFn,
				   double *vec, WlzDBox2 *extentDB,
				   double range, int forX)
{
  int		idN;
  double	tD0,
		tD1,
		rangeSq,
		sumLogCoeffRSq;
  WlzDVertex2	tDVx0;

  rangeSq = range * range;
  tD0 = 2.0 / rangeSq;
  sumLogCoeffRSq = 0.0;
  for(idN = 0; idN < basisFn->nBasis; ++idN)
  {
    tD1 = *(vec + idN + 3);
    tDVx0 = *(basisFn->vertices.d2 + idN);
    tDVx0.vtX = tDVx0.vtX - extentDB->xMin;
    tDVx0.vtX *= tDVx0.vtX;
    tDVx0.vtY = tDVx0.vtY - extentDB->yMin;
    tDVx0.vtY *= tDVx0.vtY;
    sumLogCoeffRSq += tD1 * (tDVx0.vtX + tDVx0.vtY);
    if(forX)
    {
      (basisFn->basis.d2 + idN)->vtX = tD1 * tD0;
    }
    else
    {
      (basisFn->basis.d2 + idN)->vtY = tD1 * tD0;
    }
  }
  sumLogCoeffRSq /= rangeSq;
  if(forX)
  {
    (basisFn->poly.d2 + 1)->vtX = *(vec + 1) / range;
    (basisFn->poly.d2 + 2)->vtX = *(vec + 2) / range;
    (basisFn->poly.d2 + 0)->vtX = *(vec + 0) -
			     ((basisFn->poly.d2 + 1)->vtX * extentDB->xMin) -
			     ((basisFn->poly.d2 + 2)->vtX * extentDB->yMin) -
			     (log(rangeSq) * sumLogCoeffRSq);
  }
  else
  {
    (basisFn->poly.d2 + 1)->vtY = *(vec + 1) / range;
    (basisFn->poly.d2 + 2)->vtY = *(vec + 2) / range;
    (basisFn->poly.d2 + 0)->vtY = *(vec + 0) -
			      ((basisFn->poly.d2 + 1)->vtY * extentDB->yMin) -
			      ((basisFn->poly.d2 + 2)->vtY * extentDB->xMin) -
			      (log(rangeSq) * sumLogCoeffRSq);
  }
}

/*!
* \return	Displacement due to reduced 2D polynomial.
* \ingroup	WlzFunction
* \brief	Computes the value of the reduced 2D polynomial
* 		used by the TPS, MQ and Gauss basis functions.
* \param	poly			Given polynomial coefficients.
* \param	srcVx			Source vertex.
*/
static WlzDVertex2 WlzBasisFnValueRedPoly2D(WlzDVertex2 *poly,
					WlzDVertex2 srcVx)
{
  WlzDVertex2	newVx;

  newVx.vtX = poly->vtX;
  newVx.vtY = poly->vtY;
  ++poly;
  newVx.vtX += poly->vtX * srcVx.vtX;
  newVx.vtY += poly->vtY * srcVx.vtX;
  ++poly;
  newVx.vtX += poly->vtX * srcVx.vtY;
  newVx.vtY += poly->vtY * srcVx.vtY;
  return(newVx);
}

/*!
* \return	Displacement due to reduced 3D polynomial.
* \ingroup	WlzFunction
* \brief	Computes the value of the reduced 3D polynomial
* 		used by the MOS basis function.
* \param	poly			Given polynomial coefficients.
* \param	srcVx			Source vertex.
*/
static WlzDVertex3 WlzBasisFnValueRedPoly3D(WlzDVertex3 *poly,
					WlzDVertex3 srcVx)
{
  WlzDVertex3	newVx;

  newVx.vtX = poly->vtX;
  newVx.vtY = poly->vtY;
  ++poly;
  newVx.vtX += poly->vtX * srcVx.vtX;
  newVx.vtY += poly->vtY * srcVx.vtX;
  newVx.vtZ += poly->vtZ * srcVx.vtX;
  ++poly;
  newVx.vtX += poly->vtX * srcVx.vtY;
  newVx.vtY += poly->vtY * srcVx.vtY;
  newVx.vtZ += poly->vtZ * srcVx.vtY;
  ++poly;
  newVx.vtX += poly->vtX * srcVx.vtZ;
  newVx.vtY += poly->vtY * srcVx.vtZ;
  newVx.vtZ += poly->vtZ * srcVx.vtZ;
  return(newVx);
}
