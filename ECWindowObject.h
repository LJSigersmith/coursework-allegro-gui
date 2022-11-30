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

        ECWindowObject* _editedFrom;
        //ECECWindowObject* _deletedTwin;
        //ECECWindowObject* _aliveTwin;
        ECGVColorRef _color;
        int id;
        //bool _wasDeleted;
};

class ECDeletedObject : public ECWindowObject {
    public :
        ECDeletedObject(ECWindowObject* alive) {
            _alive = alive;
        }
        ~ECDeletedObject() {}

        ECWindowObject* _alive;
};

class ECRectObject : public ECWindowObject {

public :
   
    ECRectObject(int x1, int y1, int x2, int y2, int thickness, ECGVColorRef color) {
        _x1 = x1;
        _y1 = y1; 
        _x2 = x2;
        _y2 = y2;
        _thickness = thickness;
        _color = color;
        _editedFrom = NULL;
        //_wasDeleted = false;
        //_deletedTwin = NULL;
        //_aliveTwin = NULL;
    }

    ~ECRectObject() {

    }

    int _x1;
    int _y1; 
    int _x2;
    int _y2;
    int _thickness;
};

#endif