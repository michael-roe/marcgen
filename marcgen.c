#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>

/*
 * Generate a simple MARC record.
 *
 * The record generated will be a template that the user can edit later to
 * add the details of the catalog entry.
 *
 * The output is not actually in MARC format. It is in the "plain text MARC"
 * format used by MARCEdit. MARCEdit can be used to turn the output of this
 * program into MARC format.
 *
 */
 
int main(int argc, char **argv)
{
struct timeval tv;
struct tm *timeptr;
time_t now;

  setlocale(LC_ALL, getenv("LANG"));

  /*
   * Leader
   */

  wprintf(L"=LDR\n");

  /*
   * 001 Control Number
   *
   * This is the library catalog number for the book.
   *
   */

  wprintf(L"=001  123456\n");

  /*
   * 005 Control Number Identifier
   *
   * This is who allocated the control number.
   *
   * Example values:
   *
   *   DLC     - Library of Congress
   *   OCoLC   - Online Computer Library Center (Worldcat)
   *   MiAaHDL - HATHI Trust
   */

  wprintf(L"=003  DLC\n");

  /*
   * 005 Data and Time of Latest Transaction
   */

  now = time(0);
  timeptr = gmtime(&now);
  wprintf(L"=005  %d%02d%02d%02d%02d%02d.0\n",
    timeptr->tm_year + 1900,
    timeptr -> tm_mon,
    timeptr->tm_mday,
    timeptr->tm_hour,
    timeptr->tm_min,
    timeptr->tm_sec);

  /*
   * 008 - Fixed-Length Data Elements
   */

  return 0;
}
