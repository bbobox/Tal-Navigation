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

#ifndef PersistenceInterfaceH
#define PersistenceInterfaceH
#ifdef __cplusplus
#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

namespace unitex {
extern "C" {
#endif

	
int standard_load_persistence_dictionary(const char*filename,char* persistent_filename_buffer,size_t buffer_size);
void standard_unload_persistence_dictionary(const char*filename);

int standard_load_persistence_fst2(const char*filename,char* persistent_filename_buffer,size_t buffer_size);
void standard_unload_persistence_fst2(const char*filename);

int standard_load_persistence_alphabet(const char*filename,char* persistent_filename_buffer,size_t buffer_size);
void standard_unload_persistence_alphabet(const char*filename);

UNITEX_FUNC int UNITEX_CALL persistence_public_load_dictionary(const char*filename,char* persistent_filename_buffer,size_t buffer_size);
UNITEX_FUNC void UNITEX_CALL persistence_public_unload_dictionary(const char*filename);

UNITEX_FUNC int UNITEX_CALL persistence_public_load_fst2(const char*filename,char* persistent_filename_buffer,size_t buffer_size);
UNITEX_FUNC void UNITEX_CALL persistence_public_unload_fst2(const char*filename);

UNITEX_FUNC int UNITEX_CALL persistence_public_load_alphabet(const char*filename,char* persistent_filename_buffer,size_t buffer_size);
UNITEX_FUNC void UNITEX_CALL persistence_public_unload_alphabet(const char*filename);

#ifdef __cplusplus
} // extern "C"
} // namespace unitex
#endif


#endif