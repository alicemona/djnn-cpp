/*
 *  djnn v2
 *
 *  The copyright holders for the contents of this file are:
 *      Ecole Nationale de l'Aviation Civile, France (2018)
 *  See file "license.terms" for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Mathieu Poirier <mathieu.poirier@enac.fr>
 *
 */

#include "../core-dev.h"
#include "serializer.h"

using namespace std;

namespace djnn
{


  typedef struct djnXMLDumpContext {
    struct djnXMLDumpContext* parent;
    int haschildren;
    const char* classname;
  } __XMLDumpContext;

  static int __XMLDumpNumAttrs;
  static __XMLDumpContext* __curXMLDumpContext = 0;
  static int __XMLLevel = 0;

  void
  XMLSerializer::start (const string& name) {

  
    int i;
    __XMLDumpContext* c;

    if (__curXMLDumpContext && !__curXMLDumpContext->haschildren) {
      if (__XMLDumpNumAttrs > 0)
        cout << " ";

      cout << ">\n";
      ++__XMLLevel;
      __curXMLDumpContext->haschildren = 1;
    }

    for (i = 0; i < __XMLLevel; ++i)
      cout << "  ";
    cout << "<" << name;

    if (__curXMLDumpContext == 0) {
      int l = name.length () + 3;
      for (auto module_name : djnn::loadedModules) {
        if (module_name.compare("core") == 0)
          cout << " xmlns=\"http://xml.djnn.net/2012/" << module_name << "\"";
        else {
          cout << endl ;
          while (--l)
            cout << " ";
          cout << "xmlns:" << module_name << "=\"http://xml.djnn.net/2012/" << module_name << "\"";
        }
      }
    }

    c = (djnXMLDumpContext*) malloc (sizeof (djnXMLDumpContext));
    c->haschildren = 0;
    c->classname = name.c_str();
    c->parent = __curXMLDumpContext;
    __curXMLDumpContext = c;
    __XMLDumpNumAttrs = 0;

  }

  void
  XMLSerializer::text_attribute (const string& name, const string& value){
    cout << " " << name << "=\"" << value << "\"";
    ++__XMLDumpNumAttrs;
  }

  void
  XMLSerializer::int_attribute (const string& name, int value){
    cout << " " << name << "=\"" << value << "\"";
    ++__XMLDumpNumAttrs;
  }

  void
  XMLSerializer::float_attribute (const string& name, double value){
    cout << " " << name << "=\"" << value << "\"";
    ++__XMLDumpNumAttrs;
  }

  void
  XMLSerializer::end (){

    __XMLDumpContext* c;

    if (__curXMLDumpContext->haschildren) {
      int i;
      --__XMLLevel;
      for (i = 0; i < __XMLLevel; ++i)
        cout << " ";
      cout << "</" << __curXMLDumpContext->classname << ">\n";
    } else {
      if (__XMLDumpNumAttrs > 0)
        cout << " ";
      cout << "/>\n";
    }

    c = __curXMLDumpContext->parent;
    free (__curXMLDumpContext);
    __curXMLDumpContext = c;
  }

}
