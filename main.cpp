/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Amado Martinez 
 *
 * Created on August 30, 2018, 9:38 PM
 */

#include <cstdlib>
#include <iostream>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
// for overlay
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>

#define BASIC_EVENT_MASK (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask)
#define NOT_PROPAGATE_MASK (KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask)

using namespace std;

Display *g_display;
int      g_screen;
Window   g_win;
int      g_disp_width;
int      g_disp_height;
Pixmap   g_bitmap;
Colormap g_colormap;

XWindowAttributes wrattr;
    
XColor textColor;
XColor backgroundColor;
XColor white;


void allow_input_passthrough (Window w) {
    XserverRegion region = XFixesCreateRegion (g_display, NULL, 0);

    //XFixesSetWindowShapeRegion (g_display, w, ShapeBounding, 0, 0, 0);
    XFixesSetWindowShapeRegion (g_display, w, ShapeInput, 0, 0, region);

    XFixesDestroyRegion (g_display, region);
}

// Create a XColor from 3 byte tuple (0 - 255, 0 - 255, 0 - 255).
XColor createXColorFromRGBA(short red, short green, short blue, short alpha) {
    XColor color;

    // m_color.red = red * 65535 / 255;
    color.red = (red * 0xFFFF) / 0xFF;
    color.green = (green * 0xFFFF) / 0xFF;
    color.blue = (blue * 0xFFFF) / 0xFF;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(g_display, DefaultColormap(g_display, g_screen), &color)) {
        std::cerr << "createXColorFromRGB: Cannot create color" << endl;
        exit(-1);
    }

    *(&color.pixel) = ((*(&color.pixel)) & 0x00ffffff) | (alpha << 24);
    return color;
}

void createShapedWindow() {
    XSetWindowAttributes wattr;
    XColor bgcolor = createXColorFromRGBA(0, 0, 0, 0);

    Window root    = DefaultRootWindow(g_display);
    Visual *visual = DefaultVisual(g_display, g_screen);

    // amado
//            Display display;
//        Drawable d;
//        Window root_return;
//        int x_return, y_return;
//        unsigned int width_return, height_return;
//        unsigned int border_width_return;
//        unsigned int depth_return;
//XGetGeometry(g_display, d, &root_return, &x_return, &y_return, &width_return, 
//                      &height_return, &border_width_return, &depth_return);
    fprintf(stderr, "Creating..%d x %d", g_disp_width, g_disp_height);//wrattr.width);
//cout<<"WIDTH:" << wrattr.width;
    // end 
    XVisualInfo vinfo;
    XMatchVisualInfo(g_display, DefaultScreen(g_display), 32, TrueColor, &vinfo);
    g_colormap = XCreateColormap(g_display, DefaultRootWindow(g_display), vinfo.visual, AllocNone);

    XSetWindowAttributes attr;
    attr.background_pixmap = None;
    attr.background_pixel = bgcolor.pixel;
    attr.border_pixel=0;
    attr.win_gravity=NorthWestGravity;
    attr.bit_gravity=ForgetGravity;
    attr.save_under=1;
    attr.event_mask=BASIC_EVENT_MASK;
    attr.do_not_propagate_mask=NOT_PROPAGATE_MASK;
    attr.override_redirect=1; // OpenGL > 0
    attr.colormap = g_colormap;

    //unsigned long mask = CWBackPixel|CWBorderPixel|CWWinGravity|CWBitGravity|CWSaveUnder|CWEventMask|CWDontPropagate|CWOverrideRedirect;
    unsigned long mask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask | CWWinGravity|CWBitGravity | CWSaveUnder | CWDontPropagate | CWOverrideRedirect;

//    g_win = XCreateWindow(g_display, root, 0, 0, WIDTH, HEIGHT, 0, vinfo.depth, InputOutput, vinfo.visual, mask, &attr);
    g_win = XCreateWindow(g_display, root, 0, 0, g_disp_width, g_disp_height, 0, vinfo.depth, InputOutput, vinfo.visual, mask, &attr);

//	g_bitmap = XCreateBitmapFromData (g_display, RootWindow(g_display, g_screen), (char *)myshape_bits, myshape_width, myshape_height);

    //XShapeCombineMask(g_display, g_win, ShapeBounding, 900, 500, g_bitmap, ShapeSet);
    XShapeCombineMask(g_display, g_win, ShapeInput, 0, 0, None, ShapeSet );

    // We want shape-changed event too
    #define SHAPE_MASK ShapeNotifyMask
    XShapeSelectInput (g_display, g_win, SHAPE_MASK );

    // Tell the Window Manager not to draw window borders (frame) or title.
	wattr.override_redirect = 1;
    XChangeWindowAttributes(g_display, g_win, CWOverrideRedirect, &wattr);
    allow_input_passthrough(g_win);

    // Show the window
//    XMapWindow(g_display, g_win);

    textColor = createXColorFromRGBA(0, 0, 0, 255);
    backgroundColor = createXColorFromRGBA(255, 128, 128, 128);
    white = createXColorFromRGBA(255, 255, 255, 255);
}

