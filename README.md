# coursework-allegro-gui

A GUI using Allegro that allows the user to draw, edit, and delete rectangles.

# Documentation

https://docs.google.com/document/d/1rCdaJJmonbQqhgqg3J_WZfGpaOnMTAfE4d2dHvO9X7I/edit?usp=sharing

# Design Patterns
## model-view-controller
The model is ECObserverSubject, which holds all the data for the view, like where the clicks occur in the window, what objects are currently displayed to the window, etc.
The view is ECGraphicViewImp which actually show the data from the model.
The controller is made up of the ECObserver's.
ECObserverSubject as the model updates the data for the view to display.
ECGraphicViewImp as the view sees the user through the events they trigger with keystrokes or mouse movement. 
The ECObserver's as the controllers manipulate the model, removing and adding the data to properly represent what should be in view.

## observer pattern
The individual ECObserver's are "subscribed" to the ECObserverSubject. As ECGraphicViewImp is a subclass of the ECObserverSubject, they are essentially subscribed to both, but there is only one instance, pointed to separately as the GraphicView and the ObserverSubject. So, when the GraphicView notifies its subscribers, the ECObservers are notified.

## command pattern
The ECObserver's each act when a specific event is triggered. In dealing with the event, they execute commands to the ECObserverSubject, which in turn will sometimes execute commands to the ECGraphicViewImp. 

