//
// ECObserver.cpp
//
//
//

#include "ECObserver.h"
#include "ECGraphicViewImp.h"

// ============================================================================
// =============================== Observers ==================================
// ============================================================================

// =============================== Escape Key =================================
void EscapeKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_ESCAPE) { return; }
    if (!_view->_arrowMovementEnabled) { return; }
    _view->_arrowMovementEnabled = false;
    
    if (_view->_editingObj) {
        _view->_editingObj->_color = ECGV_REF_BLACK;
    }
    _view->_isEditingObj = false;
    _view->_warning = "";

    _view->_selectedObjects.clear();
    _view->_multiDragEnabled = false;
    _view->_multiSelectEnabled = false;

    for (auto obj : _view->_windowObjects) {
        obj->_color = ECGV_REF_BLACK;
    }

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// =========================== Arrow Keys (UP) ================================
void ArrowKeyUp::Update(ECGVEventTypeRef event) {
    if (_view->_mode != ECGRAPHICVIEW_EDITMODE) { return; }
    if (_view->_multiSelectEnabled) { return; }
    switch (event)
    {
    case ECGV_REF_EV_KEY_UP_LEFT :
        xMovement = -10;
        yMovement = 0;
        break;
    case ECGV_REF_EV_KEY_UP_RIGHT :
        xMovement = 10;
        yMovement = 0;
        break;
    case ECGV_REF_EV_KEY_UP_DOWN :
        xMovement = 0;
        yMovement = 10;
        break;
    case ECGV_REF_EV_KEY_UP_UP :
        xMovement = 0;
        yMovement = -10;
        break;
    default:
        return;
        break;
    }

    _view->Clear(ECGV_REF_WHITE);
    if (_view->_multiDragEnabled) {
        // Move Collection
        _view->_arrowMovementEnabled = true;
        _view->_warning = "Key Editing ON";

        vector<ECWindowObject*> objectsToRemoveFromSelectedObjects(_view->_selectedObjects.begin(), _view->_selectedObjects.end());

        for (auto objToMove : _view->_selectedObjects) {
            ECWindowObject* movedObject = getMovedObject(objToMove);

            _view->_windowObjects.push_back(movedObject);
            _view->_selectedObjects.push_back(movedObject);

            _view->_undo.push_back(movedObject);
        }

        // Remove Old Selected Objects from Selected Objects
        for (auto objToRemove : objectsToRemoveFromSelectedObjects) {
            auto selectedObjIndexInSelectedObjects = _view->objectIndexInSelectedObjects(objToRemove);
            if (selectedObjIndexInSelectedObjects != _view->_selectedObjects.end()) {
                _view->_selectedObjects.erase(selectedObjIndexInSelectedObjects);
            }
        }

        _view->DrawAllObjects();

    } else if (_view->_isEditingObj) {
        _view->_arrowMovementEnabled = true;
        _view->_warning = "Key Editing ON";

        ECWindowObject* objToMove = _view->_editingObj;

        ECWindowObject* movedObject = getMovedObject(objToMove);

        _view->_windowObjects.push_back(movedObject);
        _view->_undo.push_back(movedObject);
        _view->_editingObj = movedObject;
        _view->DrawAllObjects();
    }
}

ECWindowObject* ArrowKeyUp::getMovedObject(ECWindowObject* objToMove) {
    ECWindowObject* movedObject;

    // Remove Being Moved Object from window
    auto removeObject = _view->objectIndexInWindow(objToMove);
    if (removeObject != _view->_windowObjects.end()) {
        _view->_windowObjects.erase(removeObject);
    }

    // Move Object
    ECRectObject* _editingRect = dynamic_cast<ECRectObject*>(objToMove);
    ECEllipseObject* _editingEllipse = dynamic_cast<ECEllipseObject*>(objToMove);

    if (_editingRect) {
        int x1 = _editingRect->_x1 + xMovement;
        int y1 = _editingRect->_y1 + yMovement;

        int x2 = _editingRect->_x2 + xMovement;
        int y2 = _editingRect->_y2 + yMovement;

        ECRectObject* movedRect = new ECRectObject(x1, y1, x2, y2, 1, ECGV_REF_BLUE, false);
        movedRect->_editedFrom = _editingRect;

        _view->DrawRectangle(x1, y1, x2, y2, 1, ECGV_REF_BLUE, false);
        movedObject = movedRect;
    } else if (_editingEllipse) {
        int xC = _editingEllipse->_xCenter + xMovement;
        int yC = _editingEllipse->_yCenter + yMovement;
        int xR = _editingEllipse->_xRadius;
        int yR = _editingEllipse->_yRadius;

        ECEllipseObject* movedEllipse = new ECEllipseObject(xC,yC,xR,yR,1,ECGV_REF_BLUE,false);
        movedEllipse->_editedFrom = _editingEllipse;

        _view->DrawEllipse(xC,yC,xR,yR,1,ECGV_REF_BLUE,false);
        movedObject = movedEllipse;
    } else { throw runtime_error("Cast Failed: ARROW KEY"); }

    return movedObject;
}

// Multi Select
// ============================ Ctrl Key Down =================================
void CtrlKeyDown::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_DOWN_CTRL) { return; }
    if (_view->_mode != ECGRAPHICVIEW_EDITMODE) { return; }
    if (_view->_multiDragEnabled) { return; }
    if (_view->_windowObjects.size() == 0) { return; }

    _view->_warning = "Multi Select ON";
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();

    _view->_multiSelectEnabled = true;
}

// ============================ Ctrl Key Up =================================
void CtrlKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_CTRL) { return; }
    if (_view->_mode != ECGRAPHICVIEW_EDITMODE) { return; }
    if (_view->_multiDragEnabled) { return; }
    if (_view->_selectedObjects.size() == 0) { _view->_multiSelectEnabled = false; _view->_multiDragEnabled = false; _view->_warning = ""; _view->Clear(ECGV_REF_WHITE); _view->DrawAllObjects(); return; }

    // If multi select enabled, disable it and enable multi drag
    if (_view->_multiSelectEnabled) {
        _view->_multiSelectEnabled = false;
        _view->_multiDragEnabled = true;
        _view->_warning = "";

        int cursorX = _view->getCurrentCursorX();
        int cursorY = _view->getCurrentCursorY();
        
        // Set all distance from cursor values
        for (auto obj : _view->_windowObjects) {
            ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
            ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(obj);

            if (rect) {
                rect->setDistFromCursor(cursorX,cursorY);
            } else if (ellipse) {
                ellipse->setDistFromCursor(cursorX,cursorY);
            } else { throw runtime_error("Cast failed : CTRL KEY"); }
        }

        _view->Clear(ECGV_REF_WHITE);
        _view->DrawAllObjects();
    }
}

