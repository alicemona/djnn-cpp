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
 *      Mathieu Poirier <mathieu.poirier@enac.fr>
 *
 */

#pragma once

#include "../core/tree/process.h"
#include "../core/tree/bool_property.h"
#include "../core/tree/int_property.h"
#include "../core/tree/double_property.h"
#include "../core/control/coupling.h"
#include <iostream>
#include <cmath>

namespace djnn
{
  using namespace std;

  class HermiteCurve : public Process
  {
  private:
    class HermiteCurveAction : public Process
    {
    public:
      HermiteCurveAction (Process* parent, const string &name, AbstractProperty *input, AbstractProperty *p1,
                          AbstractProperty *p2, AbstractProperty *t1, AbstractProperty *t2,
                          AbstractProperty *output);
      virtual ~HermiteCurveAction () {}
      void activate () override;
      void deactivate () override {}
    private:
      AbstractProperty *_input, *_p1, *_p2, *_t1, *_t2, *_output;
    };
  public:
    HermiteCurve (Process *p, const string &name, double p1, double p2, double t1, double t2);
    virtual ~HermiteCurve ();
    void activate () override;
    void deactivate () override;
    void serialize (const string& type) override;
  private:
    AbstractProperty *_input, *_p1, *_p2, *_t1, *_t2, *_output;
    Coupling *_c_input, *_c_p1, *_c_p2, *_c_t1, *_c_t2;
    Process *_action;
  };
}
