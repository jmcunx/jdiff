/*
 * Copyright (c) 2004 ... 2022 2023
 *     John McCue <jmccue@jmcunx.com>
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Sorted File Diff if Two Sorted Text Files*/

#define PROGNAME "jdiff"

#ifndef _MSDOS
#include <sys/param.h>
#endif

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

/* PATH_PAX is the one to use */
#ifdef MAXPATHLEN
#ifndef PATH_MAX
#define PATH_MAX MAXPATHLEN
#endif
#else
#ifdef PATH_MAX
#define MAXPATHLEN PATH_MAX
#endif
#endif

#define BUF_SIZE 256
#ifdef _MSDOS
#define MAX_ITERATION 999999
#else
#define MAX_ITERATION 29999999
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define NUM unsigned long long
#define SCKARG 80
#define SWITCH_CHAR '-'
#define FILE_STDIN  "-"

#define ARG_BUFF          'b'  /* Buffer Size                        */
#define ARG_HELP          'h'  /* Show Help                          */
#define ARG_MAX_SHOW      'm'  /* Max lines to print                 */
#define ARG_QUIET         'q'  /* do not show any output             */
#define ARG_VERBOSE       'v'  /* Verbose                            */
#define ARG_VERSION       'V'  /* Show Version Information           */

#define LIT_INFO_01  "\tBuild: %s %s\n"
#define LIT_PROG     "diff Two Sorted Text Files "

#define MSG_E000    "Try '%s %c%c' for more information\n"
#define MSG_E001    "E001: Need just One or Two Text Files for Comparison\n"
#define MSG_E002    "E002: cannot open %s for read\n    %s\n"
#define MSG_E003    "E003: cannot use stdin, already in use\n"
#define MSG_E004    "E004: File %s not sorted\n"
#define MSG_E005    "E005: Record %llu out of order\n"
#define MSG_E006    "E006: eek!  I am lost to the World\n"
#define MSG_E007    "E004: value %lld invalid value for %c%c\n"
#define MSG_E008    "E008: '%s' is an invalid value for %c%c\n"
#define MSG_E009    "E009: 'Too many Arguments specified for %c%c\n"
#define MSG_E010    "E010: '%lld' is an invalid value for %c%c\n"
#define MSG_E011    "E011: Cannot get System Time: %s\n"
#define MSG_E012    "E012: cannot allocate %ld characters of memory\n"

#define MSG_I091    "I091: Maximum Record Length Found: %-llu\n"
#define MSG_W092    "W092: Maximum Record Length Found: %-llu (some records truncated)\n"
#define MSG_I093    "I093: Seconds %7lld, Records Compared %15llu\n"
#define MSG_I094    "I094: %s: %llu: %s\n"
#define MSG_I095    "I095: %s: %llu: %s\n"  /* for entries if one file EOF */
#define MSG_I096    "I096: Differences Found:         : %-llu\n"
#define MSG_I097    "I097: Reads File %d               : %-llu\n"
#define MSG_I098    "I098: Differences Written        : %-llu\n"
#define MSG_I105    "I105: Verbose Level              : %-d\n"
#define MSG_I106    "I108: Text File Number %d:\n    %s\n"
#define MSG_I107    "I107: Max Record Length Used     : %-ld\n"

#define USG_MSG_USAGE_1          "usage:\t%s [OPTIONS]\n"
#define USG_MSG_OPTIONS          "Options\n"
#define USG_MSG_ARG_VERBOSE_8    "\t%c%c\t\t: verbose level, each time specified level increases\n"
#define USG_MSG_ARG_HELP         "\t%c%c\t\t: Show brief help and exit\n"
#define USG_MSG_ARG_QUIET        "\t%c%c\t\t: Do not print any output.\n"
#define USG_MSG_ARG_VERSION      "\t%c%c\t\t: Show revision information and exit\n"
#define USG_MSG_ARG_MAX_SHOW     "\t%c%c n\t\t: Maximum Number of Differences to print\n"
#define USG_MSG_ARG_BUFF         "\t%c%c n\t\t: Allocate Read Buffer of size n\n"


