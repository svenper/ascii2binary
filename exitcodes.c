#include <stdio.h>
#ifdef INTLIZE
#include <locale.h>
#include <libintl.h>
#else
#define gettext(x) (x)
#endif

void
Print_ExitCodes(void)
{
  fprintf(stderr,gettext("The codes returned on exit are:\n"));
  fprintf(stderr,gettext("\t0 Success\n"));
  fprintf(stderr,gettext("\t1 Info\n"));
  fprintf(stderr,gettext("\t2 System Error\n"));
  fprintf(stderr,gettext("\t3 Command Line Error\n"));
  fprintf(stderr,gettext("\t4 Range Error\n"));
  fprintf(stderr,gettext("\t5 Input Error\n"));
}
