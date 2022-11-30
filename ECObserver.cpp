//
// ECObserver.cpp
//
//
//

#include "ECObserver.h"
#include "ECGraphicViewImp.h"

// ============================== Space Up ====================================

void SpaceUp::Update(ECGVEventType event) {
    if (event != ECGV_EV_KEY_UP_SPACE) { return; }
    // Don't allow switching modes while currently editing or inserting
    if (_view->_isEditingRect || _view->_mouseDown) { return; }
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { _view->_mode = ECGRAPHICVIEW_INSERTIONMODE; _view->_modeStr = "Insert Mode"; }
    else { _view->_mode = ECGRAPHICVIEW_EDITMODE; _view->_modeStr = "Edit Mode"; }

    _view->Clear(ECGV_WHITE);
    _view->DrawAllObjects();
}

// ============================== Y Key Up ====================================

void YKeyUp::Update(ECGVEventType event) {
    
    if (event != ECGV_EV_KEY_UP_Y) { return; }
    _view->Clear(ECGV_WHITE);

    if (_view->_redo.size() == 0) { 
        cout << "Redo Warning" << endl;
        _view->_warning = "Nothing to Redo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    WindowObject* objToRedo = _view->_redo.back();

    if (DeletedObject* dead = dynamic_cast<DeletedObject*>(objToRedo)) {
        
        auto i = _view->_windowObjects.begin();
        cout << "How many objects are in window? " << _view->_windowObjects.size() << endl;
        for (WindowObject* obj : _view->_windowObjects) {
            if (obj == dead->_alive) {
                cout << "Found deleted object to remove from display with ID: " << obj->id << endl;
                _view->_windowObjects.erase(i);
                _view->_redo.pop_back();
                _view->_undo.push_back(objToRedo);
            }
            i++;
        }
        _view->DrawAllObjects();
        return;
    }

    _view->_undo.push_back(objToRedo);

    _view->_windowObjects.push_back(objToRedo);
    _view->_redo.pop_back();
    
    WindowObject* objToRemove = objToRedo->_editedFrom;
    if (objToRemove == NULL) {
        _view->DrawAllObjects();
        return;
    }
    
    auto i = _view->_windowObjects.begin();
    for (WindowObject* obj : _view->_windowObjects) {
        if (obj == objToRemove) {
            _view->_windowObjects.erase(i);
        }
        i++;
    }

    _view->DrawAllObjects();

}

// ============================== Z Key Up ====================================

void ZKeyUp::Update(ECGVEventType event) {
    if (event != ECGV_EV_KEY_UP_Z) { return; }

    _view->Clear(ECGV_WHITE);

    if (_view->_undo.size() == 0) { 
        cout << "Undo Warning" << endl;
        _view->_warning = "Nothing to Undo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    WindowObject* objToUndo = _view->_undo.back();

    if (DeletedObject* dead = dynamic_cast<DeletedObject*>(objToUndo)) {
        
        cout << "Found deleted object" << endl;
        _view->_windowObjects.push_back(dead->_alive);
        _view->_undo.pop_back();
        _view->_redo.push_back(objToUndo);
        _view->DrawAllObjects();
        return;
    }

    _view->_redo.push_back(objToUndo);
    
    auto objectIndex = _view->objectIndexInWindow(objToUndo);
    if (objectIndex == _view->_windowObjects.end()) { throw "Trying to undo object that doesn't exist"; }

    WindowObject* obj = *objectIndex;
    _view->_windowObjects.erase(objectIndex);
    _view->_undo.pop_back();
    
    if (obj->_editedFrom != NULL) {
        obj->_editedFrom->_color = ECGV_BLACK;
        _view->_windowObjects.push_back(obj->_editedFrom);
    }

    _view->DrawAllObjects();
}

// ============================== D Key Up ====================================

void DKeyUp::Update(ECGVEventType event) {
    
    if (event != ECGV_EV_KEY_UP_D) { return; }
    if (_view->_isEditingRect == false) { return; }
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { return; }
    
    auto objIndex = _view->objectIndexInWindow(_view->_editingRect);
    if (objIndex == _view->_windowObjects.end()) { throw "Attempting to Delete Object That Doesn't Exist"; }
    WindowObject* obj = *objIndex;
    obj->_color = ECGV_BLACK;
    DeletedObject* dead = new DeletedObject(obj);
                
    dead->id = _view->id;
    _view->id += 1;
                
    _view->_undo.push_back(dead);
    _view->_windowObjects.erase(objIndex);

    _view->_isEditingRect = false;
    _view->Clear(ECGV_WHITE);
    _view->DrawAllObjects();
}

// ============================= Mouse Down ===================================

void MouseDown::Update(ECGVEventType event) {
    if (event != ECGV_EV_MOUSE_BUTTON_DOWN) { return; }
    
    cout << "Mouse Down" << endl;
    _view->cursorxDown = _view->cursorx;
    _view->cursoryDown = _view->cursory;
    _view->_mouseDown = true;
    _view->_firstMove = true;

}

// ============================ Mouse Moving ==================================

void MouseMoving::Update(ECGVEventType event) {
    if (event != ECGV_EV_MOUSE_MOVING ) { return; }
    if (_view->isEventQueueEmpty() == false) { return; }

    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { 
        if(_view->cursorxDown >= 0 && _view->_mouseDown == true) { InsertMode(); }
    }
            
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { 
        if (_view->_isEditingRect && _view->isEventQueueEmpty()) { EditMode(); }
    }
}

void MouseMoving::InsertMode() {
    
    // Erase Last Temp Rect
    if (!_view->_firstMove) {
        auto lastIndex = _view->_windowObjects.end() - 1;
        WindowObject* destroy = _view->_windowObjects.back();
        _view->_windowObjects.erase(lastIndex);
        delete destroy;
    } else {
        _view->_firstMove = false;
    }

    // Add Temp Rect to Vector
    RectObject* tempRect = new RectObject(_view->cursorxDown, _view->cursoryDown, _view->cursorx, _view->cursory,1,ECGV_GRAY);
    _view->_windowObjects.push_back(tempRect);

    // Draw Rect to Display
    al_clear_to_color(al_map_rgb(255,255,255));
    _view->DrawRectangle(_view->cursorxDown, _view->cursoryDown, _view->cursorx, _view->cursory,1,ECGV_GRAY);

    // Draw Previous Rects to Appear Continuous
    _view->DrawAllObjects();

}

void MouseMoving::EditMode() {
    
    if (_view->_firstMove) {
        int i = 0;
        for (auto obj : _view->_windowObjects) {
            if (auto rect = dynamic_cast<RectObject*>(obj)) {
                if (rect == _view->_editingRect) {
                    _view->_objBeforeEdit = _view->_windowObjects[i];
                    _view->_windowObjects.erase(_view->_windowObjects.begin() + i);
                }
            }
            i++;
        }
        _view->_firstMove = false;
    }

    // Draw Rect to Display
    al_clear_to_color(al_map_rgb(255,255,255));
    _view->DrawRectangle(_view->_editingRect->_x1, _view->_editingRect->_y1, _view->cursorx, _view->cursory,1,ECGV_BLUE);

    // Draw Previous Rects to Appear Continuous
    _view->DrawAllObjects();
}

// ============================== Mouse Up ====================================

void MouseUp::Update(ECGVEventType event) {
    if (event != ECGV_EV_MOUSE_BUTTON_UP) { return; }
    if (_view->isEventQueueEmpty() == false) { return; }

    _view->cursorxUp = _view->cursorx;
    _view->cursoryUp = _view->cursory;
    _view->_mouseDown = false;

    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { InsertMode(); }

    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        
        if (_view->_isEditingRect == false) {
            
            bool clickInside = _view->isClickInsideRect(_view->cursorxUp, _view->cursoryUp);
            if (clickInside) {
                _view->_isEditingRect = true;
                _view->_editingRect->_color = ECGV_BLUE;
            }
        
        } else if (_view->_isEditingRect) {
            EditMode();
        }

        _view->Clear(ECGV_WHITE);
        _view->DrawAllObjects();
    }
}

void MouseUp::InsertMode() {
    _view->Clear(ECGV_WHITE);
    for (auto obj : _view->_windowObjects) {
        if (RectObject* rect = dynamic_cast<RectObject*>(obj)) {
            rect->_color = ECGV_BLACK;
        }
    }
    auto lastObj = _view->_windowObjects.back();

    lastObj->id = _view->id;
    _view->id += 1;

    _view->_undo.push_back(lastObj);
    _view->lastDisplay += 1;
    cout << "History Added" << endl;
    _view->DrawAllObjects();
}

void MouseUp::EditMode() {

    cout << "Mouse up and Not Editing" << endl;
    _view->_isEditingRect = false;

    RectObject *editedRect = new RectObject(_view->_editingRect->_x1, _view->_editingRect->_y1, _view->cursorxDown, _view->cursoryDown, 1, ECGV_BLACK);
    _view->_objBeforeEdit->_color = ECGV_RED;
    editedRect->_editedFrom = _view->_objBeforeEdit;

    editedRect->id = _view->id;
    _view->id += 1;
    _view->_windowObjects.push_back(editedRect);

    //_view->_undo.push_back(_view->_objBeforeEdit);
    //if (_view->_objBeforeEdit == NULL) {
    //    cout << "obj before edit is null" << endl;
    //} else {
    //    cout << "obj before edit valid" << endl;
    //}
    _view->_undo.push_back(editedRect);
    //_view->lastDisplay += 1;
    cout << "History Added" << endl;
    
    cout << "New Edited Rect: x1:" << editedRect->_x1 << " y1: " << editedRect->_y1 << " x2: " << editedRect->_x2 << " y2: " << editedRect->_y2 << endl;

}

// ============================================================================