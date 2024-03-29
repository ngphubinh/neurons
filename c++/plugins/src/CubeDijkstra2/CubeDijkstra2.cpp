#include <gtk/gtk.h>
#include <gmodule.h>
#include <stdio.h>
#include <Object.h>
#include "Cube_P.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "CubeDijkstra.h"
#include "Point3D.h"
#include "Cloud.h"
#include <pthread.h>

extern "C"
{

  G_MODULE_IMPORT void get_world_coordinates(double &wx, double &wy, double &wz,
                                             int x, int y);

  Cube_P* localCube;
  // Enumeration for the modes
  enum actions_cd    {CD_SELECTINITPOINT, CD_SELECTENDPOINT, CD_NONE};
  actions_cd         action;
  Point3D*           initPoint;
  Point3D*           endPoint;
  CubeDijkstra*      cubeDijkstra;
  Cloud<Point3D>*    shortestPath;
  Cloud<Point3D>*    boundary;
  pthread_t          thread;
  pthread_mutex_t    mutexBoundary;
  bool               drawBoundary;

  // Function to be done in the thread
  static void *thread_func(void *vptr_args)
  {
    vector<int > iInit(3); //indexes initial point
    localCube->micrometersToIndexes(initPoint->coords, iInit);
    printf("  computing the shortest path from %i %i %i\n",
           iInit[0], iInit[1], iInit[2]);
    cubeDijkstra->initializeCubePrevious(iInit[0], iInit[1], iInit[2]);
  }

  G_MODULE_EXPORT const bool plugin_init()
  {
    printf("init cubeDijkstra\n");
    initPoint = NULL;
    endPoint  = NULL;
    cubeDijkstra == NULL;
    action = CD_NONE;
    return true;
  }

  G_MODULE_EXPORT const bool plugin_run(vector<Object*>& objects)
  {
    action = CD_NONE;
    // initPoint = new Point3D(-15.558399, 16.319525, -18.224996);
    // endPoint  = new Point3D(-15.308799,  7.543466, -17.414998);
    initPoint = NULL;
    endPoint  = NULL;
    drawBoundary  = true;
    boundary  = new Cloud<Point3D>();
    Cube<float, double>* cubeAguet = new Cube<float, double>
      ("/media/neurons/cut/aguet_4.00_4.00.nfo");
    Cube<float, double>* cubeAguetTheta = new Cube<float, double>
      ("/media/neurons/cut/aguet_4.00_4.00_theta.nfo");
    Cube<float, double>* cubeAguetPhi = new Cube<float, double>
      ("/media/neurons/cut/aguet_4.00_4.00_phi.nfo");
    // DistanceDijkstraColorAngle* djkc = new DistanceDijkstraColorAngle
      // (cubeAguet, cubeAguetTheta, cubeAguetPhi);
    DistanceDijkstraColorInverse* djkc = new DistanceDijkstraColorInverse
      (cubeAguet);


    printf("Plugin: run\n");
    for(vector<Object*>::iterator itObject = objects.begin();
        itObject != objects.end(); itObject++)
      {
        string objType = (*itObject)->className();
        printf("Object class = %s\n", objType.c_str());
        if((*itObject)->className()=="Cube")
          {
            localCube = dynamic_cast<Cube_P*>((*itObject));
            printf("There is a Cube in here\n");
            // DistanceDijkstraColor* djkc = new DistanceDijkstraColor
              // (localCube);
            cubeDijkstra = new CubeDijkstra(localCube, djkc);
            // printf("Cube : %d\n",cube->cubeWidth);
          }
      }
    return true;
  }

  G_MODULE_EXPORT const bool plugin_key_press_event
  (GtkWidget *widget, GdkEventKey* event, gpointer user_data)
  {
    if(event->keyval == 'i'){
        printf("The action is CD_SELECTINITPOINT\n");
        action = CD_SELECTINITPOINT;
    }
    if(event->keyval == 'o'){
      printf("The action is CD_SELECTENDPOINT\n");
        action = CD_SELECTENDPOINT;
    }
    if(event->keyval == 'p'){
      printf("The action is CD_CALCULATE\n");
      if ((initPoint != NULL) && (endPoint!=NULL)){
        printf("  computing the path... patience\n");
        if (pthread_create(&thread, NULL, thread_func, NULL) != 0)
          {
            return false;
          }
      }
      action = CD_NONE;
    }
  }


  G_MODULE_EXPORT const bool plugin_unproject_mouse
  (int x, int y)
  {
    double wx, wy, wz;
    get_world_coordinates(wx, wy, wz, x, y);
    printf("The world coordinates are %f %f %f\n", wx, wy, wz);
    printf("Plugin: The position of the mouse is %i %i\n", x, y);
    switch(action){
    case CD_NONE:
      printf("UnprojectMouse: The action is CD_NONE\n");
      break;
    case CD_SELECTINITPOINT:
      printf("UnprojectMouse: The action is CD_SELECTINITPOINT\n");
      initPoint = new Point3D(wx, wy, wz);
      action = CD_SELECTENDPOINT;
      break;
    case CD_SELECTENDPOINT:
      printf("UnprojectMouse: The action is CD_SELECTENDPOINT\n");
      endPoint = new Point3D(wx, wy, wz);
      action = CD_NONE;
      break;
    }
  }

  G_MODULE_EXPORT const bool plugin_expose
  (GtkWidget *widget, GdkEventKey* event, gpointer user_data)
  {
    // printf("I am being exposed\n");
    // printf("init: %i, end %i, boundary: %i\n", initPoint, endPoint, boundary);
    // glDisable(GL_DEPTH_TEST);
    if(initPoint != NULL){
      glColor3f(0.0,1.0,0.0);
      initPoint->draw();
    }
    if(endPoint != NULL){
      glColor3f(1.0,0.0,0.0);
      endPoint->draw();
    }
    if(shortestPath!=NULL){
      shortestPath->v_r = 1.0;
      shortestPath->v_g = 1.0;
      shortestPath->v_radius = 0.05;
      shortestPath->draw();
    }
    if(drawBoundary && (boundary!=NULL) && (!cubeDijkstra->pathFound)){
        pthread_mutex_lock(&mutexBoundary);
        boundary->draw();
        pthread_mutex_unlock(&mutexBoundary);
    }
  }

  G_MODULE_EXPORT const bool plugin_quit()
  {
    printf("Plugin: Exit\n");
    return true;
  }



}
