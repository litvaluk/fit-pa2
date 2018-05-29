#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

// enable only if your implementation supports Add ( ) with more than three parameters
#define MULTIPLE_STOPS

/**
 * @brief CNode class which represents a place, has a vector containing all neighboring places
 * @tparam _T Identifying object (text, number...)
 * @tparam _E Object which holds information about the connection (node to node link)
 */
template <typename _T, typename _E>
class CNode {

  public:

    /**
     * @brief A constructor
     * @param info Identifying object (text, number...)
     */
    CNode (_T info)
    : m_info(info) {}

    /**
     * @brief A constructor
     * @param node Pointer to the node to be added as neighbor
     * @param elem Object which holds information about the connection (node to node link)
     */
    void AddNeighbor (CNode* node, _E elem) {
      m_nodes.push_back(make_pair(node, elem));
    }

    /**
     * @brief Identifying object (text, number...) getter
     * @return Identifying object (text, number...)
     */
    _T GetInfo () {
      return m_info;
    }

    vector<pair<CNode<_T, _E>*, _E> > m_nodes;

  private:

    _T m_info;

};

//=================================================================================================
/**
 * @brief CAccess class which represents a node graph
 * @tparam _T Identifying object (text, number...)
 * @tparam _E Object which holds information about the connection (node to node link)
 */
template <typename _T, typename _E>
class CAccess {

  public:

   /**
    * @brief A constructor
    */
   CAccess () {}

   /**
    * @brief A destructor
    */
   ~CAccess () {

     // deallocation of the map
     for (auto& elem : m_nodes) {
       delete elem.second;
     }

   }

   /**
    * @brief Adds new connection
    * @param elem Object which holds information about the connection (node to node link)
    * @param from Identifying object (text, number...)
    * @param to Identifying object (text, number...)
    * @return This modified object
    */
   CAccess& Add (const _E& elem, const _T& from, const _T& to) {

     CNode<_T, _E>* fromNode;
     CNode<_T, _E>* toNode;
     auto fromNodeIterator = m_nodes.find(from);
     auto toNodeIterator = m_nodes.find(to);

     // if the node doesn't exist, create one and add it to map
     if (fromNodeIterator == m_nodes.end()) {
       fromNode = new CNode<_T, _E> (from);
       m_nodes.insert(make_pair(from, fromNode));
     }

     else {
       fromNode = fromNodeIterator->second;
     }

     // if the node doesn't exist, create one and add it to map
     if (toNodeIterator == m_nodes.end()) {
       toNode = new CNode<_T, _E> (to);
       m_nodes.insert(make_pair(to, toNode));
     }

     else {
       toNode = toNodeIterator->second;
     }

     // adding neighbor to both nodes
     fromNode->AddNeighbor(toNode, elem);
     toNode->AddNeighbor(fromNode, elem);

     return *this;

   }

   /**
    * @brief Adds new connections
    * @tparam ... Identifying objects (text, number...)
    * @param elem Object which holds information about the connection (node to node link)
    * @param from Identifying object (text, number...)
    * @param to Identifying object (text, number...)
    * @param params Identifying objects (text, number...)
    * @return This modified object
    */
   template<typename ... Params>
   CAccess& Add (const _E& elem, const _T& from, const _T& to, Params const& ... params) {

     vector<_T> vec = { from, to, params... };

       for (size_t i = 0; i < vec.size() - 1; i++) {
         for (size_t j = i + 1; j < vec.size(); j++) {
           *this = Add(elem, vec[i], vec[j]);
         }
       }

     return *this;

   }

   /**
    * @brief Finds all reachable places
    * @param startingNodeName Identifying object (text, number...) to start from
    * @return Map with found Identifying objects (text, number...)
    */
   map<_T, int> Find (const _T& startingNodeName) const {

     // calling other overloaded Find method
     // using &x == &x (which is true) just to suppress unused parameter warning
     return Find(startingNodeName, INT_MAX, [] (const _E& x) {return &x == &x;});

   }