void draw()
{
//    fpsmeterc++;
//    if(fpsmeterc == FPSMETERSAMPLE) {
//        fpsmeterc = 0;
//        t1 = t2;
//        t2 = std::chrono::high_resolution_clock::now();
//        duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
//        fpsstring = /*to_string(duration) + " / " +*/ to_string(1000000*FPSMETERSAMPLE/duration);
//    }


    GC gc;
    XGCValues gcv;

    /*// Line width and type
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gcv.foreground = red.pixel;

    unsigned long mask = GCLineWidth | GCLineStyle | GCForeground;
    gc = XCreateGC(g_display, g_win, mask, &gcv);*/
    
    gc = XCreateGC (g_display, g_win, 0, 0);
    XSetBackground (g_display, gc, white.pixel); 
    XSetForeground (g_display, gc, textColor.pixel);

    XFontStruct * font;
    // const char * fontname = "-misc-fixed-bold-r-normal--18-120-100-100-c-90-iso8859-2";
    // const char * fontname = "rk24"; // ~ chinese shit
    // list_fonts(); 

    const char * fontname = "9x15bold";
    font = XLoadQueryFont (g_display, fontname);
    /* If the font could not be loaded, revert to the "fixed" font. */
    if (!font) {
//        fprintf (stderr, "unable to load font %s > using fixed\n", fontname);
        font = XLoadQueryFont (g_display, "fixed");
    }
    XSetFont (g_display, gc, font->fid);

    XSetForeground (g_display, gc, backgroundColor.pixel);
    XFillRectangle(g_display, g_win, gc, 0, 0, g_disp_width,g_disp_height);

    XSetForeground (g_display, gc, textColor.pixel);
//    if(duration > 0.0f) {
//        const char * text = fpsstring.c_str();
        const char * text = "FOO BAZZ";
        XDrawString(g_display, g_win, gc, 10, 20, text, 7);
//    }

    XFreeGC(g_display, gc);

}


void handleCapslockChange(int isON)
{
    if(isON)
        XMapWindow(g_display, g_win);
    else
        XUnmapWindow(g_display, g_win);
        std::cout <<"FOOBAR."  << isON << std::endl;
}
/*
 * 
 */
int main(int argc, char** argv) {
//    Display * g_display;
    Window root;
    XWindowAttributes attr;
    XEvent event;
    
    
    
  int maj = XkbMajorVersion;
  int min = XkbMinorVersion;
  int code;

  int           _XkbEventBase;
  int XkbErrorBase;
    
    if(!(g_display = XOpenDisplay(0x0))) return 1;
    root = DefaultRootWindow(g_display);
    
  if (!XkbLibraryVersion(&maj, &min))
      return false;
          
  if (!XkbQueryExtension(g_display, &code, &_XkbEventBase, &XkbErrorBase,
                         &maj, &min))
      return false;
    
  if (!XkbSelectEvents(g_display, XkbUseCoreKbd, XkbIndicatorStateNotifyMask,
                       XkbIndicatorStateNotifyMask))
      return false;
              
    g_screen    = DefaultScreen(g_display);

    g_disp_width  = DisplayWidth(g_display, g_screen);
    g_disp_height = DisplayHeight(g_display, g_screen);
    
    createShapedWindow();
    
    int _states[3];
  unsigned int state;
  XkbGetIndicatorState(g_display, XkbUseCoreKbd, &state);
  for( int bit = 0; bit < 3; ++bit)
  {
    _states[bit] = state & (1 << bit);
  }
                 std::cout <<"FOOBAR."<<std::endl;
                 int capsBit = 0;
                 
                 draw();
        for(;;)
    {
                 XNextEvent(g_display, &event);
                 
                if( event.type != _XkbEventBase + XkbEventCode) continue;
                 
                XkbEvent* xkbEvent = reinterpret_cast<XkbEvent*>(&event);
                
                if( xkbEvent->any.xkb_type != XkbIndicatorStateNotify) continue;
                
                if(xkbEvent->indicators.changed != capsBit+1)
                    continue;
                
                int state  = xkbEvent->indicators.state;
                int isON = state & (1 << capsBit);
                
                handleCapslockChange(isON);                
                draw();
   }
    return 0;
}

