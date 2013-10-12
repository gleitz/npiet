
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "npiet-foogol.y"

/*
 * npiet-foogol.y:					Jun 2004
 * (schoenfr@web.de)					Aug 2011
 *
 * This file is part of npiet.
 *
 * Copyright (C) 2004 Erik Schoenfelder (schoenfr@web.de)
 * see file COPYING
 *
 * npiet-foogol translates foogol input into piet output, so it
 * is some kind of a picture generator ;-)
 * 
 *
 * more about the piet programming language and the npiet interpreter see:
 *
 *	http://www.bertnase.de/npiet/
 *
 * for more about foogol see comp.sources.unix archive Volume 8
 * and about cfoogol see Volume 42, Issue 88 (here a copy):
 *
 *	http://www.bertnase.de/html/foogol.html
 *
 *
 * to compile better run ./configure and make from the npiet package, 
 * but manual compilation should work by running:
 *
 *	bison -o npiet-foogol.c npiet-foogol.y
 *	gcc -g -O -o npiet-foogol npiet-foogol.c -lgd -lpng -lm
 *
 *      echo 'begin prints ("Hello World!\n") end' | npiet-foogol -
 *      npiet npiet-foogol.png
 *
 * please note, npiet-foogol uses the libgd library.  on debian it is
 * avail by ``apt-get install libgd2-xpm-dev'' and bison can be
 * installed via ``apt-get install bison''.
 *
 * Have fun.
 */

char *version = "v1.3";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <time.h>

#include <gd.h>
#include <gdfonts.h>
#include <gdfontt.h>

/* just a default: */
#define WRAP_W	  64		/* default block wrap width in pixel */

/* filename for output: */
char *ofname = "npiet-foogol.png";

/* base factor for number generation: */
int smart_base = 7;			/* just try a funny value */

/* codel size: */
int codel_size = 1;

/* use random color at startup (opposite to pre v1.3 behavior): */
int random_color = 0;

void
usage (int rc)
{
  fprintf (stderr, "npiet-foogol %s\n\n", version);

  fprintf (stderr, "use: npiet-foogol [<options>] <filename>\n");
  fprintf (stderr, "options:\n");
  fprintf (stderr, "\t-v         - be verbose (default: off)\n");
  fprintf (stderr, "\t-q         - be quiet (default: off)\n");
  fprintf (stderr, "\t-d         - print debug messages (default: off)\n");
  fprintf (stderr, "\t-w <n>     - wrap pixel width (default: %d)\n", WRAP_W);
  fprintf (stderr, "\t-b <n>     - smart number generation base (default: %d)\n", smart_base);
  fprintf (stderr, "\t-cs <n>     - codel size of output (default: %d)\n",
           codel_size);
  fprintf (stderr, "\t-rnd        - not only red color on startup,\n");
  fprintf (stderr, "\t              use a random codel (experimental)\n");
  fprintf (stderr, "\t-o <filename> - output file (default: %s)\n", ofname);

  exit (rc);
}


/* be somewhat verbose: */
int verbose = 0;

/* be quiet: */
int quiet = 0;

/* print debugging stuff: */
int debug = 0;

/* use this width as a wrap margin: */
int wrap_wm = WRAP_W;

/* filename to work for: */
char *input_filename = 0;
FILE *f_in;

/* warning/error flag: */
int have_error = 0;
int have_warning = 0;

/* helper: */
#define dprintf		if (debug) printf
#define d2printf	if (debug > 1) printf
#define vprintf		if (verbose) printf

int 
parse_args (int argc, char **argv)
{
  while (--argc > 0) {
    argv++;
    if (! strcmp (argv [0], "-v")) {
      verbose++;
      vprintf ("info: verbose set to %d\n", verbose);
    } else if (! strcmp (argv [0], "-q")) {
      quiet = 1;
      dprintf ("info: quiet set to %d\n", quiet); 
    } else if (! strcmp (argv [0], "-d")) {
      debug++;
      dprintf ("info: debug set to %d\n", debug); 
    } else if (argc > 0 && ! strcmp (argv [0], "-w")) {
      argc--, argv++;		/* shift */
      wrap_wm = atoi (argv [0]);
      if (wrap_wm < 10) {
	fprintf (stderr, 
		 "warning: wrap width %d < 10 looks a bit odd - using 10\n",
		 wrap_wm);
	wrap_wm = 10;
      }
      vprintf ("info: wrap width set to %d\n", wrap_wm);
    } else if (argc > 0 && ! strcmp (argv [0], "-b")) {
      argc--, argv++;		/* shift */
      smart_base = atoi (argv [0]);
      if (smart_base < 2) {
	fprintf (stderr, 
		 "warning: smart_base %d < 2 looks a bit odd - using 7\n",
		 smart_base);
	smart_base = 7;
      }
      vprintf ("info: smart base set to %d\n", smart_base);
    } else if (argc > 0 && ! strcmp (argv [0], "-cs")) {
      argc--, argv++;		/* shift */
      codel_size = atoi (argv [0]);
      if (codel_size < 1) {
	fprintf (stderr, 
		 "warning: codel size %d < 1 looks a bit odd - using 1\n",
		 codel_size);
	codel_size = 1;
      }
      vprintf ("info: codel size set to %d\n", codel_size);
    } else if (argc > 0 && ! strcmp (argv [0], "-rnd")) {
      random_color = 1;
      vprintf ("info: using  random codel colors - very experimental...\n");
      /* init the random number generator - some secs should be good enough: */
      srand (time ((time_t *) 0) % 32768);
    } else if (argc > 0 && ! strcmp (argv [0], "-o")) {
      argc--, argv++;		/* shift */
      ofname = argv [0];
      vprintf ("info: output filename set to %s\n", ofname);
    } else if (argv [0][0] == '-' && argv [0][1]) {
      usage (-1);
    } else if (! input_filename) {
      input_filename = argv [0];
	vprintf ("info: using input filename %s\n", input_filename);
    } else {
      usage (-1);
    }
  }

  return 0;
}


/* force code for parser debugging to be included: */
#define YYDEBUG	1
/* force code for bettererror messages to be included: */
#define YYERROR_VERBOSE 1


/* forward: */
void yyerror (char *s);
int yylex (void);
void dump_blocks ();
struct _a_img * gen_block ();
void gd_create_img ();
int gdcol2idx ();
void gen_img_push ();
struct _a_decl *find_decl ();
void gen_img_check_wrap ();
void gen_mark ();
void gen_img_push_pos ();
void gen_wrap_right ();
int get_pixel ();
int check_fx ();



/*
 * internals:
 * 
 * we put the blocks of the code into a tree and store
 * references of the declarations and statements together with the
 * parsed expressions there too.
 *
 */

/*
 * a picture in an internal gd based format
 */
typedef struct _a_img {
  gdImagePtr im;
  int gdcol [18 + 2];
  int w, h;		/* width, height of the pic */
  int x, y;		/* paint position */
  int col;		/* last color used */
  int dx;		/* dir of code generation (1 == right, -1 == left): */
  int wrap_l;		/* left wrap position */
  int wrap_w;		/* wrap width (hint about position) */
  int wrap_maxw;	/* highest xpos used */
} a_img;


enum e_type { e_num, e_var, e_op };

#define e_type_str(e) ((e) == e_num ? "num" : \
	((e) == e_var ? "var" : ((e) == e_op ? "op" : "???")))


typedef struct _a_expr {
  enum e_type type;		/* eg: e_num or e_op */
  int op;			/* operator: eg.:  '+'  or '!' */
  struct _a_expr *arg1, *arg2;
  long num;
  char *var;
} a_expr;


enum n_type { n_nop, n_block, n_assign, n_print, n_printn, n_prints, 
	      n_if, n_while };

#define n_type_str(t) ((t) == n_assign ? "assign" : \
	((t) == n_nop ? "nop" : ((t) == n_block ? "block" : \
	 ((t) == n_print ? "print" : \
 	  ((t) == n_prints ? "prints" : \
 	   ((t) == n_printn ? "printn" : \
	    ((t) == n_if ? "if" : \
	     ((t) == n_while ? "while" : "???"))))))))