// ============================== F Key Up ====================================
// Switch Between Filled and Unfilled
void FKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_F) { return; }
    cout << "FKEY" << endl;

    // In edit mode: doesn't matter what shape mode is enabled
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { _view->_warning = "Can't change shapes in Edit Mode"; return; }

    // Switch Shape Mode to Filled
    if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE) { _view->_shape = ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED; _view->_shapeStr = "Filled Ellipse"; }
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT) { _view->_shape = ECGRAPHICVIEW_SHAPE_RECT_FILLED; _view->_shapeStr = "Filled Rectangle"; }
    
    // Switch Shape Mode to Unfilled
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) { _view->_shape = ECGRAPHICVIEW_SHAPE_ELLIPSE; _view->_shapeStr = "Ellipse"; }
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) { _view->_shape = ECGRAPHICVIEW_SHAPE_RECT; _view->_shapeStr = "Rectangle"; }

    else { throw runtime_error("SHAPE is not defined : FKey"); }

    _view->_lastShapeStr = _view->_shapeStr;

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== G Key Up ====================================
void GKeyUp::GroupObjects() {
    // Stop Dragging
    _view->_multiDragEnabled = false;

    // Mouse has either moved since ending select and pressing g or has not
    // If mouse hasn't moved, moving objects will be empty
    std::vector<ECWindowObject*>* _targetObjects;
    bool objectsHaveMoved = false;
    if (_view->_movingObjects.size() == 0) {
        _targetObjects = &(_view->_selectedObjects);
    } else if (_view->_movingObjects.size() != 0 && _view->_selectedObjects.size() != 0) {
        _targetObjects = &(_view->_movingObjects);
        objectsHaveMoved = true;
    } else {
        throw runtime_error("No objects to group");
    }

    // Remove all group objects from window
    for (auto groupObj : *_targetObjects) {
        auto i = _view->objectIndexInWindow(groupObj);
        if (i == _view->_windowObjects.end()) { continue; }
        _view->_windowObjects.erase(i);
    }

    // Make New Group, set each object in group to be a member of group, and add obj to group
    // Create new instance of each object when adding to group
    ECGroupObject* newGroup = new ECGroupObject();
    for (auto obj : *_targetObjects) {
        ECWindowObject* groupObj;
        if (auto rect = dynamic_cast<ECRectObject*>(obj)) { groupObj = new ECRectObject(rect); }
        else if (auto ellipse = dynamic_cast<ECEllipseObject*>(obj)) { groupObj = new ECEllipseObject(ellipse); }
        else { throw runtime_error("Creating new group : selected obj is not rect or ellipse"); }
        
        groupObj->_inGroup = newGroup;
        groupObj->_color = ECGV_REF_BLACK;
        newGroup->addObject(groupObj);
        newGroup->addObjectFromBeforeGrouping(obj);
    }
    newGroup->_editedFrom = nullptr;

    // Add Group to Window and Undo stack
    _view->_windowObjects.push_back(newGroup);
    _view->_undo.push_back(newGroup);

    _view->_warning = "Shapes Grouped";

    _view->_movingObjects.clear();
    _view->_selectedObjects.clear();

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}
// Switch Between Ellipse and Rectangle
void GKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_G) { return; }

    // In edit mode: grouping
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { 
        if (_view->_multiSelectEnabled) {
            _view->_warning = "Finish Selecting to Group";
            _view->Clear(ECGV_REF_WHITE);
            _view->DrawAllObjects();
            return;
        } else if (_view->_multiDragEnabled) {
            GroupObjects();
            return;
        } else {
            _view->_warning = "Select Shapes to Group";
            _view->Clear(ECGV_REF_WHITE);
            _view->DrawAllObjects();
            return;
        }
    }
    
    // Switch Shape Mode to Ellipse
    if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT) { _view->_shape = ECGRAPHICVIEW_SHAPE_ELLIPSE; _view->_shapeStr = "Ellipse"; }
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) { _view->_shape = ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED; _view->_shapeStr = "Filled Ellipse"; }

    // Switch Shape Mode to Rectangle
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE) { _view->_shape = ECGRAPHICVIEW_SHAPE_RECT; _view->_shapeStr = "Rectangle"; }
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) { _view->_shape = ECGRAPHICVIEW_SHAPE_RECT_FILLED; _view->_shapeStr = "Filled Rectangle"; }
    
    else { throw runtime_error("SHAPE is not defined : GKey"); }

    _view->_lastShapeStr = _view->_shapeStr;

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== Space Up ====================================
// Switch Between Insert and Edit Modes
void SpaceUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_SPACE) { return; }
    
    _view->_warning = "";

    // Don't allow switching modes while currently editing or inserting
    if (_view->_isEditingObj || _view->_mouseDown) { return; }
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { _view->_mode = ECGRAPHICVIEW_INSERTIONMODE; _view->_modeStr = "Insert Mode"; _view->_shapeStr = _view->_lastShapeStr; }
    else if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { _view->_mode = ECGRAPHICVIEW_EDITMODE; _view->_modeStr = "Edit Mode"; _view->_lastShapeStr = _view->_shapeStr; _view->_shapeStr = ""; }
    else { throw runtime_error("MODE is not defined"); }

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== Y Key Up ====================================

