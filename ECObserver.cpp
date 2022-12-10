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
    //if (_view->_multiDragEnabled) { return; }
    if (_view->_windowObjects.size() == 0) { return; }

    _view->_multiSelectEnabled = true;
    _view->_singleSelectEnabled = false; // change this to an enum for edit mode
    _view->_warning = "Multi Select ON";

    _view->logFile << "Ctrl Key Down" << endl;

    ECMultiSelectionObject* selectionObj = new ECMultiSelectionObject();
    _view->_editingObj = selectionObj;
    _view->_isEditingObj = false;

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================ Ctrl Key Up =================================
void CtrlKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_CTRL) { return; }
    if (_view->_mode != ECGRAPHICVIEW_EDITMODE) { return; }
    //if (_view->_multiDragEnabled) { return; }
    //if (_view->_selectedObjects.size() == 0) { _view->_multiSelectEnabled = false; _view->_warning = ""; _view->Clear(ECGV_REF_WHITE); _view->DrawAllObjects(); return; }

    // If multi select enabled, disable it and enable editing Obj
    if (_view->_multiSelectEnabled) {
        //_view->_multiSelectEnabled = false;
        _view->_warning = "";
        _view->_isEditingObj = true;
        
        _view->logFile << "Ctrl Key Up" << endl;

        _view->Clear(ECGV_REF_WHITE);
        _view->DrawAllObjects();
        }
}

// ============================== F Key Up ====================================
// Switch Between Filled and Unfilled
void FKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_F) { return; }

    _view->logFile << "F Key Up" << endl;

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
    _view->logFile << "SHAPE MODE: " << _view->_shapeStr << endl;

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== G Key Up ====================================
// Switch Between Ellipse and Rectangle or Group / Ungroup
void GKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_G) { return; }
    _view->logFile << "G Key Up" << endl;
    // In edit mode: grouping
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { 
        if (_view->_multiSelectEnabled) {
            if (ECMultiSelectionObject* selection = dynamic_cast<ECMultiSelectionObject*>(_view->_editingObj)) {
                if (selection->_objectsInSelection.size() != 0) {
                    GroupObjects();
                    return;
                } else {
                    _view->_warning = "Select Objects to Group";
                    _view->Clear(ECGV_REF_WHITE);
                    _view->DrawAllObjects();
                    return;
                }
            } else {
                throw runtime_error("Multi Select Enabled but no selection object is being edited");
            }
        } else if (_view->_singleSelectEnabled) {
            if (_view->_hasSelectedObjectInGroup) {
                UnGroupObject();
                return;
            }
        }
    }
    
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) {
    // Switch Shape Mode to Ellipse
    if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT) { _view->_shape = ECGRAPHICVIEW_SHAPE_ELLIPSE; _view->_shapeStr = "Ellipse"; }
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_RECT_FILLED) { _view->_shape = ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED; _view->_shapeStr = "Filled Ellipse"; }

    // Switch Shape Mode to Rectangle
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE) { _view->_shape = ECGRAPHICVIEW_SHAPE_RECT; _view->_shapeStr = "Rectangle"; }
    else if (_view->_shape == ECGRAPHICVIEW_SHAPE_ELLIPSE_FILLED) { _view->_shape = ECGRAPHICVIEW_SHAPE_RECT_FILLED; _view->_shapeStr = "Filled Rectangle"; }
    
    else { throw runtime_error("SHAPE is not defined : GKey"); }

    _view->logFile << "FILL MODE: " << _view->_shapeStr << endl;
    _view->_lastShapeStr = _view->_shapeStr;

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
    }
}