   /**
    * @brief Finds all reachable places with maximum distance specified
    * @param startingNodeName Identifying object (text, number...) to start from
    * @param max Maximum distance
    * @return Map with found Identifying objects (text, number...)
    */
   map<_T, int> Find (const _T& startingNodeName, int max) const {

     // calling other overloaded Find method
     // using &x == &x (which is true) just to suppress unused parameter warning
     return Find(startingNodeName, max, [] (const _E& x) {return &x == &x;});

   }

   /**
    * @brief Finds all reachable places with maximum distance and filter specified
    * @tparam Filter (pointer to filter function, functor, lambda function)
    * @param startingNodeName Identifying object (text, number...) to start from
    * @param max Maximum distance
    * @param filter Filter (pointer to filter function, functor, lambda function)
    * @return Map with found Identifying objects (text, number...)
    */
   template<typename _F>
   map<_T, int> Find (const _T& startingNodeName, int max, const _F& filter) const {

     map<_T, int> reachableNodes;
     queue<pair<CNode<_T, _E>*, int> > q;
     set<_T> visited;
     auto startingNodeIterator = m_nodes.find(startingNodeName);

     // throwing exception if there is no node with the specified name
     if (startingNodeIterator == m_nodes.end()) {
       stringstream ss;
       ss << "unknown " << startingNodeName;
       throw invalid_argument(ss.str());
     }

     // modified BFS algorithm
     CNode<_T, _E>* node = startingNodeIterator->second;
     q.push(make_pair(node, 0));

     while (!q.empty()) {

       auto nodePair = q.front();

       // if over max distance, stop
       if (nodePair.second == max + 1) {
         break;
       }

       q.pop();
       node = nodePair.first;
       int distance = nodePair.second;

       if (visited.find(node->GetInfo()) == visited.end()) {
         reachableNodes.insert(make_pair(node->GetInfo(), distance));
         visited.insert(node->GetInfo());
         // adding neighbor nodes to queue
         for (auto elem : node->m_nodes) {
           // not pushing if visited or does not meet the filter conditions
           if (visited.find(elem.first->GetInfo()) != visited.end() || filter(elem.second) == false) {
             continue;
           }
           q.push(make_pair(elem.first, distance+1));
         }
       }

     }

     return reachableNodes;

   }

  private:

    map<_T, CNode<_T, _E>* > m_nodes;

};