// Redo
void YKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_Y) { return; }
    _view->Clear((ECGVColorRef) ECGV_WHITE);

    // If nothing to redo, set warning and return
    if (_view->_redo.size() == 0) { 
        _view->_warning = "Nothing to Redo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    ECWindowObject* objToRedo = _view->_redo.back();

    // If the obj to redo was deleted, then we need to redo deletion
    if (ECDeletedObject* dead = dynamic_cast<ECDeletedObject*>(objToRedo)) {
        
        // Get object in window, check if it exists
        auto i = _view->objectIndexInWindow(dead->_alive);
        if (i == _view->_windowObjects.end()) { return; }
        //auto i = _view->_windowObjects.begin();
        //for (ECWindowObject* obj : _view->_windowObjects) {
        //    if (obj == dead->_alive) {
        // Delete object from window, modify undo and redo stack and return
        _view->_windowObjects.erase(i);
        _view->_redo.pop_back();
        _view->_undo.push_back(objToRedo);
            //}
            //i++;
        //}
        _view->DrawAllObjects();
        return;
    }

    // If object wasn't deleted, it was either drawn or edited
    ECWindowObject* obj = objToRedo;

    // Grouped Object
    // If the grouped object wasn't edited from anything, then it was created by grouping
    // Redo the grouping
    if (auto groupToReGroup = dynamic_cast<ECGroupObject*>(objToRedo)) {
        if (groupToReGroup->_editedFrom == NULL) {
            // Remove the individual objects that will make up the new group from the window
            for (auto objBeforeGrouping : groupToReGroup->_objectsBeforeGroup) {
                auto objBeforeGroupIndex = _view->objectIndexInWindow(objBeforeGrouping);
                if (objBeforeGroupIndex == _view->_windowObjects.end()) { throw runtime_error("Object to be regrouped is not in window before regrouping"); }
                _view->_windowObjects.erase(objBeforeGroupIndex);
            }
            _view->_warning = "Regrouped";
            // Add the group made of the objects to the window
            obj = groupToReGroup;
        }
    }

    // Add it back to window, modify undo and redo stack
    _view->_windowObjects.push_back(obj);
    _view->_undo.push_back(obj);
    _view->_redo.pop_back();
    
    // If object wasn't created from an edit, return
    ECWindowObject* objToRemove = objToRedo->_editedFrom;
    if (objToRemove == NULL) {
        _view->DrawAllObjects();
        return;
    }
    
    // If object was created from an edit, remove that object from window
    auto i = _view->objectIndexInWindow(objToRemove);
    _view->_windowObjects.erase(i);

    _view->DrawAllObjects();

}

// ============================== Z Key Up ====================================

// Undo
void ZKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_Z) { return; }

    _view->Clear((ECGVColorRef) ECGV_WHITE);

    // If nothing to undo, set warning, and return
    if (_view->_undo.size() == 0) { 
        _view->_warning = "Nothing to Undo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    ECWindowObject* objToUndo = _view->_undo.back();

    // If object was deleted, we need to undo that deletion and add it back to window
    if (ECDeletedObject* dead = dynamic_cast<ECDeletedObject*>(objToUndo)) {
        
        _view->_windowObjects.push_back(dead->_alive);
        _view->_undo.pop_back();
        _view->_redo.push_back(objToUndo);
        _view->DrawAllObjects();
        return;
    }
    
    // Get object in window, if it isn't in window, return
    auto objectIndex = _view->objectIndexInWindow(objToUndo);
    if (objectIndex == _view->_windowObjects.end()) { return; }

    // Remove object from window
    ECWindowObject* obj = *objectIndex;
    _view->_windowObjects.erase(objectIndex);

    // Grouped Object
    if (auto groupToUnGroup = dynamic_cast<ECGroupObject*>(objToUndo)) {
        if (groupToUnGroup->_editedFrom == NULL) {
            // Add objects that made up group back to window as individual objects
            for (auto objBeforeGrouping : groupToUnGroup->_objectsBeforeGroup) {
                cout << "Object From Before Added to Window" << endl;
                objBeforeGrouping->_color = ECGV_REF_BLACK;
                _view->_windowObjects.push_back(objBeforeGrouping);
                cout << "Number of Objects in Window Now: " << _view->_windowObjects.size() << endl;
            }
            _view->_warning = "Ungrouped";
        }
    }

    // Modify Undo and Redo Stack
    _view->_redo.push_back(objToUndo);
    _view->_undo.pop_back();
    
    // If object was edited from another object, set it to black as it
    // would've been stored blue, and add that object to window
    if (obj->_editedFrom != NULL) {
        obj->_editedFrom->_color = (ECGVColorRef) ECGV_BLACK;

        if (auto group = dynamic_cast<ECGroupObject*>(obj->_editedFrom)) { group->setAllColor(ECGV_REF_BLACK); }

        _view->_windowObjects.push_back(obj->_editedFrom);
    }
    
    cout << "Objects to Be Drawn: " << _view->_windowObjects.size() << endl;
    _view->DrawAllObjects();
}

// ============================== D Key Up ====================================

// Delete
void DKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_D) { return; }
    
    // Can't delete in insertion mode or while editing
    if (_view->_isEditingObj == false) { return; }
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { return; }
    
    // Get the object that will be deleted
    auto objIndex = _view->objectIndexInWindow(_view->_editingObj);
    if (objIndex == _view->_windowObjects.end()) { 
        // Object not in window, either error or currently editing object, so return
        return;
    }
    
    ECWindowObject* obj = *objIndex;
    // Set object to black as it will have been blue while selected
    obj->_color = (ECGVColorRef) ECGV_BLACK;
    // Create the deleted representation of this object
    ECDeletedObject* dead = new ECDeletedObject(obj);
                
    dead->id = _view->id;
    _view->id += 1;

    // Add deletion to undo stack and erase object            
    _view->_undo.push_back(dead);
    _view->_windowObjects.erase(objIndex);

    // Not editing anything anymore
    _view->_isEditingObj = false;
    _view->Clear((ECGVColorRef) ECGV_WHITE);
    _view->DrawAllObjects();
}

// ============================= Mouse Down ===================================

// Nothing really happens when you click down on mouse for any operation
// Just store where the click was, that this is now the first movement, 
// and that there was a click
void MouseDown::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_BUTTON_DOWN) { return; }
    
    cout << "Mouse Down" << endl;
    _view->cursorxDown = _view->getCurrentCursorX();
    _view->cursoryDown = _view->getCurrentCursorY();
    _view->_mouseDown = true;
    _view->_firstMove = true;
    _view->_warning = "";

}

// ============================ Mouse Moving ==================================