struct s_file
{
  FILE *fp;
  NUM reads;
  char fname[PATH_MAX];
  int  allow_close; /* TRUE/FALSE */
  int  at_eof;      /* TRUE/FALSE */
  char *buf;
  char *buf_last;
} ;

struct s_work
{
  int arg_verbose;
  int arg_quiet;
  NUM arg_show_max;
  size_t arg_buf_size;
  int stdin_assigned;
  NUM count_diff;
  NUM writes;
  NUM max_size_found;
  time_t seconds_start;
  struct s_file f1;
  struct s_file f2;
} ;
NUM rtw(char *buffer)

{
  char *last_non_space;
  char *b = buffer;

  if (buffer == (char *) NULL)
    return((NUM) 0); /* NULL pointer */

  last_non_space = buffer;

  for ( ; (*buffer) != (char) 0x00; buffer++)
    {
      if ( ! isspace((int)(*buffer)) )
	last_non_space = buffer;
    }

  if ( ! isspace ((int) *last_non_space) )
    last_non_space++;

  (*last_non_space) = (char) 0x00;

  return((NUM) strlen(b));

} /* rtw() */

/*
 * is_numr() -- determines if all characters are numeric
 */
int is_numr(char *s)

{
  if (s == (char *) NULL)
    return(FALSE);

  for ( ; (*s) != (char) 0; s++)
    {
      if ( ! isdigit((int)(*s)) )
	return(FALSE);
    }

  return(TRUE);

} /* is_numr() */

/*
 * show_info()
 */
void show_info(FILE *fp, int rmode)
{

  switch (rmode)
    {
    case 2:
      fprintf(fp,"%s\n", PROGNAME);
#ifdef OSTYPE
      fprintf(fp,"\t%s\n",OSTYPE);
#endif
      fprintf(fp, LIT_INFO_01, __DATE__, __TIME__);
      break;
    default:
      fprintf(fp, USG_MSG_USAGE_1, PROGNAME);
      fprintf(fp, "\t%s\n", LIT_PROG);
      fprintf(fp, USG_MSG_OPTIONS);
      fprintf(fp, USG_MSG_ARG_BUFF,         SWITCH_CHAR, ARG_BUFF);
      fprintf(fp, USG_MSG_ARG_HELP,         SWITCH_CHAR, ARG_HELP);
      fprintf(fp, USG_MSG_ARG_MAX_SHOW,     SWITCH_CHAR, ARG_MAX_SHOW);
      fprintf(fp, USG_MSG_ARG_QUIET,        SWITCH_CHAR, ARG_QUIET);
      fprintf(fp, USG_MSG_ARG_VERSION,      SWITCH_CHAR, ARG_VERSION);
      fprintf(fp, USG_MSG_ARG_VERBOSE_8,    SWITCH_CHAR, ARG_VERBOSE);
      break;
    }

  exit(2);

} /* show_info() */

/*
 * set_fname() -- save a file name, adjust for stdin
 */
void set_fname(char *dest, char *fname)
{

  if (strcmp(fname, FILE_STDIN) == 0)
    return; /* assume stdin */

  strncpy(dest, fname, (PATH_MAX - 1));

} /* set_fname() */

/*
 * init_file() -- clear s_file
 */
void init_file(struct s_file *f)

{
  memset(f->fname, 0, PATH_MAX);
  f->fp          = (FILE *) NULL;
  f->reads       = (NUM) 0;
  f->allow_close = FALSE;
  f->buf         = (char *) NULL;
  f->buf_last    = (char *) NULL;

} /* init_file() */

/*
 * get_ll() -- process long long argument
 */
long long get_ll(char *argval, char arg, long long current_val)
{
  long long val = 0LL;

  if (current_val != 0LL)
    {
      fprintf(stderr, MSG_E009, SWITCH_CHAR, arg);
      fprintf(stderr, MSG_E000, PROGNAME, SWITCH_CHAR, ARG_HELP);
      exit(2);
    }

  if (is_numr(argval) != TRUE)
    {
      fprintf(stderr, MSG_E008, argval,   SWITCH_CHAR, arg);
      fprintf(stderr, MSG_E000, PROGNAME, SWITCH_CHAR, ARG_HELP);
      exit(2);
    }

  val = atoll(argval);

  if (val < 0LL)
    {
      fprintf(stderr, MSG_E007, val, SWITCH_CHAR, arg);
      fprintf(stderr, MSG_E000,  PROGNAME, SWITCH_CHAR, ARG_HELP);
      exit(2);
    }

  return(val);

} /* get_ll() */

