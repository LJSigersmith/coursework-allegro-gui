//
//  Created by Yufeng Wu on 3/2/22.
//
#include "ECGraphicViewImp.h"
#include "ECObserver.h"
#include <iostream>

// Test graphical view code
int real_main(int argc, char **argv)
{
  const int widthWin = 500, heightWin = 500;
  ECGraphicViewImp gv(widthWin, heightWin);

  ECObserverSubject* view = &gv;
  view->_view = &gv;

  view->InitObserver();

  if (argc > 1) { view->LoadSaveFile(argv[1]); }

  gv.Show();

  if (argc > 1) { view->WriteToSaveFile(); }

  return 0;
}

int main(int argc, char **argv)
{
    return real_main(argc, argv);
    //return al_run_main(argc, argv, real_main);
}