void GKeyUp::GroupObjects() {

    _view->logFile << "G Key Up : Grouping Objects" << endl;
    ECMultiSelectionObject *selection = dynamic_cast<ECMultiSelectionObject*>(_view->_editingObj);

    // If selection hasn't moved since objects were selected, remove them from window
    for (auto obj : selection->_objectsInSelection) {
        auto i = _view->objectIndexInWindow(obj);
        if (i != _view->_windowObjects.end()) {
            _view->_windowObjects.erase(i);
        }
    }

    // Make New Group, set each object in group to be a member of group, and add obj to group
    // Create new instance of each object when adding to group
    ECGroupObject* newGroup = new ECGroupObject();
    for (auto obj : selection->_objectsInSelection) {
        ECWindowObject* groupObj;
        if (auto rect = dynamic_cast<ECRectObject*>(obj)) { groupObj = new ECRectObject(rect); }
        else if (auto ellipse = dynamic_cast<ECEllipseObject*>(obj)) { groupObj = new ECEllipseObject(ellipse); }
        else if (auto group = dynamic_cast<ECGroupObject*>(obj)) { groupObj = new ECGroupObject(group); }
        else { throw runtime_error("Creating new group : selected obj is not rect or ellipse"); }
        
        groupObj->_inGroup = newGroup;
        groupObj->setColor(ECGV_REF_BLACK);
        newGroup->addObject(groupObj);
        newGroup->addObjectFromBeforeGrouping(obj);
    }
    newGroup->_editedFrom = selection;

    // Add Group to Window
    _view->_windowObjects.push_back(newGroup);

    // Modify Undo Stack
    // If ediitng Obj, objects have moved, so need to store that for undo
    if (_view->_isEditingObj) {
        _view->_undo.push_back(selection);
    }
    // Regardless of editing Obj or not, grouping needs to be undone for undo
    _view->_undo.push_back(newGroup);

    _view->_warning = "Shapes Grouped";

    _view->_multiSelectEnabled = false;
    _view->_singleSelectEnabled = true;

    _view->_isEditingObj = false;
    _view->_editingObj = nullptr;
    _view->_objBeforeEdit = nullptr;

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

void GKeyUp::UnGroupObject() {

    // If the group hasn't moved since being selected (still in windowObjects), remove the group from the window
    auto i = _view->objectIndexInWindow(_view->_editingObj);
    if (i != _view->_windowObjects.end()) {
        cout << "erasing gorup" << endl;
        _view->_windowObjects.erase(i);
    }

    ECWindowObject* primaryObject = _view->_selectedObjectInGroup;
    ECGroupObject* selectedGroup = dynamic_cast<ECGroupObject*>(_view->_editingObj);
    if (!selectedGroup) { throw runtime_error("Editing Obejct is not a Group when Ungrouping"); }
    
    // Make new group without selected object
    ECGroupObject* newGroup = new ECGroupObject(selectedGroup, primaryObject);

    // Make new object for selected object
    ECRectObject* rect = dynamic_cast<ECRectObject*>(primaryObject);
    ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(primaryObject);
    ECGroupObject* group = dynamic_cast<ECGroupObject*>(primaryObject);
    
    ECWindowObject* newObject;
    if (rect) {
        ECRectObject* newRect = new ECRectObject(rect);
        newObject = newRect;
    } else if (ellipse) {
        ECEllipseObject* newEllipse = new ECEllipseObject(ellipse);
        newObject = newEllipse;
    } else if (group) {
        ECGroupObject* nGroup = new ECGroupObject(group);
        newObject = nGroup;
    }

    newObject->_createdFromUngrouping = true;
    newObject->_fromGroup = selectedGroup;

    // Set color to black
    newObject->setColor(ECGV_REF_BLACK);
    newGroup->setColor(ECGV_REF_BLACK);

    // Modify Undo Stack
    // Add to window

    // If the new group has only one object
    if (newGroup->_collectionObjects.size() == 1) {
        ECWindowObject* objFromGroup = newGroup->_collectionObjects[0];
        _view->_undo.push_back(objFromGroup);
        _view->_undo.push_back(newObject);

        _view->_windowObjects.push_back(objFromGroup);
        _view->_windowObjects.push_back(newObject);
    } else {
        _view->_undo.push_back(newGroup);
        _view->_undo.push_back(newObject);

        _view->_windowObjects.push_back(newGroup);
        _view->_windowObjects.push_back(newObject);
    }

    // Done editing for now
    _view->_isEditingObj = false;
    _view->_editingObj = nullptr;
    _view->_objBeforeEdit = nullptr;

    // Done grouping
    _view->_hasSelectedObjectInGroup = false;
    _view->_selectedGroup = nullptr;
    _view->_selectedObjectInGroup = nullptr;

    _view->_warning = "Shape Removed From Group";

    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== Space Up ====================================
// Switch Between Insert and Edit Modes

void SpaceUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_SPACE) { return; }
    _view->logFile << "Space Key Up" << endl;
    
    _view->_warning = "";

    // Don't allow switching modes while currently editing or inserting
    if (_view->_isEditingObj || _view->_mouseDown) { return; }
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) { _view->_mode = ECGRAPHICVIEW_INSERTIONMODE; _view->_modeStr = "Insert Mode"; _view->_shapeStr = _view->_lastShapeStr; }
    else if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { _view->_mode = ECGRAPHICVIEW_EDITMODE; _view->_modeStr = "Edit Mode"; _view->_lastShapeStr = _view->_shapeStr; _view->_shapeStr = ""; }
    else { throw runtime_error("MODE is not defined"); }

    _view->logFile << "MODE: " << _view->_modeStr << endl;
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

