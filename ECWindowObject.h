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
        ECWindowObject* _editedFrom = nullptr;
        ECWindowObject* _inGroup = nullptr;
        
        ECGVColorRef _color;
        bool _fill;

        int id;

        void setFilled(bool filled) { _fill = filled; }
        bool isFilled() { return _fill; }
        virtual void setColor(ECGVColorRef color) = 0;

        bool _createdFromUngrouping = false;
        ECWindowObject* _fromGroup = nullptr;
        std::vector<ECWindowObject*>* _associatedFromGroupObjects;
};

using namespace std;
class ECMultiSelectionObject : public ECWindowObject {
    public :

        ECMultiSelectionObject() {}
        ~ECMultiSelectionObject() {}
        std::vector<ECWindowObject*> _objectsInSelection;
        std::vector<ECWindowObject*> _objectsBeforeSelectionMoved;

        void addObjectToSelection(ECWindowObject* obj) {
            _objectsInSelection.push_back(obj);
        }
        void addObjectToSelectionBeforeMoved(ECWindowObject* obj) {
            _objectsBeforeSelectionMoved.push_back(obj);
        }
        void setColor(ECGVColorRef color) { 
            for (auto obj : _objectsInSelection) {
                std::cout << "setiing color" << std::endl;
                if (color == ECGV_REF_BLACK) { std::cout<<"BLACK"<<std::endl;}
                obj->_color = color;
            }
        }
        bool objectInSelection(ECWindowObject* obj, bool eraseIfFound) {
            if (_objectsInSelection.size() == 0) { return false; }
            auto i = _objectsInSelection.begin();
            for (auto selObj : _objectsInSelection) {
                if (selObj == obj) {
                    break;
                }
                i++;
            }
            if (eraseIfFound && i != _objectsInSelection.end()) { _objectsInSelection.erase(i); }
            return i != _objectsInSelection.end();
        }
        bool objectInSelectionBeforeMoved(ECWindowObject* obj, bool eraseIfFound) {
            if (_objectsBeforeSelectionMoved.size() == 0) { return false; }
            auto i = _objectsBeforeSelectionMoved.begin();
            for (auto selObj : _objectsBeforeSelectionMoved) {
                if (selObj == obj) {
                    break;
                }
                i++;
            }
            if (eraseIfFound && i != _objectsBeforeSelectionMoved.end()) { _objectsBeforeSelectionMoved.erase(i); }
            return i != _objectsBeforeSelectionMoved.end();
        }
};

// Empty Object, just represents an object that was deleted
class ECDeletedObject : public ECWindowObject {
    public :
        // Set alive object to actual object
        ECDeletedObject(ECWindowObject* alive) {
            _alive = alive;
        }
        ~ECDeletedObject() {}


        void setColor(ECGVColorRef color) { std::cout << "don't do this" << std::endl; }
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