/*
 * init_work() -- process arguments
 */
void init_work(struct s_work *w)
{
  w->arg_verbose    = 0;
  w->arg_quiet      = FALSE;
  w->stdin_assigned = FALSE;
  w->arg_show_max   = (NUM) 2000;
  w->count_diff     = (NUM) 0;
  w->writes         = (NUM) 0;
  w->max_size_found = (NUM) 0;
  w->arg_buf_size   = (size_t) BUF_SIZE;

  w->seconds_start  = time((time_t *) NULL);
  if (w->seconds_start < 0)
    {
      fprintf(stderr, MSG_E011, strerror(errno));
      exit(2);
    }

  init_file(&(w->f1));
  init_file(&(w->f2));

} /* init_work() */

/*
 * process_arg() -- process arguments
 */
void process_arg(int argc, char **argv, struct s_work *w)

{
  char ckarg[SCKARG];
  int opt;
  int i           = 0;
  int num_files   = 0;
  int set_max     = FALSE;
  int set_bsize   = FALSE;
  long long max   = 0LL;
  long long bsize = 0LL;

  init_work(w);

  snprintf(ckarg, SCKARG, "%c%c%c%c%c:%c:",
	   ARG_VERBOSE,  ARG_VERSION, ARG_HELP, ARG_QUIET,
	   ARG_MAX_SHOW, ARG_BUFF);

  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	case ARG_BUFF:
	  bsize = get_ll(optarg, ARG_MAX_SHOW, bsize);
	  set_bsize = TRUE;
	  break;
	case ARG_MAX_SHOW:
	  max = get_ll(optarg, ARG_MAX_SHOW, max);
	  set_max = TRUE;
	  break;
	case ARG_HELP:
	  show_info(stderr, 1);
	  break;
	case ARG_QUIET:
	  w->arg_quiet = TRUE;
	  break;
	case ARG_VERSION:
	  show_info(stderr, 2);
	  break;
	case ARG_VERBOSE:
	  w->arg_verbose++;
	  break;
	default:
	  fprintf(stderr, MSG_E000, PROGNAME, SWITCH_CHAR, ARG_HELP);
	  exit(2);
	  break;
	}
    }

  /* set file names, expect 1 or 2 files */
  for (i = optind, num_files = 0; i < argc; i++)
    {
      num_files++;
      if (strlen(w->f1.fname) == 0)
	set_fname(w->f1.fname, argv[i]);
      else
	{
	  if (strlen(w->f2.fname) == 0)
	    set_fname(w->f2.fname, argv[i]);
	}
    }

  /* validations */
  if (w->arg_quiet == TRUE)
    w->arg_verbose = 0;
  if (set_max == TRUE)
    {
      if (max < 0LL)
	{
	  fprintf(stderr, MSG_E010, max, SWITCH_CHAR, ARG_MAX_SHOW);
	  fprintf(stderr, MSG_E000, PROGNAME, SWITCH_CHAR, ARG_HELP);
	  exit(2);
	}
      w->arg_show_max = (NUM) max;
    }
  if (set_bsize == TRUE)
    {
      if ((bsize < 5LL) || (bsize > (LONG_MAX - 20)))
	{
	  fprintf(stderr, MSG_E010, bsize, SWITCH_CHAR, ARG_BUFF);
	  fprintf(stderr, MSG_E000, PROGNAME, SWITCH_CHAR, ARG_HELP);
	  exit(2);
	}
      w->arg_buf_size = (long) bsize + 1; /* +1 for NULL */
    }

  switch (num_files)
    {
    case 1:
      /* one file assumed to be stdin */
      break;
    case 2:
      /* both files supplied */
      break;
    default:
      fprintf(stderr, MSG_E001);
      fprintf(stderr, MSG_E000, PROGNAME, SWITCH_CHAR, ARG_HELP);
      exit(2);
      break;
    }

  /* Allocate buffer */
  w->f1.buf = calloc(w->arg_buf_size, sizeof(char));
  if (w->f1.buf == (char *) NULL)
    {
      fprintf(stderr, MSG_E012, (long) w->arg_buf_size);
      exit(2);
    }
  w->f2.buf = calloc(w->arg_buf_size, sizeof(char));
  if (w->f2.buf == (char *) NULL)
    {
      fprintf(stderr, MSG_E012, (long) w->arg_buf_size);
      exit(2);
    }
  w->f1.buf_last = calloc(w->arg_buf_size, sizeof(char));
  if (w->f1.buf_last == (char *) NULL)
    {
      fprintf(stderr, MSG_E012, (long) w->arg_buf_size);
      exit(2);
    }
  w->f2.buf_last = calloc(w->arg_buf_size, sizeof(char));
  if (w->f2.buf_last == (char *) NULL)
    {
      fprintf(stderr, MSG_E012, (long) w->arg_buf_size);
      exit(2);
    }

} /* END process_arg() */