// ============================== Y Key Up ====================================
// Redo
void YKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_Y) { return; }
    _view->logFile << "Y Key Up" << endl;
    _view->Clear((ECGVColorRef) ECGV_WHITE);

    // If nothing to redo, set warning and return
    if (_view->_redo.size() == 0) { 
        _view->_warning = "Nothing to Redo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    ECWindowObject* objToRedo = _view->_redo.back();

    // Check what type of object is being redone
    ECDeletedObject* dead = dynamic_cast<ECDeletedObject*>(objToRedo);
    ECMultiSelectionObject* selection = dynamic_cast<ECMultiSelectionObject*>(objToRedo);
    ECGroupObject* group = dynamic_cast<ECGroupObject*>(objToRedo);
    
    if (dead) {
        RedoDeletion(dead); return;
    } else if (selection) {
        RedoSelection(selection); return;
    } else if (objToRedo->_createdFromUngrouping) {
        RedoUngroup(objToRedo); return;
    } else if (group) {
        ECMultiSelectionObject *groupFrom = dynamic_cast<ECMultiSelectionObject*>(group->_editedFrom);
        if (groupFrom) {
            RedoGrouping(group, groupFrom); return;
            return;
        }
    }

    // If object wasn't deleted, it was either drawn or edited
    ECWindowObject* obj = objToRedo;

    // Grouped Object
    // If the grouped object wasn't edited from anything, then it was created by grouping
    // Redo the grouping

    // Add it back to window, modify undo and redo stack
    _view->_windowObjects.push_back(obj);
    _view->_undo.push_back(obj);
    _view->_redo.pop_back();
    
    // If its just a regular object, it was either moved or drawn

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


void YKeyUp::RedoDeletion(ECDeletedObject* dead) {
    // Get object in window, check if it exists
    auto i = _view->objectIndexInWindow(dead->_alive);
    if (i == _view->_windowObjects.end()) { throw runtime_error("Object to be deleted not in window"); }

    // Delete object from window, modify undo and redo stack and return
    _view->_windowObjects.erase(i);
    _view->_redo.pop_back();
    _view->_undo.push_back(dead);
    _view->DrawAllObjects();

    return;
}
void YKeyUp::RedoSelection(ECMultiSelectionObject* selection) {

    // Remove objects from before from window
    for (auto obj : selection->_objectsBeforeSelectionMoved) {
         auto i = _view->objectIndexInWindow(obj);
        if (i == _view->_windowObjects.end()) { throw runtime_error("Object in selection before is null at redo"); }
        _view->_windowObjects.erase(i);
    }

    // Add objects from after move to window
    for (auto obj : selection->_objectsInSelection) {
        _view->_windowObjects.push_back(obj);
    }

    _view->_redo.pop_back();
    _view->_undo.push_back(selection);
    _view->DrawAllObjects();

}
void YKeyUp::RedoGrouping(ECGroupObject* group, ECMultiSelectionObject* from) {
    // Remove the individual objects that will make up the new group from the window
    for (auto obj : from->_objectsInSelection) {
        auto i = _view->objectIndexInWindow(obj);
        if (i == _view->_windowObjects.end()) { throw runtime_error("Object to be regrouped is not in window before regrouping"); }
        _view->_windowObjects.erase(i);
    }
    _view->_warning = "Regrouped Shapes";
    
    // Add the group made of the objects to the window
    _view->_windowObjects.push_back(group);

    _view->_undo.push_back(group);
    _view->_redo.pop_back();
    _view->DrawAllObjects();

}
void YKeyUp::RedoUngroup(ECWindowObject* objCreatedFromUngrouping) {

    // Add Object Created From Ungrouping To Window
    _view->_windowObjects.push_back(objCreatedFromUngrouping);
    
    // Get Group Created After Ungrouping
    // This could be just an object (if the group after ungrouping an object only had one object in it)
    _view->_redo.pop_back();
    ECWindowObject* groupAfterUngroup = _view->_redo.back();
    if (!groupAfterUngroup) { throw runtime_error("group after ungroup is null : wasn't pushed to redo"); }

    // Add it to the window
    _view->_windowObjects.push_back(groupAfterUngroup);

    // Remove group that object was created from from window
    ECGroupObject* fromGroup = dynamic_cast<ECGroupObject*>(objCreatedFromUngrouping->_fromGroup);
    if (!fromGroup) { throw runtime_error("group object created from is null at redo time");}
    auto i = _view->objectIndexInWindow(fromGroup);
    if (i == _view->_windowObjects.end()) { throw runtime_error("from group not in window");}
    _view->_windowObjects.erase(i);

    _view->_undo.push_back(groupAfterUngroup);
    _view->_undo.push_back(objCreatedFromUngrouping);

    _view->_redo.pop_back();
    _view->DrawAllObjects();
}
// ============================== Z Key Up ====================================
// Undo
void ZKeyUp::UndoDeletion(ECDeletedObject* dead) {
    _view->_windowObjects.push_back(dead->_alive);
    _view->_undo.pop_back();
    _view->_redo.push_back(dead);
    _view->DrawAllObjects();
}
void ZKeyUp::UndoSelection(ECMultiSelectionObject* selection) {
    
    // Remove all the objects from after selection from window
    for (auto obj : selection->_objectsInSelection) {
        auto i = _view->objectIndexInWindow(obj);
        if (i == _view->_windowObjects.end()) { throw runtime_error("Object in Selection Does not exist when undone in window");}
        _view->_windowObjects.erase(i);
    }

    // Add each object from before selection to window
    for (auto obj : selection->_objectsBeforeSelectionMoved) {
        obj->setColor(ECGV_REF_BLACK);
        _view->_windowObjects.push_back(obj);
    }

    _view->_undo.pop_back();
    _view->_redo.push_back(selection);
    _view->DrawAllObjects();
}
void ZKeyUp::UndoGrouping(ECGroupObject* group, ECMultiSelectionObject* from) {

    // Remove group from window
    auto i = _view->objectIndexInWindow(group);
    if (i == _view->_windowObjects.end()) { throw runtime_error("Grouping being undone: group does not exist in window"); }
    _view->_windowObjects.erase(i);

    // Add objects that made up group back to window as individual objects
    for (auto objBeforeGrouping : from->_objectsInSelection) {
        cout << "Object From Before Added to Window" << endl;
        objBeforeGrouping->setColor(ECGV_REF_BLACK);
        _view->_windowObjects.push_back(objBeforeGrouping);
        cout << "Number of Objects in Window Now: " << _view->_windowObjects.size() << endl;
    }
    _view->_warning = "Ungrouped Shapes";

    _view->_undo.pop_back();
    _view->_redo.push_back(group);
    _view->DrawAllObjects();
}

void ZKeyUp::UndoUngroup(ECWindowObject* objCreatedFromUngrouping) {

    // Remove object from window
    auto iObj = _view->objectIndexInWindow(objCreatedFromUngrouping);
    if (iObj == _view->_windowObjects.end()) { throw runtime_error("objCreatedFromUngrouping is not in window at undo time"); }
    _view->_windowObjects.erase(iObj);

    // Group was created without split object during ungrouping
    // That group was either drawn, or it only had one object, so only the object was drawn
    // Object is either a regular object or a group, either way remove it from window
    _view->_undo.pop_back();
    ECWindowObject* objToRemove = dynamic_cast<ECWindowObject*>(_view->_undo.back());
    auto i = _view->objectIndexInWindow(objToRemove);
    if (i == _view->_windowObjects.end()) { throw runtime_error("Object from before group was split is not in window at undo time"); }
    _view->_windowObjects.erase(i);

    // Remove group that was created without split object from window
    //ECGroupObject* groupWithoutObject = dynamic_cast<ECGroupObject*>(_view->_undo.back());
    //if (!groupWithoutObject) { throw runtime_error("Group without split object was not pushed to undo before an object created from ungrouping at undo time"); }
    //auto iGroup = _view->objectIndexInWindow(groupWithoutObject);

    
    //if (iGroup == _view->_windowObjects.end()) { throw runtime_error("Group without object is not in window");}
    //_view->_windowObjects.erase(iGroup);

    // Restore group with object to window
    ECGroupObject* groupToRestore = dynamic_cast<ECGroupObject*>(objCreatedFromUngrouping->_fromGroup);
    if (!groupToRestore) { throw runtime_error("Group created from ungrouping has null _fromGroup"); }
    _view->_windowObjects.push_back(groupToRestore);

    _view->_warning = "Regrouped Shapes";

    _view->_undo.pop_back();
    _view->_redo.push_back(objToRemove);
    _view->_redo.push_back(objCreatedFromUngrouping);
    _view->DrawAllObjects();

}
void ZKeyUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_KEY_UP_Z) { return; }
    _view->logFile << "Z Key Up" << endl;

    _view->Clear((ECGVColorRef) ECGV_WHITE);

    // If nothing to undo, set warning, and return
    if (_view->_undo.size() == 0) { 
        _view->_warning = "Nothing to Undo";

        _view->DrawAllObjects();
        _view->_warning = "";
        return; 
    }

    ECWindowObject* objToUndo = _view->_undo.back();

    // Check what type of object is being undone
    ECDeletedObject* dead = dynamic_cast<ECDeletedObject*>(objToUndo);
    ECMultiSelectionObject* selection = dynamic_cast<ECMultiSelectionObject*>(objToUndo);
    ECGroupObject* group = dynamic_cast<ECGroupObject*>(objToUndo);

    // If object was deleted, we need to undo that deletion and add it back to window
    if (dead) {
        UndoDeletion(dead);
        return;
    }

    if (selection) {
        UndoSelection(selection);
        return;
    }

    // If the object was created from ungrouping, we need to add it back to the group and add that group back
    if (objToUndo->_createdFromUngrouping) {
        UndoUngroup(objToUndo);
        return;
    }

    // If the object is a group, and wasn't created by an edit, we need to remove the group and add the individual objects back
    if (group) {
        ECMultiSelectionObject *groupFrom = dynamic_cast<ECMultiSelectionObject*>(group->_editedFrom);
        if (groupFrom) {
            UndoGrouping(group, groupFrom);
            return;
        }
    }

    // If its just a regular object, it was either moved or drawn
    
    // Get object in window, if it isn't in window, return
    auto objectIndex = _view->objectIndexInWindow(objToUndo);
    if (objectIndex == _view->_windowObjects.end()) { throw runtime_error("Object to Undo is not in window"); }

    // Remove object from window
    ECWindowObject* obj = *objectIndex;
    _view->_windowObjects.erase(objectIndex);

    // Modify Undo and Redo Stack
    _view->_redo.push_back(objToUndo);
    _view->_undo.pop_back();
    
    // If object was edited from another object, set it to black as it
    // would've been stored blue, and add that object to window
    if (obj->_editedFrom != NULL) {

        obj->_editedFrom->setColor(ECGV_REF_BLACK);
        _view->_windowObjects.push_back(obj->_editedFrom);
    }
    
    cout << "Objects to Be Drawn: " << _view->_windowObjects.size() << endl;
    _view->DrawAllObjects();
}

