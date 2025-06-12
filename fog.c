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
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * 2022 GEGL Fog Beaver (This was inspired by the existing script fu Gimp Filter. But not copied from its code.)
 */

/*
GEGL Graph to test without installing. I DON'T KNOW HOW TO MAKE IT FULL SCREEN IN GRAPH MODE. I think Gimp is instructing gegl:plasma to render to canvas size.
The plugin doesn't have this problem.


plasma
 turbulence=2
seed=4 gray  color-to-alpha color-overlay value=#8aed61
gaussian-blur std-dev-x=0 std-dev-y=0
opacity value=0.9 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (turbulence, _("Turbulence of Fog"), 1.0)
    description (_("Fog Levels"))
    value_range (0.0, 2.6)


property_seed (seed, _("Random seed"), rand)

property_double (transparency, _("Isolate Fog patches"), 0.1)
    description(_("Reduce Fog."))
    value_range (0.0, 0.5)


property_double (gaus, _("Blur"), 0.0)
   description (_("mild gaussian blur"))
   value_range (0.0, 1.0)
   ui_range    (0.0, 1.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")
   ui_meta     ("axis", "x")


property_color (value, _("Color"), "white")
    description (_("The color to paint over the input"))
    ui_meta     ("role", "color-primary")


property_int    (width, _("Width"), 1024)
    description (_("Width of the generated buffer"))
    value_range (0, G_MAXINT)
    ui_range    (0, 4096)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "x")
    ui_meta     ("role", "output-extent")

property_int (height, _("Height"), 768)
    description(_("Height of the generated buffer"))
    value_range (0, G_MAXINT)
    ui_range    (0, 4096)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "y")
    ui_meta     ("role", "output-extent")


property_double (opacity, _("Opacity"), 1.0)
    description (_("Global opacity value that is always used on top of the optional auxiliary input buffer."))
    value_range (0.6, 1.5)
    ui_range    (0.6, 1.5)

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     fog
#define GEGL_OP_C_SOURCE fog.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *plasma, *gray, *c2a, *color, *gaus, *over, *opacity;

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
                                         "clip-extent", FALSE,
                                         "abyss-policy", 0,
                                  NULL);

 opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);

 over = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);


  gegl_node_connect (over, "aux", opacity, "output");
  gegl_node_link_many (input, over, output, NULL);
  gegl_node_link_many (plasma, gray, c2a, color, gaus, opacity, NULL);



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
    "name",        "lb:fog",
    "title",       _("Fog or Cloud Effect"),
    "categories",  "Generic",
    "reference-hash", "45eaaakk52185001x2001b2hc",
    "description", _("Create a fog or cloud effect using GEGL. "
                     ""),
    "gimp:menu-path", "<Image>/Filters/Render/Fun",
    "gimp:menu-label", _("Fog..."),
    NULL);
}

#endif
