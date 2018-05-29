#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <typeinfo>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <algorithm>
#include <type_traits>
using namespace std;

class CRect
{
  public:
                             CRect                         ( double            x,
                                                             double            y,
                                                             double            w,
                                                             double            h )
                             : m_X ( x ),
                               m_Y ( y ),
                               m_W ( w ),
                               m_H ( h )
    {
    }
    friend ostream         & operator <<                   ( ostream         & os,
                                                             const CRect     & x )
    {
      return os << '(' << x . m_X << ',' << x . m_Y << ',' << x . m_W << ',' << x . m_H << ')';
    }
    double                   m_X;
    double                   m_Y;
    double                   m_W;
    double                   m_H;
};
#endif /* __PROGTEST__ */

class CWindowElement {

  public:

    CWindowElement (int id, const CRect& relPos)
    : m_id(id), m_relPos(relPos), m_absPos(relPos) {}

    virtual ~CWindowElement () {};

    virtual CWindowElement* Search (int id) {
      if (id == m_id) {
        return this;
      }
      return NULL;
    }

    virtual void SetPosition (const CRect& targetPos) {
      m_absPos.m_W = m_relPos.m_W * targetPos.m_W;
      m_absPos.m_H = m_relPos.m_H * targetPos.m_H;
      m_absPos.m_X = targetPos.m_X + (m_relPos.m_X * targetPos.m_W);
      m_absPos.m_Y = targetPos.m_Y + (m_relPos.m_Y * targetPos.m_H);
    }

    virtual ostream& Print (ostream& os) const {
      os << "NO_SPECIFIC_ELEMENT" << endl;
      return os;
    }

    virtual ostream& PrintWithOffset (ostream& os, const vector<bool>& offset) const {
      os << "NO_SPECIFIC_ELEMENT_OFFSET_PRINT " << offset.size() << endl;
      return os;
    }

    virtual ostream& PrintWithOffsetLast (ostream& os, const vector<bool>& offset) const {
      os << "NO_SPECIFIC_ELEMENT_OFFSET_LAST_PRINT " << offset.size() << endl;
      return os;
    }

    friend ostream& operator << (ostream& os, const CWindowElement& elem) {
      return elem.Print(os);
    }

    virtual unique_ptr<CWindowElement> GetUniquePtr () const {
      return unique_ptr<CWindowElement>(new CWindowElement(*this));
    }

    int m_id;
    CRect m_relPos;
    CRect m_absPos;

};

// ======================================================================================================
class CWindow {

  public:

    CWindow (const string& title, const CRect& absPos)
    : m_title(title), m_absPos(absPos) {}

    CWindow (const CWindow& src)
    : m_title(src.m_title), m_absPos(src.m_absPos) {

      for (auto& elem : src.elements) {
        elements.insert(make_pair(elem.second->m_id, elem.second->GetUniquePtr()));
      }

    }

    CWindow& operator = (const CWindow& src) {

      if (&src == this) {
        return *this;
      }

      m_title = src.m_title;
      m_absPos = src.m_absPos;

      elements.clear();
      for (auto& elem : src.elements) {
        elements.insert(make_pair(elem.second->m_id, elem.second->GetUniquePtr()));
      }

      return *this;

    }

    // Add
    CWindow& Add (const CWindowElement& element) {

      auto it = elements.insert(make_pair(element.m_id, element.GetUniquePtr()));

      it->second->SetPosition(m_absPos);

      return *this;

    }

    CWindowElement* Search (int id) {
      for (auto& elem : elements) {
        CWindowElement* x = elem.second->Search(id);
        if (x != NULL) {
          return x;
        }
      }
      throw invalid_argument("NOT FOUND");
      return NULL;
    }

    void SetPosition (const CRect& targetPos) {

      for (auto& elem : elements) {
        elem.second->SetPosition(targetPos);
      }

      m_absPos.m_X = targetPos.m_X;
      m_absPos.m_Y = targetPos.m_Y;
      m_absPos.m_W = targetPos.m_W;
      m_absPos.m_H = targetPos.m_H;

    }

    ostream& Print (ostream& os) const {
      os << "Window \"" << m_title <<  "\" " << m_absPos << endl;

      size_t i = 0;
      vector<bool> offset;

      for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (i == elements.size() - 1) {
          (it->second)->PrintWithOffsetLast(os, offset);
          break;
        }
        (it->second)->PrintWithOffset(os, offset);
        i++;
      }

