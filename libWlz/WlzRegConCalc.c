#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzRegConCalc_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         WlzRegConCalc.c
* \author       Bill Hill
* \date         July 2013
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2013],
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
* \brief	Computes region connected calculus spatial classifications.
* \ingroup	WlzBinaryOps
*/

#include <string.h>
#include <Wlz.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/*!
* \enum		_WlzRCCTOIdx
* \ingroup	WlzBinaryOps
* \brief	Enumeration of temporary objects used in within
* 		WlzRegConCalcRCC(), WlzRCCMakeC() and WlzRCCMakeT().
* 		Typedef::WlzRCCTOIdx.
*/
typedef	enum	_WlzRCCTOIdx
{
  WLZ_RCCTOIDX_O0O1U	= 0,		/*!<\f$ o_0     \cup \o_1        \f$*/
  WLZ_RCCTOIDX_O0O1I,			/*!<\f$ o_0     \cap \o_1        \f$*/
  WLZ_RCCTOIDX_O0D,			/*!<\f$ o_0^+                    \f$*/
  WLZ_RCCTOIDX_O1D,			/*!<\f$ o_1^+                    \f$*/
  WLZ_RCCTOIDX_O0F,			/*!<\f$ o_0^{\bullet}            \f$*/
  WLZ_RCCTOIDX_O1F,			/*!<\f$ o_1^{\bullet}            \f$*/
  WLZ_RCCTOIDX_O0DO1U,			/*!<\f$ o_0^+   \cup o_1         \f$*/
  WLZ_RCCTOIDX_O0O1DU,			/*!<\f$ o_0     \cup o_1^+       \f$*/
  WLZ_RCCTOIDX_O0CO1I,                  /*!<\f$ o_0^{\circ} \cap o_1     \f$*/
  WLZ_RCCTOIDX_O0O1CI,			/*!<\f$ o_0     \cap o_1^{\circ} \f$*/
  WLZ_RCCTOIDX_O0FO1U,                  /*!<\f$ o_0^{\bullet} \cup o_1   \f$*/
  WLZ_RCCTOIDX_O0O1FU,                  /*!<\f$ o_0 \cup o_1^{\bullet}   \f$*/
  WLZ_RCCTOIDX_CNT			/*!< Not an index but their number. */
} WlzRCCTOIdx;
#endif

static WlzErrorNum 		WlzRCCMakeC(
                                  WlzObject **c,
				  WlzObject **o,
                                  WlzObject **t,
				  int i);
static WlzErrorNum 		WlzRCCMakeT(
				  WlzObject **t,
				  WlzObject **o,
				  WlzRCCTOIdx i);
static WlzErrorNum		WlzRCCAdjacency(
				  WlzObject *obj0,
				  WlzObject *obj1,
				  int maxDist,
				  double *dMean,
				  double *dSD);
static WlzErrorNum	WlzRCCCompDistHist2D(
			  int maxDist,
			  int *dHist,
			  WlzObject *dObj,
			  WlzObject *tobj);
static WlzErrorNum	WlzRCCCompDistHist3D(
			  int maxDist,
			  int *dHist,
			  WlzObject *dObj,
			  WlzObject *tobj);

