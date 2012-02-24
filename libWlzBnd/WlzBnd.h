#ifndef WLZBND_H
#define WLZBND_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _WlzBnd_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         libWlzBnd/WlzBnd.h
* \author       Bill Hill
* \date         August 2003
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
* \brief	Main header for the Woolz binding which includes all other
* 		header files required by the Woolz binding library.
* \ingroup	LibWlzBnd
*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <math.h>

#include <Alc.h>
#include <Alg.h>
#include <Wlz.h>
#include <bibFile.h>
#include <WlzExtFF.h>
#include <WlzBndType.h>
#include <WlzBndProto.h>

#ifdef __cplusplus
}
#endif

/*!
* \defgroup    LibWlzBnd	LibWlzBnd
*/

#endif	/* !WLZBND_H Don't put anything after this line */
