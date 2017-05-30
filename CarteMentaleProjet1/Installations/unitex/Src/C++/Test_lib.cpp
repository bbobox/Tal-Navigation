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

#include <stdio.h>
#include <stdlib.h>
#include "IOBuffer.h"
#include "Unicode.h"
#include "Reg2Grf.h"
#include "Locate.h"
#include "ProgramInvoker.h"
#include "tre.h"
#include "UnusedParameter.h"
#include "UnitexTool.h"
#include "logger/UniLogger.h"
#include "logger/UniRunLogger.h"

#ifdef HAS_UNITEX_NAMESPACE
using namespace unitex;
#endif

#ifdef HAS_LOGGER_NAMESPACE
using namespace logger;
#endif

/**
 * This program is an example of compilation using the unitex library (unitex.dll/libunitex.so).
 * It prints the .grf file corresponding to "a+(b.c)".
 */
int main(int argc,char *argv[]) {
setBufferMode();
if (argc>1) {
  if (strcmp(argv[1],"RunLog")==0) {
    return RunLog_run_main(argc-1,argv+1);
  }
  if (strcmp(argv[1],"UnitexTool")==0) {
    return UnitexTool_public_run(argc-1,argv+1,NULL,NULL);
  }
}

const char* name="biniou";
U_FILE* f=u_fopen(UTF16_LE,"biniou",U_WRITE);
if (f==NULL) {
   fatal_error("Cannot open %s\n",name);
}
u_fprintf(f,"a+(b.c)");
u_fclose(f);
const char* grf="regexp.grf";
ProgramInvoker* invoker=new_ProgramInvoker(main_Reg2Grf,"main_Reg2Grf");
add_argument(invoker,name);
int ret=invoke(invoker);
free_ProgramInvoker(invoker);
u_printf("Reg2Grf exit code: %d\n\n",ret);

f=u_fopen(UTF16_LE,grf,U_READ);
int c;
while ((c=u_fgetc(f))!=-1) {
   u_printf("%C",c);
}
u_fclose(f);
af_remove(name);
af_remove(grf);

/* These lines are just here to test if the TRE library was correctly linked. */
#ifdef TRE_WCHAR_TYPE_DEFINED
tre_wchar_t warray[512];
#else
wchar_t warray[512];
#endif
regex_t matcher;
tre_regwcomp(&matcher,warray,REG_NOSUB);
return 0;
}




