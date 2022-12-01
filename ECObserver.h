//
//  ECObserver.h
//
//
//  Created by Yufeng Wu on 2/27/20.
//
//

#ifndef ECOBERVER_H
#define ECOBERVER_H

#include <vector>
#include <algorithm>
#include <iostream>

#include "ECWindowObject.h"

using namespace std;
//********************************************
// Observer design pattern: observer interface
// Supported event codes

// Event Type Copy
enum ECGVEventTypeRef
{
    ECGV_REF_EV_NULL = -1,
    ECGV_REF_EV_CLOSE = 0,
    ECGV_REF_EV_KEY_UP_UP = 1,
    ECGV_REF_EV_KEY_UP_DOWN = 2,
    ECGV_REF_EV_KEY_UP_LEFT = 3,
    ECGV_REF_EV_KEY_UP_RIGHT = 4,
    ECGV_REF_EV_KEY_UP_ESCAPE = 5,
    ECGV_REF_EV_KEY_DOWN_UP = 6,
    ECGV_REF_EV_KEY_DOWN_DOWN = 7,
    ECGV_REF_EV_KEY_DOWN_LEFT = 8,
    ECGV_REF_EV_KEY_DOWN_RIGHT = 9,
    ECGV_REF_EV_KEY_DOWN_ESCAPE = 10,
    ECGV_REF_EV_TIMER = 11,
    ECGV_REF_EV_MOUSE_BUTTON_DOWN = 12,
    ECGV_REF_EV_MOUSE_BUTTON_UP = 13,
    ECGV_REF_EV_MOUSE_MOVING = 14,
    // more keys
    ECGV_REF_EV_KEY_UP_Z = 15,
    ECGV_REF_EV_KEY_DOWN_Z = 16,
    ECGV_REF_EV_KEY_UP_Y = 17,
    ECGV_REF_EV_KEY_DOWN_Y = 18,
    ECGV_REF_EV_KEY_UP_D = 19,
    ECGV_REF_EV_KEY_DOWN_D = 20,
    ECGV_REF_EV_KEY_UP_SPACE = 21,
    ECGV_REF_EV_KEY_DOWN_SPACE = 22,
    ECGV_REF_EV_KEY_DOWN_G = 23,
    ECGV_REF_EV_KEY_UP_G = 24,
    ECGV_REF_EV_KEY_DOWN_F = 25,
    ECGV_REF_EV_KEY_UP_F = 26,
    ECGV_REF_EV_KEY_DOWN_CTRL = 27,
    ECGV_REF_EV_KEY_UP_CTRL = 28
};

// View Mode
enum ECGRAPHICVIEW_MODE {
    ECGRAPHICVIEW_EDITMODE = 1,
    ECGRAPHICVIEW_INSERTIONMODE = 2
};

// Text Alignment
enum ALLEGRO_ALIGN {
    ALLEGRO_ALIGN_REF_LEFT = 0,
    ALLEGRO_ALIGN_REF_RIGHT = 2
};

class ECObserverSubject;
class ECObserver
{
public:
    virtual ~ECObserver() {}
    virtual void Update(ECGVEventTypeRef event) = 0;
protected :
    ECObserverSubject* _view;
};

class SpaceUp : public ECObserver {
    public :
        SpaceUp(ECObserverSubject* view) {
            _view = view;
        }
        ~SpaceUp() {}
        void Update(ECGVEventTypeRef event);
};

class MouseDown : public ECObserver {
    public :
        MouseDown(ECObserverSubject* view) {
            _view = view;
        }
        ~MouseDown() {}
        void Update(ECGVEventTypeRef event);
};

class MouseUp : public ECObserver {
    public :
        MouseUp(ECObserverSubject* view) {
            _view = view;
        }
        ~MouseUp() {}
        void Update(ECGVEventTypeRef event);
        void InsertMode();
        void EditMode();
};

class MouseMoving : public ECObserver {
    public :
        MouseMoving(ECObserverSubject* view) {
            _view = view;
        }
        ~MouseMoving() {}
        void Update(ECGVEventTypeRef event);
        void InsertMode();
        void EditMode();
};

class DKeyUp : public ECObserver {
    public :
        DKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~DKeyUp() {}
        void Update(ECGVEventTypeRef event);
};

class ZKeyUp : public ECObserver {
    public :
        ZKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~ZKeyUp() {}
        void Update(ECGVEventTypeRef event);
};

class YKeyUp : public ECObserver {
    public :
    YKeyUp(ECObserverSubject* view) {
        _view = view;
    }
    ~YKeyUp() {}
    void Update(ECGVEventTypeRef event);
};

//********************************************
// Observer design pattern: subject
class ECGraphicViewImp;

class ECObserverSubject
{
public:
    virtual ~ECObserverSubject() {}
    ECObserverSubject() {}
    ECObserverSubject(ECGraphicViewImp* gv);

    // High Level View Settings
    ECGraphicViewImp* _view;
    ECGRAPHICVIEW_MODE _mode;
    std::string _modeStr;
    vector<ECWindowObject*> _undo;
    vector<ECWindowObject*> _redo;
    std::string _warning;
    std::vector<ECWindowObject*> _windowObjects;

    // Cursor
    bool _mouseDown;
    bool _firstMove;
    int cursorxDown, cursoryDown, cursorxUp, cursoryUp;

    // Edit Mode
    bool _isEditingRect;
    ECRectObject* _editingRect;
    ECWindowObject* _objBeforeEdit;
    float firstClickX;
    float firstClickY;
    float x1Difference;
    float x2Difference;
    float y1Difference;
    float y2Difference;

    // Debug
    int id;

    // View Functions
    void Clear(ECGVColorRef color);
    void FlipDisplay();
    bool isQueueEmpty();
    std::__1::__wrap_iter<ECWindowObject **> objectIndexInWindow(ECWindowObject* obj);

    // Draw Functions
    void DrawAllObjects();
    void DrawRectangle(float x1, float y1, float x2, float y2, int thickness, ECGVColorRef);
    void DrawModeLabel();
    void DrawWarningLabel();

    // Cursor Functions
    int getCurrentCursorX();
    int getCurrentCursorY();

    // Editing Functions
    bool isPointInsideRect(ECRectObject* _rect, float xp, float yp);
    bool isClickInsideRect(float xp, float yp);

    // Observer Functions
    void Attach( ECObserver *pObs )
    {
        std::cout << "Adding an observer.\n";
        listObservers.push_back(pObs);
        cout << "New observer size: " << listObservers.size() << endl;
    }
    void Detach( ECObserver *pObs )
    {
        //listObservers.erase(std::remove(listObservers.begin(), listObservers.end(), pObs), listObservers.end());
    }
    void Notify();

    int getObsSize() { return listObservers.size(); }
    
private:
    std::vector<ECObserver *> listObservers;
};


#endif