typedef struct _a_node {

  enum n_type type;		/* eg: n_nop or n_assign */
  char *var;			/* identifier of an assignment */
  struct _a_expr *arg1;		/* expr of an if / assignment */
  struct _a_node *stmt1;	/* then / while node */
  struct _a_node *stmt2;	/* else node */
  struct _a_block *block;	/* block statement */
  char *str;			/* string */

  struct _a_node *next;
} a_node;

/* current node we are parsing: */
a_node *cur_node = 0;

/*
 * variables in use:
 */
typedef struct _a_decl {
  char *var;			/* variable name */
  int off;			/* internal offset */
  struct _a_block *block;
  struct _a_decl *next;
} a_decl;


/*
 * block hierarchie:
 */
typedef struct _a_block {
  a_decl *decl;
  int d_off;			/* decl stack offsets */
  a_node *node;
  struct _a_block *parent;
  a_img *img;
} a_block;

/* the program itself: */
a_block *root_block = 0;
a_img *root_img = 0;

/* block in progress: */
a_block *curr_block = 0;


/*
 * free zone list:
 */
typedef struct _a_fx {
  int x;
  int y1, y2;
  struct _a_fx *next;
} a_fx;

a_fx *all_fx = 0;


/* just for nicer error handling (not very accurate...): */
int lineno = 0;

/* current stack depth in use (variables and expressions); */
int s_depth = 0;

/* 
 * helper:
 */
#define i_max(x, y)  ((x) > (y) ? (x) : (y))
#define i_abs(x)     ((x) < 0 ? -(x) : (x))


/*
 * helper:
 */
char *
xstrdup (char *s1, char *s2)
{
  char *s = calloc ((s1 ? strlen (s1) : 0) + (s2 ? strlen (s2) : 0) + 1, 1);
  if (s1) {
    strcat (s, s1);
  }
  if (s2) {
    strcat (s, s2);
  }
  return s;
}

/*
 * select new color by action (hue, light): 
 */
#define adv_col(c, h, l)  (((((c) % 6) + (h)) % 6) \
				+ (6 * ((((c) / 6) + (l)) % 3)))

static struct c_color { 
  int col;			/* rgb color */
  char *l_name;			/* long color name */
  char *s_name;			/* short color name */
  int c_idx;			/* our internal color index */
} c_colors [] = {

  { 0xFFC0C0, "light red", "lR", 0 },
  { 0xFFFFC0, "light yellow", "lY", 1 },
  { 0xC0FFC0, "light green", "lG", 2 },
  { 0xC0FFFF, "light cyan", "lC", 3 },
  { 0xC0C0FF, "light blue", "lB", 4 },
  { 0xFFC0FF, "light magenta", "lM", 5 },

  { 0xFF0000, "red", "nR", 6 },
  { 0xFFFF00, "yellow", "nY", 7 },
  { 0x00FF00, "green", "nG", 8 },
  { 0x00FFFF, "cyan", "nC", 9 },
  { 0x0000FF, "blue", "nB", 10 },
  { 0xFF00FF, "magenta", "nM", 11 },

  { 0xC00000, "dark red", "dR", 12 },
  { 0xC0C000, "dark yellow", "dY", 13 },
  { 0x00C000, "dark green", "dG", 14 },
  { 0x00C0C0, "dark cyan", "dC", 15 },
  { 0x0000C0, "dark blue", "dB", 16 },
  { 0xC000C0, "dark magenta", "dM", 17 },

  { 0xFFFFFF, "white", "WW", 18 },
  { 0x000000, "black", "BB", 19 }
};


/*
 * choose 6 == red as default start color or a random value:
 */
int startup_color ()
{
  if (random_color == 0) {
    return 6;				/* red */
  } else {
    /* choose something between 6 (red) and 11 (margenta): */
    return 6 + (rand () % 4);
  }
}



void
img_resize (a_img *img, int w, int h)
{
  gdImagePtr old_im = img->im;

  gd_create_img (img, w, h);
  gdImageCopy (img->im, old_im, 0, 0, 0, 0, w, h);    
  gdImageDestroy (old_im);
}


void
set_pixel (a_img *img, int x, int y, int col)
{
  if (x > img->w - 1 || y > img->h - 1) {

    int n_w = i_max (x + 1, img->w);
    int n_h = i_max (y + 1, img->h);

    img_resize (img, n_w, n_h);
  } 

  if (img->y > 0 && get_pixel (img, img->x, img->y - 1) == img->col) {
    /* could be ok: */
    dprintf ("* set_pixel possible color clash at %d, %d\n", x, y);
  }

  if (check_fx (x, x, y)) {
    /* could be ok: */
    dprintf ("* set_pixel possible fx clash at %d, %d\n", x, y);
    have_warning++;
  }

  gdImageSetPixel (img->im, x, y, img->gdcol [col]);
}


int
get_pixel (a_img *img, int x, int y)
{
  int col = gdImageGetPixel (img->im, x, y);
  col = gdcol2idx (img, col);
  return col;
}



int
gdcol2idx (a_img *img, int col)
{
  int i;

  for (i = 0; i < 20; i++) {
    if (img->gdcol [i] == col) {
      return i;
    }
  }

  return 18;		/* default background */
}


void
gd_create_img (a_img *img, int w, int h)
{
  int i;

  /* create initial image: */
  img->im = gdImageCreate (w, h);

  img->w = w;
  img->h = h;
  
  /* background color: */
  gdImageColorAllocate (img->im, 255, 255, 255);

  for (i = 0; i < 20; i++) {
    int r = c_colors [i].col >> 16;
    int g = (c_colors [i].col >> 8) & 0xff;
    int b = c_colors [i].col & 0xff;
    img->gdcol [i] = gdImageColorAllocate (img->im, r, g, b);
  }

  return;
}


void
img_init (a_img **imgp)
{
  a_img *img =  calloc (1, sizeof (a_img));

  /* save pointer: */
  *imgp = img;

  img->w = 1;
  img->h = 1;

  gd_create_img (img, img->w, img->h);

  /* start image with default: */
  img->dx = 1;
  img->wrap_l = img->wrap_maxw = 0;
  img->wrap_w = wrap_wm;

  /* initial pixel: */
  img->col = startup_color ();		/* random or red */
  img->x = 0; 
  img->y = 0;

  set_pixel (img, img->x, img->y, img->col);
}


void
img_delete (a_img *img)
{
  gdImageDestroy (img->im);
  free (img);
}



void
save_img (a_img *img)
{
  FILE *out;

  if (! (out = fopen (ofname, "wb"))) {
    perror ("cannot open for writing; reason");
  } else {

    if (codel_size > 1) {
      /*
       * resize first: 
       */
      gdImagePtr im = gdImageCreate (img->w * codel_size, img->h * codel_size);

      gdImageCopyResized (im, img->im, 
			  0, 0, 
			  0, 0, 
			  img->w * codel_size, img->h * codel_size,
			  img->w, img->h);

      gdImagePng (im, out);      
    } else {
      gdImagePng (img->im, out);
    }
    fclose (out);
    if (! quiet) {
      fprintf (stderr, "output saved to %s\n", ofname);
    }
  }
}


/*
 * add a vertical space as a left border;
 */
a_fx *
add_free_x (int x, int y1, int y2)
{
  a_fx *n = calloc (1, sizeof (a_fx));

  n->x = x;
  n->y1 = y1;
  n->y2 = (y2 == -1) ? INT_MAX : y2;

  dprintf ("* adding free x=%d (%d <= y <= %d)\n", n->x, n->y1, n->y2);

  n->next = all_fx;
  all_fx = n;

  return n;
}


int
check_fx (int x1, int x2, int y)
{
  a_fx *fx = all_fx;

  while (fx) {
    if (y >= fx->y1 && y <= fx->y2 
 	&& x1 >= fx->x && x2 <= fx->x) {
      return 1;
    }
    fx = fx->next;
  }
  return 0;
}


int
fx_wrap_l (int x, int y)
{
  a_fx *fx = all_fx;
  int l = 0;

  while (fx) {
    if (x >= fx->x && y >= fx->y1 && y <= fx->y2) {
      l = i_max (l, fx->x);
    }
    fx = fx->next;
  }
  return l;
}