// ============================== D Key Up ====================================
// Delete
void DKeyUp::Update(ECGVEventTypeRef event) {
    
    if (event != ECGV_REF_EV_KEY_UP_D) { return; }
    _view->logFile << "D Key Up" << endl;
    
    // Can't delete in insertion mode or while editing
    //if (_view->_isEditingObj == false) { return; }
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { return; }
    
    if (_view->_multiSelectEnabled) {
        ECMultiSelectionObject* selection = dynamic_cast<ECMultiSelectionObject*>(_view->_editingObj);

        // Create deleted representation of MSObj
        ECDeletedObject* dead = new ECDeletedObject(selection);

        // Add deletion to undo stack
        _view->_undo.push_back(selection);

        // Multi Select Done
        _view->_isEditingObj = false;
        _view->_editingObj = nullptr;
        _view->_objBeforeEdit = nullptr;

        _view->_multiSelectEnabled = false;
        _view->_singleSelectEnabled = true;

        _view->Clear((ECGVColorRef) ECGV_WHITE);
        _view->DrawAllObjects();

        return;
    }
    // Get the object that will be deleted
    auto objIndex = _view->objectIndexInWindow(_view->_editingObj);
    if (objIndex == _view->_windowObjects.end()) { 
        // Object not in window, either error or currently editing object, so return
        return;
    }
    
    ECWindowObject* obj = *objIndex;
    // Set object to black as it will have been blue while selected
    obj->setColor(ECGV_REF_BLACK);
    //if (auto group = dynamic_cast<ECGroupObject*>(obj)) { group->setAllColor(ECGV_REF_BLACK); }
    
    // Create the deleted representation of this object
    ECDeletedObject* dead = new ECDeletedObject(obj);

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
    _view->logFile << "Mouse Down" << endl;
    cout << "Mouse Down" << endl;
    _view->cursorxDown = _view->getCurrentCursorX();
    _view->cursoryDown = _view->getCurrentCursorY();
    _view->_mouseDown = true;
    _view->_firstMove = true;
    _view->_warning = "";

}