/*!
* \return	RCC classification of the given objects, ie object 0 is a
*               returned classification of object 1.
* \ingroup	WlzBinaryOps
* \brief	The given pair of spatial domain objects are classified
*		using a RCC.
*
*		For an explanation of RCC8 classifications
*		see the type definition ::WlzRCCClass and the paper:
*		D.A. Randell, etal,
*		"Discrete Mereotopology for Spatial Reasoning in
*		Automated Histological Image Analysis", PAMI 35(3) 2013.
*		The RCC8 has been extended to include encloses and
*		surrounds.
* 		The classification is performed using simple combinations
* 		of the Woolz union, intersection, exclusive or, dilation,
* 		fill and convex hull operators on an ordered pair of
* 		spatial domains(\f$\Omega_0\f$ and \f$\Omega_1\f$):
*               \f{eqnarray*}{
                  C_0 &\leftarrow&
		    \Omega_0   \cap \Omega_1
		    \neq \emptyset \\
                  C_1 &\leftarrow&
		    \Omega_0^+ \cap \Omega_1
		    \neq \emptyset \\
                  C_2 &\leftarrow&
		    (\Omega_0   \oplus \Omega_1)
		    \neq \emptyset \\
                  C_3 &\leftarrow&
		    (\Omega_0   \cup \Omega_1)   \oplus
		    \Omega_1 
		    \neq \emptyset \\
                  C_4 &\leftarrow&
		    (\Omega_0^+ \cup \Omega_1)   \oplus
		    \Omega_1 
		    \neq \emptyset \\
                  C_5 &\leftarrow&
		    (\Omega_0   \cup \Omega_1)   \oplus
		    \Omega_0 
		    \neq \emptyset \\
                  C_6 &\leftarrow&
		    (\Omega_0   \cup \Omega_1^+) \oplus
		    \Omega_0 
		    \neq \emptyset \\
		  C_7 &\leftarrow&
		    (\Omega_0^{\bullet} \cup \Omega_1) \oplus
		    \Omega_0^{\bullet}
		    \neq \emptyset \\
		  C_8 &\leftarrow&
		    (\Omega_0 \cup \Omega_1^{\bullet}) \oplus
		    \Omega_1^{\bullet}
		    \neq \emptyset \\
		  C_9 &\leftarrow&
		    2|\Omega_0 \cap \Omega_1^{\circ}|   \ge |\Omega_0| \\
		  C_{10} &\leftarrow&
		    2|\Omega_0^{\circ}   \cap \Omega_1| \ge |\Omega_1|
  		\f}
*		where
*		  are the \f$\cup\f$, \f$\cap\f$ and \f$\oplus\f$
*		  are the set union (logical or), intersection (logical and)
*		  and xor (logical exclusive or) operators;
*		  \f$\Omega^+\f$ indicates the dilation of \f$\Omega\f$,
*		  \f$\Omega^{\circ}\f$ the convex hull of \f$\Omega\f$,
*		  \f$\Omega^{\bullet}\f$ indicates \f$\Omega\f$ filled and
*		  \f$|\Omega|\f$ the cardinality (area or volume) of
*		  \f$\Omega\f$.
* 		The decision tree for the classification excluding
* 		enclosure is:
* 		\f[
C_0
\left\{
\begin{array}{ll}
0 & C_1
    \left\{
    \begin{array}{ll}
    0 & C_7
	\left\{
	\begin{array}{ll}
	0 & NTSURI \\
	  & \\
	  & \\
	1 & C_8
	    \left\{
	    \begin{array}{ll}
	    0 & NTSUR \\
	      & \\
	    1 & DC
	    \end{array}
	    \right. \\
	\end{array}
	\right. \\
      & \\
      & \\
    1 & C_7
	\left\{
	\begin{array}{ll}
	0 & TSURI \\
	  & \\
	  & \\
	1 & C_8
	    \left\{
	    \begin{array}{ll}
	    0 & TSUR \\
	      & \\
	    1 & EC
	    \end{array}
	    \right. \\
	\end{array}
	\right. \\
    \end{array}
    \right. \\
  & \\
  & \\
1 & C_2
    \left\{
    \begin{array}{ll}
    0 & EQ \\
      & \\
      & \\
    1 & C_3
	\left\{
	\begin{array}{ll}
	0 & C_4
	    \left\{
	    \begin{array}{ll}
	    0 & NTPP \\
	      & \\
	    1 & TPP
	    \end{array}
	    \right. \\
	  & \\
	  & \\
	1 & C_5
	    \left\{
	    \begin{array}{ll}
	    0 & C_6
		\left\{
		\begin{array}{ll}
		0 & NTPPI \\
		 & \\
		1 & TPPI
		\end{array}
		\right. \\
	      & \\
	    1 & PO
	    \end{array}
	    \right. \\
	\end{array}
	\right. \\
    \end{array}
    \right. \\
\end{array}
\right.
		\f]
*		The statistics are computed for each classification
*		as below:
* 		<table width="500" border="0">
		<caption>
		  Basic Morphological Operations for the RCC Spatial
		  Relationships
		</caption>
                <tr>
                  <td>RCC</td>
		  <td>Normalised Volume</td>
                </tr>
		<tr>
		  <td>\f$EMPTY(\Omega_0,\Omega_1)\f$</td>
		  <td>0.0</td>
		</tr>
		<tr>
		  <td>\f$DC(\Omega_0,\Omega_1)\f$</td>
		  <td>0.0</td>
		</tr>
		<tr>
		  <td>\f$EC(\Omega_0,\Omega_1)\f$</td>
		  <td>0.0</td>
		</tr>
		<tr>
		  <td>\f$EQ(\Omega_0,\Omega_1)\f$</td>
		  <td>1.0</td>
		</tr>
		<tr>
		  <td>\f$PO(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0 \cap \Omega_1|/
		         |\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$TPP(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$NTPP(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$TPPI(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_1|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$NTPPI(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_1|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$TSUR(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$TSURI(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_1|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$NTSUR(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$NTSURI(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_1|/|\Omega_0 \cup \Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$ENC(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0 \cap \Omega_1^{\circ}|/|\Omega_0|\f$</td>
		</tr>
		<tr>
		  <td>\f$ENCI(\Omega_0,\Omega_1)\f$</td>
		  <td>\f$|\Omega_0^{\circ} \cap \Omega_1|/|\Omega_1|\f$</td>
		</tr>
		<tr>
		  <td>\f$ADJ(\Omega_0,\Omega_1)\f$</td>
		  <td>TODO</td>
		</tr>
		<tr>
		  <td>\f$ADJI(\Omega_0,\Omega_1)\f$</td>
		  <td>TODO</td>
		</tr>
		</table>
*		Many of the objects that are computed during the classification
*		are done so using a lazy evaluation with the functions
*		WlzRCCMakeC() and WlzRCCMakeT().
*		Enclosure and adjacency are somwhat more expensive to compute
*		than the other classifications, for this reason and because
*		they are not strictly part of a RCC they can be avoided by
*		setting the noEnc or noAdj flags.
*		Enclosure will be computed if the noEnc has not been set and
*		the classification is not one of WLZ_RCC_EQ, WLZ_RCC_TSUR,
*		WLZ_RCC_TSURI, WLZ_RCC_NTSUR or WLZ_RCC_NTSURI.
*		Enclosure will be computed if the noEnc parameter has not
*		been set and the classification is not WLZ_RCC_EQ.
*		Adjacency will be computed if the noAdj parameter has not
*		been set and the classification is not WLZ_RCC_EQ.
*		TODO add text for adjacency statistics HACK TODO.
* \param	obj0			First given spatial domain object.
* \param	obj1			Second given spatial domain object.
* \param	noEnc			Don't include enclosure if non-zero.
* \param	noAdj			Don't include adjacency if non-zero.
* \param	maxAdjDist		Maximum distance for adjacency, not
* 					used if noAdj is non-zero.
* \param	dstStatCnt		Destination pointer for the number
* 					of elements returned in the array of
* 					statistics (see above), may be NULL.
* 					Ignored if dstStatAry is NULL.
* \param	dstStatAry		Destination pointer for an array of
* 					statistics (see above), may be NULL.
* 					If an array is returned it should be
* 					freed using AlcFree().
* \param	dstErr			Destination error pointer, may be NULL.
*/
WlzRCCClass 			WlzRegConCalcRCC(
				  WlzObject *obj0,
				  WlzObject *obj1,
				 int noEnc,
				 int noAdj,
				 int maxAdjDist,
				 int *dstStatCnt,
				 double **dstStatAry,
				 WlzErrorNum *dstErr)
{
  int 		i;
  WlzLong	i01 = 0, 		/* |\Omega_0 \cap \Omega_1| */
  		u01 = 0; 		/* |\Omega_0 \cup \Omega_1| */
  WlzLong	u[2] = {0},		/* |\Omega_i|, i \in 0 \cdots 1 */
  		v[2] = {0};		/* |c_9|, |c_{10}| */
  WlzObject	*c[11] = {NULL},		/* c_i, i \in 0 \cdots 10 */
		*o[2] = {NULL},		/* \Omega_i, i \in 0 \cdots 1 */
		*t[WLZ_RCCTOIDX_CNT] = {NULL}; /* Temporary object as
					in the enum WlzRCCTOIdx. */
  double	stats[WLZ_RCCIDX_CNT] = {0.0}; /* Classification statistics. */
  WlzValues	nullValues;
  WlzRCCClass	cls = WLZ_RCC_EMPTY; /* Classification mask. */
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  nullValues.core = NULL;
  /* Compute classification using the decision tree. */
  if((obj0 == NULL) || (obj1 == NULL) ||
     (WlzIsEmpty(obj0, NULL) != 0) ||
     (WlzIsEmpty(obj1, NULL) != 0))
  {
    cls = WLZ_RCC_EMPTY;
  }
  else if((obj0->domain.core == NULL) || (obj1->domain.core == NULL))
  {
    errNum = WLZ_ERR_DOMAIN_NULL;
  }
  else if((obj0->type != obj1->type) ||
          ((obj0->type != WLZ_2D_DOMAINOBJ) &&
	   (obj0->type != WLZ_3D_DOMAINOBJ)))
  {
    errNum = WLZ_ERR_OBJECT_TYPE;
  }
  else if(((o[0] = WlzAssignObject(
                   WlzMakeMain(obj0->type, obj0->domain, nullValues,
  			       NULL, NULL, &errNum), NULL)) != NULL) &&
          ((o[1] = WlzAssignObject(
		   WlzMakeMain(obj1->type, obj1->domain, nullValues,
	  		       NULL, NULL, &errNum), NULL)) != NULL))
  {
    errNum = WlzRCCMakeC(c, o, t, 0);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if(WlzIsEmpty(c[0], NULL))
    {
      errNum = WlzRCCMakeC(c, o, t, 1);
      if(errNum == WLZ_ERR_NONE)
      {
	if(WlzIsEmpty(c[1], NULL))
	{
	  errNum = WlzRCCMakeC(c, o, t, 7);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    if(WlzIsEmpty(c[7], NULL))
	    {
	      cls = WLZ_RCC_NTSURI;
	    }
	    else
	    {
	      errNum = WlzRCCMakeC(c, o, t, 8);
	      if(errNum == WLZ_ERR_NONE)
	      {
		if(WlzIsEmpty(c[8], NULL))
		{
		  cls = WLZ_RCC_NTSUR;
		}
		else
		{
		  cls = WLZ_RCC_DC;
		}
	      }
	    }
	  }
	}
	else
	{
	  errNum = WlzRCCMakeC(c, o, t, 7);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    if(WlzIsEmpty(c[7], NULL))
	    {
	      cls = WLZ_RCC_TSURI;
	    }
	    else
	    {
	      errNum = WlzRCCMakeC(c, o, t, 8);
	      if(errNum == WLZ_ERR_NONE)
	      {
		if(WlzIsEmpty(c[8], NULL))
		{
		  cls = WLZ_RCC_TSUR;
		}
		else
		{
		  cls = WLZ_RCC_EC;
		}
	      }
	    }
	  }
	}
      }
    }
    else
    {
      errNum = WlzRCCMakeC(c, o, t, 2);
      if(errNum == WLZ_ERR_NONE)
      {
        if(WlzIsEmpty(c[2], NULL))
	{
	  cls = WLZ_RCC_EQ;
	}
	else
	{
	  errNum = WlzRCCMakeC(c, o, t, 3);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    if(WlzIsEmpty(c[3], NULL))
	    {
	      errNum = WlzRCCMakeC(c, o, t, 4);
	      if(errNum == WLZ_ERR_NONE)
	      {
		if(WlzIsEmpty(c[4], NULL))
		{
		  cls = WLZ_RCC_NTPP;
		}
		else
		{
		  cls = WLZ_RCC_TPP;
		}
	      }
	    }
	    else
	    {
	      errNum = WlzRCCMakeC(c, o, t, 5);
	      if(errNum == WLZ_ERR_NONE)
	      {
		if(WlzIsEmpty(c[5], NULL))
		{
		  errNum = WlzRCCMakeC(c, o, t, 6);
		  if(errNum == WLZ_ERR_NONE)
		  {
		    if(WlzIsEmpty(c[6], NULL))
		    {
		      cls = WLZ_RCC_NTPPI;
		    }
		    else
		    {
		      cls = WLZ_RCC_TPPI;
		    }
		  }
		}
		else
		{
		  cls = WLZ_RCC_PO;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  /* If enclosure is required check for it and add to classification mask. */
  if((errNum == WLZ_ERR_NONE) && (noEnc == 0) &&
     ((cls &
       (WLZ_RCC_EQ |
        WLZ_RCC_TSUR | WLZ_RCC_TSURI |
	WLZ_RCC_NTSUR | WLZ_RCC_NTSURI)) == 0))
  {
    for(i = 0; i <= 1; ++i)
    {
      errNum = WlzRCCMakeT(t, o,
                           (i == 0)? WLZ_RCCTOIDX_O0O1CI: WLZ_RCCTOIDX_O0CO1I);
      if(errNum == WLZ_ERR_NONE)
      {
        u[i] = WlzVolume(o[i], &errNum);
      }
      if(errNum == WLZ_ERR_NONE)
      {
        v[i] = WlzVolume(t[(i == 0)?
	                 WLZ_RCCTOIDX_O0O1CI: WLZ_RCCTOIDX_O0CO1I],
			 &errNum);
	if((errNum == WLZ_ERR_NONE) && (v[i] < 0))
	{
	  errNum = WLZ_ERR_DOMAIN_DATA;
	}
      }
      if(errNum != WLZ_ERR_NONE)
      {
        break;
      }
    }
    if(errNum == WLZ_ERR_NONE)
    {
      if((2 * v[0]) >= u[0])
      {
        cls |= WLZ_RCC_ENC;
      }
      if((2 * v[1]) >= u[1])
      {
        cls |= WLZ_RCC_ENCI;
      }
    }
  }
  /* Compute the maximum normalized volume for the classification(s) in the
   * classification mask. */
  if((errNum == WLZ_ERR_NONE) && (dstStatAry != NULL))
  {
    int 	i,
    		m;

    for(i = 0; i < WLZ_RCCIDX_CNT; ++i)
    {
      m = 1<<i;
      if(m & cls)
      {
	double	s = 0.0;

        switch(m)
	{
	  case WLZ_RCC_EQ:
	    s = 1.0;
	    break;
	  case WLZ_RCC_PO:
	    /* |\Omega_0 \cap \Omega_1| / |\Omega_0 \cup \Omega_1|  =
	     * u_0 / u_01 */
	    if(i01 <= 0)
	    {
	      errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1I);
	      if(errNum == WLZ_ERR_NONE)
	      {
	        i01 = WlzVolume(t[WLZ_RCCTOIDX_O0O1I], &errNum);
	      }
	    }
	    if((errNum == WLZ_ERR_NONE) && (u01 <= 0))
	    {
	      errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1U);
	      if(errNum == WLZ_ERR_NONE)
	      {
	        u01 = WlzVolume(t[WLZ_RCCTOIDX_O0O1U], &errNum);
	      }
	    }
	    if(errNum == WLZ_ERR_NONE)
	    {
	      s = (double )(i01) / (double )u01;
            }
	    break;
	  case WLZ_RCC_TSUR: /* FALLTHROUGH */
	  case WLZ_RCC_NTSUR: /* FALLTHROUGH */
	  case WLZ_RCC_TPP: /* FALLTHROUGH */
	  case WLZ_RCC_NTPP:
	    /* |\Omega_0| / |\Omega_0 \cup \Omega_1|  =
	     * u_0 / u_01 */
	    if(u[0] <= 0)
	    {
	      u[0] = WlzVolume(o[0], &errNum);
	    }
	    if(errNum == WLZ_ERR_NONE)
	    {
	      if(u01 <= 0)
	      {
	        errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1U);
		if(errNum == WLZ_ERR_NONE)
		{
		  u01 = WlzVolume(t[WLZ_RCCTOIDX_O0O1U], &errNum);
		}
	      }
	    }
	    if(errNum == WLZ_ERR_NONE)
	    {
	      s = (double )(u[0]) / (double )u01;
	    }
	    break;
	  case WLZ_RCC_TSURI: /* FALLTHROUGH */
	  case WLZ_RCC_NTSURI: /* FALLTHROUGH */
	  case WLZ_RCC_TPPI: /* FALLTHROUGH */
	  case WLZ_RCC_NTPPI:
	    /* |\Omega_1| / |\Omega_0 \cup \Omega_1|  =
	     * u_1 / u_01 */
	    if(u[1] <= 0)
	    {
	      u[1] = WlzVolume(o[1], &errNum);
	    }
	    if(errNum == WLZ_ERR_NONE)
	    {
	      if(u01 <= 0)
	      {
		errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1U);
		if(errNum == WLZ_ERR_NONE)
		{
		  u01 = WlzVolume(t[WLZ_RCCTOIDX_O0O1U], &errNum);
		}
	      }
	    }
	    if(errNum == WLZ_ERR_NONE)
	    {
	      s = (double )(u[1]) / (double )u01;
	    }
	    break;
	  case WLZ_RCC_ENC:
	    /* |\Omega_0 \cup \Omega_1^{\circ}|/|\Omega_0| =
	     * v_0 / u_0 */
	    if(u[1] >= 0)
	    {
	      s = (double )(v[0]) / (double )(u[0]);
	    }
	    break;
	  case WLZ_RCC_ENCI:
	    /* |\Omega_0^{\circ} \cup \Omega_1|/|\Omega_1| =
	     * v_1 / u_1 */
	    if(v[1] >= 0)
	    {
	      s = (double )(v[1]) / (double )(u[1]);
	    }
	    break;
	  default:
	    break;
	}
        if(errNum == WLZ_ERR_NONE)
	{
	  stats[i] = s;
	}
      }
    }
  }
  /* If adjacency is required check for it and add to both the classification
   * mask and statistics. */
  if((errNum == WLZ_ERR_NONE) && (noAdj == 0) &&
     ((cls & WLZ_RCC_EQ) == 0))
  {
    int		i;
    double	adjMean,
		adjSD;

    for(i = 0; (i < 2) && (errNum == WLZ_ERR_NONE); ++i)
    {
      int	c,
      		x;
      WlzObject	*o0,
      		*o1;

      if(i == 0)
      {
        o0 = obj0;
	o1 = obj1;
	c = WLZ_RCC_ADJ;
	x = WLZ_RCCIDX_ADJ;
      }
      else
      {
        o0 = obj1;
	o1 = obj0;
	c = WLZ_RCC_ADJI;
	x = WLZ_RCCIDX_ADJI;
      }
      errNum = WlzRCCAdjacency(o0, o1, maxAdjDist, &adjMean, &adjSD);
      if(errNum == WLZ_ERR_NONE)
      {
	adjMean = adjMean / (double)maxAdjDist;
	adjSD = adjSD / (double)maxAdjDist;
#define WLZ_RCC_DEBUG_ADJ
#ifdef WLZ_RCC_DEBUG_ADJ
        (void )fprintf(stderr,
	               "WLZ_RCC_DEBUG_ADJ %d %g %g\n", i, adjMean, adjSD);
#endif
	if((adjMean < 0.7) && (adjSD < 0.05))
	{
	  cls |= c;
	  stats[x] = (1.0 - adjSD) * (1.0 - adjSD);

	}
      }
    }
  }
  /* Free objects. */
  for(i = 0; i < WLZ_RCCTOIDX_CNT; ++i)
  {
    (void )WlzFreeObj(t[i]);
  }
  for(i = 0; i <= 8; ++i)
  {
    (void )WlzFreeObj(c[i]);
  }
  for(i = 0; i < 2; ++i)
  {
    (void )WlzFreeObj(o[i]);
  }
  if((errNum == WLZ_ERR_NONE) && (dstStatAry != NULL))
  {
    if((*dstStatAry = (double *)
                        AlcMalloc(sizeof(double) * WLZ_RCCIDX_CNT)) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    else
    {
      (void )memcpy(*dstStatAry, stats, sizeof(double) * WLZ_RCCIDX_CNT);
      if(dstStatCnt)
      {
        *dstStatCnt = WLZ_RCCIDX_CNT;
      }
    }
  }
  if(dstErr)
  {
    *dstErr = errNum;
  }
  return(cls);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzBinaryOps
* \brief	Test to see if the classification object \f$c_i\f$ as in
* 		WlzRegConCalcRCC() has been computed and if not do so.
* \param	c			Array c as in WlzRegConCalcRCC().
* \param	o			Array of objects, o[0] and o[1].
* \param	t			Array of temporary objects as in
* 					WlzRCCTOIdx.
* \param	i			Index for the classification object.
*/
static WlzErrorNum  		WlzRCCMakeC(
				  WlzObject **c,
				  WlzObject **o,
				  WlzObject **t,
				int i)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if((i >= 0) && (i <= 8))
  {
    if(c[i] == NULL)
    {
      switch(i)
      {
	case 0: /* \Omega_0 \cap \Omega_1 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1I);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(t[WLZ_RCCTOIDX_O0O1I], NULL);
	  }
	  break;
	case 1: /* \Omega_1^+ \cap \Omega_1 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0D);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzIntersect2(t[WLZ_RCCTOIDX_O0D], o[1], &errNum), NULL);
	  }
	  break;
	case 2: /* \Omega_0 \oplus \Omega_1 */
	  c[i] = WlzAssignObject(
		 WlzXORDom(o[0], o[1], &errNum), NULL);
	  break;
	case 3: /* (\Omega_0 \cup \Omega_1) \oplus \Omega_1 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1U);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzXORDom(t[WLZ_RCCTOIDX_O0O1U],
			     o[1], &errNum), NULL);
	  }
	  break;
	case 4: /* (\Omega_0^+ \cup \Omega_1) \oplus \Omega_1 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0DO1U);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzXORDom(t[WLZ_RCCTOIDX_O0DO1U], o[1], &errNum), NULL);
	  }
	  break;
	case 5: /* (\Omega_0 \cup \Omega_1) \oplus \Omega_0 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1U);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzXORDom(o[0], t[WLZ_RCCTOIDX_O0O1U], &errNum), NULL);
	  }
	  break;
	case 6: /* (\Omega_0 \cup \Omega_1^+) \oplus \Omega_0 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1DU);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzXORDom(o[0], t[WLZ_RCCTOIDX_O0O1DU], &errNum), NULL);
	  }
	  break;
	case 7:/*(\Omega_0^{\bullet} \cup \Omega_1) \oplus \Omega_0^{\bullet}*/
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0F);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0FO1U);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzXORDom(t[WLZ_RCCTOIDX_O0FO1U], t[WLZ_RCCTOIDX_O0F],
			     &errNum), NULL);
	  }
	  break;
	case 8:/*(\Omega_0 \cup \Omega_1^{\bullet}) \oplus \Omega_1^{\bullet}*/
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O1F);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0O1FU);
	  }
	  if(errNum == WLZ_ERR_NONE)
	  {
	    c[i] = WlzAssignObject(
		   WlzXORDom(t[WLZ_RCCTOIDX_O0O1FU], t[WLZ_RCCTOIDX_O1F],
			     &errNum), NULL);
	  }
	  break;
	default:
	  errNum = WLZ_ERR_PARAM_DATA;
	  break;
      }
    }
  }
  else
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzBinaryOps
* \brief	Make temporary objects as in WlzRCCTOIdx for
* 		WlzRegConCalcRCC().
* \param	t			Array of temporary objects.
* \param	o			Array of objects, o[0] and o[1].
* \param	i			Temporary object index.
*/
static WlzErrorNum 		WlzRCCMakeT(
				  WlzObject **t,
				  WlzObject **o,
				  WlzRCCTOIdx i)
{
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if((i >= 0) && (i < WLZ_RCCTOIDX_CNT))
  {
    if(t[i] == NULL)
    {
      switch(i)
      {
	case WLZ_RCCTOIDX_O0O1U:			/* o_0 \cup \o_1 */
	  t[i] = WlzAssignObject(
		 WlzUnion2(o[0], o[1], &errNum), NULL);
	  break;
	case WLZ_RCCTOIDX_O0O1I:			/* o_0 \cap \o_1 */
	  t[i] = WlzAssignObject(
		 WlzIntersect2(o[0], o[1], &errNum), NULL);
	  break;
	case WLZ_RCCTOIDX_O0D:			/* o_0^+ */
	  t[i] = WlzAssignObject(
		 WlzDilation(o[0],
			     (o[0]->type == WLZ_2D_DOMAINOBJ)?
			     WLZ_8_CONNECTED: WLZ_26_CONNECTED,
			     &errNum), NULL);
	  break;
	case WLZ_RCCTOIDX_O1D:			/* o_1^+ */
	  t[i] = WlzAssignObject(
		 WlzDilation(o[1],
			     (o[1]->type == WLZ_2D_DOMAINOBJ)?
			     WLZ_8_CONNECTED: WLZ_26_CONNECTED,
			     &errNum), NULL);
	  break;

	case WLZ_RCCTOIDX_O0F:			/* o_0^{\bullet} */
	  t[i] = WlzAssignObject(
		 WlzDomainFill(o[0], &errNum), NULL);
	  break;
	case WLZ_RCCTOIDX_O1F:			/* o_1^{\bullet} */
	  t[i] = WlzAssignObject(
		 WlzDomainFill(o[1], &errNum), NULL);
	  break;

	case WLZ_RCCTOIDX_O0DO1U:		/* o_0^+ \cup o_1 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0D);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    t[i] = WlzAssignObject(
		   WlzUnion2(t[WLZ_RCCTOIDX_O0D], o[1], &errNum), NULL);
	  }
	  break;
	case WLZ_RCCTOIDX_O0O1DU:		/* o_0   \cup o_1^+ */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O1D);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    t[i] = WlzAssignObject(
		   WlzUnion2(o[0], t[WLZ_RCCTOIDX_O1D], &errNum), NULL);
	  }
	  break;
	case WLZ_RCCTOIDX_O0CO1I:               /* o_0^{\circ} \cap o_1   */
	case WLZ_RCCTOIDX_O0O1CI:  /* FALLTHROUGH  o_0   \cap o_1^{\circ} */
	  {
	    int		i0;
	    WlzObject	*c = NULL,
			  *x = NULL;

	    i0 = (i == WLZ_RCCTOIDX_O0CO1I)? 0: 1;
	    c = WlzObjToConvexHull(o[i0], &errNum);
	    if((errNum == WLZ_ERR_NONE) || (errNum == WLZ_ERR_DEGENERATE))
	    {
	      x = WlzAssignObject(
		  WlzConvexHullToObj(c, o[i0]->type, &errNum), NULL);
	    }
	    if(errNum == WLZ_ERR_NONE)
	    {
	      t[i] = WlzAssignObject(
		     WlzIntersect2(o[!i0], x, &errNum), NULL);
	    }
	    (void )WlzFreeObj(c);
	    (void )WlzFreeObj(x);
	  }
	  break;
	case WLZ_RCCTOIDX_O0FO1U:		/* o_0^{\bullet} \cup o_1 */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O0F);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    t[i] = WlzAssignObject(
		   WlzUnion2(t[WLZ_RCCTOIDX_O0F], o[1], &errNum), NULL);
	  }
	  break;
	case WLZ_RCCTOIDX_O0O1FU:		/* o_0 \cup o_1^{\bullet} */
	  errNum = WlzRCCMakeT(t, o, WLZ_RCCTOIDX_O1F);
	  if(errNum == WLZ_ERR_NONE)
	  {
	    t[i] = WlzAssignObject(
		   WlzUnion2(o[0], t[WLZ_RCCTOIDX_O1F], &errNum), NULL);
	  }
	  break;
	default:
	  errNum = WLZ_ERR_PARAM_DATA;
	  break;
      }
    }
  }
  else
  {
    errNum = WLZ_ERR_PARAM_DATA;
  }
  return(errNum);
}