/*
 * alloc a node element:
 */
a_node *
new_node (enum n_type type)
{
  a_node *n = calloc (1, sizeof (a_node));
  n->type = type;
  n->block = curr_block;
  return n;
}



/*
 * alloc a  element:xpr
 */
a_expr *
new_expr (enum e_type type)
{
  a_expr *n = calloc (1, sizeof (a_expr));
  n->type = type;
  return n;
}


void
dump_expr (a_expr *expr)
{
  if (! expr) {
    return;
  }

  printf (" ");
  if (expr->type == e_num) {
    printf ("%ld", expr->num);
  } else if (expr->type == e_var) {
    printf ("%s", expr->var);
  } else if (expr->type == e_op) {
    printf ("(");
    dump_expr (expr->arg1);
    printf (" %c", expr->op);
    dump_expr (expr->arg2);
    printf (" )");
  } else {
    printf ("??? (type %s) ", e_type_str (expr->type));
  }
}



/*
 * dump a decl list
 *
 * (dont care about mem leaks...)
 */
void
dump_decls (char *off, a_decl *decl)
{
  if (! decl) {
    return;
  }

  dprintf ("%s  decl: %s  off: %d  (block 0x%08lx)\n", off, decl->var, 
	  decl->off, (unsigned long) decl->block);
    
  dump_decls (off, decl->next);
}


/*
 * dump a node list
 *
 * (dont care about mem leaks...)
 */
void
dump_nodes (char *off, a_node *node)
{
  if (node) {
    printf ("%s  node: %s\n", off, n_type_str (node->type));

    if (node->type == n_assign) {
      printf ("%s %s := ", off, node->var);
      dump_expr (node->arg1);
      printf ("\n");
    } else if (node->type == n_prints) {
      printf ("%s str %s\n", off, node->str);
    } else if (node->type == n_printn) {
      printf ("%s ", off);
      dump_expr (node->arg1);
      printf ("\n");
    } else if (node->type == n_while) {
      printf ("%s ", off);
      dump_expr (node->arg1);
      printf ("\n");
      dump_nodes (off, node->stmt1);
    } else if (node->type == n_if) {
      printf ("%s if ", off);
      dump_expr (node->arg1);
      printf ("\n%s then\n", off);
      dump_nodes (xstrdup (off, "    "), node->stmt1);
      if (node->stmt2) {
	printf ("%s else\n", off);
	dump_nodes (xstrdup (off, "    "), node->stmt2);
      }
    } else if (node->type == n_block) {
      dump_blocks (xstrdup (off, "  "), node->block);
    } else {
      printf ("???\n");
    } 
    
    dump_nodes (off, node->next);

  }
}


/*
 * dump the blocks tree
 *
 * (dont care about mem leaks...)
 */
void
dump_blocks (char *off, a_block *block)
{
  if (block) {
    printf ("%s  block: 0x%08lx\n", off, (unsigned long) block);
    
    dump_decls (off, block->decl);
    dump_nodes (off, block->node);
  }
}


void
gen_cmd_nowrap (a_img *img, int h, int l)
{
  int c1, cc = adv_col (img->col, h, l);

  if (img->y > 0 && get_pixel (img, img->x + img->dx, img->y - 1) == cc) {
    /* color clash - set new mark and look about valid colors: */
    img->x += 2 * img->dx;
    c1 = startup_color ();	/* start cycle with random or red */
    cc = adv_col (c1, h, l);

    while (get_pixel (img, img->x, img->y - 1) == c1
	   || get_pixel (img, img->x + img->dx, img->y - 1) == cc) {
      c1 = (c1 + 1) % 18;
      cc = adv_col (c1, h, l);
    }
    img->col = c1;
    set_pixel (img, img->x, img->y, c1);
  }

    img->x += img->dx;
    img->col = adv_col (img->col, h, l);
    set_pixel (img, img->x, img->y, img->col);
}


/*
 * generate a command by the given hue and light.
 *
 * for now, put the command along the command chain.
 */
void
gen_cmd (a_img *img, int h, int l)
{
  gen_img_check_wrap (img);
  gen_cmd_nowrap (img, h, l);
}


/*
 * place a codel right here, but check about color above:
 */
void
gen_codel (a_img *img, int col)
{
  if (col == -1) {
    /* choose a color not matching the codel above: */
    img->col = startup_color ();    /* choose random or red */
    while (img->y > 0 && get_pixel (img, img->x, img->y - 1) == img->col) {
      img->col++;
    }
  } else {
    img->col = col;
    if (img->y > 0 && get_pixel (img, img->x, img->y - 1) == col) {
      fprintf (stderr, "oops - gen_mark color clash with %d at (%d, %d)...\n",
	       img->x, img->y , col);
    }
  }
  set_pixel (img, img->x, img->y, img->col);
}


/*
 * advance and create codel as a new code generation start:
 */
void
gen_mark (a_img *img, int col)
{
  img->x += 2 * img->dx;
  gen_codel (img, col);
}


void
gen_wrap_right (a_img *img, int expand_flag)
{
  if (img->dx > 0) {
    fprintf (stderr, "oops - internal error in gen_wrap_right: dx = %d\n",
	     img->dx);
  }

  if (expand_flag) {
    /* expand to leftmost turn: */
    while (img->dx < 0 && img->x > img->wrap_l + 4) {
      gen_mark (img, -1);
      gen_img_check_wrap (img);
    }
    if (img->dx > 0) {
      return;
    }
  }

  /*
   * we need minimum 6 codels and 8 if we need a new mark.
   * and as a worst case it will simply not work due to color clash.
   * xxx fix me: check harder
   */
  gen_img_push_pos (img, 3, 0);
  gen_cmd_nowrap (img, 4, 0);			/* dup */
  gen_cmd_nowrap (img, 3, 1);			/* pointer */
  img->col = adv_col (img->col, 3, 1);		/* pointer */
  img->y++;
  set_pixel (img, img->x, img->y, img->col);
  img->wrap_maxw = i_max (img->wrap_maxw, img->x);
  img->dx = 1;
}


void
gen_wrap_left (a_img *img)
{
  if (img->dx < 0) {
    fprintf (stderr, "oops - internal error in gen_wrap_left: dx = %d\n",
	     img->dx);
  }

  dprintf ("* wrapping to left at %d, %d wrap_l = %d\n",
	   img->x, img->y, img->wrap_l);
  
  /* paint some wrap stuff: */
  gen_img_push_pos (img, 1, 0);
  gen_cmd_nowrap (img, 4, 0);			/* dup */
  gen_cmd_nowrap (img, 3, 1);			/* pointer */
  img->col = adv_col (img->col, 3, 1);		/* pointer */
  img->y++;
  set_pixel (img, img->x, img->y, img->col);
  
  img->wrap_maxw = i_max (img->wrap_maxw, img->x);
  
  img->dx = -1;
}


void
gen_img_check_wrap (a_img *img)
{
  if (img->wrap_w < 8) {
    /* ignore: */
    return;
  }

  /* yyy: */
  img->wrap_l = fx_wrap_l (img->x, img->y) + 1;

  if (img->dx > 0 && img->x > img->wrap_l + img->wrap_w) {

    gen_wrap_left (img);

  } else if (img->dx < 0 && img->x < img->wrap_l + 11) {

    /*
     * in one case img->wrap_l + 10 was too low.  gen_marks may
     * increase need for space to the left...
     */

    dprintf ("deb: * do wrap:  wrap_l = %d, x = %d\n", img->wrap_l, img->x);

    gen_wrap_right (img, 0);
  }
}


int
col_clash (a_img *img, int val)
{
  int i;

  for (i = 0; i < val; i++) {
    int x = img->x + (img->dx * i);

    if (get_pixel (img, x, img->y - 1) == img->col) {
      return 1;
    }
  }
  return 0;
}


/*
 * generate a push of the given positive number.
 * on entry we assume a single codel present and
 * expand this to the desired value if not equal 0.
 * for a 0 we create a push of 1 followed by a not.
 * optional we can force not to wrap.
 */
