#
#  This file is part of Permafrost Engine.
#  Copyright (C) 2018-2020 Eduard Permyakov
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
from constants import *
import common.constants
import map
import globals
import scene
from math import cos, pi
import traceback
import sys

import common.view_controllers.view_controller as vc
import common.view_controllers.tab_bar_vc as tbvc
import common.view_controllers.video_settings_vc as vsvc
import common.view_controllers.game_settings_vc as gsvc

import views.file_chooser_window as fc

import common.views.settings_tabbed_window as stw
import common.views.video_settings_window as vsw
import common.views.game_settings_window as gsw
import common.views.perf_stats_window as psw
import common.views.session_window as sw


class MenuVC(vc.ViewController):

    def __init__(self, view):
        self.view = view
        self.fc = None

        self.__perf_window = psw.PerfStatsWindow()
        self.__settings_vc = tbvc.TabBarVC(stw.SettingsTabbedWindow(),
                                           tab_change_event=common.constants.EVENT_SETTINGS_TAB_SEL_CHANGED)
        self.__settings_vc.push_child(
            "Video", vsvc.VideoSettingsVC(vsw.VideoSettingsWindow()))
        self.__settings_vc.push_child(
            "Game", gsvc.GameSettingsVC(gsw.GameSettingsWindow()))
        self.__settings_shown = False
        self.__session_window = sw.SessionWindow()

    def __on_new(self, event):
        tc.exec_(sys.argv[0], ())

    def __on_load(self, event):
        assert self.fc is None
        self.fc = fc.FileChooser("Load")
        self.fc.show()
        self.deactivate()

        tc.register_event_handler(
            EVENT_FILE_CHOOSER_OKAY, MenuVC.__on_load_confirm, self)
        tc.register_event_handler(
            EVENT_FILE_CHOOSER_CANCEL, MenuVC.__on_load_cancel, self)

    def __on_load_confirm(self, event):
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_OKAY, MenuVC.__on_load_confirm)
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_CANCEL, MenuVC.__on_load_cancel)

        assert self.fc is not None
        self.fc.hide()
        self.fc = None
        self.activate()

        args = event if event[1] is not None else (event[0],)
        tc.exec_(sys.argv[0], args)

    def __on_load_cancel(self, event):
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_OKAY, MenuVC.__on_load_confirm)
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_CANCEL, MenuVC.__on_load_cancel)

        assert self.fc is not None
        self.fc.hide()
        self.fc = None
        self.activate()

    def __on_save_as(self, event):
        assert self.fc is None
        self.fc = fc.FileChooser("Save")
        self.fc.show()
        self.deactivate()

        tc.register_event_handler(
            EVENT_FILE_CHOOSER_OKAY, MenuVC.__on_save_as_confirm, self)
        tc.register_event_handler(
            EVENT_FILE_CHOOSER_CANCEL, MenuVC.__on_save_as_cancel, self)

    def __on_save_as_confirm(self, event):
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_OKAY, MenuVC.__on_save_as_confirm)
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_CANCEL, MenuVC.__on_save_as_cancel)

        assert self.fc is not None
        self.fc.hide()
        self.fc = None
        self.activate()

        old_filename = globals.active_map.filename
        globals.active_map.filename = event[0]
        try:
            globals.active_map.write_to_file()
            if event[1] is not None:
                scene.save_scene(event[1])
                globals.scene_filename = event[1]
        except:
            globals.active_map.filename = old_filename
            print("Failed to save map/scene!")
            traceback.print_exc()
        else:
            self.view.hide()

    def __on_save_as_cancel(self, event):
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_OKAY, MenuVC.__on_save_as_confirm)
        tc.unregister_event_handler(
            EVENT_FILE_CHOOSER_CANCEL, MenuVC.__on_save_as_cancel)

        assert self.fc is not None
        self.fc.hide()
        self.fc = None
        self.activate()

    def __on_save(self, event):
        if globals.active_map.filename is None:
            self.__on_save_as(None)
            return
        try:
            globals.active_map.write_to_file()
            if globals.scene_filename:
                scene.save_scene(globals.scene_filename)
        except:
            print("Failed to save map/scene!")
            traceback.print_exc()
        else:
            self.view.hide()

    def __on_settings_show(self, event):
        if not self.__settings_shown:
            self.__settings_vc.activate()
            self.__settings_shown = True

    def __on_settings_hide(self, event):
        if self.__settings_shown:
            self.__settings_vc.deactivate()
            self.__settings_shown = False

    def __on_perf_show(self, event):
        if self.__perf_window.hidden:
            self.__perf_window.show()

    def __on_session_show(self, event):
        if self.__session_window.hidden:
            self.__session_window.show()

    def __on_session_save(self, event):
        self.__session_window.hide()
        tc.save_session(event)

    def __on_session_load(self, event):
        self.__session_window.hide()
        tc.load_session(event)

    def __on_exit(self, event):
        tc.global_event(tc.SDL_QUIT, None)

    def __on_cancel(self, event):
        self.view.hide()

    def activate(self):
        tc.register_event_handler(EVENT_MENU_NEW, MenuVC.__on_new, self)
        tc.register_event_handler(EVENT_MENU_LOAD, MenuVC.__on_load, self)
        tc.register_event_handler(EVENT_MENU_SAVE, MenuVC.__on_save, self)
        tc.register_event_handler(
            EVENT_MENU_SAVE_AS, MenuVC.__on_save_as, self)
        tc.register_event_handler(EVENT_MENU_EXIT, MenuVC.__on_exit, self)
        tc.register_event_handler(EVENT_MENU_CANCEL, MenuVC.__on_cancel, self)
        tc.register_event_handler(
            EVENT_MENU_SETTINGS_SHOW, MenuVC.__on_settings_show, self)
        tc.register_event_handler(
            common.constants.EVENT_SETTINGS_HIDE, MenuVC.__on_settings_hide, self)
        tc.register_event_handler(
            EVENT_MENU_PERF_SHOW, MenuVC.__on_perf_show, self)
        tc.register_event_handler(
            EVENT_MENU_SESSION_SHOW, MenuVC.__on_session_show, self)
        tc.register_ui_event_handler(
            common.constants.EVENT_SESSION_SAVE_REQUESTED, MenuVC.__on_session_save, self)
        tc.register_ui_event_handler(
            common.constants.EVENT_SESSION_LOAD_REQUESTED, MenuVC.__on_session_load, self)

    def deactivate(self):
        tc.unregister_event_handler(EVENT_MENU_NEW, MenuVC.__on_new)
        tc.unregister_event_handler(EVENT_MENU_LOAD, MenuVC.__on_load)
        tc.unregister_event_handler(EVENT_MENU_SAVE, MenuVC.__on_save)
        tc.unregister_event_handler(EVENT_MENU_SAVE_AS, MenuVC.__on_save_as)
        tc.unregister_event_handler(EVENT_MENU_EXIT, MenuVC.__on_exit)
        tc.unregister_event_handler(EVENT_MENU_CANCEL, MenuVC.__on_cancel)
        tc.unregister_event_handler(
            EVENT_MENU_SETTINGS_SHOW, MenuVC.__on_settings_show)
        tc.unregister_event_handler(
            common.constants.EVENT_SETTINGS_HIDE, MenuVC.__on_settings_hide)
        tc.unregister_event_handler(
            EVENT_MENU_PERF_SHOW, MenuVC.__on_perf_show)
        tc.unregister_event_handler(
            EVENT_MENU_SESSION_SHOW, MenuVC.__on_session_show)
        tc.unregister_event_handler(
            common.constants.EVENT_SESSION_SAVE_REQUESTED, MenuVC.__on_session_save)
        tc.unregister_event_handler(
            common.constants.EVENT_SESSION_LOAD_REQUESTED, MenuVC.__on_session_load)