/*
 * close_file()
 */
void close_file(struct s_file *f)
{

  if (f->allow_close == TRUE)
    fclose(f->fp);
  f->allow_close = FALSE;
  f->fp = (FILE *) NULL;

} /* close_file() */

/*
 * open_read()
 */
int open_read(struct s_file *f, int *stdin_assigned)
{

  if (strlen(f->fname) < 1)
    {
      if ((*stdin_assigned) == TRUE)
	{
	  fprintf(stderr,  MSG_E003);
	  return(FALSE);
	}
      (*stdin_assigned) = TRUE;
      f->fp = stdin;
      return(TRUE);
    }

  f->fp = fopen(f->fname, "r");
  if (f->fp == (FILE *) NULL)
    {
      fprintf(stderr, MSG_E002, f->fname, strerror(errno));
      return(FALSE);
    }

  f->allow_close = TRUE;
  return(TRUE);

} /* END: open_read() */

/*
 * read_rec() -- get next record
 */
int read_rec(struct s_file *f, NUM *max, size_t buf_size)
{
  static char *b = (char *) NULL;
  static size_t bsize = (size_t) 0;
  ssize_t cread = (ssize_t) 0;

  if (f->at_eof == TRUE)
    return(FALSE);

  if (b == (char *) NULL)
    {
      bsize = buf_size + (size_t) 10;
      b = calloc(bsize, sizeof(char));
      if (b == (char *) NULL)
	{
	  fprintf(stderr, MSG_E012, (long) bsize);
	  exit(2);
	}
    }

  strncpy(f->buf_last, f->buf, buf_size);

  cread = getline(&b, &bsize, f->fp);

  if (cread < (ssize_t) 0)
    {
      f->at_eof = TRUE;
      memset(f->buf_last, 0, buf_size);
      memset(f->buf,      0, buf_size);
      return(FALSE);
    }

  f->reads++;

  memset(f->buf,  0, buf_size);
  strncpy(f->buf, b, buf_size);
  rtw(f->buf);

  if (cread > (ssize_t) (*max))
    (*max) = cread;

  if (strncmp(f->buf_last, f->buf, buf_size) > 0)
    {
      fprintf(stderr, MSG_E004, f->fname);
      fprintf(stderr, MSG_E005, f->reads);
      exit(2);
    }

  return(TRUE);

} /* read_rec() */

/*
 * diff_both() -- show results of both files active
 */
void diff_both(struct s_work *w)
{
  int results = 0;

  results = strncmp(w->f1.buf,w->f2.buf, (w->arg_buf_size - 1));
  switch (results)
    {
    case 0:
      read_rec(&(w->f1), &(w->max_size_found), w->arg_buf_size);
      read_rec(&(w->f2), &(w->max_size_found), w->arg_buf_size);
      break;
    default:
      w->count_diff++;
      if (results < 0)
	{
	  if ((w->arg_quiet == FALSE) && ((w->arg_show_max == 0) || (w->writes < w->arg_show_max)))
	    {
	      w->writes++;
	      fprintf(stdout, MSG_I094, "F1", w->f1.reads, w->f1.buf);
	    }
	  read_rec(&(w->f1), &(w->max_size_found), w->arg_buf_size);
	}
      else
	{
	  if ((w->arg_quiet == FALSE) && ((w->arg_show_max == 0) || (w->writes < w->arg_show_max)))
	    {
	      w->writes++;
	      fprintf(stdout, MSG_I094, "F2", w->f2.reads, w->f2.buf);
	    }
	  read_rec(&(w->f2), &(w->max_size_found), w->arg_buf_size);
	}
      break;
    }

} /* diff_both() */