void MouseMoving::MultiDrag() {

    _view->Clear(ECGV_REF_WHITE);

    int x = _view->getCurrentCursorX();
    int y = _view->getCurrentCursorY();

    // Empty Moving Objects
    _view->_movingObjects.clear();
    assert(_view->_movingObjects.size() == 0);

    // Create the moved version of each selected object
    for (auto selectedObj : _view->_selectedObjects) {

        ECRectObject* selectedRect = dynamic_cast<ECRectObject*>(selectedObj);
        ECEllipseObject *selectedEllipse = dynamic_cast<ECEllipseObject*>(selectedObj);

        if (selectedRect) {

            ECRectObject* movingRect = dynamic_cast<ECRectObject*>(_view->getNewMovingObject(selectedRect, x, y));
            _view->_movingObjects.push_back(movingRect);
            _view->DrawRectangle(movingRect->_x1, movingRect->_y1, movingRect->_x2, movingRect->_y2, movingRect->_thickness, movingRect->_color, movingRect->isFilled());

        } else if (selectedEllipse) {

            ECEllipseObject* movingEllipse = dynamic_cast<ECEllipseObject*>(_view->getNewMovingObject(selectedEllipse,x,y));
            _view->_movingObjects.push_back(movingEllipse);
            _view->DrawEllipse(movingEllipse->_xCenter, movingEllipse->_yCenter, movingEllipse->_xRadius, movingEllipse->_yRadius, movingEllipse->_thickness, movingEllipse->_color, movingEllipse->isFilled());

        } else { throw runtime_error("Cast failed : MULTIDRAG"); }
    }

    // Delete originals from window
    for (auto selectedObj : _view->_selectedObjects) {
        auto selectedObjIndexInWindow = _view->objectIndexInWindow(selectedObj);
        if (selectedObjIndexInWindow != _view->_windowObjects.end()) {
            _view->_windowObjects.erase(selectedObjIndexInWindow);
        }
    }

    // Draw any unselected previous objects
    _view->DrawAllObjects();
}

// If queue is not empty, drawing looks not continuous
// Make sure everything is up to date by checking queue is empty
void MouseMoving::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_MOVING ) { return; }
    if (_view->isQueueEmpty() == false) { return; }

    // In insertion mode, mouse moving is either nothing or there has been a click before
    // If there was a click before, we're drawing an object
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { 
        if(_view->cursorxDown >= 0 && _view->_mouseDown == true) { InsertMode(); }
    }
    
    // In editing mode, mouse is either just moving around, or we're moving an object
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        if(_view->_arrowMovementEnabled) { return; }
        if (_view->_multiDragEnabled) { 
            MultiDrag(); return; 
            
            }
        if (_view->_isEditingObj && _view->isQueueEmpty()) { EditMode(); }
    }
}

// Drawing an object
void MouseMoving::InsertMode() {
    // Erase Last Temp Obj so drawing looks continuous
    if (!_view->_firstMove) {
        auto lastIndex = _view->_windowObjects.end() - 1;
        ECWindowObject* destroy = _view->_windowObjects.back();
        _view->_windowObjects.erase(lastIndex);
        delete destroy;
    } else {
        _view->_firstMove = false;
    }

    // Add Temp Obj to window vector
    // If this is the last one drawn before the mouse is released, we'll need the obejct
    if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT || _view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) {
        ECRectObject* tempRect = new ECRectObject(_view->cursorxDown, _view->cursoryDown, _view->getCurrentCursorX(), _view->getCurrentCursorY(),1,(ECGVColorRef) ECGV_GRAY, false);
        if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) { tempRect->setFilled(true); }
        _view->_windowObjects.push_back(tempRect);
    } else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE || _view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) {
        int xRadius = abs(_view->getCurrentCursorX() - _view->cursorxDown);
        int yRadius = abs(_view->getCurrentCursorY() - _view->cursoryDown);

        ECEllipseObject* tempEllipse = new ECEllipseObject(_view->cursorxDown, _view->cursoryDown, xRadius, yRadius, 1, (ECGVColorRef) ECGV_GRAY, false);
        if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) { tempEllipse->setFilled(true); }
        _view->_windowObjects.push_back(tempEllipse);
    }

    // Draw all the other objects too so it looks continuous
    _view->Clear(ECGV_REF_WHITE);

    bool filled = false;
    if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT || _view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) {
        if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) { filled = true; }
        _view->DrawRectangle(_view->cursorxDown, _view->cursoryDown, _view->getCurrentCursorX(), _view->getCurrentCursorY(),1, ECGV_REF_GRAY, filled);
    } else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE || _view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) {
        int xRadius = abs(_view->getCurrentCursorX() - _view->cursorxDown);
        int yRadius = abs(_view->getCurrentCursorY() - _view->cursoryDown);
        if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) { filled = true; }
        _view->DrawEllipse(_view->cursorxDown, _view->cursoryDown, xRadius, yRadius, 1, ECGV_REF_GRAY, filled);
    } else { throw runtime_error("SHAPE not defined : Mouse Moving INSERT"); }
    
    _view->DrawAllObjects();

}

