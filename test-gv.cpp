//
//  Created by Yufeng Wu on 3/2/22.
//
#include "ECGraphicViewImp.h"
#include "ECObserver.h"

// Test graphical view code
int real_main(int argc, char **argv)
{
  const int widthWin = 500, heightWin = 500;
  ECGraphicViewImp gv(widthWin, heightWin);

  ECObserverSubject* view = &gv;
  view->_view = &gv;

  MouseUp *mouseUpObserver = new MouseUp(view);
  MouseDown *mouseDownObserver = new MouseDown(view);
  MouseMoving *mouseMovingObserver = new MouseMoving(view);
  SpaceUp* spaceUpObersver = new SpaceUp(view);
  DKeyUp* dKeyUpObserver = new DKeyUp(view);
  ZKeyUp* zKeyUpObserver = new ZKeyUp(view);
  YKeyUp* yKeyUpObserver = new YKeyUp(view);


  view->Attach(mouseUpObserver);
  view->Attach(mouseDownObserver);
  view->Attach(mouseMovingObserver);
  view->Attach(spaceUpObersver);
  view->Attach(dKeyUpObserver);
  view->Attach(zKeyUpObserver);
  view->Attach(yKeyUpObserver);

  // Set Default Mode to EDIT
  view->_mode = ECGRAPHICVIEW_EDITMODE;
  view->_modeStr = "Edit Mode";
    
  view->_isEditingRect = false;
  view->_mouseDown = false;

  view->_warning = "";

    // Set cursor positions to default
  view->cursorxDown = view->cursoryDown = view->cursorxUp = view->cursoryUp = -1;

  view->Clear(ECGV_REF_WHITE);
  cout << "Attached all observers" << endl;
  view->DrawModeLabel();
  view->FlipDisplay();

  gv.Show();

  return 0;
}

int main(int argc, char **argv)
{
    return real_main(argc, argv);
    //return al_run_main(argc, argv, real_main);
}

