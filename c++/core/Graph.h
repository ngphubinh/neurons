#ifndef GRAPH_H_
#define GRAPH_H_

#include "Cloud.h"
#include "EdgeSet.h"
#include "Graph_P.h"
#include "Cube_P.h"
#include "Cube.h"

template< class P=Point, class E=Edge<P> >
class Graph : public Graph_P
{
public:
  EdgeSet<P, E> eset;
  // Cloud<P>   cloud;

  Graph();

  Graph(string filename);

  Graph(Cloud_P* cl);

  ~Graph();

  void changeCloud(Cloud_P* cl);

  bool load(istream &in);

  void save(ostream &out);

  void draw();

  Graph<P, E >* subGraphToPoint(int nPoint);

  double distanceEuclidean(Point* p1, Point* p2);

  // Calculates the Minimum spanning tree using prim algorithm
  void prim();

  // Calculates the MST using the edges already in the graph
  Graph<Point3D, EdgeW<Point3D> >* primFromThisGraph();

  /** Finds the distance between to points in the graph. If there is an edge between
      those points, it returns the weight of the edge and if there is none, it will return
      infinity.
   */
  double distanceInGraph(int idx1, int idx2);



  /** Samples a lattice split of dimension nx, ny, nz arround the
      edges in the given cube, the distance in micrometers between the
      points is indicated in dy, dz. It returns the values on
      those points.  ~X is defined allong the line of the edge, Z is
      defined as the vector perpendicular to ~X in the plane ~XZ and Y
      is perpendicular to both.
   */
  vector< vector< double > > sampleLatticeArroundEdges
  (Cube_P* cube, int nx, int ny, int nz, double dy, double dz);

  /** Returns a vector with the indexes of the vertices of the graph
      that have are connected to it by only one edge. */
  vector<int> findLeaves();

} ;

// FOR THE MOMENT THE DEFINITIONS GO HERE, BUT NEED TO BE TAKEN OUT

typedef Graph<Point3D, Edge<Point3D> > g1;

template< class P, class E>
Graph<P,E>::Graph() : Graph_P(){
  v_saveVisibleAttributes = true;
  cloud = new Cloud<P>();
}

template< class P, class E>
Graph<P,E>::~Graph()
{
  delete (cloud);
  delete (&eset);
}

template< class P, class E>
Graph<P,E>::Graph(string filename) : Graph_P(){
  v_saveVisibleAttributes = true;
  cloud = new Cloud<P>();
  loadFromFile(filename);
}

template< class P, class E>
Graph<P,E>::Graph(Cloud_P* cl) : Graph_P(){
  v_saveVisibleAttributes = true;
  changeCloud(cl);
  string className = cloud->className();
  printf("Graph created with a: %s\n", className.c_str());
}

template< class P, class E>
void Graph<P,E>::changeCloud(Cloud_P* cl)
{
  this->cloud = cl;
  eset.setPointVector(&cloud->points);
}

template< class P, class E>
bool Graph<P,E>::load(istream &in){
  P* p = new P();
  E* e = new E();
  string s;
  in >> s;
  assert(s=="<Graph");
  in >> s;
  assert(s == p->className());
  in >> s;
  assert(s == (e->className()+">"));
  assert(VisibleE::load(in));
  cloud->load(in);
  // assert(cloud->load(in));
  assert(eset.load(in));
  eset.setPointVector(&cloud->points);
  delete p;
  delete e;
  return true;
}

template< class P, class E>
void Graph<P,E>::save(ostream &out){
  // printf("Saving the graph\n");
  P* p = new P();
  E* e = new E();
  out << "<Graph " << p->className() << " " << e->className() << ">\n";
  VisibleE::save(out);
  cloud->save(out);
  eset.save(out);
  out << "</Graph>\n";
  delete p;
  delete e;
}

template< class P, class E>
void Graph<P,E>::draw(){
  if(v_glList == 0){
    v_glList = glGenLists(1);
    glNewList(v_glList, GL_COMPILE);
    VisibleE::draw();
    eset.draw();
    cloud->draw();
    glEndList();
  }
  else{
    glCallList(v_glList);
  }
  // VisibleE::draw();
  // eset.draw();
  // cloud->draw();
}

template< class P, class E>
double Graph<P,E>::distanceEuclidean(Point* p1, Point* p2){
  return p1->distanceTo(p2);
}