/*
 * diff_single() -- show results if only 1 file is active
 */
void diff_single(struct s_work *w)
{
  w->count_diff++;

  if (w->f1.at_eof == FALSE)
    {
      if (w->arg_quiet == FALSE)
	{
	  if ((w->arg_show_max == 0) || (w->writes < w->arg_show_max))
	    {
	      w->writes++;
	      fprintf(stdout, MSG_I095, "F1", w->f1.reads, w->f1.buf);
	    }
	}
      read_rec(&(w->f1), &(w->max_size_found), w->arg_buf_size);
      return;
    }

  if (w->f2.at_eof == FALSE)
    {
      if (w->arg_quiet == FALSE)
	{
	  if ((w->arg_show_max == 0) || (w->writes < w->arg_show_max))
	    {
	      w->writes++;
	      fprintf(stdout, MSG_I095, "F2", w->f2.reads, w->f2.buf);
	    }
	}
      read_rec(&(w->f2), &(w->max_size_found), w->arg_buf_size);
      return;
    }

  fprintf(stderr, MSG_E006);
  exit(2);

} /* diff_single() */

/*
 * diff() -- process diff of 2 files
 */
void diff(struct s_work *w)
{
  NUM iteration = (NUM) 0;
  NUM show_mark = (NUM) 0;
  time_t seconds_now;

  /*** get first record ***/
  read_rec(&(w->f1), &(w->max_size_found), w->arg_buf_size);
  read_rec(&(w->f2), &(w->max_size_found), w->arg_buf_size);

  /*** process diff ***/
  while((w->f1.at_eof == FALSE) || (w->f2.at_eof == FALSE))
    {
      iteration++;
      show_mark++;
      if ((show_mark > (NUM) MAX_ITERATION) && (w->arg_verbose > 3))
	{
	  show_mark = (NUM) 0;
	  seconds_now  = time((time_t *) NULL);
	  fprintf(stderr, MSG_I093, (long long) (seconds_now - w->seconds_start), iteration);
	}
      if ((w->f1.at_eof == FALSE) && (w->f2.at_eof == FALSE))
	diff_both(w);
      else
	diff_single(w);
    }

} /* diff() */

/*
 * main
 */
int main(int argc, char **argv)
{
  struct s_work w;

  process_arg(argc, argv, &w);
  if (open_read(&(w.f1), &w.stdin_assigned) != TRUE)
    exit(2);
  if (open_read(&(w.f2), &w.stdin_assigned) != TRUE)
    {
      close_file(&(w.f1));
      exit(2);
    }

  if (w.arg_verbose > 2)
    {
      fprintf(stderr, MSG_I105, w.arg_verbose);
      fprintf(stderr, MSG_I107, (w.arg_buf_size - 1));
      fprintf(stderr, MSG_I106, 1,
	      (strlen(w.f1.fname) == 0 ? "stdin" : w.f1.fname));
      fprintf(stderr, MSG_I106, 2,
	      (strlen(w.f2.fname) == 0 ? "stdin" : w.f2.fname));
    }

  diff(&w);

  if (w.arg_verbose > 0)
    {
      if (w.max_size_found == w.arg_buf_size)
	fprintf(stderr, MSG_I091, (w.max_size_found - 1));
      else
	fprintf(stderr, MSG_W092, (w.max_size_found - 1));
      if (w.arg_verbose > 1)
	{
	  fprintf(stderr, MSG_I096, w.count_diff);
	  fprintf(stderr, MSG_I097, 1, w.f1.reads);
	  fprintf(stderr, MSG_I097, 2, w.f2.reads);
	  fprintf(stderr, MSG_I098, w.writes);
	}
    }

  close_file(&(w.f1));
  close_file(&(w.f2));

  if (w.count_diff == (NUM) 0)
    exit(0);
  exit(1);

} /* main() */