    ECRectObject(ECRectObject* rect) {
        _x1 = rect->_x1;
        _y1 = rect->_y1; 
        _x2 = rect->_x2;
        _y2 = rect->_y2;
        _thickness = rect->_thickness;
        _color = rect->_color;
        _editedFrom = rect->_editedFrom;
        _fill = rect->_fill;
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

    void setColor(ECGVColorRef color) { _color = color; }

    void setDistFromCursor(int cursorX, int cursorY) {
        _x1DistFromCursor = cursorX - _x1;
        _x2DistFromCursor = cursorX - _x2;
        _y1DistFromCursor = cursorY - _y1;
        _y2DistFromCursor = cursorY - _y2;
    }

    void setDistFromCursor(int x1Dist, int y1Dist, int x2Dist, int y2Dist) {
        _x1DistFromCursor = x1Dist;
        _y1DistFromCursor = y1Dist;
        _x2DistFromCursor = x2Dist;
        _y2DistFromCursor = y2Dist;
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

        ECEllipseObject(ECEllipseObject* ellipse) {
            _xCenter = ellipse->_xCenter;
            _yCenter = ellipse->_yCenter;
            _xRadius = ellipse->_xRadius;
            _yRadius = ellipse->_yRadius;
            _thickness = ellipse->_thickness;
            _color = ellipse->_color;
            _editedFrom = ellipse->_editedFrom;
            _fill = ellipse->_fill;
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
        
        // _ doesnt do anything, just need another arg to be able to overload
        void setDistFromCursor(int xcDist, int ycDist, int _) {
            _xCDistFromCursor = xcDist;
            _yCDistFromCursor = ycDist;
        }

        void getDistFromCursor(int &xcDist, int &ycDist) {
            xcDist = _xCDistFromCursor;
            ycDist = _yCDistFromCursor;
        }

        void setColor(ECGVColorRef color) { _color = color; }
};

class ECGroupObject : public ECWindowObject {
     public :
         ECGroupObject() { _isMoving = false; }
         ~ECGroupObject() {}

        ECGroupObject(ECGroupObject* group) {
            for (auto obj : group->_collectionObjects) {
                ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
                ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(ellipse);
                ECGroupObject* group = dynamic_cast<ECGroupObject*>(group);
                
                ECWindowObject* newObject;
                if (rect) {
                    ECRectObject* newRect = new ECRectObject(rect);
                    newObject = newRect;
                } else if (ellipse) {
                    ECEllipseObject* newEllipse = new ECEllipseObject(ellipse);
                    newObject = newEllipse;
                } else if (group) {
                    ECGroupObject* newGroup = new ECGroupObject(group);
                    newObject = newGroup;
                }

                _collectionObjects.push_back(newObject);
            }
            _objectsBeforeGroup = vector<ECWindowObject*>(group->_objectsBeforeGroup.begin(), group->_objectsBeforeGroup.end());
        }

        ECGroupObject(ECGroupObject* group, ECWindowObject* without) {
            for (auto obj : group->_collectionObjects) {
                if (obj == without) { continue; }
                ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
                ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(ellipse);
                ECGroupObject* group = dynamic_cast<ECGroupObject*>(group);
                
                ECWindowObject* newObject;
                if (rect) {
                    ECRectObject* newRect = new ECRectObject(rect);
                    newObject = newRect;
                } else if (ellipse) {
                    ECEllipseObject* newEllipse = new ECEllipseObject(ellipse);
                    newObject = newEllipse;
                } else if (group) {
                    ECGroupObject* newGroup = new ECGroupObject(group);
                    newObject = newGroup;
                }

                _collectionObjects.push_back(newObject);
            }
            _objectsBeforeGroup = vector<ECWindowObject*>(group->_objectsBeforeGroup.begin(), group->_objectsBeforeGroup.end());
        }

         ECGroupObject(std::vector<ECWindowObject*> objects) {
            _collectionObjects = objects;
         }

        bool _isMoving;
        std::vector<ECWindowObject*> _collectionObjects;
        std::vector<ECWindowObject*> _objectsBeforeGroup; // store objects as they were before they were grouped (for creation of group)
        std::vector<ECWindowObject*> _objectsBeforeUngroup; // store objects as they were before they were ungrouped (if ungrouped by g key)

        void addObject(ECWindowObject* obj) { _collectionObjects.push_back(obj); }
        void addObjectFromBeforeGrouping(ECWindowObject* obj) { _objectsBeforeGroup.push_back(obj); }
        std::vector<ECWindowObject*>* objectsInGroup() { return &_collectionObjects; }

        void setDistFromCursor(int cursorX, int cursorY);

        void removeObjFromGroup(ECWindowObject* objToRemove) {
            auto i = _collectionObjects.begin();
            for (auto obj : _collectionObjects) {
                if (obj == objToRemove) {
                    break;
                }
            }
            if (i != _collectionObjects.end()) { _collectionObjects.erase(i); }
        }

        bool _isTemp = false;

        void setColor(ECGVColorRef color) { 
            for (auto obj : _collectionObjects) {
                std::cout << "setiing color" << std::endl;
                if (color == ECGV_REF_BLACK) { std::cout<<"BLACK"<<std::endl;}
                obj->_color = color;
            }
        }
};

#endif