template< class P, class E>
void Graph<P,E>::prim(){
  printf("Graph<P,E>::prim of %i points  [", cloud->points.size());
  //Erases the edges
  eset.edges.resize(0);

  //Implementation of Prim's algtrithm as described in "Algorithms in C", by Robert Sedgewick
  vector< int > parents(cloud->points.size());
  vector< int > closest_in_tree(cloud->points.size());
  vector< double > distances_to_tree(cloud->points.size());

  //Initialization
  for(int i = 0; i < cloud->points.size(); i++)
    {
      parents[i] = -1;
      closest_in_tree[i] = 0;
      distances_to_tree[i] = distanceEuclidean(cloud->points[0],cloud->points[i]);
    }
  parents[0] = 0;

  for(int i = 0; i < cloud->points.size(); i++)
    {
      //Find the point that is not in the graph but closer to the graph
      int cls_idx = 0;
      double min_distance = 2e9;
      for(int cls = 0; cls < cloud->points.size(); cls++)
        if( (parents[cls] == -1) && (distances_to_tree[cls] < min_distance) ){
            min_distance = distances_to_tree[cls];
            cls_idx = cls;}

      //Now we update the data structures for new iterations
      parents[cls_idx] = closest_in_tree[cls_idx];
      double distance_update = 0;
      for(int cls2 = 0; cls2 < cloud->points.size(); cls2++)
        {
          if (parents[cls2] == -1)
            {
              distance_update = distanceEuclidean(cloud->points[cls2], cloud->points[cls_idx]);
              if(distance_update < distances_to_tree[cls2])
                {
                  distances_to_tree[cls2] = distance_update;
                  closest_in_tree[cls2] = cls_idx;
                }
            }
        }
      // if(i%max(cloud->points.size()/100,1)==0)
        // { printf("#"); fflush(stdout);}
    }

  printf("]\n");

  for(int i = 0; i < parents.size(); i++)
    eset.addEdge(i, parents[i]);
}

template< class P, class E>
vector< vector< double > > Graph<P,E>::sampleLatticeArroundEdges
(Cube_P* cubep, int nx, int ny, int nz, double sy, double sz)
{
  Cube<float, double>* cube = dynamic_cast< Cube< float, double >* >(cubep);
  vector< vector< double > > toReturn;

  float m_p0_p1, length, length_step, dot_z_p0p1;
  vector< double > latticeEdge;
  vector< float > p0(3);
  vector< float > p1(3);
  vector< float > p0_p1(3);
  vector< float > p0_p1_n(3);
  vector< float > anchor(3);
  vector< float > dz(3);
  vector< float > dy(3);
  vector< float > micrometers(3);
  vector< int   > indexes(3);
  for(int nEdge; nEdge < eset.edges.size(); nEdge++){
    // Gets the coordinates at the points
    vector< Point* > vp = *eset.points;
    p0[0] = vp[eset.edges[nEdge]->p0]->coords[0];
    p0[1] = vp[eset.edges[nEdge]->p0]->coords[1];
    p0[2] = vp[eset.edges[nEdge]->p0]->coords[2];
    p1[0] = vp[eset.edges[nEdge]->p1]->coords[0];
    p1[1] = vp[eset.edges[nEdge]->p1]->coords[1];
    p1[2] = vp[eset.edges[nEdge]->p1]->coords[2];

    // Direction between the point
    p0_p1[0] = p1[0] - p0[0];
    p0_p1[1] = p1[1] - p0[1];
    p0_p1[2] = p1[2] - p0[2];
    m_p0_p1 = sqrt(p0_p1[0]*p0_p1[0]+
                   p0_p1[1]*p0_p1[1]+
                   p0_p1[2]*p0_p1[2]);
    p0_p1_n[0] = p0_p1[0] / m_p0_p1;
    p0_p1_n[1] = p0_p1[1] / m_p0_p1;
    p0_p1_n[2] = p0_p1[2] / m_p0_p1;

    // And the perpendicular in the ~Z direction and in the ~Y direction
    dot_z_p0p1 = p0_p1_n[2];
    dz[0] =   - dot_z_p0p1 * p0_p1_n[0];
    dz[1] =   - dot_z_p0p1 * p0_p1_n[1];
    dz[2] = 1 - dot_z_p0p1 * p0_p1_n[2];

    dy[0] =  p0_p1_n[1]*dz[2] - dz[1]*p0_p1_n[2] ;
    dy[1] =-(p0_p1_n[0]*dz[2] - dz[0]*p0_p1_n[2]);
    dy[2] =  p0_p1_n[0]*dz[1] - dz[0]*p0_p1_n[1];

    // Gets the length of the 
    length = m_p0_p1;
    length_step = length / nx;

    // Travels the lattice in the direction of p0_p1
    latticeEdge.resize(0);
    for( float step = length_step/2; step < length; step+=length_step)
      {
        //Point from who to spand the perpendicular plane
        anchor[0] = p0[0] + step*p0_p1_n[0];
        anchor[1] = p0[1] + step*p0_p1_n[1];
        anchor[2] = p0[2] + step*p0_p1_n[2];

        for(float z = sz*nz/2; z >= -sz*nz/2; nz-=sz){
          for(float y = ny*sy/2; y >= -ny*sy/2; y-=sy){
            micrometers[0] = anchor[0] + z*dz[0] + y*dy[0];
            micrometers[1] = anchor[1] + z*dz[1] + y*dy[1];
            micrometers[2] = anchor[2] + z*dz[2] + y*dy[2];
            cube->micrometersToIndexes(micrometers, indexes);
            if((indexes[0] < 0) || (indexes[1] < 0) || (indexes[2] < 0) ||
               (indexes[0] >= cube->cubeWidth)  ||
               (indexes[1] >= cube->cubeHeight) ||
               (indexes[2] >= cube->cubeDepth) )
              {
                break;
              }
            else{
              latticeEdge.push_back(cube->at(indexes[0],indexes[1],indexes[2]));
            }
          }
        }
        if(latticeEdge.size() == nx*ny*nz){
          toReturn.push_back(latticeEdge);
        }
      } //step
  } // all edges
  return toReturn;
} //method

