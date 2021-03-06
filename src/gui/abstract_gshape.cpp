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
 *      Mathieu Magnaudet <mathieu.magnaudet@enac.fr>
 *
 */

#include "abstract_gshape.h"
#include "transformation/transformations.h"
#include "../core/tree/spike.h"
#include "../core/tree/set.h"

namespace djnn
{
  vector<string> AbstractGShape::_ui =
    { "press", "release", "move", "enter", "leave", "touches" };

  Process*
  SVGHolder::find_component (const string &path)
  {
    Process *p = Container::find_component (path);
    if (p == nullptr && _gobj != nullptr)
      p = _gobj->find_component (path);
    return p;
  }

  Process*
  SVGHolder::clone ()
  {
    SVGHolder* newh = new SVGHolder ();

    for (auto c : _children) {
      newh->add_child (c->clone (), this->find_component_name(c));
    }

    newh->_gobj = newh->_children.back ();

    return newh;
  }

  Touch::Touch (Process *p, const string &n) :
      Process (p, n), _shape (nullptr)
  {
    _x = new DoubleProperty (this, "x", 0);
    _y = new DoubleProperty (this, "y", 0);
    _local_x = new DoubleProperty (this, "local_x", 0);
    _local_y = new DoubleProperty (this, "local_y", 0);
    _activation_state = activated;
    Process::finalize ();
  }

  Touch::Touch () :
      Process (), _shape (nullptr)
  {
    _x = new DoubleProperty (this, "x", 0);
    _y = new DoubleProperty (this, "y", 0);
    _local_x = new DoubleProperty (this, "local_x", 0);
    _local_y = new DoubleProperty (this, "local_y", 0);
    _activation_state = activated;
  }

  Touch::~Touch ()
  {
  }

  void
  AbstractGShape::init_mouse_ui ()
  {
    Spike* press = new Spike (this, "press");
    new Spike (this, "release");
    Spike* move = new Spike (this, "move");
    new Spike (this, "enter");
    new Spike (this, "leave");
    Process* s = new Set (this, "touches");
    s->set_state (activated);
    DoubleProperty* px = new DoubleProperty (0);
    DoubleProperty* py = new DoubleProperty (0);
    DoubleProperty* mx = new DoubleProperty (0);
    DoubleProperty* my = new DoubleProperty (0);
    DoubleProperty* local_x = new DoubleProperty (0);
    DoubleProperty* local_y = new DoubleProperty (0);
    press->add_symbol ("x", px);
    press->add_symbol ("y", py);
    press->add_symbol ("local_x", local_x);
    press->add_symbol ("local_y", local_y);
    move->add_symbol ("x", mx);
    move->add_symbol ("y", my);
    move->add_symbol ("local_x", local_x);
    move->add_symbol ("local_y", local_y);
    _matrix = new Homography (this, "matrix", 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    _matrix->set_state (activated);
    _inverted_matrix = new Homography (this, "inverted_matrix", 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    _inverted_matrix->set_state (activated);
    _has_ui = true;
  }

  AbstractGShape::AbstractGShape () :
      AbstractGObj (), _matrix (nullptr), _inverted_matrix (nullptr), _has_ui (false)
  {
    _origin_x = new DoubleProperty (this, "origin_x", 0);
    _origin_y = new DoubleProperty (this, "origin_y", 0);
  }

  AbstractGShape::AbstractGShape (Process *p, const std::string& n) :
      AbstractGObj (p, n), _matrix (nullptr), _inverted_matrix (nullptr), _has_ui (false)
  {
    _origin_x = new DoubleProperty (this, "origin_x", 0);
    _origin_y = new DoubleProperty (this, "origin_y", 0);
  }

  Process*
  AbstractGShape::find_component (const string &path)
  {
    if (_has_ui)
      return Process::find_component (path);
    else {
      size_t found = path.find_first_of ('/');
      string key = path;
      if (found != string::npos) {
        key = path.substr (0, found);
      }
      vector<string>::iterator it = _ui.begin ();
      found = false;
      while (!found && it != _ui.end ()) {
        if (key.compare (*it) == 0) {
          found = true;
          init_mouse_ui ();
        }
        it++;
      }
      return Process::find_component (path);
    }
  }

  AbstractGShape::~AbstractGShape ()
  {
    delete _origin_x;
    delete _origin_y;
    if (_has_ui) {
      Process* press = find_component ("press");
      Process* release = find_component ("release");
      Process* move = find_component ("move");
      Process* enter = find_component ("enter");
      Process* leave = find_component ("leave");
      Process* touches = find_component ("touches");
      Process* x = find_component ("move/x");
      Process* y = find_component ("move/y");
      press->remove_symbol ("x");
      press->remove_symbol ("y");
      move->remove_symbol ("x");
      move->remove_symbol ("y");
      delete _matrix;
      delete _inverted_matrix;
      delete press;
      delete release;
      delete move;
      delete enter;
      delete leave;
      delete x;
      delete y;
      delete touches;
    }
  }

} /* namespace djnn */
