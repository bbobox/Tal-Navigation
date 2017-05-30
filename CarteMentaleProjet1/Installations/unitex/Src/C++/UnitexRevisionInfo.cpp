/*
 * Unitex
 *
 * Copyright (C) 2001-2012 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 */

/*
 * File created and contributed by Gilles Vollant (Ergonotics SAS) 
 * as part of an UNITEX optimization and reliability effort
 *
 * additional information: http://www.ergonotics.com/unitex-contribution/
 * contact : unitex-contribution@ergonotics.com
 *
 */


#include "Unicode.h"
#include "Copyright.h"
#include "UnitexRevisionInfo.h"


#if (!defined(SVN_REVISION))
#include "Unitex_revision.h"
#define SVN_REVISION UNITEX_REVISION
#endif

#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

namespace unitex {

int get_unitex_revision()
{
#ifdef SVN_REVISION
return (int)SVN_REVISION;
#else
return (int)-1;
#endif
}


UNITEX_FUNC int UNITEX_CALL GetUnitexRevision()
{
	return get_unitex_revision();
}

void get_unitex_version(unsigned int* major_version_number, unsigned int* minor_version_number)
{
// macro UNITEX_MAJOR_VERSION_NUMBER and UNITEX_MINOR_VERSION_NUMBER were introduced
//   in Unitex 3.0. Unitex 2.1 is the only version compatible with UnitexRevisionInfo without version
#ifdef UNITEX_MAJOR_VERSION_NUMBER
unsigned int unitexMajorVersion = UNITEX_MAJOR_VERSION_NUMBER;
#else
unsigned int unitexMajorVersion = 2;
#endif

#ifdef UNITEX_MINOR_VERSION_NUMBER
unsigned int unitexMinorVersion = UNITEX_MINOR_VERSION_NUMBER;
#else
unsigned int unitexMinorVersion = 1;
#endif

	if (major_version_number != NULL) {
		*major_version_number = unitexMajorVersion;
	}

	if (minor_version_number != NULL) {
		*minor_version_number = unitexMinorVersion;
	}
}


UNITEX_FUNC void UNITEX_CALL GetUnitexVersion(unsigned int* major_version_number, unsigned int* minor_version_number)
{
	get_unitex_version(major_version_number, minor_version_number);
}

} // namespace unitex
