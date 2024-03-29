
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



// TODO : where should we put this declaration ?
struct aPoint3ds
{
  int x;
  int y;
  int z;
}  aPoint3d;

void draw_last_point()
{
  if(last_point != NULL){
    vector< float > microm(3);
    neuronita->neuronToMicrometers(last_point->coords,microm);
    glPushMatrix();
    glTranslatef(microm[0],
                 microm[1],
                 microm[2]);
    glColor4f(0.0,1.0,0.0,0.5);
    glutWireSphere(last_point->coords[3], 10, 10);
    glPopMatrix();
  }
}

void setUpVolumeMatrices()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy3D, aspect3D, zNear3D, zFar3D);
  //glScalef(1.0,1.0,1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(disp3DX,disp3DY,-disp3DZ);
  glRotatef(rot3DX,1,0,0);
  glRotatef(rot3DY,0,1,0);

  if(!flag_drawing_combo)
    glViewport ((GLsizei)0,(GLsizei)0,
                (GLsizei)widgetWidth, (GLsizei)widgetHeight);
}


void setUpMatricesXY(int layerSpan)
{
  //Aurelien part of the code to deal with images
  if(cube->dummy)
    {
      for(vector< VisibleE* >::iterator itObj = toDraw.begin();
          itObj != toDraw.end(); itObj++)
        {
          if((*itObj)->className()=="Image")
            {
              Image<float>* img = (Image<float>*)*itObj;

              //Gets the cube coordinates
              GLfloat widthStep = float(img->width/2);
              GLfloat heightStep = float(img->height/2);
              GLfloat depthStep = -1.0f;
              //GLint max_texture_size = 0;
              //glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
              //max_texture_size = D_MAX_TEXTURE_SIZE;
              glMatrixMode(GL_PROJECTION);
              glLoadIdentity();

              glOrtho (-widthStep, widthStep, -heightStep, heightStep, depthStep, 100.0f);
              //glScalef(1.0,1.0,-1.0);
              glMatrixMode(GL_MODELVIEW);
              glLoadIdentity();
              //glTranslatef(0,0,depthStep);

              if(!flag_drawing_combo)
                glViewport ((GLsizei)0,(GLsizei)0,
                (GLsizei)widgetWidth, (GLsizei)widgetHeight);

              break;
            }
        }
    }
  //In case there is a real cube in the toDraw objects
  else
    {
      //Gets the cube coordinates
      GLfloat widthStep = float(cube->cubeWidth)*cube->voxelWidth/2;
      GLfloat heightStep = float(cube->cubeHeight)*cube->voxelHeight/2;
      GLfloat depthStep = float(cube->cubeDepth)*cube->voxelDepth/2;
      GLint max_texture_size = 0;
      glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
      max_texture_size = D_MAX_TEXTURE_SIZE;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      float step_x = 1.0;
      float step_y = 1.0;

      if(cube->cubeWidth < max_texture_size){
        step_x = float(cube->cubeWidth)/max_texture_size;
      }
      if(cube->cubeHeight < max_texture_size){
        step_y = float(cube->cubeHeight)/max_texture_size;
      }

      glOrtho(-widthStep + cubeColToDraw*max_texture_size*cube->voxelWidth,
              -widthStep + (cubeColToDraw+step_x)*max_texture_size*cube->voxelWidth,
              heightStep - (cubeRowToDraw+step_y)*max_texture_size*cube->voxelHeight,
              heightStep - cubeRowToDraw*max_texture_size*cube->voxelHeight,
              (layerToDrawXY-layerSpan)*cube->voxelDepth,
              (layerToDrawXY+layerSpan)*cube->voxelDepth);

      glScalef(1.0,1.0,-1.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(0,0,depthStep);

      //float tileWidth = min(float(cube->cubeWidth - max_texture_size*cubeColToDraw), float(max_texture_size));
      //float tileHeight = min(float(cube->cubeHeight - max_texture_size*cubeRowTDraw), float(max_texture_size));

      if(!flag_drawing_combo)
        glViewport ((GLsizei)0,(GLsizei)0,
                    (GLsizei)widgetWidth, (GLsizei)widgetHeight);
    }
}

void setUpMatricesMIP()
{
  //Aurelien part of the code to deal with images
  if(cube->dummy)
    {
      for(vector< VisibleE* >::iterator itObj = toDraw.begin();
          itObj != toDraw.end(); itObj++)
        {
          if((*itObj)->className()=="Image")
            {
              Image<float>* img = (Image<float>*)*itObj;

              //Gets the cube coordinates
              GLfloat widthStep = float(img->width/2);
              GLfloat heightStep = float(img->height/2);
              GLfloat depthStep = -1.0f;
              //GLint max_texture_size = 0;
              //glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
              //max_texture_size = D_MAX_TEXTURE_SIZE;
              glMatrixMode(GL_PROJECTION);
              glLoadIdentity();

              glOrtho (-widthStep, widthStep, -heightStep, heightStep, depthStep, 100.0f);
              //glScalef(1.0,1.0,-1.0);
              glMatrixMode(GL_MODELVIEW);
              glLoadIdentity();
              //glTranslatef(0,0,depthStep);

              if(!flag_drawing_combo)
                glViewport ((GLsizei)0,(GLsizei)0,
                (GLsizei)widgetWidth, (GLsizei)widgetHeight);

              break;
            }
        }
    }
  //In case there is a real cube in the toDraw objects
  else
    {
      //Gets the cube coordinates
      GLfloat widthStep = float(cube->cubeWidth)*cube->voxelWidth/2;
      GLfloat heightStep = float(cube->cubeHeight)*cube->voxelHeight/2;
      GLfloat depthStep = float(cube->cubeDepth)*cube->voxelDepth/2;
      GLint max_texture_size = 0;
      glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
      max_texture_size = D_MAX_TEXTURE_SIZE;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      float step_x = 1.0;
      float step_y = 1.0;

      if(cube->cubeWidth < max_texture_size){
        step_x = float(cube->cubeWidth)/max_texture_size;
      }
      if(cube->cubeHeight < max_texture_size){
        step_y = float(cube->cubeHeight)/max_texture_size;
      }

      glOrtho(-widthStep + cubeColToDraw*max_texture_size*cube->voxelWidth,
              -widthStep + (cubeColToDraw+step_x)*max_texture_size*cube->voxelWidth,
              heightStep - (cubeRowToDraw+step_y)*max_texture_size*cube->voxelHeight,
              heightStep - cubeRowToDraw*max_texture_size*cube->voxelHeight,
              0,
              1000000000);

      glScalef(1.0,1.0,-1.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(0,0,depthStep);

      //float tileWidth = min(float(cube->cubeWidth - max_texture_size*cubeColToDraw), float(max_texture_size));
      //float tileHeight = min(float(cube->cubeHeight - max_texture_size*cubeRowTDraw), float(max_texture_size));

      if(!flag_drawing_combo)
        glViewport ((GLsizei)0,(GLsizei)0,
                    (GLsizei)widgetWidth, (GLsizei)widgetHeight);
    }
}



void setUpMatricesYZ(int layerSpan)
{
  //Gets the cube coordinates
  GLfloat widthStep = float(cube->cubeWidth)*cube->voxelWidth/2;
  GLfloat heightStep = float(cube->cubeHeight)*cube->voxelHeight/2;
  GLfloat depthStep = float(cube->cubeDepth)*cube->voxelDepth/2;
  GLint max_texture_size = 0;
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
  max_texture_size = D_MAX_TEXTURE_SIZE;
  int size_texture = D_MAX_TEXTURE_SIZE;
  int max_texture  = D_MAX_TEXTURE_SIZE;

  GLfloat increment_height =
    min(float(cube->cubeHeight - cube->nRowToDraw*size_texture), float(max_texture));
  float x_max = min(size_texture, (int)cube->cubeWidth - cube->nColToDraw*size_texture);


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //FIXMEEEEEE
  glOrtho(-depthStep,
          +depthStep,
          heightStep - cubeRowToDraw*max_texture_size*cube->voxelHeight -
          increment_height*cube->voxelHeight,
          heightStep - cubeRowToDraw*max_texture_size*cube->voxelHeight,
          -widthStep + cubeColToDraw*max_texture_size*cube->voxelWidth+
          (layerToDrawYZ - layerSpan)*cube->voxelWidth,
          -widthStep + cubeColToDraw*max_texture_size*cube->voxelWidth +
          (layerToDrawYZ + layerSpan)*cube->voxelWidth
          );
  glScalef(1.0,1.0,-1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef(-90,0,1,0);
/*   glTranslatef(widthStep,0,0); */
  glColor3f(1.0,1.0,1.0);

  if(!flag_drawing_combo)
    glViewport ((GLsizei)0,(GLsizei)0,
                (GLsizei)widgetWidth, (GLsizei)widgetHeight);

}

void setUpMatricesXZ(int layerSpan)
{
  //Gets the cube coordinates
  GLfloat widthStep = float(cube->cubeWidth)*cube->voxelWidth/2;
  GLfloat heightStep = float(cube->cubeHeight)*cube->voxelHeight/2;
  GLfloat depthStep = float(cube->cubeDepth)*cube->voxelDepth/2;
  GLint max_texture_size = 0;
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
  max_texture_size = D_MAX_TEXTURE_SIZE;
  int size_texture = D_MAX_TEXTURE_SIZE;
  int max_texture  = D_MAX_TEXTURE_SIZE;
  glMatrixMode(GL_PROJECTION);
  GLfloat increment_width =
    min(float(cube->cubeWidth - cube->nColToDraw*size_texture), float(max_texture));
  float y_max = min(size_texture, (int)cube->cubeHeight - cube->nRowToDraw*size_texture);

  glLoadIdentity();
  glOrtho(-widthStep + cubeColToDraw*max_texture_size*cube->voxelWidth,
          -widthStep + cubeColToDraw*max_texture_size*cube->voxelWidth+
          increment_width*cube->voxelWidth,
          +depthStep,
          -depthStep,
          heightStep
          - cubeRowToDraw*max_texture_size*cube->voxelHeight
          - (layerToDrawXZ-layerSpan)*cube->voxelHeight,
          heightStep
          - cubeRowToDraw*max_texture_size*cube->voxelHeight
          -(layerToDrawXZ+layerSpan)*cube->voxelHeight
          );
  glScalef(1.0,1.0,1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef(-90,1,0,0);
/*   glTranslatef(0,-heightStep,0); */
  glColor3f(1.0,1.0,1.0);

  if(!flag_drawing_combo)
    glViewport ((GLsizei)0,(GLsizei)0,
                (GLsizei)widgetWidth, (GLsizei)widgetHeight);

}

// TODO : Create a call list to speed display
void draw_selection()
{
  if(majorMode & MOD_SELECT_EDITOR)
    {
      float radius = 0.1f;
      GtkSpinButton* brush_size=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(selectionEditor),"brush_size"));
      if(brush_size)
        radius = gtk_spin_button_get_value(brush_size);

      int x = -1;
      int y = -1;
      int z = -1;
      //float wx,wy,wz;

      if(mod_display == MOD_DISPLAY_XY)
        {
          GtkSpinButton* layer_XY_spin=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(ascEditor),"layer_XY_spin"));
          z = gtk_spin_button_get_value_as_int(layer_XY_spin);
        }
      else
        if(mod_display == MOD_DISPLAY_XZ)
          {
            GtkSpinButton* layer_XZ_spin=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(ascEditor),"layer_XZ_spin"));
            y = gtk_spin_button_get_value_as_int(layer_XZ_spin);
          }
        else
          if(mod_display == MOD_DISPLAY_YZ)
            {
              GtkSpinButton* layer_YZ_spin=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(ascEditor),"layer_YZ_spin"));
              x = gtk_spin_button_get_value_as_int(layer_YZ_spin);
            }

      //printf("Radius : %f\n", radius);
      //radius = 0.1f;
      for(vector< DoubleSet<float>* >::iterator itSel = lSelections.begin();
          itSel != lSelections.end(); itSel++)
        {
          (*itSel)->draw(x,y,z,radius);
        }
    }
}

