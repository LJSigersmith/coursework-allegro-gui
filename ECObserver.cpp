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

void YKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_Y) { return; }
    _view->Clear((ECGVColorRef) ECGV_WHITE);

    if (_view->_redo.size() == 0) { 
        cout << "Redo Warning" << endl;
        _view->_warning = "Nothing to Redo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    ECWindowObject* objToRedo = _view->_redo.back();

    if (ECDeletedObject* dead = dynamic_cast<ECDeletedObject*>(objToRedo)) {
        
        auto i = _view->_windowObjects.begin();
        cout << "How many objects are in window? " << _view->_windowObjects.size() << endl;
        for (ECWindowObject* obj : _view->_windowObjects) {
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
    
    ECWindowObject* objToRemove = objToRedo->_editedFrom;
    if (objToRemove == NULL) {
        _view->DrawAllObjects();
        return;
    }
    
    auto i = _view->_windowObjects.begin();
    for (ECWindowObject* obj : _view->_windowObjects) {
        if (obj == objToRemove) {
            _view->_windowObjects.erase(i);
        }
        i++;
    }

    _view->DrawAllObjects();

}

// ============================== Z Key Up ====================================

void ZKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_Z) { return; }

    _view->Clear((ECGVColorRef) ECGV_WHITE);

    if (_view->_undo.size() == 0) { 
        cout << "Undo Warning" << endl;
        _view->_warning = "Nothing to Undo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    ECWindowObject* objToUndo = _view->_undo.back();

    if (ECDeletedObject* dead = dynamic_cast<ECDeletedObject*>(objToUndo)) {
        
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

    ECWindowObject* obj = *objectIndex;
    _view->_windowObjects.erase(objectIndex);
    _view->_undo.pop_back();
    
    if (obj->_editedFrom != NULL) {
        obj->_editedFrom->_color = (ECGVColorRef) ECGV_BLACK;
        _view->_windowObjects.push_back(obj->_editedFrom);
    }

    _view->DrawAllObjects();
}

// ============================== D Key Up ====================================

void DKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_D) { return; }
    if (_view->_isEditingRect == false) { return; }
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { return; }
    
    auto objIndex = _view->objectIndexInWindow(_view->_editingRect);
    if (objIndex == _view->_windowObjects.end()) { 
        // Object not in window, either error or currently editing object
        // TO DO
        return;
    }
    
    ECWindowObject* obj = *objIndex;

    obj->_color = (ECGVColorRef) ECGV_BLACK;
    ECDeletedObject* dead = new ECDeletedObject(obj);
                
    dead->id = _view->id;
    _view->id += 1;
                
    _view->_undo.push_back(dead);
    _view->_windowObjects.erase(objIndex);

    _view->_isEditingRect = false;
    _view->Clear((ECGVColorRef) ECGV_WHITE);
    _view->DrawAllObjects();
}

// ============================= Mouse Down ===================================

void MouseDown::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_BUTTON_DOWN) { return; }
    
    cout << "Mouse Down" << endl;
    _view->cursorxDown = _view->getCurrentCursorX();
    _view->cursoryDown = _view->getCurrentCursorY();
    _view->_mouseDown = true;
    _view->_firstMove = true;

}

// ============================ Mouse Moving ==================================

void MouseMoving::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_MOVING ) { return; }
    if (_view->isQueueEmpty() == false) { return; }

    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { 
        if(_view->cursorxDown >= 0 && _view->_mouseDown == true) { InsertMode(); }
    }
            
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { 
        if (_view->_isEditingRect && _view->isQueueEmpty()) { EditMode(); }
    }
}

void MouseMoving::InsertMode() {
    // Erase Last Temp Rect
    if (!_view->_firstMove) {
        auto lastIndex = _view->_windowObjects.end() - 1;
        ECWindowObject* destroy = _view->_windowObjects.back();
        _view->_windowObjects.erase(lastIndex);
        delete destroy;
    } else {
        _view->_firstMove = false;
    }

    // Add Temp Rect to Vector
    ECRectObject* tempRect = new ECRectObject(_view->cursorxDown, _view->cursoryDown, _view->getCurrentCursorX(), _view->getCurrentCursorY(),1,(ECGVColorRef) ECGV_GRAY);
    _view->_windowObjects.push_back(tempRect);

    // Draw Rect to Display
    _view->Clear(ECGV_REF_WHITE);
    //TEST WITH THIS:
    _view->DrawRectangle(_view->cursorxDown, _view->cursoryDown, _view->getCurrentCursorX(), _view->getCurrentCursorY(),1, ECGV_REF_GRAY);
    // Draw Previous Rects to Appear Continuous
    _view->DrawAllObjects();

}

void MouseMoving::EditMode() {
    
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

    // Draw Rect to Display
    al_clear_to_color(al_map_rgb(255,255,255));

    float x1 = _view->getCurrentCursorX() - _view->x1Difference;
    float y1 = _view->getCurrentCursorY() - _view->y1Difference;

    float x2 = _view->getCurrentCursorX() - _view->x2Difference;
    float y2 = _view->getCurrentCursorY() - _view->y2Difference;

    _view->DrawRectangle(x1, y1,x2, y2, 1, (ECGVColorRef) ECGV_BLUE);
    // Draw Previous Rects to Appear Continuous
    _view->DrawAllObjects();
}

// ============================== Mouse Up ====================================

void MouseUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_BUTTON_UP) { return; }
    if (_view->isQueueEmpty() == false) { return; }

    _view->cursorxUp = _view->getCurrentCursorX();
    _view->cursoryUp = _view->getCurrentCursorY();
    _view->_mouseDown = false;

    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { InsertMode(); }

    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        
        if (_view->_isEditingRect == false) {
            
            bool clickInside = _view->isClickInsideRect(_view->cursorxUp, _view->cursoryUp);
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
        
        } else if (_view->_isEditingRect) {
            EditMode();
        }

        _view->Clear(ECGVColorRef(ECGV_WHITE));
        _view->DrawAllObjects();
    }
}

void MouseUp::InsertMode() {
    _view->Clear(ECGV_REF_WHITE);
    for (auto obj : _view->_windowObjects) {
        if (ECRectObject* rect = dynamic_cast<ECRectObject*>(obj)) {
            rect->_color = (ECGVColorRef) ECGV_BLACK;
        }
    }
    auto lastObj = _view->_windowObjects.back();

    lastObj->id = _view->id;
    _view->id += 1;

    _view->_undo.push_back(lastObj);
    cout << "History Added" << endl;
    _view->DrawAllObjects();
}

void MouseUp::EditMode() {

    cout << "Mouse up and Not Editing" << endl;
    _view->_isEditingRect = false;

    float x1 = _view->cursorxUp - _view->x1Difference;
    float y1 = _view->cursoryUp - _view->y1Difference;

    float x2 = _view->cursorxUp - _view->x2Difference;
    float y2 = _view->cursoryUp - _view->y2Difference;

    _view->DrawRectangle(x1, y1,x2, y2, 1, ECGV_REF_BLUE);

    ECRectObject *editedRect = new ECRectObject(x1, y1, x2, y2, 1, (ECGVColorRef) ECGV_BLACK);
    _view->_objBeforeEdit->_color = (ECGVColorRef) ECGV_RED;
    editedRect->_editedFrom = _view->_objBeforeEdit;

    editedRect->id = _view->id;
    _view->id += 1;
    _view->_windowObjects.push_back(editedRect);
    _view->_undo.push_back(editedRect);

    cout << "History Added" << endl;
    
    cout << "New Edited Rect: x1:" << editedRect->_x1 << " y1: " << editedRect->_y1 << " x2: " << editedRect->_x2 << " y2: " << editedRect->_y2 << endl;

}

// ============================================================================
// =============================== Observer Subject ===========================
// ============================================================================

ECObserverSubject::ECObserverSubject(ECGraphicViewImp* gv)
{
        _view = gv;
}

// View Functions
    void ECObserverSubject::Clear(ECGVColorRef color) {
        _view->RenderStart();
        //al_clear_to_color(arrayAllegroColors[color]);
    }
    bool ECObserverSubject::isQueueEmpty() { return _view->isEventQueueEmpty(); }
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

    void ECObserverSubject::FlipDisplay() { _view->Flip(); }

    // Draw Functions
    void ECObserverSubject::DrawAllObjects() {
    //cout << endl << "Drawing All Objects" << endl;
    cout << "Objects to Draw: " << _windowObjects.size();
    for (auto obj : _windowObjects) {
        if (auto rect = dynamic_cast<ECRectObject*>(obj)) {
            if (rect->_color == ECGV_REF_BLUE) { cout << "BLUE" << endl;}
            DrawRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, rect->_thickness, rect->_color);
        }
    }

    DrawWarningLabel();
    DrawModeLabel();
    FlipDisplay();

 }
    void ECObserverSubject::DrawRectangle(float x1, float y1, float x2, float y2, int thickness, ECGVColorRef color) {
        _view->DrawRectangle(x1, y1, x2, y2, thickness, (ECGVColor) color);
    }
    
    void ECObserverSubject::DrawModeLabel() {
        cout << "Drawing Mode Label" << endl;
        cout << "View Height: " << _view->GetHeight() << endl;
        _view->DrawText(5,_view->GetHeight()-28,20,arrayAllegroColors[ECGV_BLACK],ALLEGRO_ALIGN_REF_LEFT, _modeStr);
}
    void ECObserverSubject::DrawWarningLabel() {
    _view->DrawText(_view->GetWidth() - 5,_view->GetHeight()-28,20,arrayAllegroColors[ECGV_RED],ALLEGRO_ALIGN_REF_RIGHT, _warning);
}

    // Cursor Functions
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

    // Editing Functions
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
    bool ECObserverSubject::isClickInsideRect(float xp, float yp) {
    cout << "Is Point Inside? " << xp << "," << yp << endl;
    for (auto obj : _windowObjects) {
        if (ECRectObject* rect = dynamic_cast<ECRectObject*>(obj)) {
            cout << "Is Point Inside Rect: x1:" << rect->_x1 << " y1: " << rect->_y1 << " x2: " << rect->_x2 << " y2: " << rect->_y2 << endl;
            if (isPointInsideRect(rect, cursorxDown, cursoryDown)) {
                _editingRect = rect;
                return true;
            }
        }
    }
    return false;
 }

    // Observer Functions
    void ECObserverSubject::Notify()
    {
        //std::cout << "Notify: number of observer: " << listObservers.size() << std::endl;
        for(unsigned int i=0; i<listObservers.size(); ++i)
        {
            //cout << "Update" << endl;
            ECGVEventTypeRef event = (ECGVEventTypeRef) _view->GetCurrEvent();
            listObservers[i]->Update(event);
        }
    }