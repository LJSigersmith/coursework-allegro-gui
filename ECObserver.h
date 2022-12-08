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

// Shape Mode
enum ECGRAPHICVIEW_SHAPE {
    ECGRAPHICVIEW_SHAPE_RECT = 1,
    ECGRAPHICVIEW_SHAPE_ELLIPSE = 2,
    ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED = 3,
    ECGRAPHICVIEW_SHAPE_RECT_FILLED = 4
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
        void MultiSelect();
        void EndMultiDrag();
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
        void MultiDrag();
};

class DKeyUp : public ECObserver {
    public :
        DKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~DKeyUp() {}
        void Update(ECGVEventTypeRef event);
};

class GKeyUp : public ECObserver {
    public :
        GKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~GKeyUp() {}
        void Update(ECGVEventTypeRef event);
        void GroupObjects();
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

class FKeyUp : public ECObserver {
    public :
        FKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~FKeyUp() {}

        void Update(ECGVEventTypeRef event);
};

class CtrlKeyDown : public ECObserver {
    public :
        CtrlKeyDown(ECObserverSubject* view) {
            _view = view;
        }
        ~CtrlKeyDown() {}

        void Update(ECGVEventTypeRef event);
};

class CtrlKeyUp : public ECObserver {
    public :
        CtrlKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~CtrlKeyUp() {}

        void Update(ECGVEventTypeRef event);
};

class ArrowKeyUp : public ECObserver {
    public :
        ArrowKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~ArrowKeyUp() {}

        void Update(ECGVEventTypeRef event);
        ECWindowObject* getMovedObject(ECWindowObject* objToMove);
        int xMovement;
        int yMovement;
};

class EscapeKeyUp : public ECObserver {
    public :
        EscapeKeyUp(ECObserverSubject* view) {
            _view = view;
        }
        ~EscapeKeyUp() {}

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
    void InitObserver();
    void setAppDefaults();
    void attachObservers();

    // High Level View Settings
    ECGraphicViewImp* _view;
    ECGRAPHICVIEW_MODE _mode;
    ECGRAPHICVIEW_SHAPE _shape;
    std::string _modeStr;
    std::string _shapeStr;
    std::string _lastShapeStr; // for when switching back to insert from edit
    vector<ECWindowObject*> _undo;
    vector<ECWindowObject*> _redo;
    std::string _warning;
    std::vector<ECWindowObject*> _windowObjects;

    // Cursor
    bool _mouseDown;
    bool _firstMove;
    int cursorxDown, cursoryDown, cursorxUp, cursoryUp;
    float firstClickX;
    float firstClickY;

    // Edit Mode
    bool _isEditingObj;
    ECWindowObject* _editingObj;
    ECWindowObject* _objBeforeEdit;
    ECWindowObject* _lastDrawnEditObject;

    // Multi Select
    bool _multiSelectEnabled;
    bool _multiDragEnabled;
    vector<ECWindowObject*> _selectedObjects;
    vector<ECWindowObject*> _movingObjects;

    // Grouping
    bool _hasSelectedObjectInGroup;
    ECGroupObject* _selectedGroup;
    ECWindowObject* _selectedObjectInGroup;

    // Arrow Key Movement
    bool _arrowMovementEnabled;

    // Difference For Rect
    float x1Difference;
    float x2Difference;
    float y1Difference;
    float y2Difference;

    // Difference for Ellipse
    float xCDifference;
    float yCDifference;

    // Debug
    int id;

    // View Functions
    void Clear(ECGVColorRef color);
    void FlipDisplay();
    bool isQueueEmpty();
    std::__1::__wrap_iter<ECWindowObject **> objectIndexInWindow(ECWindowObject* obj);
    std::__1::__wrap_iter<ECWindowObject **> objectIndexInSelectedObjects(ECWindowObject* obj);

    // Draw Functions
    void DrawAllObjects();
    void DrawRectangle(float x1, float y1, float x2, float y2, int thickness, ECGVColorRef, bool filled);
    void DrawEllipse(int xC, int yC, int xR, int yR, int thickness, ECGVColorRef, bool filled);
    
    void DrawRectangle(ECRectObject* rect);
    void DrawEllipse(ECEllipseObject* ellipse);
    void DrawGroup(ECGroupObject* group);

    void DrawModeLabel();
    void DrawWarningLabel();
    void DrawShapeLabel();

    // Cursor Functions
    int getCurrentCursorX();
    int getCurrentCursorY();

    // Editing Functions
    bool isPointInsideRect(ECRectObject* _rect, float xp, float yp);
    bool isPointInsideEllipse(ECEllipseObject*, float xp, float yp);
    bool isClickInsideObj(float xp, float yp);

    // Multi Drag Functions
    ECWindowObject* getNewMovingObject(ECWindowObject* original, int x, int y);
    ECRectObject* getMovingRect(ECRectObject* originalRect, int x, int y);
    ECEllipseObject* getMovingEllipse(ECEllipseObject* originalEllipse, int x, int y);
    ECGroupObject* getMovingGroup(ECGroupObject* originalGroup, int x, int y);

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