void
gen_img_push_pos (a_img *img, long val, int w_flag)
{
  int i;

  if (val < 0) {
    fprintf (stderr, "oops - internal error: unexpected negative value\n");
    abort ();
    exit (-1);
  }

  if (val == 0) {
    if (w_flag) {
      gen_cmd (img, 0, 1);			/* push 1 */
      gen_cmd (img, 2, 2);			/* not */
    } else {
      gen_cmd_nowrap (img, 0, 1);		/* push 1 */
      gen_cmd_nowrap (img, 2, 2);		/* not */
    }
    /* we are done: */
    return;
  }

  if (w_flag && img->dx < 0 && img->x - val <= img->wrap_l + 8) {
    /*
     * not enough space to put the push plus a wrap and some spare pixel
     * in.  so we fill the space with noop's and head the other way:
     */
    while (img->dx < 0 && img->x > 0) {

      d2printf ("* ----- fill at %d, %d  (wrap_l=%d)\n", img->x, img->y, 
		img->wrap_l);

      gen_mark (img, -1);
      gen_img_check_wrap (img);
    }
  }

  if (img->y > 0) {

    int xx = img->x + img->dx * val;
    int cc = adv_col (img->col, 0, 1);		/* push */
    int have_mark = 0;

    while (col_clash (img, val) || get_pixel (img, xx, img->y - 1) == cc) {
      
      d2printf ("* -- cc fill at %d, %d  val=%ld (wrap_l=%d)\n", 
		img->x, img->y, val, img->wrap_l);
      
      if (! have_mark) {
	gen_mark (img, -1);
	have_mark = 1;
	xx = img->x + img->dx * val;
      } else {
	/* try next color in cycle: */
	img->col = (img->col + 1) % 18;
      }
      cc = adv_col (img->col, 0, 1);		/* push */
    }
  }
    
  /* the color block: */
  for (i = 0; i < val; i++) {
    int xx = img->x + (img->dx * i);
    set_pixel (img, xx, img->y, img->col);
  }

  /* the push itself: */
  img->x += img->dx * val;
  img->col = adv_col (img->col, 0, 1);

  set_pixel (img, img->x, img->y, img->col);
}



/*
 * generate a push of zero minus the given number
 */
void
gen_img_push_neg (a_img *img, long val)
{
  gen_cmd (img, 0, 1);		/* push 1 */
  gen_cmd (img, 2, 2);		/* not */
  gen_img_push_pos (img, val, 1);
  gen_cmd (img, 1, 1);		/* sub */
}



void
gen_img_push (a_img *img, long val)
{
  if (val < 0) {
    gen_img_push_neg (img, -1 * val);
  } else {
    gen_img_push_pos (img, val, 1);
  }
}



void
gen_img_push_smart (a_img *img, long val)
{
  /* check if a wrap should be done: */
  gen_img_check_wrap (img);

  if (i_abs (val) <= smart_base) {
    /*
     * for now just use a straight line to the right: 
     */
    gen_img_push (img, val);
  } else {
    /*
     * try to calculate the value in a smart way:
     */
    if (val < 0) {
      gen_img_push (img, -1);
      gen_img_push_smart (img, i_abs (val));
      gen_cmd (img, 1, 2);			/* mul */
    } else {
      int ival = val;
      if (ival > 0) {
	gen_img_push_smart (img, ival % smart_base);
	gen_img_push_smart (img, ival / smart_base);
	gen_img_push_smart (img, smart_base);
	gen_cmd (img, 1, 2);			/* mul */
	gen_cmd (img, 1, 0);			/* add */
      }
    }
  }
}



void
gen_img_program_end (a_img *img)
{
  if (img->dx < 0) {
    gen_wrap_right (img, 0);
  }

  gen_mark (img, -1);

  set_pixel (img, img->x + (img->dx * 1), img->y,     19);
  set_pixel (img, img->x + (img->dx * 1), img->y + 1, 6);
  set_pixel (img, img->x,            img->y + 2, 6);
  set_pixel (img, img->x + (img->dx * 1), img->y + 2, 6);
  /* just in case: add always black blocks: */
  set_pixel (img, img->x + (img->dx * 2), img->y + 1, 19);
  set_pixel (img, img->x + (img->dx * 2), img->y + 2, 19);
  set_pixel (img, img->x - (img->dx * 1), img->y + 2, 19);
  set_pixel (img, img->x,            img->y + 3, 19);
  set_pixel (img, img->x + (img->dx * 1), img->y + 3, 19);
}



void
gen_expr (a_img *img, a_block *p_block, a_expr *expr, int *y_max)
{
  int l_y_max = *y_max = img->y;

  if (! expr) {
    return;
  }
  
  if (expr->type == e_num) {

    dprintf ("  push %ld  (s_depth now %d)\n", expr->num, s_depth + 1);

    gen_img_push_smart (img, expr->num);
    s_depth += 1;

  } else if (expr->type == e_var) {

    int s_off = s_depth;
    a_decl *d = find_decl (expr->var, p_block);
    if (! d) {
      fprintf (stderr, "internal error: var %s not found ?\n", expr->var);
    }
    dprintf ("  push var %s (d_off %d, s_off %d)\n", 
	     expr->var, d->off, s_off);

    /*
     * create a copy of the variables value: 
     *
     * roll up, dup and roll down again (if on top, dup only)
     */
    if (1 && s_off - d->off > 0) {
      gen_img_push_smart (img, s_off - d->off);
      gen_img_push_smart (img, -1);
      gen_cmd (img, 4, 1);		/* roll */
    }
    gen_cmd (img, 4, 0);		/* dup */
    if (1 && s_off - d->off > 0) {
      gen_img_push_smart (img, s_off - d->off + 1);
      gen_img_push_smart (img, 1);
      gen_cmd (img, 4, 1);		/* roll */
    }

    s_depth += 1;

  } else if (expr->type == e_op) {
    gen_expr (img, p_block, expr->arg1, &l_y_max);
    if (expr->op != '!') {
      gen_expr (img, p_block, expr->arg2, &l_y_max);
    }
    dprintf ("  op: %c\n", expr->op);

    if (expr->op == '+') { gen_cmd (img, 1, 0); } 
    else if (expr->op == '-') {  gen_cmd (img, 1, 1); }
    else if (expr->op == '*') {  gen_cmd (img, 1, 2); }
    else if (expr->op == '/') {  gen_cmd (img, 2, 0); }
    else if (expr->op == '%') {  gen_cmd (img, 2, 1); }
    else if (expr->op == '!') {  gen_cmd (img, 2, 2); }
    else if (expr->op == '>') {  gen_cmd (img, 3, 0); }
    else if (expr->op == '=') { 
      gen_cmd (img, 1, 1);	/* sub */
      gen_cmd (img, 2, 2);	/* not */
    } else if (expr->op == '#') { 
      gen_cmd (img, 1, 1);	/* sub */
    } else { fprintf (stderr, "error: unknown op `%c'\n", expr->op); }

    if (expr->op != '!') {
      s_depth -= 1;
    }
    
  } else {
    printf ("  ??? (fix me) \n");
  }

  *y_max = i_max (l_y_max, img->y);
}



void
deb_dump_img (a_img *img)
{
  int i, j;

  printf ("deb: dump image (w = %d, h = %d, x = %d, y= %d, col = %d):\n",
	  img->w, img->h, img->x, img->y, img->col);

  printf ("----\n");
  for (j = 0; j < img->h; j++) {
    for (i = 0; i < img->w; i++) {
      int col = get_pixel (img, i, j);
      printf (" %2d", col);
    }
    printf ("\n");
  }
  printf ("----\n");
}