// Moving an object
void MouseMoving::EditMode() {
// If this is the first move, store the object before it was moved
// If this is undone, we'll need what it was before it was moved
if (_view->_firstMove) {
    auto i = _view->objectIndexInWindow(_view->_editingObj);
    // If object is not in window, either error or moving mouse after having clicked when done moving object : either way return
    if (i == _view->_windowObjects.end()) { return; }
    
    // If object being edited is a group, make copy of all members in group
    // Make new group with object copies and set that group to objBeforeEdit
    // ECGroupObject* group = dynamic_cast<ECGroupObject*>(_view->_editingObj);
    // if (group) {
    //     // Make copies of objects
    //     std::vector<ECWindowObject*>* oldObjects = new std::vector<ECWindowObject*>(*group->objectsInGroup());
        
    //     // DEBUG: Assert copies are new pointers
    //     for (auto i : *group->objectsInGroup()) {
    //         cout << "OLD: " << &i;
    //     }
    //     for (auto i : *oldObjects) {
    //         cout << "COPY: " << &i;
    //     }
    //     // END DEBUG

    //     _view->_objBeforeEdit = group;
    // }

    _view->_objBeforeEdit = *i;
    _view->_windowObjects.erase(i);
    _view->_firstMove = false;
}

// If its not the first move, then keep moving the object around
_view->Clear(ECGV_REF_WHITE);

ECRectObject* _editingRect = dynamic_cast<ECRectObject*>(_view->_editingObj);
ECEllipseObject* _editingEllipse = dynamic_cast<ECEllipseObject*>(_view->_editingObj);
ECGroupObject* _editingGroup = dynamic_cast<ECGroupObject*>(_view->_editingObj);

// TODO: Delete last drawn obj

// Get the moving version of the object and draw it
if (_editingRect) {
    ECRectObject* temp = dynamic_cast<ECRectObject*>(_view->getNewMovingObject(_editingRect, _view->getCurrentCursorX(), _view->getCurrentCursorY()));
    temp->_color = ECGV_REF_BLUE;
    temp->setFilled(_editingRect->isFilled());

    _view->DrawRectangle(temp);

    _view->_editingObj = temp;

} else if (_editingEllipse) {
    ECEllipseObject* temp = dynamic_cast<ECEllipseObject*>(_view->getNewMovingObject(_editingEllipse, _view->getCurrentCursorX(), _view->getCurrentCursorY()));
    temp->_color = ECGV_REF_BLUE;
    temp->setFilled(_editingEllipse->isFilled());

    _view->DrawEllipse(temp);
    
    _view->_editingObj = temp;

} else if (_editingGroup) {
    ECGroupObject* temp = dynamic_cast<ECGroupObject*>(_view->getNewMovingObject(_editingGroup, _view->getCurrentCursorX(), _view->getCurrentCursorY()));
    temp->_isMoving = true; // sets moving colors
    
    _view->DrawGroup(temp);

    _view->_editingObj = temp;

} else {
    throw runtime_error("Both casts NULL while moving object");
}

// Draw all previous objects to look continuous
_view->DrawAllObjects();
}

// ============================== Mouse Up ====================================

void MouseUp::MultiSelect() {
    int x = _view->cursorxUp;
    int y = _view->cursoryUp;
    bool clickInside = _view->isClickInsideObj(x,y);
    
    if (clickInside) {
        ECWindowObject* selectedObj = _view->_editingObj;
        auto selectedObjIndex = _view->objectIndexInSelectedObjects(selectedObj);
        if (selectedObjIndex == _view->_selectedObjects.end()) {
            // Object Has Not Yet Been Selected
            selectedObj->_color = ECGV_REF_BLUE;
            _view->_selectedObjects.push_back(selectedObj);
        } else {
            selectedObj->_color = ECGV_REF_BLACK;
            _view->_selectedObjects.erase(selectedObjIndex);
        }
        _view->Clear(ECGV_REF_WHITE);
        _view->DrawAllObjects();
    }
}

void MouseUp::EndMultiDrag() {

    // Ensure newly made objects are black
    for(auto newObject : _view->_movingObjects) {
        newObject->_color = ECGV_REF_BLACK;
    }

    // Add newly moved objects to window and undo stack
    for (auto newObject : _view->_movingObjects) {
         _view->_windowObjects.push_back(newObject);
         _view->_undo.push_back(newObject);
    }
    
    // Clear moving objects and selected objects
    _view->_movingObjects.clear();
    _view->_selectedObjects.clear();

    _view->_multiDragEnabled = false;
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

void MouseUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_BUTTON_UP) { return; }
    if (_view->isQueueEmpty() == false) { return; }
    _view->cursorxUp = _view->getCurrentCursorX();
    _view->cursoryUp = _view->getCurrentCursorY();
    _view->_mouseDown = false;

    // Mouse Up in Insert Mode means an object is done being drawn and should be added to the window
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { InsertMode(); }

    // Mouse Up in Edit Mode means an object has been selected or an object is done being moved
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        if (_view->_multiSelectEnabled) { MultiSelect(); return; }
        if (_view->_multiDragEnabled) { EndMultiDrag(); return; }
        if (_view->_arrowMovementEnabled) { return; }
        
        // If an object isn't currently being moved, see if the click was inside an object
        if (_view->_isEditingObj == false) {
            bool clickInside = _view->isClickInsideObj(_view->cursorxUp, _view->cursoryUp);
            
            // If the click was inside a member of a group
            if (clickInside && _view->_hasSelectedObjectInGroup) {
                
                ECGroupObject* group = _view->_selectedGroup;

                // Set editingObj to group
                _view->_editingObj = group;

                // Set color of selected obj to blue and set color of group members to purple
                _view->_selectedObjectInGroup->_color = ECGV_REF_BLUE;
                for (auto groupMember : *(group->objectsInGroup())) {
                    if (groupMember != _view->_selectedObjectInGroup) {
                        groupMember->_color = ECGV_REF_PURPLE;
                    }
                }

            }
            // If the click was inside an object, that object is selected, and is now being edited
            // Store the difference between the click point and the object's boundaries to maintain the shape while moving it around
            if (clickInside) {
                _view->_isEditingObj = true;
                _view->_editingObj->_color = (ECGVColorRef) ECGV_BLUE;
                
                _view->firstClickX = _view->cursorxUp;
                _view->firstClickY = _view->cursoryUp;
                ECRectObject* _editingRect = dynamic_cast<ECRectObject*>(_view->_editingObj);
                ECEllipseObject* _editingEllipse = dynamic_cast<ECEllipseObject*>(_view->_editingObj);
                ECGroupObject* _editingGroup = dynamic_cast<ECGroupObject*>(_view->_editingObj);
                
                // Set distance of points from cursor
                // When moving objects, need this
                if (_editingRect) {
                    _editingRect->setDistFromCursor(_view->cursorxUp, _view->cursoryUp);
                }
                else if (_editingEllipse) {
                    _editingEllipse->setDistFromCursor(_view->cursorxUp, _view->cursoryUp);
                } else if (_editingGroup) {
                    for (auto groupObj : *(_editingGroup->objectsInGroup())) {
                        ECRectObject* selectedRect = dynamic_cast<ECRectObject*>(groupObj);
                        ECEllipseObject *selectedEllipse = dynamic_cast<ECEllipseObject*>(groupObj);
                        if (selectedRect) {
                            selectedRect->setDistFromCursor(_view->cursorxUp, _view->cursoryUp);
                        } else if (selectedEllipse) {
                            selectedEllipse->setDistFromCursor(_view->cursorxUp, _view->cursoryUp);
                        } else { throw runtime_error("Group Obj is undefined"); }
                    }
                } else {
                    throw runtime_error("Cast to ellipse and rect were NULL in MouseUp EDIT Mode");
                }

                _view->_objBeforeEdit = _view->_editingObj;
            }
        // If an object is currently being moved, keep moving it around
        } else if (_view->_isEditingObj) {
            EditMode();
        }

        _view->Clear(ECGVColorRef(ECGV_WHITE));
        _view->DrawAllObjects();
    }
}