#ifndef __PROGTEST__
//=================================================================================================
class CTrain
{
  public:
                             CTrain                        ( const string    & company,
                                                             int               speed )
                             : m_Company ( company ),
                               m_Speed ( speed )
    {
    }
    //---------------------------------------------------------------------------------------------
    string                   m_Company;
    int                      m_Speed;
};
//=================================================================================================
class TrainFilterCompany
{
  public:
                             TrainFilterCompany            ( const set<string> & companies )
                             : m_Companies ( companies )
    {
    }
    //---------------------------------------------------------------------------------------------
    bool                     operator ()                   ( const CTrain & train ) const
    {
      return m_Companies . find ( train . m_Company ) != m_Companies . end ();
    }
    //---------------------------------------------------------------------------------------------
  private:
    set <string>             m_Companies;
};
//=================================================================================================
class TrainFilterSpeed
{
  public:
                             TrainFilterSpeed              ( int               min,
                                                             int               max )
                             : m_Min ( min ),
                               m_Max ( max )
    {
    }
    //---------------------------------------------------------------------------------------------
    bool                     operator ()                   ( const CTrain    & train ) const
    {
      return train . m_Speed >= m_Min && train . m_Speed <= m_Max;
    }
    //---------------------------------------------------------------------------------------------
  private:
    int                      m_Min;
    int                      m_Max;
};
//=================================================================================================
bool                         NurSchnellzug                 ( const CTrain    & zug )
{
  return ( zug . m_Company == "OBB" || zug . m_Company == "DB" ) && zug . m_Speed > 100;
}
//=================================================================================================
int                          main                          ( void )
{
  CAccess<string,CTrain> lines;
  lines . Add ( CTrain (   "DB", 120 ), "Berlin", "Prague"   )
        . Add ( CTrain (   "CD",  80 ), "Berlin", "Prague"   )
        . Add ( CTrain (   "DB", 160 ), "Berlin", "Dresden"  )
        . Add ( CTrain (   "DB", 160 ), "Dresden", "Munchen" )
        . Add ( CTrain (   "CD",  90 ), "Munchen", "Prague"  )
        . Add ( CTrain (   "DB", 200 ), "Munchen", "Linz"    )
        . Add ( CTrain (  "OBB",  90 ), "Munchen", "Linz"    )
        . Add ( CTrain (   "CD",  50 ), "Linz", "Prague"     )
        . Add ( CTrain (   "CD", 100 ), "Prague", "Wien"     )
        . Add ( CTrain (  "OBB", 160 ), "Linz", "Wien"       )
        . Add ( CTrain ( "SNCF", 300 ), "Paris", "Marseille" )
        . Add ( CTrain ( "SNCF", 250 ), "Paris", "Dresden"   )
        . Add ( CTrain ( "SNCF", 200 ), "London", "Calais"   );
  assert ( lines . Find ( "Berlin" ) == (map<string,int>
  {
    make_pair ( "Berlin", 0 ),
    make_pair ( "Dresden", 1 ),
    make_pair ( "Linz", 2 ),
    make_pair ( "Marseille", 3 ),
    make_pair ( "Munchen", 2 ),
    make_pair ( "Paris", 2 ),
    make_pair ( "Prague", 1 ),
    make_pair ( "Wien", 2 )
  }) );
  assert ( lines . Find ( "London" ) == (map<string,int>
  {
    make_pair ( "Calais", 1 ),
    make_pair ( "London", 0 )
  }) );
  assert ( lines . Find ( "Wien" ) == (map<string,int>
  {
    make_pair ( "Berlin", 2 ),
    make_pair ( "Dresden", 3 ),
    make_pair ( "Linz", 1 ),
    make_pair ( "Marseille", 5 ),
    make_pair ( "Munchen", 2 ),
    make_pair ( "Paris", 4 ),
    make_pair ( "Prague", 1 ),
    make_pair ( "Wien", 0 )
  }) );
  assert ( lines . Find ( "Wien", 3 ) == (map<string,int>
  {
    make_pair ( "Berlin", 2 ),
    make_pair ( "Dresden", 3 ),
    make_pair ( "Linz", 1 ),
    make_pair ( "Munchen", 2 ),
    make_pair ( "Prague", 1 ),
    make_pair ( "Wien", 0 )
  }) );
  assert ( lines . Find ( "Wien", 5, NurSchnellzug ) == (map<string,int>
  {
    make_pair ( "Berlin", 4 ),
    make_pair ( "Dresden", 3 ),
    make_pair ( "Linz", 1 ),
    make_pair ( "Munchen", 2 ),
    make_pair ( "Prague", 5 ),
    make_pair ( "Wien", 0 )
  }) );
  assert ( lines . Find ( "Prague", 3,  TrainFilterCompany ( { "CD", "DB" } ) ) == (map<string,int>
  {
    make_pair ( "Berlin", 1 ),
    make_pair ( "Dresden", 2 ),
    make_pair ( "Linz", 1 ),
    make_pair ( "Munchen", 1 ),
    make_pair ( "Prague", 0 ),
    make_pair ( "Wien", 1 )
  }) );
  assert ( lines . Find ( "Munchen", 4, TrainFilterSpeed ( 160, 250 ) ) == (map<string,int>
  {
    make_pair ( "Berlin", 2 ),
    make_pair ( "Dresden", 1 ),
    make_pair ( "Linz", 1 ),
    make_pair ( "Munchen", 0 ),
    make_pair ( "Paris", 2 ),
    make_pair ( "Wien", 2 )
  }) );
  assert ( lines . Find ( "Munchen", 4, [] ( const CTrain & x ) { return x . m_Company == "CD"; } ) == (map<string,int>
  {
    make_pair ( "Berlin", 2 ),
    make_pair ( "Linz", 2 ),
    make_pair ( "Munchen", 0 ),
    make_pair ( "Prague", 1 ),
    make_pair ( "Wien", 2 )
  }) );
  assert ( lines . Find ( "London", 20, [] ( const CTrain & x ) { return x . m_Company == "CD"; } ) == (map<string,int>
  {
    make_pair ( "London", 0 )
  }) );
  try
  {
     auto res = lines . Find ( "Salzburg" );
     assert ( "No exception thrown" == NULL );
  }
  catch ( const invalid_argument & e )
  {
    assert ( string ( e . what () ) == "unknown Salzburg" );
  }
#ifdef MULTIPLE_STOPS
  lines . Add ( CTrain ( "RZD",  80 ), "Prague", "Kiev", "Moscow", "Omsk", "Irkutsk", "Vladivostok" );
  lines . Add ( CTrain ( "LAV", 160 ), "Malaga", "Cordoba", "Toledo", "Madrid", "Zaragoza", "Tarragona", "Barcelona", "Montpellier", "Marseille" );
  assert ( lines . Find ( "Madrid" ) == (map<string,int>
  {
    make_pair ( "Barcelona", 1 ),
    make_pair ( "Berlin", 4 ),
    make_pair ( "Cordoba", 1 ),
    make_pair ( "Dresden", 3 ),
    make_pair ( "Irkutsk", 6 ),
    make_pair ( "Kiev", 6 ),
    make_pair ( "Linz", 5 ),
    make_pair ( "Madrid", 0 ),
    make_pair ( "Malaga", 1 ),
    make_pair ( "Marseille", 1 ),
    make_pair ( "Montpellier", 1 ),
    make_pair ( "Moscow", 6 ),
    make_pair ( "Munchen", 4 ),
    make_pair ( "Omsk", 6 ),
    make_pair ( "Paris", 2 ),
    make_pair ( "Prague", 5 ),
    make_pair ( "Tarragona", 1 ),
    make_pair ( "Toledo", 1 ),
    make_pair ( "Vladivostok", 6 ),
    make_pair ( "Wien", 6 ),
    make_pair ( "Zaragoza", 1 )
  }) );
  assert ( lines . Find ( "Omsk", 4 ) == (map<string,int>
  {
    make_pair ( "Berlin", 2 ),
    make_pair ( "Dresden", 3 ),
    make_pair ( "Irkutsk", 1 ),
    make_pair ( "Kiev", 1 ),
    make_pair ( "Linz", 2 ),
    make_pair ( "Moscow", 1 ),
    make_pair ( "Munchen", 2 ),
    make_pair ( "Omsk", 0 ),
    make_pair ( "Paris", 4 ),
    make_pair ( "Prague", 1 ),
    make_pair ( "Vladivostok", 1 ),
    make_pair ( "Wien", 2 )
  }) );
  assert ( lines . Find ( "Moscow", 10, TrainFilterCompany ( { "RZD", "DB" } ) ) == (map<string,int>
  {
    make_pair ( "Berlin", 2 ),
    make_pair ( "Dresden", 3 ),
    make_pair ( "Irkutsk", 1 ),
    make_pair ( "Kiev", 1 ),
    make_pair ( "Linz", 5 ),
    make_pair ( "Moscow", 0 ),
    make_pair ( "Munchen", 4 ),
    make_pair ( "Omsk", 1 ),
    make_pair ( "Prague", 1 ),
    make_pair ( "Vladivostok", 1 )
  }) );
#endif /* MULTIPLE_STOPS */
  return 0;
}
#endif  /* __PROGTEST__ */