/*!
* \return	Woolz error code.
* \ingroup	WlzBinaryOps
* \brief	Computes measures of adjacency within a given distance range.
* 		The distances of all boundary vertices of obj0 from obj1
* 		are found, The mean and standard deviation
* 		(\f$\sqrt(\sigma)\f$) of the lowest 50% of these
* 		distance (clamped at maxDist) are then the adjacency
* 		adjacency statistics returned.
* \param	obj0			First given spatial domain object,
* 					must not be NULL.
* \param	obj1			Second given spatial domain object,
* 					must not be NULL.
* \param	maxDist			Maximum distance for adjacency. This
* 					is used to compute a distance object,
* 					large distances will significantly
* 					increase the processing time.
* \param	dMean			Destination pointer for mean
* 					adjacency distance, must not be NULL.
* \param	dSD			Destination pointer for standard
* 					deviation in adjacency distance,
* 					must not be NULL.
*/
static WlzErrorNum		WlzRCCAdjacency(
				  WlzObject *obj0,
				  WlzObject *obj1,
				  int maxDist,
				  double *dMean,
				  double *dSD)
{
  int		*dHist = NULL;
  WlzObject	*dObj = NULL;
  WlzErrorNum	errNum = WLZ_ERR_NONE;

  if(((obj0->type == WLZ_2D_DOMAINOBJ) && (obj1->type == WLZ_2D_DOMAINOBJ)) ||
     ((obj0->type == WLZ_3D_DOMAINOBJ) && (obj1->type == WLZ_3D_DOMAINOBJ)))
  {
    if((obj0->domain.core == NULL) || (obj1->domain.core == NULL))
    {
      errNum = WLZ_ERR_DOMAIN_NULL;
    }
  }
  else
  {
    errNum = WLZ_ERR_OBJECT_TYPE;
  }
  if(errNum == WLZ_ERR_NONE)
  {
    WlzObject	*rObj = NULL,
    		*sObj = NULL;

    /* Compute distance object surrounding obj1 (out to given maximum
     * distance, background value of maximum distance and value within
     * obj1's domain of zero. */
    sObj = WlzAssignObject(
	   WlzMakeSphereObject(obj1->type, maxDist, 0, 0, 0, &errNum), NULL);
    if(errNum == WLZ_ERR_NONE)
    {
      rObj = WlzAssignObject(
	     WlzStructDilation(obj1, sObj, &errNum), NULL);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      dObj = WlzAssignObject(
	     WlzDistanceTransform(rObj, obj1, WLZ_OCTAGONAL_DISTANCE, 0.0,
	     			  maxDist, &errNum), NULL);
    }
    if(errNum == WLZ_ERR_NONE)
    {
      WlzPixelV bgdV;

      bgdV.type = WLZ_GREY_INT;
      bgdV.v.inv = maxDist;
      errNum = WlzSetBackground(dObj, bgdV);
    }
    (void )WlzFreeObj(rObj);
    (void )WlzFreeObj(sObj);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    if((dHist = (int *)AlcCalloc(maxDist + 1, sizeof(int))) == NULL)
    {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
  }
  if(errNum == WLZ_ERR_NONE)
  {
    /* Compute the quantised distance histogram in which distances from
     * all obj0's pixels/voxels to obj1 are quantized to integer values.
     * Do this using the distance object. */
    if(dObj->type == WLZ_2D_DOMAINOBJ)
    {
      errNum = WlzRCCCompDistHist2D(maxDist, dHist, dObj, obj0);
    }
    else
    {
      errNum = WlzRCCCompDistHist2D(maxDist, dHist, dObj, obj0);
    }
  }
  WlzFreeObj(dObj);
  if(errNum == WLZ_ERR_NONE)
  {
    /* Compute the adjacency statistics (mean and standard
     * deviation) on the closest half of the distances. */
    int		i,
    		n,
		n2;
    double	mean = 0.0,
		sum = 0.0,
		sumSq = 0.0,
		sd = 0.0;

    n = 0;
    for(i = 0; i <= maxDist; ++i)
    {
      n += dHist[i];
    }
    n2 = n / 2;
    i = 0;
    n = 0;
    while((i <= maxDist) && ((n + dHist[i]) < n2))
    {
      double	d;

      n += dHist[i];
      d = (double )i;
      sum   += d * (double )(dHist[i]);
      sumSq += d * d * (double )(dHist[i]);
      ++i;
    }
    if(n < n2)
    {
      double 	d;

      d = (double )i;
      sum   += d * (double )(n2 - n);
      sumSq += d * d * (double )(n2 - n);
    }
    if(n2 > 0)
    {
      mean = sum / (double )n2;
      if(n2 > 1)
      {
        double	var;

        var = (sumSq - (sum * sum / (double )n2)) / ((double )n2 - 1.0);
        sd = sqrt(var);
      }
    }
    *dMean = mean;
    *dSD = sd;
  }
  AlcFree(dHist);
  return(errNum);
}

static WlzErrorNum	WlzRCCCompDistHist2D(
			  int maxDist,
			  int *dHist,
			  WlzObject *dObj,
			  WlzObject *tObj)
{
  WlzIntervalWSpace iWsp;
  WlzGreyValueWSpace *gVWSp = NULL;
  WlzErrorNum   errNum = WLZ_ERR_NONE;

  gVWSp = WlzGreyValueMakeWSp(dObj, &errNum);
  if(errNum == WLZ_ERR_NONE)
  {
    errNum = WlzInitRasterScan(tObj, &iWsp, WLZ_RASTERDIR_ILIC);
  }
  if(errNum == WLZ_ERR_NONE)
  {
    while((errNum = WlzNextInterval(&iWsp)) == WLZ_ERR_NONE)
    {
      WlzIVertex2 p;

      p.vtY = iWsp.linpos;
      for(p.vtX = iWsp.lftpos; p.vtX <= iWsp.lftpos; ++(p.vtX))
      {
	int	d;

	WlzGreyValueGet(gVWSp, 0, p.vtY, p.vtX);
	d = gVWSp->gVal[0].inv;
	d = WLZ_CLAMP(d, 0, maxDist);
	++(dHist[d]);
      }
    }
    if(errNum == WLZ_ERR_EOO)
    {
      errNum = WLZ_ERR_NONE;
    }
  }
  return(errNum);
}

static WlzErrorNum	WlzRCCCompDistHist3D(
			  int maxDist,
			  int *dHist,
			  WlzObject *dObj,
			  WlzObject *tobj)
{
  WlzErrorNum   errNum = WLZ_ERR_NONE;

if(1) /* HACK */
{
FILE	*fP;
fP = fopen("debug.wlz", "w");
(void )WlzWriteObj(fP, dObj);
(void )fclose(fP);
}
  /* HACK TODO */
  return(errNum);
}
