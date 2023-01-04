#
#  This file is part of Permafrost Engine.
#  Copyright (C) 2017-2020 Eduard Permyakov
#
#  Permafrost Engine is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Permafrost Engine is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#  Linking this software statically or dynamically with other modules is making
#  a combined work based on this software. Thus, the terms and conditions of
#  the GNU General Public License cover the whole combination.
#
#  As a special exception, the copyright holders of Permafrost Engine give
#  you permission to link Permafrost Engine with independent modules to produce
#  an executable, regardless of the license terms of these independent
#  modules, and to copy and distribute the resulting executable under
#  terms of your choice, provided that you also meet, for each linked
#  independent module, the terms and conditions of the license of that
#  module. An independent module is a module which is not derived from
#  or based on Permafrost Engine. If you modify Permafrost Engine, you may
#  extend this exception to your version of Permafrost Engine, but you are not
#  obliged to do so. If you do not wish to do so, delete this exception
#  statement from your version.
#

from bpy.props import(
    StringProperty,
    BoolProperty
)
from bpy_extras.io_utils import(
    ImportHelper,
    ExportHelper,
    axis_conversion
)
import bpy
bl_info = {
    "name": "Permafrost Engine Object (.tcobj)",
    "author": "Eduard Permyakov",
    "version": (1, 0, 0),
    "blender": (2, 72),
    "location": "File > Import-Export",
    "description": "Exports and imports models in Permafrost Engine Object format",
    "category": "Import-Export"
}


class ImportPFOBJ(bpy.types.Operator, ImportHelper):
    """Load a Permafrost Engine OBJ File"""

    bl_idname = "import_scene.tcobj"
    bl_label = "Import PFOBJ"
    filename_ext = ".tcobj"
    filter_glob = StringProperty(
        default="*.tcobj",
        options={'HIDDEN'}
    )

    def execute(self, context):
        # TODO
        return {'FINISHED'}


class ExportPFOBJ(bpy.types.Operator, ExportHelper):
    """Save a Permafrost Engine Object File"""

    bl_idname = "export_scene.tcobj"
    bl_label = 'Export PFOBJ'

    filter_glob = StringProperty(
        default="*.tcobj",
        options={'HIDDEN'}
    )
    check_extension = True
    filename_ext = ".tcobj"

    export_bbox = BoolProperty(
        name="Export Object Bounding Box",
        description="Export Bounding Box (used for entities which have collision). For animated "
        "entities, this is the bind pose bounding box.",
        default=True
    )

    local_origin = BoolProperty(
        name="Use Local Object Origin",
        description="Worldspace transform is not applied to vertices, they are exported in object space instead.",
        default=False
    )

    def execute(self, context):
        from . import export_tcobj
        from mathutils import Matrix

        keywords = self.as_keywords(ignore=("check_existing",
                                            "filter_glob"))

        # Convert to OpenGL coordinate system
        global_matrix = axis_conversion(to_forward='-Z', to_up='Y').to_4x4()
        keywords["global_matrix"] = global_matrix

        return export_tcobj.save(self, context, **keywords)


def menu_func_import(self, context):
    self.layout.operator(ImportPFOBJ.bl_idname,
                         text="Permafrost Engine Object (.tcobj)")


def menu_func_export(self, context):
    self.layout.operator(ExportPFOBJ.bl_idname,
                         text="Permafrost Engine Object (.tcobj)")


def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_import.append(menu_func_import)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_import.remove(menu_func_import)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