      return os;
    }

    friend ostream& operator << (ostream& os, const CWindow& window) {
      return window.Print(os);
    }


  private:

    string m_title;
    CRect m_absPos;
    multimap<int, unique_ptr<CWindowElement> > elements;

};

// ======================================================================================================
class CPanel : public CWindowElement {

  public:

    CPanel (int id, const CRect& relPos)
    : CWindowElement(id, relPos) {}

    CPanel (const CPanel& src)
    : CWindowElement(src){

      for (auto& elem : src.elements) {
        elements.push_back(elem->GetUniquePtr());
      }

    }

    CPanel& operator = (const CPanel& src) {

      if (&src == this) {
        return *this;
      }

      m_id = src.m_id;
      m_absPos = src.m_absPos;
      m_relPos = src.m_relPos;

      elements.clear();
      for (auto& elem : src.elements) {
        elements.push_back(elem->GetUniquePtr());
      }

      return *this;

    }

    void SetPosition (const CRect& targetPos) {
        m_absPos.m_W = m_relPos.m_W * targetPos.m_W;
        m_absPos.m_H = m_relPos.m_H * targetPos.m_H;
        m_absPos.m_X = targetPos.m_X + (m_relPos.m_X * targetPos.m_W);
        m_absPos.m_Y = targetPos.m_Y + (m_relPos.m_Y * targetPos.m_H);
        for (auto& elem : elements) {
          elem->SetPosition(m_absPos);
        }
    }

    virtual CWindowElement* Search (int id) {
      if (id == m_id) {
        return this;
      }
      for (auto& elem : elements) {
        CWindowElement* x = elem->Search(id);
        if (x != NULL) {
          return x;
        }
      }
      return NULL;
    }

    CPanel& Add (const CWindowElement& element) {

      elements.push_back(element.GetUniquePtr());

      elements[elements.size() - 1]->SetPosition(m_absPos);

      return *this;

    }

    ostream& Print (ostream& os) const {

      os << "[" << m_id << "] Panel " << m_absPos << endl;

      vector<bool> offset;

      for (size_t i = 0; i < elements.size(); i++) {
        if (i == elements.size() - 1) {
          elements[i]->PrintWithOffsetLast(os, offset);
          return os;
        }
        elements[i]->PrintWithOffset(os, offset);
      }

      return os;

    }

    ostream& PrintWithOffset (ostream& os, const vector<bool>& constOffset) const {

      vector<bool> offset = constOffset;

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      os << "+- [" << m_id << "] Panel " << m_absPos << endl;

      for (size_t i = 0; i < elements.size(); i++) {
        if (i == elements.size() - 1) {
          offset.push_back(true);
          elements[i]->PrintWithOffsetLast(os, offset);
          offset.pop_back();
          return os;
        }
        offset.push_back(true);
        elements[i]->PrintWithOffset(os, offset);
        offset.pop_back();
      }

      return os;
    }

    ostream& PrintWithOffsetLast (ostream& os, const vector<bool>& constOffset) const {

      vector<bool> offset = constOffset;

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      os << "+- [" << m_id << "] Panel " << m_absPos << endl;

      for (size_t i = 0; i < elements.size(); i++) {
        if (i == elements.size() - 1) {
          offset.push_back(false);
          elements[i]->PrintWithOffsetLast(os, offset);
          offset.pop_back();
          return os;
        }
        offset.push_back(false);
        elements[i]->PrintWithOffset(os, offset);
        offset.pop_back();
      }

      return os;
    }

    unique_ptr<CWindowElement> GetUniquePtr () const {
      return unique_ptr<CWindowElement>(new CPanel(*this));
    }

  private:

    vector<unique_ptr<CWindowElement> > elements;

};

// ======================================================================================================
class CButton : public CWindowElement {

  public:

    CButton (int id, const CRect& relPos, const string& name)
    : CWindowElement(id, relPos), m_name(name) {}

    ostream& Print (ostream& os) const {
      os << "+- [" << m_id << "] Button \"" << m_name << "\" " << m_absPos << endl;
      return os;
    }

    ostream& PrintWithOffset (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      return Print(os);
    }

    ostream& PrintWithOffsetLast (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      return Print(os);
    }

    unique_ptr<CWindowElement> GetUniquePtr () const {
      return unique_ptr<CWindowElement>(new CButton(*this));
    }

  protected:

    string m_name;

};

// ======================================================================================================
class CInput : public CWindowElement {

  public:

    CInput (int id, const CRect& relPos, const string& value)
    : CWindowElement(id, relPos), m_value(value) {}

    void SetValue (const string& value) {
      m_value = value;
    }

    string GetValue () const {
      return m_value;
    }

    ostream& Print (ostream& os) const {
      os << "+- [" << m_id << "] Input \"" << m_value << "\" " << m_absPos << endl;
      return os;
    }

    ostream& PrintWithOffset (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      return Print(os);
    }

    ostream& PrintWithOffsetLast (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }
      return Print(os);
    }

    unique_ptr<CWindowElement> GetUniquePtr () const {
      return unique_ptr<CWindowElement>(new CInput(*this));
    }

  private:

    string m_value;

};

// ======================================================================================================
class CLabel : public CWindowElement {

  public:

    CLabel (int id, const CRect& relPos, const string& label)
    : CWindowElement(id, relPos), m_label(label) {}

    ostream& Print (ostream& os) const {
      os << "+- [" << m_id << "] Label \"" << m_label << "\" " << m_absPos << endl;
      return os;
    }

    ostream& PrintWithOffset (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      return Print(os);
    }

    ostream& PrintWithOffsetLast (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      return Print(os);
    }

    unique_ptr<CWindowElement> GetUniquePtr () const {
      return unique_ptr<CWindowElement>(new CLabel(*this));
    }

  private:

    string m_label;

};

// ======================================================================================================
class CComboBoxElement {

  public:

    CComboBoxElement (const string& name)
    : m_name(name) {}

    friend ostream& operator << (ostream& os, const CComboBoxElement& elem) {
      return os << elem.m_name;
    }

  private:

    string m_name;

};

// ======================================================================================================
class CComboBox : public CWindowElement {

  public:

    CComboBox (int id, const CRect& relPos)
    : CWindowElement(id, relPos), m_selected(0) {};

    CComboBox& Add (const string& str) {
      comboBoxElements.push_back(CComboBoxElement(str));
      return *this;
    }

    void SetSelected (int index) {
      m_selected = index;
    }

    int GetSelected () const {
      return m_selected;
    }

    ostream& Print (ostream& os) const {

      os << "[" << m_id << "] ComboBox " << m_absPos << endl;

      for (size_t i = 0; i < comboBoxElements.size(); i++) {

        os << "+-";

        if (i == m_selected) {
          os << ">" << comboBoxElements[i] << "<" << endl;
        }

        else {
          os << " " << comboBoxElements[i] << endl;
        }

      }

      return os;

    }

    ostream& PrintWithOffset (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      os << "+- [" << m_id << "] ComboBox " << m_absPos << endl;

      for (size_t i = 0; i < comboBoxElements.size(); i++) {

        for (auto elem : offset) {
          if (elem == true) {
            os << "|  ";
          }
          else {
            os << "   ";
          }
        }

        os << "|  +-";

        if (i == m_selected) {
          os << ">" << comboBoxElements[i] << "<" << endl;
        }

        else {
          os << " " << comboBoxElements[i] << endl;
        }

      }

      return os;

    }

    ostream& PrintWithOffsetLast (ostream& os, const vector<bool>& offset) const {

      for (auto elem : offset) {
        if (elem == true) {
          os << "|  ";
        }
        else {
          os << "   ";
        }
      }

      os << "+- [" << m_id << "] ComboBox " << m_absPos << endl;

      for (size_t i = 0; i < comboBoxElements.size(); i++) {

        for (auto elem : offset) {
          if (elem == true) {
            os << "|  ";
          }
          else {
            os << "   ";
          }
        }

        os << "   +-";

        if (i == m_selected) {
          os << ">" << comboBoxElements[i] << "<" << endl;
        }

        else {
          os << " " << comboBoxElements[i] << endl;
        }

      }

      return os;

    }

    unique_ptr<CWindowElement> GetUniquePtr () const {
      return unique_ptr<CWindowElement>(new CComboBox(*this));
    }

  private:

    size_t m_selected;
    vector<CComboBoxElement> comboBoxElements;

};

#ifndef __PROGTEST__
template <typename _T>
string toString ( const _T & x )
{
  ostringstream oss;
  oss << x;
  return oss . str ();
}