// Object is done being drawn
void MouseUp::InsertMode() {
    _view->Clear(ECGV_REF_WHITE);
    for (auto obj : _view->_windowObjects) {
        // Change its color to black
        //if (ECRectObject* rect = dynamic_cast<ECRectObject*>(obj)) {
            obj->_color = (ECGVColorRef) ECGV_BLACK;
        //}
    }

    // The last object added to window is the new object
    auto lastObj = _view->_windowObjects.back();

    // Add it to undo and draw everything
    _view->_undo.push_back(lastObj);
    _view->DrawAllObjects();
}

// Object is done being edited, commit edit
void MouseUp::EditMode() {
    _view->_isEditingObj = false;

    ECWindowObject* _editedObj = _view->_editingObj;
    _view->_objBeforeEdit->_color = (ECGVColorRef) ECGV_RED; //just in case it shows up when it shouldn't

    // If either the object being committed or the obj it was edited from is null throw error
    if (_editedObj == NULL) { throw runtime_error("Obj was edited, but it is NULL"); }
    if (_view->_objBeforeEdit == NULL) { throw runtime_error("Obj edited from is NULL"); }
    
    // Set edited from (for undo/redo) and color to black
    _editedObj->_editedFrom = _view->_objBeforeEdit;
    _editedObj->_color = ECGV_REF_BLACK;

    // If object that was moved is a group
    if (_view->_hasSelectedObjectInGroup) {
        cout << "Committing Group" << endl;
        // Group objects should be black now
        ECGroupObject* editedGroup = dynamic_cast<ECGroupObject*>(_editedObj);
        editedGroup->setAllColor(ECGV_REF_BLACK);

        // Group is not moving anymore (for colors)
        editedGroup->_isMoving = false;
        
        // Not editing a group anymore
        _view->_hasSelectedObjectInGroup = false;
        _view->_selectedGroup = nullptr;
        _view->_selectedObjectInGroup = nullptr;
    }

    // Add new object to window and add to undo stack
    _view->_windowObjects.push_back(_editedObj);
    _view->_undo.push_back(_editedObj);
}

// ============================================================================
// =============================== Observer Subject ===========================
// ============================================================================

ECObserverSubject::ECObserverSubject(ECGraphicViewImp* gv)
{
        _view = gv;
        if (_view == NULL) { throw runtime_error("GraphicView passed to ObserverSubject is NULL"); }
}

// === Init Functions === 
void ECObserverSubject::InitObserver() {

    if (_view == NULL) { throw runtime_error("GraphicView passed to ObserverSubject is NULL"); }
    setAppDefaults();
    attachObservers();
}

void ECObserverSubject::setAppDefaults() {
      // Set Defaults on Open

    // Set Default Mode to EDIT
    _view->_mode = ECGRAPHICVIEW_EDITMODE;
    _view->_modeStr = "Edit Mode";
    
    // Set Edit Vars to Default (Not Currently Editing)
    _view->_isEditingObj = false;
    _view->_mouseDown = false;

    // Set Warning Message to empty
    _view->_warning = "";

    // Set Shape Mode to Default
    _view->_shapeStr = ""; // Not in a shape mode when in Edit Mode
    _view->_shape = ECGRAPHICVIEW_SHAPE_RECT; // Once switch to Insert Mode, default mode will be Rectangle
    _view->_lastShapeStr = "Rectangle";
    
    // Set cursor positions to null
    _view->cursorxDown = _view->cursoryDown = _view->cursorxUp = _view->cursoryUp = -1;

    // Set Multi Select Disabled
    _view->_multiSelectEnabled = false;
    _view->_multiDragEnabled = false;

    // Set Arrow Key Movement Disabled
    _view->_arrowMovementEnabled = false;

    // Set Grouping Defaults
    _view->_hasSelectedObjectInGroup = false;

    _view->Clear(ECGV_REF_WHITE);
    cout << "Attached all observers" << endl;
    _view->DrawModeLabel();
    _view->DrawShapeLabel();
    _view->FlipDisplay();
}

// === View Functions === 
// Clear objects from display
void ECObserverSubject::Clear(ECGVColorRef color) { _view->ClearDisplay((ECGVColor) color); }
// Check if event queue is empty
bool ECObserverSubject::isQueueEmpty() { return _view->isEventQueueEmpty(); }
// Check if object is in window currently, and if it exists
std::__1::__wrap_iter<ECWindowObject **> ECObserverSubject::objectIndexInWindow(ECWindowObject* obj) {
    auto i = _windowObjects.begin();
    for (ECWindowObject* o : _windowObjects) {
        if (o == obj) {
            return i;
        }
        i++;
    }
    return _windowObjects.end();
}
// Check if object has been selected, and if it exists
std::__1::__wrap_iter<ECWindowObject **> ECObserverSubject::objectIndexInSelectedObjects(ECWindowObject* obj) {
    auto i = _selectedObjects.begin();
    for (ECWindowObject* o : _selectedObjects) {
        if (o == obj) {
            return i;
        }
        i++;
    }
    return _selectedObjects.end();
}
// Flip back buffer to front
void ECObserverSubject::FlipDisplay() { _view->Flip(); }

// === Draw Functions ===
// Draw all objects currently in window, then warning label and mode label
void ECObserverSubject::DrawAllObjects() {
    for (auto obj : _windowObjects) {
        if (auto rect = dynamic_cast<ECRectObject*>(obj)) {
            DrawRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, rect->_thickness, rect->_color, rect->isFilled());
        }
        else if (auto ellipse = dynamic_cast<ECEllipseObject*>(obj)) {
            DrawEllipse(ellipse->_xCenter, ellipse->_yCenter, ellipse->_xRadius, ellipse->_yRadius, ellipse->_thickness, ellipse->_color, ellipse->isFilled());
        }
        else if (auto group = dynamic_cast<ECGroupObject*>(obj)) {
            DrawGroup(group);
        }
    }

    DrawWarningLabel();
    DrawModeLabel();
    DrawShapeLabel();
    FlipDisplay();

}
// Draw a Rectangle
void ECObserverSubject::DrawRectangle(float x1, float y1, float x2, float y2, int thickness, ECGVColorRef color, bool filled) {
    if (filled) {
        _view->DrawFilledRectangle(x1, y1, x2, y2, (ECGVColor) color);
    } else {
        _view->DrawRectangle(x1, y1, x2, y2, thickness, (ECGVColor) color);
    }
}

