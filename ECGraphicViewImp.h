//
//  ECGraphicViewImp.h
//  
//
//  Created by Yufeng Wu on 3/2/22.
//

//***********************************************************
//          GraphicView version 1.0.0 alpha
//                  March 2, 2022
//                  By Yufeng Wu (all rights reserved)
//    Disclaimer: this code builts on Allegro game engine
//
//***********************************************************

#ifndef ECGraphicViewImp_h
#define ECGraphicViewImp_h

#include <vector>
#include <map>
#include <string>
#include "ECObserver.h"
#include <allegro5/allegro.h>

//***********************************************************
// Supported event codes

enum ECGVEventType
{
    ECGV_EV_NULL = -1,
    ECGV_EV_CLOSE = 0,
    ECGV_EV_KEY_UP_UP = 1,
    ECGV_EV_KEY_UP_DOWN = 2,
    ECGV_EV_KEY_UP_LEFT = 3,
    ECGV_EV_KEY_UP_RIGHT = 4,
    ECGV_EV_KEY_UP_ESCAPE = 5,
    ECGV_EV_KEY_DOWN_UP = 6,
    ECGV_EV_KEY_DOWN_DOWN = 7,
    ECGV_EV_KEY_DOWN_LEFT = 8,
    ECGV_EV_KEY_DOWN_RIGHT = 9,
    ECGV_EV_KEY_DOWN_ESCAPE = 10,
    ECGV_EV_TIMER = 11,
    ECGV_EV_MOUSE_BUTTON_DOWN = 12,
    ECGV_EV_MOUSE_BUTTON_UP = 13,
    ECGV_EV_MOUSE_MOVING = 14,
    // more keys
    ECGV_EV_KEY_UP_Z = 15,
    ECGV_EV_KEY_DOWN_Z = 16,
    ECGV_EV_KEY_UP_Y = 17,
    ECGV_EV_KEY_DOWN_Y = 18,
    ECGV_EV_KEY_UP_D = 19,
    ECGV_EV_KEY_DOWN_D = 20,
    ECGV_EV_KEY_UP_SPACE = 21,
    ECGV_EV_KEY_DOWN_SPACE = 22,
    ECGV_EV_KEY_DOWN_G = 23,
    ECGV_EV_KEY_UP_G = 24
};

//***********************************************************
// Pre-defined color

enum ECGVColor
{
    ECGV_BLACK = 0,
    ECGV_WHITE = 1,
    ECGV_RED = 2,
    ECGV_GREEN = 3,
    ECGV_BLUE = 4,
    ECGV_YELLOW = 5,    // red + green
    ECGV_PURPLE = 6,    // red+blue
    ECGV_CYAN = 7,
    ECGV_GRAY = 8,
    ECGV_NONE = 9,
    ECGV_NUM_COLORS
};

// Allegro color
extern ALLEGRO_COLOR arrayAllegroColors[ECGV_NUM_COLORS];

class WindowObject {
    public :
        virtual ~WindowObject() {};
};

class RectObject : public WindowObject {

public :
   
    RectObject(int x1, int y1, int x2, int y2, int thickness, ECGVColor color) {
        _x1 = x1;
        _y1 = y1; 
        _x2 = x2;
        _y2 = y2;
        _thickness = thickness;
        _color = color;
    }

    ~RectObject() {

    }

    int _x1;
    int _y1; 
    int _x2;
    int _y2;
    int _thickness;
    ECGVColor _color;
};
//***********************************************************
// Drawing context (thickness and so on)

class ECDrawiingContext
{
public:
    ECDrawiingContext() : thickness(3), color(ECGV_NONE) {}
    void SetThickness(int t) { thickness = t; }
    int GetThickness() const { return thickness; }
    void SetColor(ECGVColor c) { color = c; }
    ECGVColor GetColor() const { return color; }
    
private:
    int thickness;
    ECGVColor color;
};


//***********************************************************
// A graphic view implementation
// This is built on top of Allegro library
//
// Note: ECGraphicViewImp implements *** Observer *** pattern
// It is the subject that accepts observers.
// Whenver something happens (i.e., a key is pressed), all observers
// are notified through Observer's Notify function
// then an observer would check for update (in this case, what key is pressed)
//
enum ECGRAPHICVIEW_MODE {
    ECGRAPHICVIEW_EDITMODE = 1,
    ECGRAPHICVIEW_INSERTIONMODE = 2
};

class ECGraphicViewImp : public ECObserverSubject
{
public:
    ECGraphicViewImp(int width, int height);
    virtual ~ECGraphicViewImp();
    
    // Show the view. This would enter a forever loop, until quit is set. To do things you want to do, implement code for event handling
    void Show();
    
    // Set flag to redraw (or not). Invoke SetRedraw(true) after you make changes to the view
    void SetRedraw(bool f) { fRedraw = f; }
    
    // Access view properties
    int GetWith() const { return widthView; }
    int GetHeight() const { return heightView; }
    
    // Get cursor position (cx, cy)
    void GetCursorPosition(int &cx, int &cy) const;
    
    // The current event
    ECGVEventType GetCurrEvent() const { return evtCurrent; }
    
    // Drawing functions
    void DrawLine(int x1, int y1, int x2, int y2, int thickness=3, ECGVColor color=ECGV_BLACK);
    void DrawRectangle(int x1, int y1, int x2, int y2, int thickness=3, ECGVColor color=ECGV_BLACK);
    void DrawFilledRectangle(int x1, int y1, int x2, int y2, ECGVColor color=ECGV_BLACK);
    void DrawCircle(int xcenter, int ycenter, double radius, int thickness=3, ECGVColor color=ECGV_BLACK);
    void DrawFilledCircle(int xcenter, int ycenter, double radius, ECGVColor color=ECGV_BLACK);
    void DrawEllipse(int xcenter, int ycenter, double radiusx, double radiusy, int thickness=3, ECGVColor color=ECGV_BLACK);
    void DrawFilledEllipse(int xcenter, int ycenter, double radiusx, double radiusy, ECGVColor color=ECGV_BLACK);
    void DrawText(float x, float y, float sz, ALLEGRO_COLOR color, int alignment, std::string text);

    // Display Functions
    void Clear(ECGVColor color) {
        al_clear_to_color(arrayAllegroColors[color]);
    }
    
private:
    // Internal functions
    // Initialize and reset view
    void Init();
    void Shutdown();
    
    // View utiltiles
    void RenderStart();
    void RenderEnd();
    
    // Process event
    ECGVEventType  WaitForEvent();
    
    // data members
    // size of view
    int widthView;
    int heightView;
    
    // whether to redraw or not
    bool fRedraw;
    
    // keep track of what happened to view
    ECGVEventType evtCurrent;
    
    // allegro stuff
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *timer;

    ECGRAPHICVIEW_MODE _mode;
    std::string _modeStr;
    void DrawModeLabel();

    int cursorxDown, cursoryDown, cursorxUp, cursoryUp;
    bool _mouseDown, _firstMove;
    std::vector<ALLEGRO_BITMAP*> _bitmapLayers;
    std::vector<WindowObject*> _windowObjects;

    // Rectangle Functions
    //bool isPointInsideRect(float x1, float x2, float y1, float y2);
    bool isPointOnLineRect(RectObject* _rect, float xp, float yp);
};

#endif /* ECGraphicViewImp_h */