void draw_objects()
{
  if(flag_draw_neuron && neuronita)
    neuronita->draw();

  for(vector< VisibleE* >::iterator itObj = toDraw.begin();
      itObj != toDraw.end(); itObj++)
    {
      if((*itObj)->className()=="Image")
        {
          Image<float>* img = (Image<float>*)*itObj;
          glPushMatrix();
          glTranslatef(-img->width/2,-img->height/2,0);
          img->draw();
          glPopMatrix();
        }
      else if( ((*itObj)->className()=="Cube") ||
               ((*itObj)->className()=="Cube_T") ||
               ((*itObj)->className()=="Cube_C") ||
               ((*itObj)->className()=="Cube_P")
               ){
        //If we are not supposed to draw the cube, ignore it
        /* if(!drawCube_flag) continue; */
        Cube_P* cubeDraw = dynamic_cast<Cube_P*>(*itObj);
        if((mod_display == MOD_DISPLAY_3D) ||
           (mod_display == MOD_DISPLAY_COMBO) || 
           (mod_display == MOD_DISPLAY_MIP)){
          cubeDraw->draw();
          /* std::cout << "I should be drawing a " << (*itObj)->className() << std::endl; */
          setUpVolumeMatrices();
        }
        else if (mod_display == MOD_DISPLAY_XY)
          cubeDraw->draw_layer_tile_XY(layerToDrawXY);
        else if (mod_display == MOD_DISPLAY_XZ)
          cubeDraw->draw_layer_tile_XZ(layerToDrawXZ);
        else if (mod_display == MOD_DISPLAY_YZ)
          cubeDraw->draw_layer_tile_YZ(layerToDrawYZ);
      }
      else{
        //If we do not want to draw only the cube
        if(!drawOnlyCube_flag) (*itObj)->draw();
      }
    }

  draw_last_point();
  if(display_selection)
    {
      draw_selection();
    }
}

