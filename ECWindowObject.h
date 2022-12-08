#ifndef ECWindowObject_h
#define ECWindowObject_h

#include <vector>

enum ECGVColorRef
{
    ECGV_REF_BLACK = 0,
    ECGV_REF_WHITE = 1,
    ECGV_REF_RED = 2,
    ECGV_REF_GREEN = 3,
    ECGV_REF_BLUE = 4,
    ECGV_REF_YELLOW = 5,    // red + green
    ECGV_REF_PURPLE = 6,    // red+blue
    ECGV_REF_CYAN = 7,      // blue+green,
    ECGV_REF_GRAY = 8,
    ECGV_REF_NONE = 9,
    ECGV_REF_NUM_COLORS
};

class ECWindowObject {
    public :
        virtual ~ECWindowObject() {};

        // If object was created by being edited,
        // store what object was edited
        ECWindowObject* _editedFrom;
        ECWindowObject* _inGroup;
        
        ECGVColorRef _color;
        bool _fill;

        int id;

        void setFilled(bool filled) { _fill = filled; }
        bool isFilled() { return _fill; }
};

class ECCollectionObject : public ECWindowObject {
     public :
         ECCollectionObject() {}
         ~ECCollectionObject() {}

        std::vector<ECWindowObject*> _collectionObjects;

        void addObject(ECWindowObject* obj) { _collectionObjects.push_back(obj); }
        std::vector<ECWindowObject*>* objectsInGroup() { return &_collectionObjects; }
};

// Empty Object, just represents an object that was deleted
class ECDeletedObject : public ECWindowObject {
    public :
        // Set alive object to actual object
        ECDeletedObject(ECWindowObject* alive) {
            _alive = alive;
        }
        ~ECDeletedObject() {}

        ECWindowObject* _alive;
};

class ECRectObject : public ECWindowObject {

public :
   
   // Rectangle Object
    ECRectObject(int x1, int y1, int x2, int y2, int thickness, ECGVColorRef color, bool filled) {
        _x1 = x1;
        _y1 = y1; 
        _x2 = x2;
        _y2 = y2;
        _thickness = thickness;
        _color = color;
        _editedFrom = NULL;
        _fill = filled;
    }

    ~ECRectObject() {

    }

    int _x1;
    int _y1; 
    int _x2;
    int _y2;
    int _thickness;
    
    int _x1DistFromCursor;
    int _x2DistFromCursor;
    int _y1DistFromCursor;
    int _y2DistFromCursor;

    void setDistFromCursor(int cursorX, int cursorY) {
        _x1DistFromCursor = cursorX - _x1;
        _x2DistFromCursor = cursorX - _x2;
        _y1DistFromCursor = cursorY - _y1;
        _y2DistFromCursor = cursorY - _y2;
    }

    void getDistFromCursor(int &x1Dist, int &y1Dist, int &x2Dist, int &y2Dist) {
        x1Dist = _x1DistFromCursor;
        y1Dist = _y1DistFromCursor;
        x2Dist = _x2DistFromCursor;
        y2Dist = _y2DistFromCursor;
    }
};

class ECEllipseObject : public ECWindowObject {

    public :
        
        // Ellipse Object
        ECEllipseObject(int xc, int yc, int xr, int yr, int thickness, ECGVColorRef color, bool filled) {
            _xCenter = xc;
            _yCenter = yc;
            _xRadius = xr;
            _yRadius = yr;
            _thickness = thickness;
            _color = color;
            _editedFrom = NULL;
            _fill = filled;
        }

        ~ECEllipseObject() {}

        int _xCenter;
        int _yCenter;
        int _xRadius;
        int _yRadius;
        int _thickness;

        int _xCDistFromCursor;
        int _yCDistFromCursor;

        void setDistFromCursor(int cursorX, int cursorY) {
            _xCDistFromCursor = cursorX - _xCenter;
            _yCDistFromCursor = cursorY - _yCenter;
        }
        void getDistFromCursor(int &xcDist, int &ycDist) {
            xcDist = _xCDistFromCursor;
            ycDist = _yCDistFromCursor;
        }
};

#endif