#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Pixmap.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <FL/x.H>
#include <stdio.h>

#include "webpalette.h"

/* XPM */
static char *resize[] = {
/* width height num_colors chars_per_pixel */
"    25    15        2            1",
/* colors */
". c #000000",
"# c none",
/* pixels */
"#########################",
"#########################",
"#########.....##.########",
"#######..........########",
"#######..####....########",
"######..#####....########",
"######..####.....########",
"######..#################",
"######..#################",
"######..#######.#########",
"#######..#####..#########",
"#######........##########",
"#########.....###########",
"#########################",
"#########################"
};

static Fl_Pixmap pixmap(resize);

#define BOXSIZE 14
#define BORDER 4

Fl_Window *w;
Fl_Button *b;
Fl_Output *o;
int sizes[3]={8,18,36};
int current=0;
int INLINE=sizes[current];



class ColorMenu : public Fl_Widget {
  Fl_Color initial;
  Fl_Color which, previous;
  unsigned long pallete_map[215];
  int done;
  void drawbox(Fl_Color);
  void draw();
   void do_output(int color);
  int handle(int);
public:
  ColorMenu(Fl_Color oldcol);
};

ColorMenu::ColorMenu(Fl_Color oldcol) :
  Fl_Widget(0,0,BOXSIZE*INLINE+1+2*BORDER, BOXSIZE*216/INLINE+1+2*BORDER) {
  initial = which = oldcol;
  for(int c=0;c<200;c++){
    Fl::set_color((Fl_Color)(c+56),web_pal[c*3],web_pal[c*3+1],web_pal[c*3+2]);   }
  for(int c=200;c<216;c++){
    Fl::set_color((Fl_Color)(c-184),web_pal[c*3],web_pal[c*3+1],web_pal[c*3+2]);
  }
}

void ColorMenu::do_output(int color){
  char s[10];
  uchar r=web_pal[color*3];
  uchar g=web_pal[color*3+1];
  uchar b=web_pal[color*3+2];
  sprintf(s,"#%2.2x%2.2x%2.2x",r,g,b);
  Fl::selection(*this,s,7);
  o->value(s);
}


void ColorMenu::drawbox(Fl_Color c) {
  if (c < 0 || c > 216) return;
  int x = (c%INLINE)*BOXSIZE+BORDER;
  int y = (c/INLINE)*BOXSIZE+BORDER;
  Fl_Color rc;
  rc=(Fl_Color)(c+56);
  if (c>=200) rc=(Fl_Color)(c-184);
#if BORDER_WIDTH < 3
  if (c == which) fl_draw_box(FL_DOWN_BOX, x+1, y+1, BOXSIZE-1,
BOXSIZE-1,(Fl_Color)(rc));
  else fl_draw_box(FL_BORDER_BOX, x, y, BOXSIZE+1, BOXSIZE+1,(Fl_Color)(rc));
#else
  fl_draw_box(c == which ? FL_DOWN_BOX : FL_BORDER_BOX,
	      x, y, BOXSIZE+1, BOXSIZE+1,(Fl_Color)(rc));
#endif
}

void ColorMenu::draw() {
  if (damage() != FL_DAMAGE_CHILD) {
    fl_draw_box(FL_FLAT_BOX,0,0,w(),h(),color());
    for (int c = 0; c < 216; c++) drawbox((Fl_Color)c);
  } else {
    drawbox(previous);
    drawbox(which);
  }
  previous = which;
}

int ColorMenu::handle(int e) {
  int c = which;
  switch (e) {
  case FL_CLOSE:
    exit(1);
  case FL_PUSH:
  case FL_DRAG: {
    int X = (Fl::event_x() -  BORDER);
    if (X >= 0) X = X/BOXSIZE;
    int Y = (Fl::event_y() - BORDER);
    if (Y >= 0) Y = Y/BOXSIZE;
    if (X >= 0 && X < INLINE && Y >= 0 && Y < 216/INLINE)
      c = INLINE*Y + X;
    else
      c = initial;
    } break;
  case FL_RELEASE:
    done=1;
    break;
  case FL_KEYBOARD:
    switch (Fl::event_key()) {
    case FL_Up: if (c > 7) c -= 8; break;
    case FL_Down: if (c < 256-8) c += 8; break;
    case FL_Left: if (c > 0) c--; break;
    case FL_Right: if (c < 255) c++; break;
    case FL_Escape: which = initial; done = 1; return 1;
    case FL_Enter: done = 1; return 1;
    default: return 0;
    }
    break;
  default:
    return 0;
  }
  if (c != which) {
    which = (Fl_Color)c; damage(FL_DAMAGE_CHILD);
    int bx = (c%INLINE)*BOXSIZE+BORDER;
    int by = (c/INLINE)*BOXSIZE+BORDER;
    int px = x();
    int py = y();
    if (px+bx+BOXSIZE+BORDER >= Fl::w()) px = Fl::w()-bx-BOXSIZE-BORDER;
    if (py+by+BOXSIZE+BORDER >= Fl::h()) py = Fl::h()-by-BOXSIZE-BORDER;
    if (px+bx < BORDER) px = BORDER-bx;
    if (py+by < BORDER) py = BORDER-by;
    position(px,py);do_output((int)which);
  }
  if (done) do_output((int)which);
  done=0;
  return 1;
}

extern char fl_override_redirect; // hack for menus



ColorMenu *m;

void do_resize(Fl_Widget *,void *){
 INLINE=sizes[++current%3];
 w->resize(w->x(),w->y(),BOXSIZE*INLINE+1+2*BORDER,
  	   BOXSIZE*216/INLINE+1+2*BORDER+18);
 m->resize(0,0,BOXSIZE*INLINE+1+2*BORDER, BOXSIZE*216/INLINE+1+2*BORDER);
 o->resize(BORDER,BOXSIZE*216/INLINE+2*BORDER,55,18);
 b->resize(2*BORDER+55,BOXSIZE*216/INLINE+2*BORDER,35,18);

}

int main(int argc, char **argv) {
  w=new Fl_Window(BOXSIZE*INLINE+1+2*BORDER,
  			     BOXSIZE*216/INLINE+1+2*BORDER+18,"SAFEWEB");
  m=new ColorMenu((Fl_Color )0);
  o=new Fl_Output(BORDER,BOXSIZE*216/INLINE+2*BORDER,55,18);
  o->textsize(10);
  b =new Fl_Button(2*BORDER+55,BOXSIZE*216/INLINE+2*BORDER,
  			     35,18,"Resize");
  pixmap.label(b);
  b->callback(do_resize);
  w->end();
  w->show(argc,argv);
  return Fl::run();;
}