template< class P, class E>
Graph<P, E >* Graph<P,E>::subGraphToPoint(int nPoint)
{
  // assert(nPoint < cloud->points.size());
  Graph<P,E>* toReturn = new Graph<P,E>();
  for(int i = 0; i < nPoint; i++)
    toReturn->cloud->points.push_back(cloud->points[i]);

  for(int i = 0; i < eset.edges.size(); i++){
        if( (eset.edges[i]->p0 < nPoint) &&
            (eset.edges[i]->p1 < nPoint) )
          toReturn->eset.addEdge(eset.edges[i]->p0, eset.edges[i]->p1);
     }
  return toReturn;
}


template< class P, class E>
double Graph<P,E>::distanceInGraph(int idx1, int idx2)
{
  double toReturn = DBL_MAX;
  for(int i = 0; i < eset.edges.size(); i++){
    if( ((eset.edges[i]->p0 == idx1) && (eset.edges[i]->p1 == idx2)) ||
        ((eset.edges[i]->p0 == idx2) && (eset.edges[i]->p1 == idx1)) )
      {
        //The edge should be at least EdgeW
        EdgeW<P> * edg = dynamic_cast<EdgeW<P>* >(eset.edges[i]);
        if(toReturn >= edg->w)
          toReturn = edg->w;
      }
  }
  return toReturn;
}


template< class P, class E>
Graph<Point3D, EdgeW<Point3D> >* Graph<P,E>::primFromThisGraph()
{
  printf("Graph<P,E>::primFromThisGraph of %i points  [", cloud->points.size());
  //Erases the edges

  Graph<Point3D, EdgeW<Point3D> >* toReturn = new Graph<Point3D, EdgeW<Point3D> >();

  //Copies the cloud to the new graph
  for(int i = 0; i < cloud->points.size(); i++)
    toReturn->cloud->points.push_back(cloud->points[i]);

  //Implementation of Prim's algtrithm as described in "Algorithms in C", by Robert Sedgewick
  vector< int > parents(cloud->points.size());
  vector< int > closest_in_tree(cloud->points.size());
  vector< double > distances_to_tree(cloud->points.size());

  //Initialization
  for(int i = 0; i < cloud->points.size(); i++)
    {
      parents[i] = -1;
      closest_in_tree[i] = 0;
      distances_to_tree[i] = distanceInGraph(0,i);
    }
  parents[0] = 0;

  for(int i = 0; i < cloud->points.size(); i++)
    {
      //Find the point that is not in the graph but closer to the graph
      int cls_idx = 0;
      double min_distance = DBL_MAX;
      for(int cls = 0; cls < cloud->points.size(); cls++)
        if( (parents[cls] == -1) && (distances_to_tree[cls] < min_distance) ){
            min_distance = distances_to_tree[cls];
            cls_idx = cls;}

      //Now we update the data structures for new iterations
      parents[cls_idx] = closest_in_tree[cls_idx];
      double distance_update = 0;
      for(int cls2 = 0; cls2 < cloud->points.size(); cls2++)
        {
          if (parents[cls2] == -1)
            {
              distance_update = distanceInGraph(cls2, cls_idx);
              if(distance_update < distances_to_tree[cls2])
                {
                  distances_to_tree[cls2] = distance_update;
                  closest_in_tree[cls2] = cls_idx;
                }
            }
        }
      // if(i%max(cloud->points.size()/100,1)==0)
        // { printf("#"); fflush(stdout);}
    }

  printf("]\n");

  float edgeWeight = 0;
  for(int i = 0; i < parents.size(); i++){
    for(int j = 0; j < eset.edges.size(); j++){
      if( ((eset.edges[j]->p0 == i) && (eset.edges[j]->p1 == parents[i])) ||
          ((eset.edges[j]->p1 == i) && (eset.edges[j]->p0 == parents[i])) ){
        EdgeW<Point3D>* tmp = dynamic_cast< EdgeW<Point3D>* >(eset.edges[j]);
        edgeWeight = tmp->w;
      }
    }
    toReturn->eset.edges.push_back
      (new EdgeW<Point3D>(toReturn->eset.points, i, parents[i], edgeWeight));
  }

  return toReturn;
}


template< class P, class E>
vector<int> Graph<P,E>::findLeaves()
{
  vector<int> leaves;
  vector< int > connections(cloud->points.size());
  for(int i = 0; i < connections.size(); i++){
    connections[i] = 0;
  }
  for(int i = 0; i < eset.edges.size(); i++)
    {
      connections[eset.edges[i]->p0]++;
      connections[eset.edges[i]->p1]++;
    }
  for(int i = 0; i < connections.size(); i++){
    if(connections[i] == 1){
      leaves.push_back(i);}
  }
        return leaves;
}

#endif