void
gen_nodes (a_img *img, a_block *p_block, a_node *node, int *y_max)
{
  int l_y_max = *y_max = img->y;

  if (! node) {
    return;
  }

  dprintf ("gen node: %s  (s_depth = %d)\n", n_type_str (node->type),
	   s_depth);

  if (node->type == n_assign) {

    int s_off = s_depth;
    a_decl *d = find_decl (node->var, node->block);
    if (!d ) {
      fprintf (stderr, "internal error: var %s not found ?\n", node->var);
    }

    gen_expr (img, p_block, node->arg1, &l_y_max);
    dprintf ("assign %s (d_off %d, s_off %d)\n", node->var, d->off, s_off);

    /*
     * move the value to the variables place on the stack:
     *
     * roll old value up, discard and roll down new value
     */

    gen_img_push_smart (img, s_off - d->off + 1);
    gen_img_push_smart (img, -1);
    gen_cmd (img, 4, 1);		/* roll */
    gen_cmd (img, 0, 2);		/* pop */
    gen_img_push_smart (img, s_off - d->off);
    gen_img_push_smart (img, 1);
    gen_cmd (img, 4, 1);		/* roll */

    s_depth -= 1;

  } else if (node->type == n_print) {

    gen_img_push_smart (img, 10);		/* linefeed */
    gen_cmd (img, 5, 2);			/* outchar */

  } else if (node->type == n_prints) {
    int i;

    for (i = 0; i < (int) strlen (node->str); i++) {
      gen_img_push_smart (img, node->str [i]);
      gen_cmd (img, 5, 2);			/* outchar */
    }

  } else if (node->type == n_printn) {

    gen_expr (img, p_block, node->arg1, &l_y_max);

    gen_cmd (img, 5, 1);			/* outnum */
    s_depth -= 1;

  } else if (node->type == n_block) {

    /*
     * pass the same image:
     */
    gen_block (node->block, img, &l_y_max);

  } else if (node->type == n_while) {

    int c, j;
    int b_x, b_y, b_col;		/* branch */
    int e_x, e_y;			/* expr */
    int w_l_y_max = 0;
    a_fx *fx;

    dprintf ("while start:\n");

    l_y_max = img->y;

    /* lets start left to right: */
    if (img->dx < 0) {
      gen_wrap_right (img, 1);
    }

    /* start of expr: */
    gen_mark (img, -1);
    /* return to expr: */
    c = adv_col (img->col, 3, 2);		/* pointer reverse */
    set_pixel (img, img->x, img->y + 1, c);

    /* save pos; here we return from the end of the loop: */
    e_x = img->x;
    e_y = img->y;

    fx = add_free_x (e_x, e_y, -1);

    img->wrap_maxw = img->x;

    /* push turn left into the while body: */
    gen_img_push (img, 3);			/* pointer arg */
    s_depth += 1;

    /* condition: */
    gen_expr (img, p_block, node->arg1, &w_l_y_max);

    /* XXX */
    img->wrap_maxw = i_max (img->wrap_maxw, img->x);

    /* lets again start left to right: */
    if (img->dx < 0) {
      gen_wrap_right (img, 1);
      w_l_y_max = i_max (w_l_y_max, img->y);
    }

    l_y_max = i_max (w_l_y_max, l_y_max);

    /* check: */
    gen_cmd_nowrap (img, 2, 2);			/* not */
    gen_cmd_nowrap (img, 2, 2);			/* not */
    gen_cmd_nowrap (img, 3, 1);			/* pointer */
    s_depth -= 1;

    dprintf ("* while condition done;  x=%d, y=%d\n", img->x, img->y);
  
    /* save pos; here we decide to execute the loop-body or if we are done: */
    b_x = img->x;
    b_y = img->y;
    b_col = img->col;

    dprintf ("deb: * wrap_l = %d\n", img->wrap_l);

    /* false path (loop exit): */
    gen_cmd_nowrap (img, 0, 2);			/* pop */

    /* and a usefull (useless ?) mark: */
    gen_mark (img, -1);

    /* set start point for while body branch: */
    b_col = adv_col (b_col, 3, 1);		/* pointer */
    set_pixel (img, b_x, b_y + 1, b_col);
    set_pixel (img, b_x, b_y + 2, b_col);
    set_pixel (img, b_x + 1, b_y + 1, 19);

    img->x = b_x + 2;
    img->y = b_y + 2;
    /* 
     * v1.3 experimental random_color selection may fail here (at least
     * withe the 99-bottels.foo test - nomen est omen ;-)
     */
#if 0
    img->col = startup_color (); 
#else
    img->col = 6;
#endif
    set_pixel (img, img->x, img->y, img->col);

    s_depth -= 1;

    /* two turns right to reenter the loop: */
    gen_img_push (img, 1);
    gen_cmd (img, 4, 0);		/* dup */
    s_depth += 2;

    /* create while-body statements: */
    gen_nodes (img, p_block, node->stmt1, &w_l_y_max);

    if (img->dx > 0) {
      gen_wrap_left (img);	
      w_l_y_max = i_max (w_l_y_max, img->y);
    }

    dprintf ("* w wrapw: %d  (x = %d)  w_l_y_max = %d (y = %d)  "
	     "l_y_max = %d\n",
	     img->wrap_maxw, img->x, w_l_y_max, img->y, l_y_max);

    l_y_max = i_max (l_y_max, w_l_y_max);

    /* XXX */
    img->wrap_maxw = i_max (img->wrap_maxw, img->x);

    /* return to expression condition: */
    if (img->y <= w_l_y_max) {
      for (j = img->y + 1; j <= w_l_y_max + 1; j++) {
	set_pixel (img, img->x, j, img->col);
      }
      img->y = j - 1;
      set_pixel (img, img->x - 1, img->y, img->col);
      l_y_max = i_max (l_y_max, img->y);
    }

    /* turn up and turn to pointer into the loop: */
    img->x = e_x;
    gen_codel (img, -1);
    c = adv_col (img->col, 3, 2);		/* pointer reverse */
    set_pixel (img, img->x + 1, img->y, c);

    if (img->y == e_y + 1) {
      printf ("internal: oops - possible color clash in while... = fix me\n");
    }
    s_depth -= 2;

    /* adjust: */
    fx->y2 = img->y;

    /* add: */
    add_free_x (img->wrap_maxw + 1, e_y, img->y);

    /* end of while statement: */
    img->x = b_x + 2;
    img->y = b_y;
    img->dx = 1;

    /* where to continue with: */
    img->x = i_max (b_x + 2, img->wrap_maxw + 1);
    gen_mark (img, -1);

    l_y_max = i_max (l_y_max, img->y);

    dprintf ("while end.   (l_y_max = %d)\n", l_y_max);

  } else if (node->type == n_if) {
    
    int j;
    int b_x, b_y, b_col;		/* branch */
    int fi_y;				/* final else pos */
    int el_x, el_y, el_max_y;		/* else x y pos, max y */
    int old_wrap_l = img->wrap_l;
    int if_l_y_max = 0;
    a_fx *fx;

    dprintf ("if start:   at %d, %d\n", img->x, img->y);

    l_y_max = img->y;

    if (img->dx < 0) {
      gen_wrap_right (img, 1);
    }
    img->wrap_maxw = img->x;

    gen_img_push (img, 3);			/* pointer argument */
    s_depth += 1;

    /* branch condition: */
    gen_expr (img, p_block, node->arg1, &if_l_y_max);

    l_y_max = i_max (l_y_max, if_l_y_max);

    /* lets again start left to right: */
    if (img->dx < 0) {
      gen_wrap_right (img, 1);
      l_y_max = i_max (l_y_max, img->y);
    }

    /* branch: (first normalize with not's)*/
    gen_cmd_nowrap (img, 2, 2);			/* not */
    gen_cmd_nowrap (img, 2, 2);			/* not */
    gen_cmd_nowrap (img, 3, 1);			/* pointer */
    s_depth -= 1;

    dprintf ("* if condition done;  x=%d, y=%d\n", img->x, img->y);

    /* save pos for then branch: */
    b_x = img->x;
    b_y = img->y;
    b_col = img->col;

    /* init else path: */
    gen_cmd_nowrap (img, 0, 2);			/* pop */
    s_depth -= 1;

    gen_mark (img, -1);

    /*
     * if we have a else path, create this one first:
     */
    if (node->stmt2) {
      int o_maxw = img->wrap_maxw;

      img->wrap_maxw = img->x;

      /* left border: */
      fx = add_free_x (img->x, img->y, -1);
      
      gen_nodes (img, p_block, node->stmt2, &if_l_y_max);

      if (img->dx < 0) {
	gen_wrap_right (img, 0);
	if_l_y_max = i_max (if_l_y_max, img->y);
      }

      l_y_max = i_max (l_y_max, if_l_y_max);

      /* adjust: */
      fx->y2 = img->y;
      img->wrap_maxw = i_max (o_maxw, img->wrap_maxw);

      el_x = i_max (img->x, img->wrap_maxw);
      el_y = img->y;
      el_max_y = if_l_y_max;
     
      dprintf ("* else end:  el_x = %d, el_y = %d\n", el_x, el_y);

    } else {
      el_x = img->x;
      el_max_y = el_y = img->y;
    }

    fi_y = img->y;

    /*
     * find start point for then branch: 
     */
    b_col = adv_col (b_col, 3, 1);		/* pointer */
    dprintf ("** b_y %d, el_y %d  el_max_y %d (el_x = %d)\n", 
	     b_y, el_y, el_max_y, el_x);
    for (j = b_y + 1; j < el_max_y + 3; j++) {
      set_pixel (img, b_x, j, b_col);
      if ( j < el_y + 2) {
	set_pixel (img, b_x + 1, j, 19);
      }
    }

    img->x = b_x;
    img->y = j - 1;
    gen_mark (img, -1);

    gen_nodes (img, p_block, node->stmt1, &if_l_y_max);

    if (img->dx < 0) {
      gen_wrap_right (img, 0);	
    }
    if_l_y_max = i_max (if_l_y_max, img->y);

    dprintf ("* if wrapw: %d  (x = %d)  if_l_y_max = %d (y = %d) "
	     "l_y_max = %d\n",
	     img->wrap_maxw, img->x, if_l_y_max, img->y, l_y_max);

    l_y_max = i_max (l_y_max, if_l_y_max);

    /* return to end of the else path: */

    img->x = i_max (img->x, img->wrap_maxw);
    img->x = i_max (img->x, el_x);

    dprintf ("** see x %d  (el_x %d)\n", img->x, el_x);

    /* at least a single mark is needed: */
    gen_mark (img, -1);

    while  (img->x <= el_x) {
      gen_mark (img, -1);
    }

    add_free_x (img->x + 1, el_y, img->y);

    if (el_y > 0) {
      while (get_pixel (img, img->x, el_y - 1) == img->col 
	     || get_pixel (img, img->x + 1, el_y - 1) == img->col) {
	img->col = (img->col + 1) % 18;
      }
    }

    for (j = img->y; j >= el_y; j--) {
      img->y = j;
      set_pixel (img, img->x, img->y, img->col);
    }

    img->x++;
    set_pixel (img, img->x, img->y, img->col);

    img->y = fi_y;

    /* xxx */
    gen_mark (img, -1);
    
    img->wrap_l = old_wrap_l;
    img->wrap_maxw = i_max (old_wrap_l, img->wrap_maxw);

    dprintf ("if end.\n");

  } else {

    fprintf (stderr, "internal error: gen %s (fix me)\n", 
	     n_type_str (node->type));
  }

  *y_max = i_max (l_y_max, img->y);

  /* next statement in this block: */
  gen_nodes (img, p_block, node->next, &l_y_max);

  *y_max = i_max (l_y_max, *y_max);
}


