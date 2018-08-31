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

using namespace std;

void handleCapslockChange(int isON)
{
                std::cout <<"FOOBAR."  << isON << std::endl;
}
/*
 * 
 */
int main(int argc, char** argv) {
    Display * dpy;
    Window root;
    XWindowAttributes attr;
    XEvent event;
    
  int maj = XkbMajorVersion;
  int min = XkbMinorVersion;
  int code;

  int           _XkbEventBase;
  int XkbErrorBase;
    
    if(!(dpy = XOpenDisplay(0x0))) return 1;
    root = DefaultRootWindow(dpy);
    
  if (!XkbLibraryVersion(&maj, &min))
      return false;
          
  if (!XkbQueryExtension(dpy, &code, &_XkbEventBase, &XkbErrorBase,
                         &maj, &min))
      return false;
    
  if (!XkbSelectEvents(dpy, XkbUseCoreKbd, XkbIndicatorStateNotifyMask,
                       XkbIndicatorStateNotifyMask))
      return false;
              
    int _states[3];
  unsigned int state;
  XkbGetIndicatorState(dpy, XkbUseCoreKbd, &state);
  for( int bit = 0; bit < 3; ++bit)
  {
    _states[bit] = state & (1 << bit);
  }
                 std::cout <<"FOOBAR."<<std::endl;
                 int capsBit = 0;
        for(;;)
    {
                 XNextEvent(dpy, &event);
                 
                if( event.type != _XkbEventBase + XkbEventCode) continue;
                 
                XkbEvent* xkbEvent = reinterpret_cast<XkbEvent*>(&event);
                
                if( xkbEvent->any.xkb_type != XkbIndicatorStateNotify) continue;
                
                if(xkbEvent->indicators.changed != capsBit+1)
                    continue;
                
                int state  = xkbEvent->indicators.state;
                int isON = state & (1 << capsBit);
                
                handleCapslockChange(isON);                
   }
    return 0;
}

