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

#include "linux_input.h"
#include "../../core/error.h"
#include <linux/input.h>
#include <libevdev/libevdev.h>


namespace djnn {
  unique_ptr<LinuxDevice>
  map_device (const struct libevdev *dev, const string &n)
  {
    //int keyFound = 0;

    /* mouse if we have at least relative x and y and button left */
    if (libevdev_has_event_code (dev, EV_REL, REL_X)
      && libevdev_has_event_code (dev, EV_REL, REL_Y)
      && libevdev_has_event_code (dev, EV_KEY, BTN_LEFT)) {
        return make_unique<LinuxMouse> (Mice, n, dev);
    }
    warning ("unknown device type");
    return nullptr;
  }
}