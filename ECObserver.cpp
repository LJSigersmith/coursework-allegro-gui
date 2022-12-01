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

// ============================== Space Up ====================================

// Switch Between Insert and Edit Modes
void SpaceUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_SPACE) { return; }
    
    // Don't allow switching modes while currently editing or inserting
    if (_view->_isEditingRect || _view->_mouseDown) { return; }
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { _view->_mode = ECGRAPHICVIEW_INSERTIONMODE; _view->_modeStr = "Insert Mode"; }
    else { _view->_mode = ECGRAPHICVIEW_EDITMODE; _view->_modeStr = "Edit Mode"; }

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== Y Key Up ====================================

// Undo
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

    // Add it back to window, modify undo and redo stack
    _view->_windowObjects.push_back(objToRedo);
    _view->_undo.push_back(objToRedo);
    _view->_redo.pop_back();
    
    // If object wasn't created from an edit, return
    ECWindowObject* objToRemove = objToRedo->_editedFrom;
    if (objToRemove == NULL) {
        _view->DrawAllObjects();
        return;
    }
    
    // If object was created from an edit, remove that object from window
    auto i = _view->objectIndexInWindow(objToRemove);
    //auto i = _view->_windowObjects.begin();
    //for (ECWindowObject* obj : _view->_windowObjects) {
    //    if (obj == objToRemove) {
    _view->_windowObjects.erase(i);
    //    }
    //    i++;
    //}

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

    // Delete object from window, modify undo and redo stack
    ECWindowObject* obj = *objectIndex;
    _view->_windowObjects.erase(objectIndex);
    _view->_redo.push_back(objToUndo);
    _view->_undo.pop_back();
    
    // If object was edited from another object, set it to black as it
    // would've been stored blue, and add that object to window
    if (obj->_editedFrom != NULL) {
        obj->_editedFrom->_color = (ECGVColorRef) ECGV_BLACK;
        _view->_windowObjects.push_back(obj->_editedFrom);
    }

    _view->DrawAllObjects();
}

// ============================== D Key Up ====================================

// Delete
void DKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_D) { return; }
    
    // Can't delete in insertion mode or while editing
    if (_view->_isEditingRect == false) { return; }
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { return; }
    
    // Get the object that will be deleted
    auto objIndex = _view->objectIndexInWindow(_view->_editingRect);
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
    _view->_isEditingRect = false;
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

}

// ============================ Mouse Moving ==================================

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
        if (_view->_isEditingRect && _view->isQueueEmpty()) { EditMode(); }
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
    ECRectObject* tempRect = new ECRectObject(_view->cursorxDown, _view->cursoryDown, _view->getCurrentCursorX(), _view->getCurrentCursorY(),1,(ECGVColorRef) ECGV_GRAY);
    _view->_windowObjects.push_back(tempRect);

    // Draw all the other objects too so it looks continuous
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawRectangle(_view->cursorxDown, _view->cursoryDown, _view->getCurrentCursorX(), _view->getCurrentCursorY(),1, ECGV_REF_GRAY);
    _view->DrawAllObjects();

}

// Moving an object
void MouseMoving::EditMode() {
    
// If this is the first move, store the object before it was moved
// If this is undone, we'll need what it was before it was moved
if (_view->_firstMove) {
    int i = 0;
    for (auto obj : _view->_windowObjects) {
        if (auto rect = dynamic_cast<ECRectObject*>(obj)) {
            if (rect == _view->_editingRect) {
                _view->_objBeforeEdit = _view->_windowObjects[i];
                _view->_windowObjects.erase(_view->_windowObjects.begin() + i);
            }
        }
        i++;
    }
    _view->_firstMove = false;
}

// If its not the first move, then keep moving the object around
_view->Clear(ECGV_REF_WHITE);

// Maintain the object by drawing it as far away from the cursor as it was when first selected
float x1 = _view->getCurrentCursorX() - _view->x1Difference;
float y1 = _view->getCurrentCursorY() - _view->y1Difference;

float x2 = _view->getCurrentCursorX() - _view->x2Difference;
float y2 = _view->getCurrentCursorY() - _view->y2Difference;

// Draw the newly moved object and all the stored objects to look continuous
_view->DrawRectangle(x1, y1,x2, y2, 1, (ECGVColorRef) ECGV_BLUE);
_view->DrawAllObjects();
}

