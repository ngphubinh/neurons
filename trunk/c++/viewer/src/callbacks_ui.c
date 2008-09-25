#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <fstream>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "Neuron.h"
#include "globalsE.h"
#include "CubeFactory.h"
#include "CloudFactory.h"
#include "GraphFactory.h"
#include "utils.h"
#include "functions.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <fstream>

void get_world_coordinates(double &wx, double &wy, double &wz, int x, int y)
{
  GLint realy; /*  OpenGL y coordinate position, not the Mouse one of Gdk */
               /*   realy = widgetHeight - mouse_last_y; */
  realy =(GLint) widgetHeight - 1 - y;
  int window_x = x;
  int window_y = realy;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLdouble nx,ny,nz;

  if(flag_draw_XY)
    setUpMatricesXY(layerSpanViewZ);
  if(flag_draw_XZ)
    setUpMatricesXZ(layerSpanViewZ);
  if(flag_draw_YZ)
    setUpMatricesYZ(layerSpanViewZ);
  if(flag_draw_combo){
    //If the click is on the XY corner
    if( (window_x > widgetWidth/2) && (window_y > widgetHeight/2) ){
      setUpMatricesXY(layerSpanViewZ);
      glViewport ((GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2,
                  (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    }
    // In the YZ corner
    if( (window_x > widgetWidth/2) && (window_y < widgetHeight/2) ){
      setUpMatricesYZ(layerSpanViewZ);
      glViewport ((GLsizei)widgetWidth/2, (GLsizei)0,
                  (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    }
    //In the XZ corner
    if( (window_x < widgetWidth/2) && (window_y > widgetHeight/2) ){
      setUpMatricesXZ(layerSpanViewZ);
      glViewport ((GLsizei)0,(GLsizei)widgetHeight/2,
                  (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    }
    //In the 3D view, it makes no sense
    if( (window_x < widgetWidth/2) && (window_y < widgetHeight/2) ){
      return;
    }
  }

  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
  GLfloat depth;
  glReadPixels( x,
                realy,
                1,
                1,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                &depth );


  gluUnProject ((GLdouble) mouse_last_x, (GLdouble) realy, depth,
                mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
}

void get_world_coordinates(double &wx, double &wy, double &wz, bool change_layers )
{
  GLint realy; /*  OpenGL y coordinate position, not the Mouse one of Gdk */
               /*   realy = widgetHeight - mouse_last_y; */
  realy =(GLint) widgetHeight - 1 - mouse_last_y;
  int window_x = mouse_last_x;
  int window_y = realy;
/*   printf("WidgetSize =          [%f, %f]\n", widgetWidth, widgetHeight); */
/*   printf("Window coordinates:   [%i, %i]\n", mouse_last_x ,mouse_last_y); */
/*   printf("ViewPort coordinates: [%i, %i]\n", mouse_last_x ,realy); */

  //There is no information on the depth (as it is only the first layer)
  /* if(flag_draw_3D) */
    /* return; */

  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLdouble nx,ny,nz;

  if(flag_draw_XY)
    setUpMatricesXY(layerSpanViewZ);
  if(flag_draw_XZ)
    setUpMatricesXZ(layerSpanViewZ);
  if(flag_draw_YZ)
    setUpMatricesYZ(layerSpanViewZ);
  if(flag_draw_combo){
    //If the click is on the XY corner
    if( (window_x > widgetWidth/2) && (window_y > widgetHeight/2) ){
      setUpMatricesXY(layerSpanViewZ);
      glViewport ((GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2,
                  (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    }
    // In the YZ corner
    if( (window_x > widgetWidth/2) && (window_y < widgetHeight/2) ){
      setUpMatricesYZ(layerSpanViewZ);
      glViewport ((GLsizei)widgetWidth/2, (GLsizei)0,
                  (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    }
    //In the XZ corner
    if( (window_x < widgetWidth/2) && (window_y > widgetHeight/2) ){
      setUpMatricesXZ(layerSpanViewZ);
      glViewport ((GLsizei)0,(GLsizei)widgetHeight/2,
                  (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    }
    //In the 3D view, it makes no sense
    if( (window_x < widgetWidth/2) && (window_y < widgetHeight/2) ){
      return;
    }
  }

  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
  GLfloat depth;
  glReadPixels( mouse_last_x,
                realy,
                1,
                1,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                &depth );


  gluUnProject ((GLdouble) mouse_last_x, (GLdouble) realy, depth,
                mvmatrix, projmatrix, viewport, &wx, &wy, &wz);

  vector< int > indexes(3);
  vector< float > world(3);
  world[0] = wx;
  world[1] = wy;
  world[2] = wz;
  cube->micrometersToIndexes(world, indexes);

  if(flag_draw_combo && change_layers){
    //If the click is on the XY corner
    if( (window_x > widgetWidth/2) && (window_y > widgetHeight/2) ){
      layerToDrawXZ = indexes[1]%512;
      layerToDrawYZ = indexes[0]%512;
    }
    // In the YZ corner
    if( (window_x > widgetWidth/2) && (window_y < widgetHeight/2) ){
      layerToDrawXY = indexes[2]%512;
      layerToDrawXZ = indexes[1]%512;
    }
    //In the XZ corner
    if( (window_x < widgetWidth/2) && (window_y > widgetHeight/2) ){
      layerToDrawXY = indexes[2]%512;
      layerToDrawYZ = indexes[0]%512;
    }
    //In the 3D view, it makes no sense
    if( (window_x < widgetWidth/2) && (window_y < widgetHeight/2) ){
      return;
    }
  }
}


void unProjectMouse()
{


  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLdouble wx, wy, wz;
  GLdouble nx,ny,nz;
  GLint realy; /*  OpenGL y coordinate position, not the Mouse one of Gdk */
               /*   realy = widgetHeight - mouse_last_y; */
  realy = (GLint)widgetHeight - 1 - mouse_last_y;

  get_world_coordinates(wx, wy, wz, true);


  if(1)
    {
      printf("World Coordinates: %f %f %f\n", wx, wy, wz);
      vector< float > world(3);
      world[0] = wx;
      world[1] = wy;
      world[2] = wz;
      if(cube!=NULL){
        vector< int > indexes(3);
        cube->micrometersToIndexes(world, indexes);

/*         layerToDrawXY = indexes[2]%512; */
/*         layerToDrawXZ = indexes[1]%512; */
/*         layerToDrawYZ = indexes[0]%512; */

/*         printf("Indexes: %i %i %i\n", indexes[0], indexes[1], indexes[2]); */
        on_drawing3D_expose_event(drawing3D,NULL, NULL);
      }
    }

  /** If the mode is MOD_ASCEDITOR, change the asc.*/
  if(majorMode == MOD_ASCEDITOR){
    unProjectMouseAsc(mouse_last_x, mouse_last_y);
  }

}

gboolean
on_drawing3D_motion_notify_event       (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
  int x, y;
  GdkModifierType state;
  gdk_window_get_pointer (event->window, &x, &y, &state);
  mouse_current_x = x;
  mouse_current_y = y;
  int diffx=x-mouse_last_x;
  int diffy=y-mouse_last_y;
  if((abs(diffx) + abs(diffy)) < 2)
    return true;
  mouse_last_x=x;
  mouse_last_y=y;
  if( mouse_buttons[2] )
  {
    disp3DZ -= (float) 0.20f * diffx;
    on_drawing3D_expose_event(widget, NULL, user_data);
  }
  else
    if( mouse_buttons[0] )
      {
        rot3DX -= (float) 0.5f * diffy;
        rot3DY -= (float) 0.5f * diffx;
        on_drawing3D_expose_event(widget, NULL, user_data);
      }
    else
      if( mouse_buttons[1] )
        {
          disp3DX += (float) 0.5f * diffx;
          disp3DY -= (float) 0.5f * diffy;
          on_drawing3D_expose_event(widget, NULL, user_data);
        }

  if( flag_draw_combo &&
      !((mouse_last_x < widgetWidth/2) && (mouse_last_y > widgetHeight/2))
      ){
    get_world_coordinates(wx, wy, wz, false); //Nasty global variables
/*     printf("%f %f %f\n", wx, wy, wz); */
  }

  if(MOD_ASCEDITOR)
    on_drawing3D_expose_event(drawing3D,NULL, NULL);
  return FALSE;
}


gboolean
on_drawing3D_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  mouse_last_x= (int)event->x;
  mouse_last_y= (int)event->y;
  switch(event->button)
    {
    case 1:
      mouse_buttons[0] = (mouse_buttons[0]==0)?1:0;
      unProjectMouse();
      break;
    case 2:
      mouse_buttons[1] = (mouse_buttons[1]==0)?1:0;
      break;
    case 3:
      mouse_buttons[2] = (mouse_buttons[2]==0)?1:0;
      break;
    default:
      break;
     }
  return FALSE;
}


gboolean
on_drawing3D_button_release_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  mouse_last_x=(int)event->x;
  mouse_last_y=(int)event->y;
  switch(event->button)
  {
  case 1:
    mouse_buttons[0] = 0;
    break;
  case 2:
    mouse_buttons[1] = 0;
    break;
  case 3:
    mouse_buttons[2] = 0;
    break;
  default:
    break;
  }
  return FALSE;
}


gboolean
on_drawing3D_key_press_event           (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
  if(event->keyval == 'a')
    {
      layerToDrawXY++;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }
  if(event->keyval == 's')
    {
      layerToDrawXY--;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }

  if(event->keyval == 'u')
    {
      layerToDrawXZ++;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }
  if(event->keyval == 'j')
    {
      layerToDrawXZ--;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }
  if(event->keyval == 'o')
    {
      layerToDrawYZ++;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }
  if(event->keyval == 'l')
    {
      layerToDrawYZ--;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }

  if(event->keyval == 'e')
    {
      flag_draw_neuron = !flag_draw_neuron;
      on_drawing3D_expose_event(drawing3D,NULL, NULL);
    }

  if(event->keyval == 'r')
    {
/*       if(flag_cube_transparency){ */
/*         printf("The neuron is not rendered with depth information, the point will not be saved\n"); */
/*         return false; */
/*       } */
/*       flag_save_neuron_coordinate = !flag_save_neuron_coordinate; */
/*       printf("The next point is saved as a neuron coordinate\n"); */
    }

  if(majorMode == MOD_ASCEDITOR)
    keyPressedAsc(widget,event,user_data);

  return FALSE;
}

gboolean
on_drawing3D_scroll_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  if(majorMode == MOD_ASCEDITOR){
    scrollAsc(widget, event, user_data);
  }

  return FALSE;
}