// ============================ Mouse Moving ==================================

void MouseMoving::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_MOVING ) { return; }
    if (_view->isQueueEmpty() == false) { return; }
    if (_view->_firstMove) { _view->logFile << "Mouse Began Moving" << endl; }

    // In insertion mode, mouse moving is either nothing or there has been a click before
    // If there was a click before, we're drawing an object
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { 
        if(_view->cursorxDown >= 0 && _view->_mouseDown == true) { InsertMode(); return; }
    }
    
    // In editing mode, mouse is either just moving around, or we're moving an object
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        if (!_view->isQueueEmpty()) { return; }
        if(_view->_arrowMovementEnabled) { return; }
        if (!_view->_isEditingObj) { return; }

        if (_view->_singleSelectEnabled) { SingleSelect(); return; }
        if (_view->_multiSelectEnabled) { MultiSelect(); return; }
        //if (_view->_isEditingObj && _view->isQueueEmpty()) { EditMode(); }
    }
}

void MouseMoving::SingleSelect() {
    if (_view->_firstMove) {
        auto i = _view->objectIndexInWindow(_view->_editingObj);
        if (i == _view->_windowObjects.end()) { return; }
        _view->_windowObjects.erase(i);
        _view->_firstMove = false;
    }

    _view->Clear(ECGV_REF_WHITE);
    int cursorX = _view->getCurrentCursorX();
    int cursorY = _view->getCurrentCursorY();

    ECWindowObject* movingObject = _view->getNewMovingObject(_view->_editingObj, cursorX, cursorY);
    movingObject->setColor(ECGV_REF_BLUE);
    movingObject->setFilled(movingObject->isFilled());

    _view->DrawObject(movingObject);
    _view->_editingObj = movingObject;

    _view->DrawAllObjects();
}