/*
 * head along the node tree and put funny color pixel into
 * a picture:
 */
a_img *
gen_block (a_block *block, a_img *img, int *y_max)
{
  a_decl *decl;
  int pre_depth, l_y_max = 998;

  if (! block) {
    *y_max = img ? img->y : 0;
    return 0;
  }

  dprintf ("gen block: 0x%08lx\n", (unsigned long) block);
  
  if (! img) {
    img_init (&img);
  }

  /* save stack depth: */
  pre_depth = s_depth;

  /* create variable space: */
  for (decl = block->decl; decl; decl = decl->next) {
    /* for every declared variable put a 0 on the stack: */
    if (pre_depth == s_depth) {
      gen_img_push_smart (img, 0);
    } else {
      gen_cmd (img, 4, 0);		/* dup */
    }
    s_depth++;
  }

  gen_nodes (img, block, block->node, &l_y_max);

  /* remove variable space: */
  for (decl = block->decl; decl; decl = decl->next) {
    gen_cmd (img, 0, 2);		/* pop */
    s_depth--;
  }

  /* restore stack depth: */
  if (s_depth != pre_depth) {
    fprintf (stderr, "*** gen block oops - depth %d != %d\n",
	     s_depth, pre_depth);
    s_depth = pre_depth;
  }

  *y_max = i_max (l_y_max, img->y);

  return img;
}



a_img *
gen_root_img (a_block *root_block)
{
  int l_y_max = 997;

  return gen_block (root_block, 0, &l_y_max);
 }



a_expr *
new_biop_expr (enum e_type type, a_expr *e1, int op, a_expr *e2)
{
  a_expr *n = new_expr (type);
  n->op = op; 
  n->arg1 = e1;
  n->arg2 = e2;
  return n;
}



void
new_decl (char *var, a_block *block)
{
  a_decl *n = calloc (1, sizeof (a_decl));
  n->var = strdup (var);
  n->block = block;

  /* decl offset: */
  n->off = block->d_off++;

  /* chain in: */
  n->next = block->decl;
  block->decl = n;
}


/*
 * lookup decl in current scope up to top:
 */
a_decl *
find_decl (char *var, a_block *block)
{
  a_block *b = block;

  while (b) {
    a_decl *d = b->decl;

    while (d) {
      if (! strcmp (d->var, var)) {
	return d;
      }
      d = d->next;
    }
    b = b->parent;
  }

  return 0;
}


a_node *
new_if_stmt (a_expr *expr, a_node *node1, a_node *node2)
{
  a_node *n = new_node (n_if);
  n->arg1 = expr;
  n->stmt1 = node1;
  n->stmt2 = node2;
  return n;
}





/* Line 189 of yacc.c  */
#line 1788 "npiet-foogol.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TBEGIN = 258,
     TEND = 259,
     TINTEGER = 260,
     TPRINTS = 261,
     TPRINTN = 262,
     TPRINT = 263,
     TIF = 264,
     TTHEN = 265,
     TELSE = 266,
     TWHILE = 267,
     TDO = 268,
     NUM = 269,
     STR = 270,
     QSTR = 271,
     UNARY = 272
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 1721 "npiet-foogol.y"

  long num;
  char *str;
  a_expr *expr;
  a_node *node;
  a_block *block;