void ECObserverSubject::DrawRectangle(ECRectObject* rect) {
    if (rect->isFilled()) {
        _view->DrawFilledRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, (ECGVColor) rect->_color);
    } else {
         _view->DrawRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, rect->_thickness, (ECGVColor) rect->_color);
    }
}
// Draw an Ellipse
void ECObserverSubject::DrawEllipse(int xC, int yC, int xR, int yR, int thickness, ECGVColorRef color, bool filled) {
    if (filled) {
        _view->DrawFilledEllipse(xC, yC, xR, yR, (ECGVColor) color);
    } else {
        _view->DrawEllipse(xC, yC, xR, yR, thickness, (ECGVColor) color);
    }
}

void ECObserverSubject::DrawEllipse(ECEllipseObject* ellipse) {
    if (ellipse->isFilled()) {
        _view->DrawFilledEllipse(ellipse->_xCenter, ellipse->_yCenter, ellipse->_xRadius, ellipse->_yRadius, (ECGVColor) ellipse->_color);
    } else {
        _view->DrawEllipse(ellipse->_xCenter, ellipse->_yCenter, ellipse->_xRadius, ellipse->_yRadius, ellipse->_thickness, (ECGVColor) ellipse->_color);
    }
}

void ECObserverSubject::DrawGroup(ECGroupObject* group) {
    
    auto groupObjs = *(group->objectsInGroup());
    ECWindowObject* primaryObject = _view->_selectedObjectInGroup;

    for (auto groupObj : groupObjs) {
        ECGVColorRef objColor = groupObj->_color;
        if (group->_isMoving) {
            if (groupObj == primaryObject) { objColor = ECGV_REF_BLUE; }
            else { objColor = ECGV_REF_PURPLE; }
        }
        if (auto rect = dynamic_cast<ECRectObject*>(groupObj)) {
            DrawRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, rect->_thickness, objColor, rect->isFilled());
        } else if (auto ellipse = dynamic_cast<ECEllipseObject*>(groupObj)) {
            DrawEllipse(ellipse->_xCenter, ellipse->_yCenter, ellipse->_xRadius, ellipse->_yRadius, ellipse->_thickness, objColor, ellipse->isFilled());
        } else {
            throw runtime_error("Group Object Type is not Defined");
        }
    }
}
// Draw the Shape Label
void ECObserverSubject::DrawShapeLabel() {
    _view->DrawText(_view->GetWidth()-28, 5, 20, arrayAllegroColors[ECGV_BLACK], ALLEGRO_ALIGN_REF_RIGHT, _shapeStr);
}

// Draw the Insert/Edit Mode Label 
void ECObserverSubject::DrawModeLabel() {
    _view->DrawText(5,_view->GetHeight()-28,20,arrayAllegroColors[ECGV_BLACK],ALLEGRO_ALIGN_REF_LEFT, _modeStr);
}
// Draw the Warning Label (Nothing to Undo/Redo)
void ECObserverSubject::DrawWarningLabel() {
    _view->DrawText(_view->GetWidth() - 5,_view->GetHeight()-28,20,arrayAllegroColors[ECGV_RED],ALLEGRO_ALIGN_REF_RIGHT, _warning);
}

// === Cursor Functions ===
int ECObserverSubject::getCurrentCursorX() {
    int x,y;
    _view->GetCursorPosition(x, y);
    return x;
}

int ECObserverSubject::getCurrentCursorY() {
    int x,y;
    _view->GetCursorPosition(x, y);
    return y;
}

// ============= Multi Drag ==============
ECWindowObject* ECObserverSubject::getNewMovingObject(ECWindowObject* originalObj, int cursorX, int cursorY) {
    
    ECRectObject* originalRect = dynamic_cast<ECRectObject*>(originalObj);
    ECEllipseObject* originalEllipse = dynamic_cast<ECEllipseObject*>(originalObj);
    ECGroupObject* originalGroup = dynamic_cast<ECGroupObject*>(originalObj);

    if (originalRect) {        
        return getMovingRect(originalRect, cursorX, cursorY);
    } else if (originalEllipse) {
        return getMovingEllipse(originalEllipse, cursorX, cursorY);
    } else if (originalGroup) {
        return getMovingGroup(originalGroup, cursorX, cursorY);
    } else { throw runtime_error("Cast Failed : getMovingCoordinates"); }
}

ECRectObject* ECObserverSubject::getMovingRect(ECRectObject* originalRect, int cursorX, int cursorY) {
    int x1D, y1D, x2D, y2D;
    originalRect->getDistFromCursor(x1D,y1D,x2D,y2D);

    int x1 = cursorX - x1D;
    int x2 = cursorX - x2D;
    int y1 = cursorY - y1D;
    int y2 = cursorY - y2D;

    ECRectObject* movingRect = new ECRectObject(x1,y1,x2,y2,1,originalRect->_color,false);
    movingRect->_editedFrom = originalRect;
    movingRect->setDistFromCursor(x1D,y1D,x2D,y2D);
    return movingRect;
}
ECEllipseObject* ECObserverSubject::getMovingEllipse(ECEllipseObject* originalEllipse, int cursorX, int cursorY) {
    int xcD, ycD;
    originalEllipse->getDistFromCursor(xcD, ycD);

    int xC = cursorX - xcD;
    int yC = cursorY - ycD;
    int xR = originalEllipse->_xRadius;
    int yR = originalEllipse->_yRadius;

    ECEllipseObject* movingEllipse = new ECEllipseObject(xC, yC, xR, yR, 1, originalEllipse->_color, false);
    movingEllipse->_editedFrom = originalEllipse;
    movingEllipse->setDistFromCursor(xcD, ycD, 0);
    return movingEllipse;
}
ECGroupObject* ECObserverSubject::getMovingGroup(ECGroupObject* originalGroup, int cursorX, int cursorY) {
    auto groupObjects = originalGroup->objectsInGroup();
    auto primaryObject = _view->_selectedObjectInGroup;

    ECGroupObject* movingGroup = new ECGroupObject();
    for (auto groupObj : *groupObjects) {
        if (groupObj == _view->_selectedObjectInGroup) {
            cout << "SELECTED OBJECT IN GROUP" << endl;
            if (groupObj->_color == ECGV_REF_BLUE) { cout << "IS BLUE" << endl;}
        }
        ECRectObject* rectObj = dynamic_cast<ECRectObject*>(groupObj);
        ECEllipseObject* ellipseObj = dynamic_cast<ECEllipseObject*>(groupObj);

        if (rectObj) {
            ECRectObject* movingRect = getMovingRect(rectObj, cursorX, cursorY);
            movingGroup->addObject(movingRect);
            
            // To maintain color and status as primary object
            if (rectObj == _view->_selectedObjectInGroup) {
                _view->_selectedObjectInGroup = movingRect;
            }
        }
        else if (ellipseObj) {
            ECEllipseObject* movingEllipse = getMovingEllipse(ellipseObj, cursorX, cursorY);
            movingGroup->addObject(movingEllipse);

            // To maintain color and status as primary object
            if (ellipseObj == _view->_selectedObjectInGroup) {
                _view->_selectedObjectInGroup = movingEllipse;
            }

        } else {
            throw runtime_error("Group obj moving is not rect or ellipse");
        }
    }
    return movingGroup;
}