void MouseMoving::MultiSelect() {

    if (_view->_mouseDown) { return; }

    ECMultiSelectionObject* selection = dynamic_cast<ECMultiSelectionObject*>(_view->_editingObj);

    int cursorX = _view->getCurrentCursorX();
    int cursorY = _view->getCurrentCursorY();

    if (_view->_firstMove) {
        // Remove objects in selection from window
        for (auto obj : selection->_objectsInSelection) {
            auto i = _view->objectIndexInWindow(obj);
            if (i == _view->_windowObjects.end()) { throw runtime_error("Selection object not in window"); }
            _view->_windowObjects.erase(i);

            _view->setSelectionDistFromCursor(selection, cursorX, cursorY);
        }
        _view->_firstMove = false;
    }
    _view->Clear(ECGV_REF_WHITE);

    vector<ECWindowObject*> objectsInSelection = selection->_objectsInSelection;
    selection->_objectsInSelection.clear();
    assert(selection->_objectsInSelection.size() == 0);
    
    // No objects were selected or error, either way end multi select
    if (objectsInSelection.size() <= 0) {
        _view->_isEditingObj = false;
        _view->_editingObj = nullptr;
        _view->_objBeforeEdit = nullptr;

        _view->_multiSelectEnabled = false;
        _view->_singleSelectEnabled = true;
        return;
    }

    for (auto obj : objectsInSelection) {
        ECWindowObject* movingObj = _view->getNewMovingObject(obj, cursorX, cursorY);
        selection->_objectsInSelection.push_back(movingObj);
    }

    _view->DrawObject(selection);
    _view->DrawAllObjects();
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

// ============================== Mouse Up ====================================
void MouseUp::Update(ECGVEventTypeRef event) {
    if (event != ECGV_REF_EV_MOUSE_BUTTON_UP) { return; }
    if (_view->isQueueEmpty() == false) { return; }
    _view->logFile << "Mouse Up" << endl;
    _view->cursorxUp = _view->getCurrentCursorX();
    _view->cursoryUp = _view->getCurrentCursorY();
    _view->_mouseDown = false;

    // Mouse Up in Insert Mode means an object is done being drawn and should be added to the window
    if (_view->_mode == ECGRAPHICVIEW_INSERTIONMODE) { InsertMode(); }

    // Mouse Up in Edit Mode means an object has been selected or an object is done being moved
    if (_view->_mode == ECGRAPHICVIEW_EDITMODE) {
        if (_view->_multiSelectEnabled) { MultiSelect(); return; }
        if (_view->_singleSelectEnabled) { SingleSelect(); return; }
        //if (_view->_multiDragEnabled) { EndMultiDrag(); return; }
        // If an object is currently being moved, keep moving it around
       //} else if (_view->_isEditingObj) {
       //     EditMode();
       // }

        _view->Clear(ECGVColorRef(ECGV_WHITE));
        _view->DrawAllObjects();
    }
}

void MouseUp::SingleSelect() {

    // Clicked on view : Was an object clicked?
    if (!_view->_isEditingObj) {
        ECWindowObject* clickedObject = _view->isClickInsideObj(_view->cursorxUp, _view->cursoryUp);
        bool clickInside = (clickedObject != nullptr);
        if (clickInside) {
            if (dynamic_cast<ECGroupObject*>(clickedObject)) { _view->logFile << "Clicked Inside Group" << endl; } else { _view->logFile << "Clicked Inside Object" << endl;}
            _view->_isEditingObj = true;
            _view->_editingObj = clickedObject;
            _view->_editingObj->setColor(ECGV_REF_BLUE);
            if (_view->_hasSelectedObjectInGroup) { _view->_selectedObjectInGroup->_color = ECGV_REF_PURPLE; }

            int cursorX, cursorY;
            _view->firstClickX = cursorX;
            _view->firstClickY = cursorY;

            _view->setWindowObjDistFromCursor(_view->_editingObj,cursorX,cursorY);
            _view->_objBeforeEdit = _view->_editingObj;
        }
    
    
    }
    // Clicked on view : Commit edit 
    else {
        _view->logFile << "Single Select : Committing Edit" << endl;
        ECWindowObject* _editedObj = _view->_editingObj;
        _view->_objBeforeEdit->setColor(ECGV_REF_RED);

        // If either the object being committed or the obj it was edited from is null throw error
        if (!_editedObj || !_view->_objBeforeEdit) { throw runtime_error("Edited object or object before edit is null"); }

        // Set edited from (for undo/redo) and color to black
        _editedObj->_editedFrom = _view->_objBeforeEdit;
        _editedObj->setColor(ECGV_REF_BLACK);
        // _editedGroup->isMoving = false;

        _view->_windowObjects.push_back(_editedObj);
        _view->_undo.push_back(_editedObj);

        _view->_isEditingObj = false;
        _view->_editingObj = nullptr;
        if (_view->_hasSelectedObjectInGroup) {
            _view->_hasSelectedObjectInGroup = false;
            _view->_selectedObjectInGroup = nullptr;
            _view->_selectedGroup = nullptr;
        }
        _view->_objBeforeEdit = nullptr;
    }
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
}

void MouseUp::MultiSelect() {

    ECMultiSelectionObject* multipleSelectionObj = dynamic_cast<ECMultiSelectionObject*>(_view->_editingObj);
    if (!_view->_isEditingObj) {
        ECWindowObject* clickedObject = _view->isClickInsideObj(_view->cursorxUp, _view->cursoryUp);
        bool clickInside = (clickedObject != nullptr);
        // Clicked on object : Add it to the selection or remove it
        if (clickInside) {
        if (dynamic_cast<ECGroupObject*>(clickedObject)) { _view->logFile << "Clicked Inside Group" << endl; } else { _view->logFile << "Clicked Inside Object" << endl;}
        int cursorX = _view->cursorxUp;
        int cursorY = _view->cursoryUp;

        // Check if object in selection, remove from selection if it is, and set back to black
        bool objInSelection = multipleSelectionObj->objectInSelection(clickedObject, true);
        if (objInSelection) {
            clickedObject->setColor(ECGV_REF_BLACK);
        // Otherwise, add it to the selection
        } else {
            clickedObject->setColor(ECGV_REF_BLUE);
            multipleSelectionObj->addObjectToSelection(clickedObject);
        }

        // Same for objects before selection
        bool objInSelectionBeforeMoved = multipleSelectionObj->objectInSelectionBeforeMoved(clickedObject, true);
        if (objInSelectionBeforeMoved) {
            clickedObject->setColor(ECGV_REF_BLACK);
        } else {
        // Otherwise, add it to to selection before moved
            clickedObject->setColor(ECGV_REF_BLUE);
            multipleSelectionObj->addObjectToSelectionBeforeMoved(clickedObject);
        }
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
    }
    } else {
        _view->logFile << "Multi Select: Committing Edit" << endl;
        if (!_view->_editingObj) { throw runtime_error("Selection object is null when committing it");}

        for (auto obj : multipleSelectionObj->_objectsInSelection) {
            obj->setColor(ECGV_REF_BLACK);
            _view->_windowObjects.push_back(obj);
        }
        _view->_undo.push_back(multipleSelectionObj);

        _view->_isEditingObj = false;
        _view->_editingObj = nullptr;
        _view->_objBeforeEdit = nullptr;

        _view->_multiSelectEnabled = false;
        _view->_singleSelectEnabled = true;
    }
    _view->Clear(ECGV_REF_WHITE);
    _view->DrawAllObjects();
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

    _view->_singleSelectEnabled = true;

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
    cout << "obj index in window" << endl;
    for (ECWindowObject* o : _windowObjects) {
        cout << "o in window: " << o << endl;
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
// Check if object is in undo stack
std::__1::__wrap_iter<ECWindowObject **>ECObserverSubject::objectIndexInUndoStack(ECWindowObject* obj) {
    auto i = _undo.begin();
    for (ECWindowObject* o : _undo) {
        if (o == obj) {
            return i;
        }
        i++;
    }
    return _undo.end();
}
// Check if object is in redo stack
std::__1::__wrap_iter<ECWindowObject **>ECObserverSubject::objectIndexInRedoStack(ECWindowObject* obj) {
    auto i = _redo.begin();
    for (ECWindowObject* o : _redo) {
        if (o == obj) {
            return i;
        }
        i++;
    }
    return _redo.end();
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

void ECObserverSubject::DrawSelection(ECMultiSelectionObject* selection) {
    auto selectionObjs = selection->_objectsInSelection;
    for (auto obj : selectionObjs) {
        DrawObject(obj);
    }
}

void ECObserverSubject::DrawObject(ECWindowObject* obj) {
    if (auto rect = dynamic_cast<ECRectObject*>(obj)) {
        DrawRectangle(rect->_x1, rect->_y1, rect->_x2, rect->_y2, rect->_thickness, rect->_color, rect->isFilled());
    }
    else if (auto ellipse = dynamic_cast<ECEllipseObject*>(obj)) {
        DrawEllipse(ellipse->_xCenter, ellipse->_yCenter, ellipse->_xRadius, ellipse->_yRadius, ellipse->_thickness, ellipse->_color, ellipse->isFilled());
    }
    else if (auto group = dynamic_cast<ECGroupObject*>(obj)) {
        DrawGroup(group);
    } else if (auto selection = dynamic_cast<ECMultiSelectionObject*>(obj)) {
        cout << "drawing selection" << endl;
        DrawSelection(selection);
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

    ECRectObject* movingRect = new ECRectObject(x1,y1,x2,y2,1,originalRect->_color,originalRect->isFilled());
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

    ECEllipseObject* movingEllipse = new ECEllipseObject(xC, yC, xR, yR, 1, originalEllipse->_color, originalEllipse->isFilled());
    movingEllipse->_editedFrom = originalEllipse;
    movingEllipse->setDistFromCursor(xcD, ycD, 0);
    return movingEllipse;
}
ECGroupObject* ECObserverSubject::getMovingGroup(ECGroupObject* originalGroup, int cursorX, int cursorY) {
    auto groupObjects = originalGroup->objectsInGroup();
    auto primaryObject = _view->_selectedObjectInGroup;

    ECGroupObject* movingGroup = new ECGroupObject();
    for (auto groupObj : *groupObjects) {
        if (groupObj == primaryObject) {
            if (groupObj->_color == ECGV_REF_PURPLE) { cout << "IS PURPLE" << endl;}
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
ECRectObject* ECObserverSubject::isPointInsideRect(ECRectObject* _rect, float xp, float yp) {

    float maxX = max(_rect->_x1, _rect->_x2);
    float maxY = max(_rect->_y1, _rect->_y2);

    float minX = min(_rect->_x1, _rect->_x2);
    float minY = min(_rect->_y1, _rect->_y2);

    if (xp >= minX && xp <= maxX && yp >= minY && yp <= maxY) {
        return _rect;
    }
    return nullptr;
}

ECEllipseObject* ECObserverSubject::isPointInsideEllipse(ECEllipseObject* _ellipse, float xp, float yp) {
    
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

    if (result <= 1) { return _ellipse; }
    return nullptr;
}

// Determine if a click is inside an object, and if it is, set that object to the one now being edited
ECWindowObject* ECObserverSubject::isClickInsideObj(float xp, float yp) {
    
    for (auto obj : _windowObjects) {
        
        ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
        ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(obj);
        ECGroupObject* group = dynamic_cast<ECGroupObject*>(obj);

        if (group) {
            for (auto groupObj : *(group->objectsInGroup())) {
                ECRectObject* rect = dynamic_cast<ECRectObject*>(groupObj);
                ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(groupObj);

                if (rect) {
                    if (isPointInsideRect(rect, cursorxDown, cursoryDown)) {
                        _view->_hasSelectedObjectInGroup = true;
                        _view->_selectedGroup = group;
                        _view->_selectedObjectInGroup = rect;

                        //_editingObj = group;
                        return group;
                        break;
                    }
                }
                if (ellipse) {
                    if (isPointInsideEllipse(ellipse, cursorxDown, cursoryDown)) {
                        _view->_hasSelectedObjectInGroup = true;
                        _view->_selectedGroup = group;
                        _view->_selectedObjectInGroup = ellipse;

                        //_editingObj = group;
                        return group;
                        break;
                    }
                }
            }
        }

        if (rect) {
            if (isPointInsideRect(rect, cursorxDown, cursoryDown)) {
                //_editingObj = rect;
                return rect;
                break;
            }
        }

        if (ellipse) {
            if (isPointInsideEllipse(ellipse, cursorxDown, cursoryDown)) {
                //_editingObj = ellipse;
                return ellipse;
                break;
            }
        }
    }

    return nullptr;

}

// ==== Temp ======
void ECObserverSubject::setGroupDistFromCursor(ECGroupObject* group, int cursorX, int cursorY) {
    for (auto obj : group->_collectionObjects) {
        ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
        ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(obj);
        ECGroupObject* group = dynamic_cast<ECGroupObject*>(obj);

        if (rect) { rect->setDistFromCursor(cursorX, cursorY); }
        else if (ellipse) { ellipse->setDistFromCursor(cursorX, cursorY); }
        else if (group) { setGroupDistFromCursor(group,cursorX,cursorY); }
        else { throw std::runtime_error("GROUP SET DIST : Group obj is not valid shape");}
    }
}

void ECObserverSubject::setSelectionDistFromCursor(ECMultiSelectionObject* selection, int cursorX, int cursorY) {
    for (auto obj : selection->_objectsInSelection) {
        ECRectObject* rect = dynamic_cast<ECRectObject*>(obj);
        ECEllipseObject* ellipse = dynamic_cast<ECEllipseObject*>(obj);
        ECGroupObject* group = dynamic_cast<ECGroupObject*>(obj);

        if (rect) { rect->setDistFromCursor(cursorX, cursorY); }
        else if (ellipse) { ellipse->setDistFromCursor(cursorX, cursorY); }
        else if (group) { setGroupDistFromCursor(group,cursorX,cursorY); }
        else { throw std::runtime_error("GROUP SET DIST : Group obj is not valid shape");}
    }
}
void ECObserverSubject::setWindowObjDistFromCursor(ECWindowObject* obj, int cursosrX, int cursorY) {
    
    ECRectObject* _editingRect = dynamic_cast<ECRectObject*>(obj);
    ECEllipseObject* _editingEllipse = dynamic_cast<ECEllipseObject*>(obj);
    ECGroupObject* _editingGroup = dynamic_cast<ECGroupObject*>(obj);

    // Set distance of points from cursor
    // When moving objects, need this
    if (_editingRect) {
        _editingRect->setDistFromCursor(_view->cursorxUp, _view->cursoryUp);
    }
    else if (_editingEllipse) {
        _editingEllipse->setDistFromCursor(_view->cursorxUp, _view->cursoryUp);
    } else if (_editingGroup) {
        _view->setGroupDistFromCursor(_editingGroup, _view->cursorxUp, _view->cursoryUp);
    } else {
        throw runtime_error("Cast to ellipse and rect were NULL in MouseUp EDIT Mode");
    }
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

    CreateLog();
}