/* Line 214 of yacc.c  */
#line 1851 "npiet-foogol.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 1863 "npiet-foogol.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   135

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  45
/* YYNRULES -- Number of states.  */
#define YYNSTATES  84

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   272

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,    18,     2,     2,     2,     2,
      26,    27,    23,    21,    30,    22,     2,    24,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    31,    29,
      20,    17,    19,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    25
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    10,    12,    16,    19,
      22,    26,    30,    34,    38,    41,    45,    49,    53,    57,
      59,    61,    63,    64,    71,    72,    77,    80,    84,    86,
      90,    92,    94,    96,   101,   108,   113,   115,   122,   127,
     129,   131,   133,   135,   140,   145
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      33,     0,    -1,    -1,    36,    -1,    15,    -1,    14,    -1,
      34,    -1,    26,    35,    27,    -1,    21,    35,    -1,    22,
      35,    -1,    35,    23,    35,    -1,    35,    24,    35,    -1,
      35,    21,    35,    -1,    35,    22,    35,    -1,    28,    35,
      -1,    35,    19,    35,    -1,    35,    20,    35,    -1,    35,
      17,    35,    -1,    35,    18,    35,    -1,    39,    -1,     3,
      -1,     4,    -1,    -1,    37,    42,    29,    44,    40,    38,
      -1,    -1,    37,    44,    41,    38,    -1,     5,    43,    -1,
      34,    30,    43,    -1,    34,    -1,    45,    29,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,     9,    35,    10,    45,
      -1,     9,    35,    10,    47,    11,    46,    -1,    12,    35,
      13,    46,    -1,    48,    -1,     9,    35,    10,    47,    11,
      47,    -1,    12,    35,    13,    47,    -1,    49,    -1,    50,
      -1,    39,    -1,     8,    -1,     6,    26,    16,    27,    -1,
       7,    26,    35,    27,    -1,    34,    31,    17,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1752,  1752,  1753,  1759,  1767,  1769,  1778,  1780,  1782,
    1788,  1790,  1792,  1794,  1796,  1798,  1800,  1802,  1804,  1813,
    1818,  1833,  1839,  1838,  1845,  1844,  1854,  1857,  1859,  1865,
    1870,  1875,  1877,  1882,  1884,  1886,  1896,  1898,  1900,  1910,
    1912,  1914,  1923,  1925,  1931,  1940
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TBEGIN", "TEND", "TINTEGER", "TPRINTS",
  "TPRINTN", "TPRINT", "TIF", "TTHEN", "TELSE", "TWHILE", "TDO", "NUM",
  "STR", "QSTR", "'='", "'#'", "'>'", "'<'", "'+'", "'-'", "'*'", "'/'",
  "UNARY", "'('", "')'", "'!'", "';'", "','", "':'", "$accept", "foogol",
  "identifier", "expr", "program", "begin", "end", "block", "@1", "@2",
  "decl", "id_seq", "stmt_list", "stmt", "op_stmt", "cl_stmt",
  "simple_stmt", "io_stmt", "assign_stmt", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    61,    35,    62,
      60,    43,    45,    42,    47,   272,    40,    41,    33,    59,
      44,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    33,    34,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    36,
      37,    38,    40,    39,    41,    39,    42,    43,    43,    44,
      44,    45,    45,    46,    46,    46,    47,    47,    47,    48,
      48,    48,    49,    49,    49,    50
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     3,     2,     2,
       3,     3,     3,     3,     2,     3,     3,     3,     3,     1,
       1,     1,     0,     6,     0,     4,     2,     3,     1,     3,
       1,     1,     1,     4,     6,     4,     1,     6,     4,     1,
       1,     1,     1,     4,     4,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,    20,     0,     3,     0,    19,     1,     0,     0,     0,
      42,     0,     0,     4,     0,    41,     0,    24,    30,    31,
      32,    36,    39,    40,    28,    26,     0,     0,     5,     0,
       0,     0,     0,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,    21,
      25,    29,    27,    43,    44,     7,    33,    32,    17,    18,
      15,    16,    12,    13,    10,    11,    35,    38,    45,     0,
       0,    23,    34,    37
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,    33,    34,     3,     4,    60,    15,    79,    38,
      16,    25,    17,    18,    19,    20,    21,    22,    23
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -55
static const yytype_int8 yypact[] =
{
       6,   -55,    20,   -55,    41,   -55,   -55,    23,    -1,    19,
     -55,    56,    56,   -55,    24,   -55,    22,   -55,    25,   -55,
     -55,   -55,   -55,   -55,    27,   -55,    50,    56,   -55,    56,
      56,    56,    56,   -55,    -6,    81,    51,    80,    65,    80,
      23,    45,    89,   -55,   -55,   100,   -55,    80,    56,    56,
      56,    56,    56,    56,    56,    56,    80,    56,   -55,   -55,
     -55,   -55,   -55,   -55,   -55,   -55,   -55,    62,     8,     8,
     -16,   -16,     0,     0,   -55,   -55,   -55,   -55,   111,    65,
      80,   -55,   -55,   -55
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -55,   -55,    -4,    10,   -55,   -55,    -5,    75,   -55,   -55,
     -55,    39,   -18,    33,   -54,   -46,   -55,   -55,   -55
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      14,    67,    76,    24,    47,    52,    53,    54,    55,     1,
      77,    48,    49,    50,    51,    52,    53,    54,    55,    58,
       6,    61,    35,    54,    55,    26,    82,    50,    51,    52,
      53,    54,    55,    14,    83,    14,    24,    42,    13,    43,
      44,    45,    46,    14,     1,    27,     7,     8,     9,    10,
      11,    37,    14,    12,    39,    36,    13,    40,    68,    69,
      70,    71,    72,    73,    74,    75,    41,    78,    57,    59,
      28,    13,    63,    80,    81,     5,    14,    29,    30,    62,
      66,     0,    31,     1,    32,     0,     8,     9,    10,    11,
       0,     0,    12,     0,    56,    13,     0,     0,    48,    49,
      50,    51,    52,    53,    54,    55,    48,    49,    50,    51,
      52,    53,    54,    55,     0,     0,    64,    48,    49,    50,
      51,    52,    53,    54,    55,     0,     0,    65,    48,    49,
      50,    51,    52,    53,    54,    55
};