int main ( void )
{
  assert ( sizeof ( CButton ) - sizeof ( string ) < sizeof ( CComboBox ) - sizeof ( vector<string> ) );
  assert ( sizeof ( CInput ) - sizeof ( string ) < sizeof ( CComboBox ) - sizeof ( vector<string> ) );
  assert ( sizeof ( CLabel ) - sizeof ( string ) < sizeof ( CComboBox ) - sizeof ( vector<string> ) );
  assert ( sizeof ( CButton ) - sizeof ( string ) <= sizeof ( CPanel ) - sizeof ( vector<void*> ) );
  assert ( sizeof ( CInput ) - sizeof ( string ) <= sizeof ( CPanel ) - sizeof ( vector<void*> ) );
  assert ( sizeof ( CLabel ) - sizeof ( string ) <= sizeof ( CPanel ) - sizeof ( vector<void*> ) );

  CWindow a ( "Sample window", CRect ( 10, 10, 600, 480 ) );
  a . Add ( CButton ( 1, CRect ( 0.1, 0.8, 0.3, 0.1 ), "Ok" ) ) . Add ( CButton ( 2, CRect ( 0.6, 0.8, 0.3, 0.1 ), "Cancel" ) );
  a . Add ( CLabel ( 10, CRect ( 0.1, 0.1, 0.2, 0.1 ), "Username:" ) );
  a . Add ( CInput ( 11, CRect ( 0.4, 0.1, 0.5, 0.1 ), "chucknorris" ) );
  a . Add ( CPanel ( 12, CRect ( 0.1, 0.3, 0.8, 0.7 ) ) . Add ( CComboBox ( 20, CRect ( 0.1, 0.3, 0.8, 0.1 ) ) . Add ( "Karate" ) . Add ( "Judo" ) . Add ( "Box" ) . Add ( "Progtest" ) ) );
  assert ( toString ( a ) ==
    "Window \"Sample window\" (10,10,600,480)\n"
    "+- [1] Button \"Ok\" (70,394,180,48)\n"
    "+- [2] Button \"Cancel\" (370,394,180,48)\n"
    "+- [10] Label \"Username:\" (70,58,120,48)\n"
    "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
    "+- [12] Panel (70,154,480,336)\n"
    "   +- [20] ComboBox (118,254.8,384,33.6)\n"
    "      +->Karate<\n"
    "      +- Judo\n"
    "      +- Box\n"
    "      +- Progtest\n" );
  CWindow b = a;
  assert ( toString ( *b . Search ( 20 ) ) ==
    "[20] ComboBox (118,254.8,384,33.6)\n"
    "+->Karate<\n"
    "+- Judo\n"
    "+- Box\n"
    "+- Progtest\n" );
  assert ( dynamic_cast<CComboBox &> ( *b . Search ( 20 ) ) . GetSelected () == 0 );
  dynamic_cast<CComboBox &> ( *b . Search ( 20 ) ) . SetSelected ( 3 );
  assert ( dynamic_cast<CInput &> ( *b . Search ( 11 ) ) . GetValue () == "chucknorris" );
  dynamic_cast<CInput &> ( *b . Search ( 11 ) ) . SetValue ( "chucknorris@fit.cvut.cz" );
  CPanel & p = dynamic_cast<CPanel &> ( *b . Search ( 12 ) );
  p . Add ( CComboBox ( 21, CRect ( 0.1, 0.5, 0.8, 0.1 ) ) . Add ( "PA2" ) . Add ( "OSY" ) . Add ( "Both" ) );
  assert ( toString ( b ) ==
    "Window \"Sample window\" (10,10,600,480)\n"
    "+- [1] Button \"Ok\" (70,394,180,48)\n"
    "+- [2] Button \"Cancel\" (370,394,180,48)\n"
    "+- [10] Label \"Username:\" (70,58,120,48)\n"
    "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
    "+- [12] Panel (70,154,480,336)\n"
    "   +- [20] ComboBox (118,254.8,384,33.6)\n"
    "   |  +- Karate\n"
    "   |  +- Judo\n"
    "   |  +- Box\n"
    "   |  +->Progtest<\n"
    "   +- [21] ComboBox (118,322,384,33.6)\n"
    "      +->PA2<\n"
    "      +- OSY\n"
    "      +- Both\n" );
  assert ( toString ( a ) ==
    "Window \"Sample window\" (10,10,600,480)\n"
    "+- [1] Button \"Ok\" (70,394,180,48)\n"
    "+- [2] Button \"Cancel\" (370,394,180,48)\n"
    "+- [10] Label \"Username:\" (70,58,120,48)\n"
    "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
    "+- [12] Panel (70,154,480,336)\n"
    "   +- [20] ComboBox (118,254.8,384,33.6)\n"
    "      +->Karate<\n"
    "      +- Judo\n"
    "      +- Box\n"
    "      +- Progtest\n" );
  assert ( toString ( p ) ==
    "[12] Panel (70,154,480,336)\n"
    "+- [20] ComboBox (118,254.8,384,33.6)\n"
    "|  +- Karate\n"
    "|  +- Judo\n"
    "|  +- Box\n"
    "|  +->Progtest<\n"
    "+- [21] ComboBox (118,322,384,33.6)\n"
    "   +->PA2<\n"
    "   +- OSY\n"
    "   +- Both\n" );
  b . SetPosition ( CRect ( 20, 30, 640, 520 ) );
  assert ( toString ( b ) ==
    "Window \"Sample window\" (20,30,640,520)\n"
    "+- [1] Button \"Ok\" (84,446,192,52)\n"
    "+- [2] Button \"Cancel\" (404,446,192,52)\n"
    "+- [10] Label \"Username:\" (84,82,128,52)\n"
    "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
    "+- [12] Panel (84,186,512,364)\n"
    "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
    "   |  +- Karate\n"
    "   |  +- Judo\n"
    "   |  +- Box\n"
    "   |  +->Progtest<\n"
    "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
    "      +->PA2<\n"
    "      +- OSY\n"
    "      +- Both\n" );
  p . Add ( p );
  assert ( toString ( p ) ==
    "[12] Panel (84,186,512,364)\n"
    "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
    "|  +- Karate\n"
    "|  +- Judo\n"
    "|  +- Box\n"
    "|  +->Progtest<\n"
    "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
    "|  +->PA2<\n"
    "|  +- OSY\n"
    "|  +- Both\n"
    "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "   |  +- Karate\n"
    "   |  +- Judo\n"
    "   |  +- Box\n"
    "   |  +->Progtest<\n"
    "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "      +->PA2<\n"
    "      +- OSY\n"
    "      +- Both\n" );
  p . Add ( p );
  assert ( toString ( p ) ==
    "[12] Panel (84,186,512,364)\n"
    "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
    "|  +- Karate\n"
    "|  +- Judo\n"
    "|  +- Box\n"
    "|  +->Progtest<\n"
    "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
    "|  +->PA2<\n"
    "|  +- OSY\n"
    "|  +- Both\n"
    "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "|  |  +- Karate\n"
    "|  |  +- Judo\n"
    "|  |  +- Box\n"
    "|  |  +->Progtest<\n"
    "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "|     +->PA2<\n"
    "|     +- OSY\n"
    "|     +- Both\n"
    "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "   |  +- Karate\n"
    "   |  +- Judo\n"
    "   |  +- Box\n"
    "   |  +->Progtest<\n"
    "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "   |  +->PA2<\n"
    "   |  +- OSY\n"
    "   |  +- Both\n"
    "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "      |  +- Karate\n"
    "      |  +- Judo\n"
    "      |  +- Box\n"
    "      |  +->Progtest<\n"
    "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "         +->PA2<\n"
    "         +- OSY\n"
    "         +- Both\n" );
  p . Add ( p );
  assert ( toString ( p ) ==
    "[12] Panel (84,186,512,364)\n"
    "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
    "|  +- Karate\n"
    "|  +- Judo\n"
    "|  +- Box\n"
    "|  +->Progtest<\n"
    "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
    "|  +->PA2<\n"
    "|  +- OSY\n"
    "|  +- Both\n"
    "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "|  |  +- Karate\n"
    "|  |  +- Judo\n"
    "|  |  +- Box\n"
    "|  |  +->Progtest<\n"
    "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "|     +->PA2<\n"
    "|     +- OSY\n"
    "|     +- Both\n"
    "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "|  |  +- Karate\n"
    "|  |  +- Judo\n"
    "|  |  +- Box\n"
    "|  |  +->Progtest<\n"
    "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "|  |  +->PA2<\n"
    "|  |  +- OSY\n"
    "|  |  +- Both\n"
    "|  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "|     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "|     |  +- Karate\n"
    "|     |  +- Judo\n"
    "|     |  +- Box\n"
    "|     |  +->Progtest<\n"
    "|     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "|        +->PA2<\n"
    "|        +- OSY\n"
    "|        +- Both\n"
    "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "   |  +- Karate\n"
    "   |  +- Judo\n"
    "   |  +- Box\n"
    "   |  +->Progtest<\n"
    "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "   |  +->PA2<\n"
    "   |  +- OSY\n"
    "   |  +- Both\n"
    "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "   |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "   |  |  +- Karate\n"
    "   |  |  +- Judo\n"
    "   |  |  +- Box\n"
    "   |  |  +->Progtest<\n"
    "   |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "   |     +->PA2<\n"
    "   |     +- OSY\n"
    "   |     +- Both\n"
    "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "      |  +- Karate\n"
    "      |  +- Judo\n"
    "      |  +- Box\n"
    "      |  +->Progtest<\n"
    "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "      |  +->PA2<\n"
    "      |  +- OSY\n"
    "      |  +- Both\n"
    "      +- [12] Panel (208.928,425.148,262.144,124.852)\n"
    "         +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
    "         |  +- Karate\n"
    "         |  +- Judo\n"
    "         |  +- Box\n"
    "         |  +->Progtest<\n"
    "         +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
    "            +->PA2<\n"
    "            +- OSY\n"
    "            +- Both\n" );
  assert ( toString ( b ) ==
    "Window \"Sample window\" (20,30,640,520)\n"
    "+- [1] Button \"Ok\" (84,446,192,52)\n"
    "+- [2] Button \"Cancel\" (404,446,192,52)\n"
    "+- [10] Label \"Username:\" (84,82,128,52)\n"
    "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
    "+- [12] Panel (84,186,512,364)\n"
    "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
    "   |  +- Karate\n"
    "   |  +- Judo\n"
    "   |  +- Box\n"
    "   |  +->Progtest<\n"
    "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
    "   |  +->PA2<\n"
    "   |  +- OSY\n"
    "   |  +- Both\n"
    "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "   |  |  +- Karate\n"
    "   |  |  +- Judo\n"
    "   |  |  +- Box\n"
    "   |  |  +->Progtest<\n"
    "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "   |     +->PA2<\n"
    "   |     +- OSY\n"
    "   |     +- Both\n"
    "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "   |  |  +- Karate\n"
    "   |  |  +- Judo\n"
    "   |  |  +- Box\n"
    "   |  |  +->Progtest<\n"
    "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "   |  |  +->PA2<\n"
    "   |  |  +- OSY\n"
    "   |  |  +- Both\n"
    "   |  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "   |     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "   |     |  +- Karate\n"
    "   |     |  +- Judo\n"
    "   |     |  +- Box\n"
    "   |     |  +->Progtest<\n"
    "   |     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "   |        +->PA2<\n"
    "   |        +- OSY\n"
    "   |        +- Both\n"
    "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
    "      +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
    "      |  +- Karate\n"
    "      |  +- Judo\n"
    "      |  +- Box\n"
    "      |  +->Progtest<\n"
    "      +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
    "      |  +->PA2<\n"
    "      |  +- OSY\n"
    "      |  +- Both\n"
    "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "      |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "      |  |  +- Karate\n"
    "      |  |  +- Judo\n"
    "      |  |  +- Box\n"
    "      |  |  +->Progtest<\n"
    "      |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "      |     +->PA2<\n"
    "      |     +- OSY\n"
    "      |     +- Both\n"
    "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
    "         +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
    "         |  +- Karate\n"
    "         |  +- Judo\n"
    "         |  +- Box\n"
    "         |  +->Progtest<\n"
    "         +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
    "         |  +->PA2<\n"
    "         |  +- OSY\n"
    "         |  +- Both\n"
    "         +- [12] Panel (208.928,425.148,262.144,124.852)\n"
    "            +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
    "            |  +- Karate\n"
    "            |  +- Judo\n"
    "            |  +- Box\n"
    "            |  +->Progtest<\n"
    "            +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
    "               +->PA2<\n"
    "               +- OSY\n"
    "               +- Both\n" );
  assert ( toString ( a ) ==
    "Window \"Sample window\" (10,10,600,480)\n"
    "+- [1] Button \"Ok\" (70,394,180,48)\n"
    "+- [2] Button \"Cancel\" (370,394,180,48)\n"
    "+- [10] Label \"Username:\" (70,58,120,48)\n"
    "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
    "+- [12] Panel (70,154,480,336)\n"
    "   +- [20] ComboBox (118,254.8,384,33.6)\n"
    "      +->Karate<\n"
    "      +- Judo\n"
    "      +- Box\n"
    "      +- Progtest\n" );
  return 0;
}
#endif /* __PROGTEST__ */
