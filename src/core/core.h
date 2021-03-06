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
 *		Mathieu Poirier <mathieu.poirier@enac.fr>
 *
 */

#pragma once

#include "tree/process.h"
#include "control/binding.h"
#include "control/activator.h"
#include "tree/component.h"
#include "control/native_action.h"
#include "control/native_expression_action.h"
#include "tree/spike.h"
#include "control/assignment.h"
#include "tree/list.h"
#include "xml/xml.h"
#include "control/exit.h"
#include "serializer/serializer.h"
#include "syshook/timer.h"
#include "tree/abstract_property.h"
#include "tree/blank.h"
#include "tree/bool_property.h"
#include "tree/int_property.h"
#include "tree/double_property.h"
#include "tree/text_property.h"
#include "tree/ref_property.h"
#include "tree/set.h"
#include "error.h"


namespace djnn {

  extern std::vector<string> loadedModules;

  void init_core ();
  void clear_core ();

}
