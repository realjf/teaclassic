#
#  This file is part of Permafrost Engine.
#  Copyright (C) 2019-2020 Eduard Permyakov
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

import tc
from common.constants import *


class SessionWindow(tc.Window):
    WINDOW_WIDTH = 500
    WINDOW_HEIGHT = 140

    def __init__(self):
        vresx, vresy = (1920, 1080)
        super(SessionWindow, self).__init__("Session",
                                            (vresx / 2 - SessionWindow.WINDOW_WIDTH / 2, vresy / 2 - SessionWindow.WINDOW_HEIGHT / 2,
                                             SessionWindow.WINDOW_WIDTH, SessionWindow.WINDOW_HEIGHT),
                                            tc.NK_WINDOW_BORDER | tc.NK_WINDOW_NO_SCROLLBAR | tc.NK_WINDOW_TITLE | tc.NK_WINDOW_CLOSABLE, (vresx, vresy))
        self.filestring = tc.get_basedir()

    def update(self):

        self.layout_row_dynamic(20, 1)
        self.label_colored_wrap("Session File:", (175, 175, 175))
        self.layout_row_dynamic(30, 1)
        self.filestring = self.edit_string(tc.NK_EDIT_SIMPLE, self.filestring)

        def on_save():
            tc.global_event(EVENT_SESSION_SAVE_REQUESTED, self.filestring)

        def on_load():
            tc.global_event(EVENT_SESSION_LOAD_REQUESTED, self.filestring)

        self.layout_row_dynamic(30, 2)
        self.button_label("Save", on_save)
        self.button_label("Load", on_load)