// === Editing Functions ===
bool ECObserverSubject::isPointInsideRect(ECRectObject* _rect, float xp, float yp) {

    float maxX = max(_rect->_x1, _rect->_x2);
    float maxY = max(_rect->_y1, _rect->_y2);

    float minX = min(_rect->_x1, _rect->_x2);
    float minY = min(_rect->_y1, _rect->_y2);

    if (xp >= minX && xp <= maxX && yp >= minY && yp <= maxY) {
        return true;
    }
    return false;
}

bool ECObserverSubject::isPointInsideEllipse(ECEllipseObject* _ellipse, float xp, float yp) {
    
    // [ (x-h)^2 / r_x^2 ] + [ (y-k)^2 / r_y^2 ] less than equal to 1 if pt inside ellipse
    float h = _ellipse->_xCenter;
    float k = _ellipse->_yCenter;

    float r_x = _ellipse->_xRadius;
    float r_y = _ellipse->_yRadius;

    float numerator1 = (xp - h) * (xp - h);
    float numerator2 = (yp - k) * (yp - k);

    float term1 = numerator1/(r_x * r_x);
    float term2 = numerator2/(r_y * r_y);

    float result = term1 + term2;

    if (result <= 1) { return true; }
    return false;
}

// Determine if a click is inside an object, and if it is, set that object to the one now being edited
bool ECObserverSubject::isClickInsideObj(float xp, float yp) {
    
    bool clickInside = false;
    for (auto obj : _windowObjects) {
        
        ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
        ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(obj);
        ECGroupObject* group = dynamic_cast<ECGroupObject*>(obj);

        if (rect) {
            if (isPointInsideRect(rect, cursorxDown, cursoryDown)) {
                _editingObj = rect;
                clickInside = true;
                break;
            }
        }

        if (ellipse) {
            if (isPointInsideEllipse(ellipse, cursorxDown, cursoryDown)) {
                _editingObj = ellipse;
                clickInside = true;
                break;
            }
        }

        if (group) {
            for (auto groupObj : *(group->objectsInGroup())) {
                ECRectObject* rect = dynamic_cast<ECRectObject*>(groupObj);
                ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(groupObj);

                if (rect) {
                    if (isPointInsideRect(rect, cursorxDown, cursoryDown)) {
                        _view->_hasSelectedObjectInGroup = true;
                        _view->_selectedGroup = group;
                        _view->_selectedObjectInGroup = rect;

                        _editingObj = group;
                        clickInside = true;
                        break;
                    }
                }
                if (ellipse) {
                    if (isPointInsideEllipse(ellipse, cursorxDown, cursoryDown)) {
                        _view->_hasSelectedObjectInGroup = true;
                        _view->_selectedGroup = group;
                        _view->_selectedObjectInGroup = ellipse;

                        _editingObj = group;
                        clickInside = true;
                        break;
                    }
                }
            }
        }
    }

    return clickInside;

}

// === Observer Functions ===
void ECObserverSubject::Notify()
{
    for(unsigned int i=0; i<listObservers.size(); ++i) {
        ECGVEventTypeRef event = (ECGVEventTypeRef) _view->GetCurrEvent();
        listObservers[i]->Update(event);
    }
}

void ECObserverSubject::attachObservers() {

    MouseUp *mouseUpObserver = new MouseUp(_view);
    MouseDown *mouseDownObserver = new MouseDown(_view);
    MouseMoving *mouseMovingObserver = new MouseMoving(_view);
    SpaceUp* spaceUpObersver = new SpaceUp(_view);
    DKeyUp* dKeyUpObserver = new DKeyUp(_view);
    ZKeyUp* zKeyUpObserver = new ZKeyUp(_view);
    YKeyUp* yKeyUpObserver = new YKeyUp(_view);
    GKeyUp* gKeyUpObserver = new GKeyUp(_view);
    FKeyUp* fKeyUpObserver = new FKeyUp(_view);
    CtrlKeyUp* ctrlKeyUpObserver = new CtrlKeyUp(_view);
    CtrlKeyDown* ctrlKeyDownObserver = new CtrlKeyDown(_view);
    ArrowKeyUp* arrowKeysUpObserver = new ArrowKeyUp(_view);
    EscapeKeyUp* escapeKeyUpObserver = new EscapeKeyUp(_view);

    _view->Attach(mouseUpObserver);
    _view->Attach(mouseDownObserver);
    _view->Attach(mouseMovingObserver);
    _view->Attach(spaceUpObersver);
    _view->Attach(dKeyUpObserver);
    _view->Attach(zKeyUpObserver);
    _view->Attach(yKeyUpObserver);
    _view->Attach(gKeyUpObserver);
    _view->Attach(fKeyUpObserver);
    _view->Attach(ctrlKeyDownObserver);
    _view->Attach(ctrlKeyUpObserver);
    _view->Attach(arrowKeysUpObserver);
    _view->Attach(escapeKeyUpObserver);
}