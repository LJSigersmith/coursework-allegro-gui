//
//  ECObserver.h
//
//
//  Created by Yufeng Wu on 2/27/20.
//
//

#ifndef ECOBERVER_H
#define ECOBERVER_H

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class ECGraphicViewImp;
//********************************************
// Observer design pattern: observer interface
// Supported event codes

enum ECGVEventType
{
    ECGV_EV_NULL = -1,
    ECGV_EV_CLOSE = 0,
    ECGV_EV_KEY_UP_UP = 1,
    ECGV_EV_KEY_UP_DOWN = 2,
    ECGV_EV_KEY_UP_LEFT = 3,
    ECGV_EV_KEY_UP_RIGHT = 4,
    ECGV_EV_KEY_UP_ESCAPE = 5,
    ECGV_EV_KEY_DOWN_UP = 6,
    ECGV_EV_KEY_DOWN_DOWN = 7,
    ECGV_EV_KEY_DOWN_LEFT = 8,
    ECGV_EV_KEY_DOWN_RIGHT = 9,
    ECGV_EV_KEY_DOWN_ESCAPE = 10,
    ECGV_EV_TIMER = 11,
    ECGV_EV_MOUSE_BUTTON_DOWN = 12,
    ECGV_EV_MOUSE_BUTTON_UP = 13,
    ECGV_EV_MOUSE_MOVING = 14,
    // more keys
    ECGV_EV_KEY_UP_Z = 15,
    ECGV_EV_KEY_DOWN_Z = 16,
    ECGV_EV_KEY_UP_Y = 17,
    ECGV_EV_KEY_DOWN_Y = 18,
    ECGV_EV_KEY_UP_D = 19,
    ECGV_EV_KEY_DOWN_D = 20,
    ECGV_EV_KEY_UP_SPACE = 21,
    ECGV_EV_KEY_DOWN_SPACE = 22,
    ECGV_EV_KEY_DOWN_G = 23,
    ECGV_EV_KEY_UP_G = 24,
    ECGV_EV_KEY_DOWN_F = 25,
    ECGV_EV_KEY_UP_F = 26,
    ECGV_EV_KEY_DOWN_CTRL = 27,
    ECGV_EV_KEY_UP_CTRL = 28
};

class ECObserver
{
public:
    virtual ~ECObserver() {}
    virtual void Update(ECGVEventType event) = 0;
protected :
    ECGraphicViewImp* _view;
};

class SpaceUp : public ECObserver {
    public :
        SpaceUp(ECGraphicViewImp* view) {
            _view = view;
        }
        ~SpaceUp() {}
        void Update(ECGVEventType event);
};

class MouseDown : public ECObserver {
    public :
        MouseDown(ECGraphicViewImp* view) {
            _view = view;
        }
        ~MouseDown() {}
        void Update(ECGVEventType event);
};

class MouseUp : public ECObserver {
    public :
        MouseUp(ECGraphicViewImp* view) {
            _view = view;
        }
        ~MouseUp() {}
        void Update(ECGVEventType event);
};

class MouseMoving : public ECObserver {
    public :
        MouseMoving(ECGraphicViewImp* view) {
            _view = view;
        }
        ~MouseMoving() {}
        void Update(ECGVEventType event);
};

class DKeyUp : public ECObserver {
    public :
        DKeyUp(ECGraphicViewImp* view) {
            _view = view;
        }
        ~DKeyUp() {}
        void Update(ECGVEventType event);
};

class ZKeyUp : public ECObserver {
    public :
        ZKeyUp(ECGraphicViewImp* view) {
            _view = view;
        }
        ~ZKeyUp() {}
        void Update(ECGVEventType event);
};

//********************************************
// Observer design pattern: subject

class ECObserverSubject
{
public:
    ECObserverSubject() {}
    virtual ~ECObserverSubject() {}
    void Attach( ECObserver *pObs )
    {
        std::cout << "Adding an observer.\n";
        listObservers.push_back(pObs);
        cout << "New observer size: " << listObservers.size() << endl;
    }
    void Detach( ECObserver *pObs )
    {
        //listObservers.erase(std::remove(listObservers.begin(), listObservers.end(), pObs), listObservers.end());
    }
    void Notify(ECGVEventType event)
    {
        //std::cout << "Notify: number of observer: " << listObservers.size() << std::endl;
        for(unsigned int i=0; i<listObservers.size(); ++i)
        {
            //cout << "Update" << endl;
            listObservers[i]->Update(event);
        }
    }

    int getObsSize() { return listObservers.size(); }
    
private:
    std::vector<ECObserver *> listObservers;
};


#endif
