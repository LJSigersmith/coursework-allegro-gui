//
// ECObserver.cpp
//
//
//

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
void ZKeyUp::Update(ECGVEventType event) {
    if (event != ECGV_EV_KEY_UP_Z) { return; }
    cout << "Undo" << endl;
    _view->_windowObjects = _view->_history.back();
    _view->Clear(ECGV_WHITE);
    _view->DrawAllObjects();
    _view->_history.pop_back();
    cout << "End Undo" << endl;
}
void DKeyUp::Update(ECGVEventType event) {
    if (event != ECGV_EV_KEY_UP_D) { return; }
    if (_view->_isEditingRect == false) { return; }
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { return; }
    
    int i = 0;
    for (auto obj : _view->_windowObjects) {
        if (RectObject* rect = dynamic_cast<RectObject*>(obj)) {
            if (rect == _view->_editingRect) {
                _view->_windowObjects.erase(_view->_windowObjects.begin() + i);
            }
        }
        i++;
    }

    _view->_isEditingRect = false;
    _view->Clear(ECGV_WHITE);
    _view->DrawAllObjects();
}

void SpaceUp::Update(ECGVEventType event) {
    if (event == ECGV_EV_KEY_UP_SPACE) {
        cout << "Space Up" << endl;
        // Don't allow switching modes while currently editing or inserting
        if (_view->_isEditingRect || _view->_mouseDown) { return; }
        if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { _view->_mode = ECGRAPHICVIEW_INSERTIONMODE; _view->_modeStr = "Insert Mode"; }
        else { _view->_mode = ECGRAPHICVIEW_EDITMODE; _view->_modeStr = "Edit Mode"; }

        _view->Clear(ECGV_WHITE);
        _view->DrawAllObjects();
    }
}

void MouseDown::Update(ECGVEventType event) {
    if (event == ECGV_EV_MOUSE_BUTTON_DOWN) {
        cout << "Mouse Down" << endl;
        _view->cursorxDown = _view->cursorx;
        _view->cursoryDown = _view->cursory;
        _view->_mouseDown = true;
        _view->_firstMove = true;
    }
}

void MouseUp::Update(ECGVEventType event) {
    if (event != ECGV_EV_MOUSE_BUTTON_UP) { return; }
    if (_view->isEventQueueEmpty() == false) { return; }

    cout << "Mouse Up" << endl;
    _view->cursorxUp = _view->cursorx;
    _view->cursoryUp = _view->cursory;
    //_view->fRedraw = true;
    _view->_mouseDown = false;

    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) {
        _view->Clear(ECGV_WHITE);
        for (auto obj : _view->_windowObjects) {
            if (RectObject* rect = dynamic_cast<RectObject*>(obj)) {
                rect->_color = ECGV_BLACK;
            }
        }
        _view->_history.push_back(_view->_windowObjects);
        cout << "History Added" << endl;
        _view->DrawAllObjects();
    }

    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        if (_view->_isEditingRect == false) {
            cout << "Mouse Up and Editing" << endl;
            if (_view->isClickInsideRect(_view->cursorxUp, _view->cursoryUp)) {
                cout << "Click INSIDE rect" << endl;
                _view->_isEditingRect = true;
                _view->_editingRect->_color = ECGV_BLUE;
            } else {
                cout << "Click Outsid rect" << endl;
            }
        } else if (_view->_isEditingRect) {
            cout << "Mouse up and Not Editing" << endl;
            _view->_isEditingRect = false;

            RectObject *editedRect = new RectObject(_view->_editingRect->_x1, _view->_editingRect->_y1, _view->cursorxDown, _view->cursoryDown, 1, ECGV_BLACK);
            _view->_windowObjects.push_back(editedRect);
            _view->_history.push_back(_view->_windowObjects);
            cout << "History Added" << endl;
            cout << "New Edited Rect: x1:" << editedRect->_x1 << " y1: " << editedRect->_y1 << " x2: " << editedRect->_x2 << " y2: " << editedRect->_y2 << endl;
        }
        _view->Clear(ECGV_WHITE);
        _view->DrawAllObjects();
        //_view->FlipDisplay();
    }
}

void MouseMoving::Update(ECGVEventType event) {
    if (event != ECGV_EV_MOUSE_MOVING ) { return; }
    if (_view->isEventQueueEmpty() == false) { return; }

    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) {
                if(_view->cursorxDown >= 0 && _view->_mouseDown == true)
            {
                std::cout << "Drawing" << endl;
                //std::cout << "Current cursor position: " << cursorx << "," << cursory << endl;
                //RenderStart();

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
                //FlipDisplay();
                //DrawRectangle(200,200, 50, 50, 3);

                // Draw Previous Rects to Appear Continuous
                _view->DrawAllObjects();
                //_view->DrawModeLabel();
                //_view->FlipDisplay();

            }
            }
            
            if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
                if (_view->_isEditingRect && _view->isEventQueueEmpty()) {
                    if (_view->_firstMove) {
                        int i = 0;
                        for (auto obj : _view->_windowObjects) {
                            if (auto rect = dynamic_cast<RectObject*>(obj)) {
                                if (rect == _view->_editingRect) {
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
            }
}