// ============================== Mouse Up ====================================

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
        
        // If an object isn't currently being moved, see if the click was inside an object
        if (_view->_isEditingRect == false) {
            
            bool clickInside = _view->isClickInsideRect(_view->cursorxUp, _view->cursoryUp);
            // If the click was inside an object, that object is selected, and is now being edited
            // Store the difference between the click point and the object's boundaries to maintain the shape while moving it around
            if (clickInside) {
                _view->_isEditingRect = true;
                _view->_editingRect->_color = (ECGVColorRef) ECGV_BLUE;
                
                _view->firstClickX = _view->cursorxUp;
                _view->firstClickY = _view->cursoryUp;

                _view->x1Difference = _view->cursorxUp - _view->_editingRect->_x1;
                _view->y1Difference = _view->cursoryUp - _view->_editingRect->_y1;

                _view->x2Difference = _view->cursorxUp - _view->_editingRect->_x2;
                _view->y2Difference = _view->cursoryUp - _view->_editingRect->_y2;
            }
        // If an object is currently being moved, keep moving it around
        } else if (_view->_isEditingRect) {
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
        if (ECRectObject* rect = dynamic_cast<ECRectObject*>(obj)) {
            rect->_color = (ECGVColorRef) ECGV_BLACK;
        }
    }
    // The last object added to window is the new object
    auto lastObj = _view->_windowObjects.back();

    lastObj->id = _view->id;
    _view->id += 1;

    // Add it to undo and draw everything
    _view->_undo.push_back(lastObj);
    _view->DrawAllObjects();
}

// Keep moving Object around
void MouseUp::EditMode() {

    cout << "Mouse up and Not Editing" << endl;
    _view->_isEditingRect = false;

    // Find new coordinates for moved object and draw it
    float x1 = _view->cursorxUp - _view->x1Difference;
    float y1 = _view->cursoryUp - _view->y1Difference;

    float x2 = _view->cursorxUp - _view->x2Difference;
    float y2 = _view->cursoryUp - _view->y2Difference;

    _view->DrawRectangle(x1, y1,x2, y2, 1, ECGV_REF_BLUE);

    // The object that was moved to be this new object is what the new object was edited from
    // We'll need this if we undo this edit
    ECRectObject *editedRect = new ECRectObject(x1, y1, x2, y2, 1, (ECGVColorRef) ECGV_BLACK);
    _view->_objBeforeEdit->_color = (ECGVColorRef) ECGV_RED;
    editedRect->_editedFrom = _view->_objBeforeEdit;

    editedRect->id = _view->id;
    _view->id += 1;

    // Add new object to window and add to undo stack
    _view->_windowObjects.push_back(editedRect);
    _view->_undo.push_back(editedRect);
}

// ============================================================================
// =============================== Observer Subject ===========================
// ============================================================================

ECObserverSubject::ECObserverSubject(ECGraphicViewImp* gv)
{
        _view = gv;
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
// Flip back buffer to front
void ECObserverSubject::FlipDisplay() { _view->Flip(); }

// === Draw Functions ===
// Draw all objects currently in window, then warning label and mode label
void ECObserverSubject::DrawAllObjects() {
    for (auto obj : _windowObjects) {
        if (auto rect = dynamic_cast<ECRectObject*>(obj)) {
            DrawRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, rect->_thickness, rect->_color);
        }
    }

    DrawWarningLabel();
    DrawModeLabel();
    FlipDisplay();

}
// Draw a Rectangle
void ECObserverSubject::DrawRectangle(float x1, float y1, float x2, float y2, int thickness, ECGVColorRef color) {
     _view->DrawRectangle(x1, y1, x2, y2, thickness, (ECGVColor) color);
}

// Draw the Insert/Edit Mode Label 
void ECObserverSubject::DrawModeLabel() {
    cout << "Drawing Mode Label" << endl;
    cout << "View Height: " << _view->GetHeight() << endl;
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

// Determine if a click is inside an object, and if it is, set that object to the one now being edited
bool ECObserverSubject::isClickInsideRect(float xp, float yp) {
    for (auto obj : _windowObjects) {
        if (ECRectObject* rect = dynamic_cast<ECRectObject*>(obj)) {
            if (isPointInsideRect(rect, cursorxDown, cursoryDown)) {
                _editingRect = rect;
                return true;
            }
        }
    }
return false;
}

// === Observer Functions ===
void ECObserverSubject::Notify()
{
    for(unsigned int i=0; i<listObservers.size(); ++i) {
        ECGVEventTypeRef event = (ECGVEventTypeRef) _view->GetCurrEvent();
        listObservers[i]->Update(event);
    }
}