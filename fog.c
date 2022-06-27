/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2022 GEGL Fog Beaver (This was inspired by the existing script fu Gimp Filter. But not copied from its code.)
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (turbulence, _("Turbulence"), 1.0)
    description (_("Fog Levels"))
    value_range (0.0, 2.6)


property_seed (seed, _("Random seed"), rand)

property_double (transparency, _("Transparency threshold"), 0.1)
    description(_("Reduce Fog."))
    value_range (0.0, 0.5)

 
property_double (gaus, _("Blur"), 0.0)
   description (_("mild gaussian blur"))
   value_range (0.0, 1.0)
   ui_range    (0.0, 1.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")
   ui_meta     ("axis", "x")


property_color (value, _("Color"), "transparent")
    description (_("The color to paint over the input"))
    ui_meta     ("role", "color-primary")


property_int    (width, _("Width"), 2048)
    description (_("Width of the generated buffer"))
    value_range (0, G_MAXINT)
    ui_range    (0, 4096)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "x")


property_int (height, _("Height"), 1536)
    description(_("Height of the generated buffer"))
    value_range (0, G_MAXINT)
    ui_range    (0, 4096)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "y")


property_double (opacity, _("Opacity"), 1.0)
    description (_("Global opacity value that is always used on top of the optional auxiliary input buffer."))
    value_range (0.6, 2.2)
    ui_range    (0.6, 2.0)

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     fog
#define GEGL_OP_C_SOURCE fog.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *plasma, *gray, *c2a, *color, *gaus, *opacity;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


  plasma = gegl_node_new_child (gegl,
                                  "operation", "gegl:plasma",
                                  NULL);

  gray = gegl_node_new_child (gegl,
                                  "operation", "gegl:gray",
                                  NULL);


  c2a = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-to-alpha",
                                  NULL);

  color = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  gaus = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);

 opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);



  gegl_node_link_many (input, plasma, gray, c2a, color, gaus, opacity, output, NULL);





  gegl_operation_meta_redirect (operation, "gaus", gaus, "std-dev-x");

  gegl_operation_meta_redirect (operation, "gaus", gaus, "std-dev-y");

  gegl_operation_meta_redirect (operation, "turbulence", plasma, "turbulence");

  gegl_operation_meta_redirect (operation, "seed", plasma, "seed");

    gegl_operation_meta_redirect (operation, "value", color, "value");
 
    gegl_operation_meta_redirect (operation, "transparency", c2a, "transparency-threshold");

 
    gegl_operation_meta_redirect (operation, "width", plasma, "width");

 
    gegl_operation_meta_redirect (operation, "height", plasma, "height");

    gegl_operation_meta_redirect (operation, "opacity", opacity, "value");



}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:fog",
    "title",       _("Fog or Cloud Effect"),
    "categories",  "Generic",
    "reference-hash", "45eaaakk52185001x2001b2hc",
    "description", _("Create a fog or cloud effect using GEGL. For edits directly on top of the image without layers use the normal or other blending options. If the image is very large or long you need to manually adjust the width and height of the pixels. "
                     ""),
    NULL);
}

#endif