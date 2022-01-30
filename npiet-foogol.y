%{
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



%}

/*
 * grammer start:
 */

%start foogol

%union {
  long num;
  char *str;
  a_expr *expr;
  a_node *node;
  a_block *block;
}

%token TBEGIN TEND TINTEGER TPRINTS TPRINTN TPRINT
%token TIF TTHEN TELSE TWHILE TDO

%token <num>  NUM
%token <str>  STR QSTR

%type <str>   identifier
%type <block> block program foogol begin

%type <expr>  expr
%type <node>  stmt_list stmt op_stmt cl_stmt simple_stmt
%type <node>  assign_stmt io_stmt 

%left '=' '#'
%left '>' '<'
%left '+' '-'
%left '*' '/'
%left UNARY  

%%

foogol
	: /* empty */
	  { yyerror ("no vaild input found"); }
	| program
	  { root_block = $1; }
	;


identifier
	: STR
		{ $$ = $1; }
	;


/* expressions : */

expr
	: NUM
		{ $$ = new_expr (e_num); $$->num = $1; }
	| identifier
		{ 
		  $$ = new_expr (e_var); 
		  if (! find_decl ($1, curr_block)) {
		    yyerror ("unknown identifier");
		    have_error = 1;
		  }
		  $$->var = $1; 
		}
	| '(' expr ')'
		{ $$ = $2; }
	| '+' expr %prec UNARY
		{ $$ = $2; }
	| '-' expr %prec UNARY
		{ 
		  a_expr *e = new_expr (e_num); 
		  e->num = 0;
		  $$ = new_biop_expr (e_op, e, '-', $2); 
		}
	| expr '*' expr
		{ $$ = new_biop_expr (e_op, $1, '*', $3); }
	| expr '/' expr
		{ $$ = new_biop_expr (e_op, $1, '/', $3); }
	| expr '+' expr
		{ $$ = new_biop_expr (e_op, $1, '+', $3); }
	| expr '-' expr
		{ $$ = new_biop_expr (e_op, $1, '-', $3); }
	| '!' expr %prec UNARY
		{ $$ = new_biop_expr (e_op, $2, '!', 0); }
	| expr '>' expr
		{ $$ = new_biop_expr (e_op, $1, '>', $3); }
	| expr '<' expr
		{ $$ = new_biop_expr (e_op, $3, '>', $1); }
	| expr '=' expr
		{ $$ = new_biop_expr (e_op, $1, '=', $3); }
	| expr '#' expr
		{ $$ = new_biop_expr (e_op, $1, '#', $3); }
	;



/* compound statements and blocks : */

program
	: block
	  { $$ = $1; }
	;

begin
	: TBEGIN 
	  {
	    a_block *n = calloc (1, sizeof (a_block));
	    n->parent = curr_block;
	    /* initialize offset with currect offset: */
	    d2printf ("** curr_block 0x%08lx  off %d\n", 
		      (unsigned long) curr_block, 
		      curr_block ? curr_block->d_off : 0);
	    n->d_off = curr_block ? curr_block->d_off : 0;
	    curr_block = n;
	    $$ = curr_block;
	  }
	;

end
	: TEND
	  { curr_block = curr_block->parent; }
	;

block
	: begin decl ';' stmt_list
	  { 
	    curr_block->node = $<node>4;
	    $<block>$ = curr_block;
	  }
	  end
	| begin stmt_list
	  {
	    curr_block->node = $<node>2;
	    $<block>$ = curr_block;
	  }
	  end
	;


decl
	: TINTEGER id_seq ;

id_seq 
	: identifier ',' id_seq		/* right recursion - don't care ... */
	  { new_decl ($1, curr_block); } 
	| identifier 
	  { new_decl ($1, curr_block); } 
	;


stmt_list 
	: stmt ';' stmt_list		/* right recursion - don't care ... */
	  {
	    $1->next = $3;
	    $$ = $1;
	  }
	| stmt 
	  { $$ = $1; }
	;

stmt
	: op_stmt 
	  { $$ = $1; }
	| cl_stmt
	  { $$ = $1; }
	;

op_stmt
	: TIF expr TTHEN stmt 
	  { $$ = new_if_stmt ($2, $4, 0); }
	| TIF expr TTHEN cl_stmt TELSE op_stmt
	  { $$ = new_if_stmt ($2, $4, $6); }
	| TWHILE expr TDO op_stmt
	  {
	    a_node *n = new_node (n_while);
	    n->arg1 = $2;
	    n->stmt1 = $4;
	    $$ = n;
	  } 
	;

cl_stmt
	: simple_stmt
	  { $$ = $1; }
	| TIF expr TTHEN cl_stmt TELSE cl_stmt
	  { $$ = new_if_stmt ($2, $4, $6); }
	| TWHILE expr TDO cl_stmt
	  {
	    a_node *n = new_node (n_while);
	    n->arg1 = $2;
	    n->stmt1 = $4;
	    $$ = n;
	  } 
	;

simple_stmt
	: io_stmt 
	  { $$ = $1; }
	| assign_stmt
	  { $$ = $1; }
	| block  
	  { 
	    a_node *n = new_node (n_block);
	    n->block = $1;
	    $$ = n;
	  }
	;

io_stmt
	: TPRINT
	  { $$ = new_node (n_print); }
	| TPRINTS '(' QSTR ')'
	  {
	    a_node *n = new_node (n_prints);
	    n->str = $3;
	    $$ = n;
	  }
	| TPRINTN '(' expr ')'
	  {
	    a_node *n = new_node (n_printn);
	    n->arg1 = $3;
	    $$ = n;
	  }
	;

assign_stmt
	: identifier ':' '=' expr
	  {
	    a_node *n = new_node (n_assign);
	    n->var = $1;
	    if (! find_decl ($1, curr_block)) {
	      yyerror ("unknown identifier");
	      have_error = 1;
	    }
	    n->arg1 = $4;
	    $$ = n;
	  } 
	;

%%


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
