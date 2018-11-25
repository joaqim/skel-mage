//Solution using Xlib for those who use Linux
#include <X11/Xlib.h>
#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* Check if this window has the name we seek */
bool windowHasName(Display *display, char const *needle) {
  char *window_name;

  Window root = DefaultRootWindow(display);
  if(XFetchName(display, root, &window_name) > 0) {
    int r = strcmp(needle, window_name);
    XFree(window_name);
    if(r == 0) {
      return true;
    }
  }
  return false;
}


void mouseClick(int button) {
  Display *display = XOpenDisplay(NULL);

  XEvent event;

  if(display == NULL) {
    fprintf(stderr, "Cannot initialize the display\n");
    exit(EXIT_FAILURE);
  }

  memset(&event, 0x00, sizeof(event));

  event.type = ButtonPress;
  event.xbutton.button = button;
  event.xbutton.same_screen = True;

  XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

  event.xbutton.subwindow = event.xbutton.window;

  while(event.xbutton.subwindow)
  {
    event.xbutton.window = event.xbutton.subwindow;

    XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  }

  if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");

  XFlush(display);

  usleep(100000);

  event.type = ButtonRelease;
  event.xbutton.state = 0x100;

  if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");

  XFlush(display);

  XCloseDisplay(display);
}

class MouseInput {
 public:
  MouseInput() {
#if 0
    _pDisplay = XOpenDisplay(NULL);
    if(_pDisplay == NULL) {
      fprintf(stderr, "Cannot initialize the display\n");
      exit(EXIT_FAILURE);
    }


    _window = DefaultRootWindow(_pDisplay);
#endif
  }

  void click(int const x, int const y, unsigned int const button=Button2) {
#if 1

    //XWarpPointer(_pDisplay, None, _window, 0, 0, 0, 0, x, y);
    if(windowHasName(_pDisplay, "Diablo III"))
      mouseClick(button);
#else
    XWarpPointer(_pDisplay, None, _window, 0, 0, 0, 0, x, y);

    memset(&_event, 0x00, sizeof(_event));


    _event.type = ButtonPress;
    _event.xbutton.button = button;
    _event.xbutton.same_screen = True;

    XQueryPointer(_pDisplay, RootWindow(_pDisplay, DefaultScreen(_pDisplay)), &_event.xbutton.root, &_event.xbutton.window, &_event.xbutton.x_root, &_event.xbutton.y_root, &_event.xbutton.x, &_event.xbutton.y, &_event.xbutton.state);

    _event.xbutton.subwindow = _event.xbutton.window;

    while(_event.xbutton.subwindow)
    {
      _event.xbutton.window = _event.xbutton.subwindow;

      XQueryPointer(_pDisplay, _event.xbutton.window, &_event.xbutton.root, &_event.xbutton.subwindow, &_event.xbutton.x_root, &_event.xbutton.y_root, &_event.xbutton.x, &_event.xbutton.y, &_event.xbutton.state);
    }

    if(XSendEvent(_pDisplay, PointerWindow, True, 0xfff, &_event) == 0) fprintf(stderr, "Error\n");

    XFlush(_pDisplay);

    usleep(100000);

    _event.type = ButtonRelease;
    _event.xbutton.state = 0x100;

    if(XSendEvent(_pDisplay, PointerWindow, True, 0xfff, &_event) == 0) fprintf(stderr, "Error\n");
    XFlush(_pDisplay);

    //    XCloseDisplay(_pDisplay);
#endif
  }
  ~MouseInput() {
#if 0
    XFlush(_pDisplay);
    XCloseDisplay(_pDisplay);
#endif

  }
 private:
  Display *_pDisplay;
  Window _window;
  XEvent _event;
};

#if 0
int main(int argc,char * argv[]) {

 
  Display *display = XOpenDisplay(NULL);

  Window root = DefaultRootWindow(display);
  int x , y;
  if(argc > 1) {
    x=atoi(argv[1]);
    y=atoi(argv[2]);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
  }
  mouseClick(Button1);
  XFlush(display);
  XCloseDisplay(display);
  return 0;
}
#endif
