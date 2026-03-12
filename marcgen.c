/*-
 * Copyright (c) 2026 Michael Roe
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <string.h>

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
 
/* Different types of date that can be in an 008 field */

#define DATE_SINGLE 0
#define DATE_REPRINT 1

/*
 * Valid Genre and Form terms that correspond to flags in the 008 header
 *
 * Poetry
 * Drama
 * Fiction
 * Short stories
 * Autobiographies
 * Biographies
 * Festschriften
 * Conference papers and proceedings
 *
 * Some genres can be written is several different forms, in which case
 * the Genre and Form field might need to have two values, one for the genre
 * and one for the form.
 *
 * For example, the genre "Travel writing" includes:
 * Autobiographies (of explorers)
 * Diaries (of expeditions)
 * Essays (about travel)
 * Personal correspondence (letters from abroad)
 * Creative nonfiction
 */

#define GENRE_MAX 2
#define GENRE_MAX_LEN 80
static wchar_t lit_form[GENRE_MAX][GENRE_MAX_LEN];

static int genre_count = 0;

static int form_contains(wchar_t *s)
{
int i;

  for (i=0; i<genre_count; i++)
  {
    if (wcscmp(lit_form[i], s) == 0)
    {
      return 1;
    }
  }
  return 0;
}

int main(int argc, char **argv)
{
struct timeval tv;
struct tm *timeptr;
time_t now;
int d1;
int d2;
wchar_t date1[5];
wchar_t date2[5];
wchar_t lang[4];
wchar_t country[4];
wchar_t leader[25];
wchar_t fixed_fields[41];
int opt;
int date_type;
int i;

  setlocale(LC_ALL, getenv("LANG"));

  genre_count = 0;

  date_type = DATE_SINGLE;

  d1 = 1960;
  d2 = 0;
  wcpncpy(lang, L"eng", sizeof(lang)/sizeof(wchar_t) - 1);
  lang[3] = 0;

  wcpncpy(country, L"enk", sizeof(country)/sizeof(wchar_t) - 1);
  country[3] = 0;

  while ((opt = getopt(argc, argv, "d:f:l:p:rD:")) != -1)
  {
    switch (opt)
    {
      case 'd':
        d1 = strtol(optarg, NULL, 10);
        break;
      case 'f':
        if (genre_count < GENRE_MAX)
        {
          swprintf(lit_form[genre_count], 80, L"%s", optarg);
          genre_count++;
        }
        break;
      case 'l':
        if (strlen(optarg) > 3)
        {
          fprintf(stderr, "Language name must be at most 3 characters\n");
          return -1;
        }
        swprintf(lang, sizeof(lang)/sizeof(wchar_t), L"%s", optarg);
        lang[3] = 0;
        if (lang[2] == 0)
        {
          lang[2] = ' ';
        }
        break;
      case 'p':
        if (strlen(optarg) > 3)
        {
          fprintf(stderr, "Place name must be at most 3 characters\n");
          return -1;
        }
        swprintf(country, sizeof(country)/sizeof(wchar_t), L"%s", optarg);
        country[3] = 0;
        break;
      case 'r':
        date_type = DATE_REPRINT;
        break;
      case 'D':
        d2 = strtol(optarg, NULL, 10);
        break;
      default:
        break;
    }
  }

  swprintf(date1, 5, L"%04d", d1);
  date1[4] = 0;

  if (d2 == 0)
  {
    swprintf(date2, 5, L"    ");
  }
  else
  {
    swprintf(date2, 5, L"%04d", d2);
  }
  date2[4] = 0;

  /*
   * Leader
   */

  /*
   * 00-04 Record Length
   *
   * Put in zeros; MARCEdit will calculate the real length
   */

  leader[0] = '0';
  leader[1] = '0';
  leader[2] = '0';
  leader[3] = '0';
  leader[4] = '0';

  /*
   * 05 Record Status
   *
   * c means corrected
   * n means new
   */

  leader[5] = 'n';

  /*
   * 06 Type of Record
   *
   * a means language material
   */

  leader[6] = 'a';

  /*
   * 07 Bibliographic Level
   *
   * m means monograph, i.e. a book
   */

  leader[7] = 'm';

  /*
   * 08 Type of Control
   *
   * space means unspecified
   */

  leader[8] = ' ';

  /*
   * 09 Character Encoding Scheme
   *
   * a means Unicode
   */

  leader[9] = 'a';

  /*
   * 10 Indicator Count
   *
   * Must be 2
   */

  leader[10] = '2';

  /*
   * 11 Subfield Code Count
   *
   * Must be 2
   */

  leader[11] = '2';

  /*
   * 12-16 Base Address of Data
   */

  leader[12] = '0';
  leader[13] = '0';
  leader[14] = '0';
  leader[15] = '0';
  leader[16] = '0';

  /*
   * 17 Encoding Level
   *
   * space means full level
   */

  leader[17] = ' ';

  /*
   * 18 Descriptive Cataloging Form
   *
   * a means AACR 2
   */

  leader[18] = 'a';

  /*
   * 19 Multipart Resource Record Level
   *
   * space means not specified
   */

  leader[19] = ' ';

  /*
   * 20 Length of the Length of Field Portion
   *
   * Must be 4
   */

  leader[20] = '4';

  /*
   * 21 Length of the Starting Character Position
   *
   * Must be 5
   */

  leader[21] = '5';

  /*
   * 22 Length of the Implementation Defined Portion
   */

  leader[22] = '0';

  /*
   * 23 Undefined
   *
   * Must be 0
   */

  leader[23] = '0';
   
  leader[24] = 0;

  wprintf(L"=LDR  %ls\n", leader);

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

  wprintf(L"=003  OCoLC\n");

  /*
   * 005 Data and Time of Latest Transaction
   */

  now = time(0);
  timeptr = gmtime(&now);
  wprintf(L"=005  %d%02d%02d%02d%02d%02d.0\n",
    timeptr->tm_year + 1900,
    timeptr -> tm_mon + 1,
    timeptr->tm_mday,
    timeptr->tm_hour,
    timeptr->tm_min,
    timeptr->tm_sec);

  /*
   * 008 Fixed-Length Data Elements
   */

  /*
   * 008/00-05 Date of Creation
   */

  swprintf(fixed_fields, 7, L"%02d%02d%02d",
    timeptr->tm_year % 100,
    timeptr->tm_mon + 1,
    timeptr->tm_mday);

  /*
   * 008/06 Type of Publication Date
   */

  switch (date_type)
  {
    case DATE_SINGLE:
      fixed_fields[6] = 's';
      break;
    case DATE_REPRINT:
      fixed_fields[6] = 'r';
      break;
    default:
      fwprintf(stderr, L"Invalid date format\n");
      return -1;
  }

  /*
   * 008/07-10 Date 1
   */

  swprintf(fixed_fields + 7, 5, date1);

  /*
   * 008/11-14 Date 2
   */

  swprintf(fixed_fields + 11, 5, date2);

  /*
   * 008/15-17 Country of Publication
   *
   * MARC has its own country codes; these are not ISO country codes.
   *
   * Examples:
   *  England is enk
   */

  swprintf(fixed_fields + 15, 4, country);

  /*
   * Fields 18-34 depend on the type of media (e.g. book)
   */

  /*
   * 008/18-21 Illustrations
   */

  fixed_fields[18] = ' ';
  fixed_fields[19] = ' ';
  fixed_fields[20] = ' ';
  fixed_fields[21] = ' ';

  /*
   * 008/22 Target Audience
   *
   * Space means unspecified
   */

  fixed_fields[22] = ' ';

  /*
   * 008/23 Form of Item
   *
   * e.g. 'a' means microfilm
   * space means "none of the above", a regular book
   */

  fixed_fields[23] = ' ';

  /*
   * 008/24-27 Nature of Contents
   *
   * space means unspecified
   */

  fixed_fields[24] = ' ';
  fixed_fields[25] = ' ';
  fixed_fields[26] = ' ';
  fixed_fields[27] = ' ';

  /*
   * 008/28 Government Publication
   *
   * space means not a government publication
   */

  fixed_fields[28] = ' ';

  /*
   * 008/29 Conference Publication
   *
   * 0 means not a conference publication
   */

  fixed_fields[29] = '0';

  /*
   * 008/30 Festschrift
   *
   * 0 means not a festschrift
   */

  fixed_fields[30] = '0';

  /*
   * 008/31 Index
   *
   * 0 means does not have an index
   */

  fixed_fields[31] = '0';

  /*
   * 008/32 Undefined
   */

  fixed_fields[32] = ' ';

  /*
   * 008/33 Literary Form
   *
   * 0 means not fiction
   * 1 means fiction
   * p means poetry
   */

  if (form_contains(L"Poetry"))
  {
    fixed_fields[33] = 'p';
  }
  else
  {
    fixed_fields[33] = '0';
  }

  /*
   * 008/34 Biography
   *
   * space means no biographical material
   * a means autobiography
   * b means individual biography
   */

  if (form_contains(L"Autobiographies"))
  {
    fixed_fields[34] = 'a';
  }
  else if (form_contains(L"Biographies"))
  {
    fixed_fields[34] = 'b';
  }
  else
  {
    fixed_fields[34] = ' ';
  }

  /*
   * 008/35-37 Language
   */

  swprintf(fixed_fields + 35, 4, L"%ls", lang);

  /*
   * 008/38 Modified Record
   *
   * space means not modified
   */

  fixed_fields[38] = ' ';

  /*
   * 008/39 Cataloging Source
   *
   * d means 'other'
   */

  fixed_fields[39] = 'd';

  fixed_fields[40] = 0;

  wprintf(L"=008  %ls\n", fixed_fields);


  /*
   * 040 Cataloging Source
   *
   * Who created and transcribed this cataog record.
   *
   * Value can be a MARC organization code or an OCLC "Symbol".
   *
   * INARC is the OCLC Symbol for The Internet Archive.
   */

  wprintf(L"=040  \\\\$a%ls$c%ls\n", L"INARC", L"INARC");

  /*
   * 100 Main Entry -- Personal Name
   *
   * First indicator 1 denotes surname
   */

  wprintf(L"=100  1\\$aSmith, John\n");

  /*
   * 245 Title Statement
   *
   * First indicator is 1 for "added entry"
   */

  wprintf(L"=245  10$aTitle\n");

  /*
   * 260 Publication. Distribution
   */

  wprintf(L"=260  \\\\$aCity:$bPublisher,$c%d\n", d1);
  
  /*
   * 300 Physical Description
   */

  wprintf(L"=300  \\\\$a2 pages;$c23 cm.\n");

  /*
   * 655 Index Term -- Genre/Form
   */

  if (genre_count > 0)
  {
    for (i=0; i<genre_count; i++)
    {
      wprintf(L"=655  \\7$a%ls$2lcgft\n", lit_form[i]);
    }
  }

  return 0;
}