static const yytype_int8 yycheck[] =
{
       4,    47,    56,     7,    10,    21,    22,    23,    24,     3,
      56,    17,    18,    19,    20,    21,    22,    23,    24,    37,
       0,    39,    12,    23,    24,    26,    80,    19,    20,    21,
      22,    23,    24,    37,    80,    39,    40,    27,    15,    29,
      30,    31,    32,    47,     3,    26,     5,     6,     7,     8,
       9,    29,    56,    12,    29,    31,    15,    30,    48,    49,
      50,    51,    52,    53,    54,    55,    16,    57,    17,     4,
      14,    15,    27,    11,    79,     0,    80,    21,    22,    40,
      47,    -1,    26,     3,    28,    -1,     6,     7,     8,     9,
      -1,    -1,    12,    -1,    13,    15,    -1,    -1,    17,    18,
      19,    20,    21,    22,    23,    24,    17,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    27,    17,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    27,    17,    18,
      19,    20,    21,    22,    23,    24
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    33,    36,    37,    39,     0,     5,     6,     7,
       8,     9,    12,    15,    34,    39,    42,    44,    45,    46,
      47,    48,    49,    50,    34,    43,    26,    26,    14,    21,
      22,    26,    28,    34,    35,    35,    31,    29,    41,    29,
      30,    16,    35,    35,    35,    35,    35,    10,    17,    18,
      19,    20,    21,    22,    23,    24,    13,    17,    44,     4,
      38,    44,    43,    27,    27,    27,    45,    47,    35,    35,
      35,    35,    35,    35,    35,    35,    46,    47,    35,    40,
      11,    38,    46,    47
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 1752 "npiet-foogol.y"
    { yyerror ("no vaild input found"); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 1754 "npiet-foogol.y"
    { root_block = (yyvsp[(1) - (1)].block); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 1760 "npiet-foogol.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 1768 "npiet-foogol.y"
    { (yyval.expr) = new_expr (e_num); (yyval.expr)->num = (yyvsp[(1) - (1)].num); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 1770 "npiet-foogol.y"
    { 
		  (yyval.expr) = new_expr (e_var); 
		  if (! find_decl ((yyvsp[(1) - (1)].str), curr_block)) {
		    yyerror ("unknown identifier");
		    have_error = 1;
		  }
		  (yyval.expr)->var = (yyvsp[(1) - (1)].str); 
		;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 1779 "npiet-foogol.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 1781 "npiet-foogol.y"
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 1783 "npiet-foogol.y"
    { 
		  a_expr *e = new_expr (e_num); 
		  e->num = 0;
		  (yyval.expr) = new_biop_expr (e_op, e, '-', (yyvsp[(2) - (2)].expr)); 
		;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 1789 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '*', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 1791 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '/', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 1793 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '+', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 1795 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '-', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 1797 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(2) - (2)].expr), '!', 0); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 1799 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '>', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 1801 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(3) - (3)].expr), '>', (yyvsp[(1) - (3)].expr)); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 1803 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '=', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 1805 "npiet-foogol.y"
    { (yyval.expr) = new_biop_expr (e_op, (yyvsp[(1) - (3)].expr), '#', (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 1814 "npiet-foogol.y"
    { (yyval.block) = (yyvsp[(1) - (1)].block); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 1819 "npiet-foogol.y"
    {
	    a_block *n = calloc (1, sizeof (a_block));
	    n->parent = curr_block;
	    /* initialize offset with currect offset: */
	    d2printf ("** curr_block 0x%08lx  off %d\n", 
		      (unsigned long) curr_block, 
		      curr_block ? curr_block->d_off : 0);
	    n->d_off = curr_block ? curr_block->d_off : 0;
	    curr_block = n;
	    (yyval.block) = curr_block;
	  ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 1834 "npiet-foogol.y"
    { curr_block = curr_block->parent; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 1839 "npiet-foogol.y"
    { 
	    curr_block->node = (yyvsp[(4) - (4)].node);
	    (yyval.block) = curr_block;
	  ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 1845 "npiet-foogol.y"
    {
	    curr_block->node = (yyvsp[(2) - (2)].node);
	    (yyval.block) = curr_block;
	  ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 1858 "npiet-foogol.y"
    { new_decl ((yyvsp[(1) - (3)].str), curr_block); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 1860 "npiet-foogol.y"
    { new_decl ((yyvsp[(1) - (1)].str), curr_block); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 1866 "npiet-foogol.y"
    {
	    (yyvsp[(1) - (3)].node)->next = (yyvsp[(3) - (3)].node);
	    (yyval.node) = (yyvsp[(1) - (3)].node);
	  ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 1871 "npiet-foogol.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 1876 "npiet-foogol.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 1878 "npiet-foogol.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 1883 "npiet-foogol.y"
    { (yyval.node) = new_if_stmt ((yyvsp[(2) - (4)].expr), (yyvsp[(4) - (4)].node), 0); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 1885 "npiet-foogol.y"
    { (yyval.node) = new_if_stmt ((yyvsp[(2) - (6)].expr), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 1887 "npiet-foogol.y"
    {
	    a_node *n = new_node (n_while);
	    n->arg1 = (yyvsp[(2) - (4)].expr);
	    n->stmt1 = (yyvsp[(4) - (4)].node);
	    (yyval.node) = n;
	  ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1897 "npiet-foogol.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 1899 "npiet-foogol.y"
    { (yyval.node) = new_if_stmt ((yyvsp[(2) - (6)].expr), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1901 "npiet-foogol.y"
    {
	    a_node *n = new_node (n_while);
	    n->arg1 = (yyvsp[(2) - (4)].expr);
	    n->stmt1 = (yyvsp[(4) - (4)].node);
	    (yyval.node) = n;
	  ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1911 "npiet-foogol.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 1913 "npiet-foogol.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 1915 "npiet-foogol.y"
    { 
	    a_node *n = new_node (n_block);
	    n->block = (yyvsp[(1) - (1)].block);
	    (yyval.node) = n;
	  ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1924 "npiet-foogol.y"
    { (yyval.node) = new_node (n_print); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1926 "npiet-foogol.y"
    {
	    a_node *n = new_node (n_prints);
	    n->str = (yyvsp[(3) - (4)].str);
	    (yyval.node) = n;
	  ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1932 "npiet-foogol.y"
    {
	    a_node *n = new_node (n_printn);
	    n->arg1 = (yyvsp[(3) - (4)].expr);
	    (yyval.node) = n;
	  ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1941 "npiet-foogol.y"
    {
	    a_node *n = new_node (n_assign);
	    n->var = (yyvsp[(1) - (4)].str);
	    if (! find_decl ((yyvsp[(1) - (4)].str), curr_block)) {
	      yyerror ("unknown identifier");
	      have_error = 1;
	    }
	    n->arg1 = (yyvsp[(4) - (4)].expr);
	    (yyval.node) = n;
	  ;}
    break;



/* Line 1455 of yacc.c  */
#line 3480 "npiet-foogol.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1953 "npiet-foogol.y"



/*
 * lex:
 *
 * return number, string, word, ...
 * skip white.
 */

#include <ctype.h>

int
yylex ()
{ 
  int c;

  /* skip over whites */
  while ((c = getc (f_in)) == ' ' || c == '\t' || c == '\n') {
    if (c == '\n') {
      lineno++;
    }
    continue;
  }

  /* skip comments: */
  if (c == '/') {
    int c2 = getc (f_in);
    if (c2 == '*') {
      c2 = 0;
      while ((c = getc (f_in)) != '/' || c2 != '*') {
	if (c == EOF) {
	  yyerror ("EOF in comment");
	  exit (1);
	} else if (c == '\n') {
	  lineno++;
	}
	c2 = c;
      }
      /* restart: */
      return yylex ();
    }

    /* no comment (tm): */
    ungetc (c2, f_in);
  }

  /* number: */
  if (isdigit (c)) {
    ungetc (c, f_in);
    if (1 != fscanf (f_in, "%ld", &yylval.num)) {
      yyerror ("cannot scan integer value");
    }
    return NUM;
  }

  /* quoted string: */
  if (c == '"') {
    char *s = calloc (1, 1);
    while ((c = getc (f_in)) > 0 && c != '"') {
      if (strlen (s) > 0 && s [strlen (s) - 1] == '\\') {
	if (c == 'n') {
	  s [strlen (s) - 1] = '\n';
	} else if (c == 't') {
	  s [strlen (s) - 1] = '\t';
	} else if (c == '"') {
	  s [strlen (s) - 1] = '"';
	} else if (c == '\\') {
	  s [strlen (s) - 1] = '\\';
	} else {
	  s [strlen (s) - 1] = c;
	}
      } else {
	s = realloc (s, strlen (s) + 2);
	s [ strlen (s) + 1] = 0;
	s [ strlen (s)] = c;
      }
    }
    yylval.str = s;
    return QSTR;
  }

  /* identifier / keyword: */
  if (isalpha (c)) {
    char *s = calloc (2, 1);
    s [0] = c;
    while (isalnum ((c = getc (f_in)))) {
      s = realloc (s, strlen (s) + 2);
      s [ strlen (s) + 1] = 0;
      s [ strlen (s)] = c;
    }
    if (c != EOF) {
      /* so kid: put it back: */
      ungetc (c, f_in);
    }

    if (! strcmp (s, "begin")) { free (s); return TBEGIN;
    } else if (! strcmp (s, "end")) { free (s); return TEND;
    } else if (! strcmp (s, "integer")) { free (s); return TINTEGER;
    } else if (! strcmp (s, "prints")) { free (s); return TPRINTS;
    } else if (! strcmp (s, "printn")) { free (s); return TPRINTN;
    } else if (! strcmp (s, "print")) { free (s); return TPRINT;
    } else if (! strcmp (s, "if")) { free (s); return TIF;
    } else if (! strcmp (s, "then")) { free (s); return TTHEN;
    } else if (! strcmp (s, "else")) { free (s); return TELSE;
    } else if (! strcmp (s, "while")) { free (s); return TWHILE;
    } else if (! strcmp (s, "do")) { free (s); return TDO;
    }

    /* identifier: */
    yylval.str = s;
    return STR;
  }

  /* EOF: */
  if (c == EOF) {
    return 0;
  }

  /* single char: */
  return c;
}



void
yyerror (char *s)
{
  printf ("line %d: %s\n", lineno + 1, s);
}


int
main (int argc, char *argv[])
{
  int rc;

  if (parse_args (argc, argv) < 0) {
    usage (-1);
  }

  /* a input filename is mandatory: */
  if (! input_filename) {
    usage (-1);
  } else if (! strcmp (input_filename, "-")) {
    vprintf ("info: reading from <stdin>\n");
    input_filename = "<stdin>";
    f_in = stdin;
  } else if (! (f_in = fopen (input_filename, "r"))) {
    fprintf (stderr, "error: cannot open `%s' for reading...\n",
	     input_filename);
    exit (-1);
  } else {
    vprintf ("info: reading from `%s'\n", input_filename);
  }

  /* with higher debugging level output parser messages: */
  if (debug > 1) {
    yydebug = 1;
  }

  /* parse input: */
  rc = yyparse ();

  if (rc || have_error) {
    fprintf (stderr, "exiting...\n");
    exit (-1);
  }

  if (debug > 1) {
    dump_blocks ("", root_block);
  }

#if 0
  if (have_warning) {
    fprintf (stderr, "warning: possibly misgenerated code - "
	     "may not run correctly.\nwarning: try to increase"
	     " the wrap-width (option -w).\n");
  }
#endif

  /* save painting: */
  root_img = gen_root_img (root_block);
  gen_img_program_end (root_img);
  save_img (root_img);

  return 0;
}

/* end of foogol-parse.y */