gboolean
on_drawing3D_expose_event              (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  //Gets the 3D drawing context
  GdkGLContext  *glcontext  = gtk_widget_get_gl_context  (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  //If we can not draw, return
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

  if(flag_minMax==0)
    glClearColor(1.0,1.0,1.0,1.0);
  else
    glClearColor(0.0,0.0,0.0,0.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(mod_display == MOD_DISPLAY_3D)
    {
      setUpVolumeMatrices();
      /* cube->draw(); */
      draw_objects();

      setUpVolumeMatrices();
      if(flag_cube_transparency)
        glDisable(GL_DEPTH_TEST);
      else
      glEnable(GL_DEPTH_TEST);
      if(flag_draw_neuron && neuronita)
        neuronita->draw();

      draw_last_point();
      if(display_selection)
        {
          draw_selection();
        }
    }

  //Draws the XY view
  if(mod_display == MOD_DISPLAY_XY)
    {
      glEnable(GL_DEPTH_TEST);
      setUpMatricesXY(layerSpanViewZ);
      if(flag_cube_transparency)
        glDisable(GL_DEPTH_TEST);
      else
        glEnable(GL_DEPTH_TEST);
      draw_objects();
      glDisable(GL_DEPTH_TEST);
    }

  if(mod_display == MOD_DISPLAY_XZ)
    {
      glEnable(GL_DEPTH_TEST);
      setUpMatricesXZ(layerSpanViewZ);
      if(flag_cube_transparency)
        glDisable(GL_DEPTH_TEST);
      else
        glEnable(GL_DEPTH_TEST);
      draw_objects();
      glDisable(GL_DEPTH_TEST);
    }

  if(mod_display == MOD_DISPLAY_YZ)
    {
      glEnable(GL_DEPTH_TEST);
      setUpMatricesYZ(layerSpanViewZ);
      if(flag_cube_transparency)
        glDisable(GL_DEPTH_TEST);
      else
        glEnable(GL_DEPTH_TEST);
      draw_objects();
      glDisable(GL_DEPTH_TEST);
    }

  if(mod_display == MOD_DISPLAY_DUAL){
    if( toDraw.size() == 3)
      {
        // The first objects in the toDraw list are the 2 images passed in arguments.
        // The last one is the list is the dummy cube created at initilization
        setUpVolumeMatrices();
        glViewport ((GLsizei)0,(GLsizei)0,
                    (GLsizei)(widgetWidth/2), (GLsizei)widgetHeight);

        if(toDraw[0]->className()=="Image")
          {
            Image<float>* img = (Image<float>*)toDraw[0];
            glPushMatrix();
            glTranslatef(-img->width/2,-img->height/2,0);
            img->draw();
            glPopMatrix();
          }
        else
          toDraw[0]->draw();

        glViewport ((GLsizei)widgetWidth/2,(GLsizei)0,
                    (GLsizei)(widgetWidth/2), (GLsizei)widgetHeight);

        if(toDraw[1]->className()=="Image")
          {
            Image<float>* img = (Image<float>*)toDraw[1];
            glPushMatrix();
            glTranslatef(-img->width/2,-img->height/2,0);
            img->draw();
            glPopMatrix();
          }
        else
          toDraw[1]->draw();
      }
  }

  if(mod_display == MOD_DISPLAY_COMBO){
    flag_drawing_combo = true;
    setUpVolumeMatrices();
    glViewport ((GLsizei)0,(GLsizei)0,
                (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    if(drawCube_flag){
      //to make the method draw_objects draw the cube
      /* flag_draw_3D = true; */
      draw_objects();
      /* flag_draw_3D = false; */
      glEnable(GL_BLEND);
      setUpVolumeMatrices();
      cube->draw_layer_tile_XY(layerToDrawXY,1);
      setUpVolumeMatrices();
      cube->draw_layer_tile_XZ(layerToDrawXZ,1);
      setUpVolumeMatrices();
      cube->draw_layer_tile_YZ(layerToDrawYZ,1);
      glPushMatrix();
      glTranslatef(wx,wy,wz);
      glColor3f(0.0,1.0,1.0);
      glutSolidSphere(3, 10,10);
      glPopMatrix();
      glDisable(GL_BLEND);
    }
    draw_last_point();

    //XZ part
    glEnable(GL_DEPTH_TEST);
    setUpMatricesXZ(100000);
    glViewport ((GLsizei)0,(GLsizei)widgetHeight/2,
                (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    cube->draw_layer_tile_XY(layerToDrawXY,1);
    cube->draw_layer_tile_XZ(layerToDrawXZ,0);
    cube->draw_layer_tile_YZ(layerToDrawYZ,1);
    setUpMatricesXZ(layerSpanViewZ);
    draw_objects();
    glDisable(GL_DEPTH_TEST);
    draw_last_point();
    glDisable(GL_DEPTH_TEST);

    // YZ part
    glEnable(GL_DEPTH_TEST);
    glViewport ((GLsizei)widgetWidth/2, (GLsizei)0,
                (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    setUpMatricesYZ(1000000);
    cube->draw_layer_tile_XY(layerToDrawXY,1);
    cube->draw_layer_tile_XZ(layerToDrawXZ,1);
    cube->draw_layer_tile_YZ(layerToDrawYZ,0);
    setUpMatricesYZ(layerSpanViewZ);
    draw_objects();
    glDisable(GL_DEPTH_TEST);
    draw_last_point();
    glDisable(GL_DEPTH_TEST);

    //XY part
    glEnable(GL_DEPTH_TEST);
    glViewport ((GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2,
                (GLsizei)widgetWidth/2, (GLsizei)widgetHeight/2);
    setUpMatricesXY(100000000);
    cube->draw_layer_tile_XY(layerToDrawXY,0);
    cube->draw_layer_tile_XZ(layerToDrawXZ,1);
    cube->draw_layer_tile_YZ(layerToDrawYZ,1);
    setUpMatricesXY(layerSpanViewZ);
    draw_objects();
    glDisable(GL_DEPTH_TEST);
    draw_last_point();
    glDisable(GL_DEPTH_TEST);

    flag_drawing_combo = false;
  }//draw_combo


  if(majorMode & MOD_ASCEDITOR){
    exposeAsc(widget, event, user_data);
  }

  if(p_expose != NULL){
    aPoint3d.x = -1;
    aPoint3d.y = -1;
    aPoint3d.z = -1;

    if(mod_display == MOD_DISPLAY_XY)
      {
        GtkSpinButton* layer_XY_spin=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(ascEditor),"layer_XY_spin"));
        aPoint3d.z = gtk_spin_button_get_value_as_int(layer_XY_spin);
      }
    else
      if(mod_display == MOD_DISPLAY_XZ)
        {
          GtkSpinButton* layer_XZ_spin=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(ascEditor),"layer_XZ_spin"));
          aPoint3d.y = gtk_spin_button_get_value_as_int(layer_XZ_spin);
        }
      else
        if(mod_display == MOD_DISPLAY_YZ)
          {
            GtkSpinButton* layer_YZ_spin=GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(ascEditor),"layer_YZ_spin"));
            aPoint3d.x = gtk_spin_button_get_value_as_int(layer_YZ_spin);
          }

    /* printf("p_expose %d %d %d\n",aPoint3d.x,aPoint3d.y,aPoint3d.z); */
    p_expose(widget, event, &aPoint3d);
  }


  //Draws the XY view
  if(mod_display == MOD_DISPLAY_MIP)
    {
      setUpMatricesMIP();
      if(flag_cube_transparency)
        glDisable(GL_DEPTH_TEST);
      else
        glEnable(GL_DEPTH_TEST);
      draw_objects();
      glDisable(GL_DEPTH_TEST);
    }



  //Show what has been drawn
  if (gdk_gl_drawable_is_double_buffered (gldrawable))
    gdk_gl_drawable_swap_buffers (gldrawable);
  else
    glFlush ();
  gdk_gl_drawable_gl_end (gldrawable);


  return TRUE;
}
