
import tc
import sys
import math

import common.views.perf_stats_window as psw

import rts.units.knight
import rts.units.berzerker
import rts.units.anim_combatable as am


ARMY_SIZE = 256

MAP_HEIGHT = 4 * tc.TILES_PER_CHUNK_HEIGHT * tc.Z_COORDS_PER_TILE
MAP_WIDTH = 4 * tc.TILES_PER_CHUNK_WIDTH * tc.X_COORDS_PER_TILE

SPACING = 12

DIR_RIGHT = (0.0, 1.0/math.sqrt(2.0), 0.0, 1.0/math.sqrt(2.0))
DIR_LEFT = (0.0, -1.0/math.sqrt(2.0), 0.0, 1.0/math.sqrt(2.0))

red_army_units = []
blue_army_units = []
war_on = False


def setup_scene():

    tc.set_ambient_light_color((1.0, 1.0, 1.0))
    tc.set_emit_light_color((1.0, 1.0, 1.0))
    tc.set_emit_light_pos((1664.0, 1024.0, 384.0))

    tc.disable_fog_of_war()

    tc.load_map("assets/maps", "plain.tcmap")

    tc.add_faction("RED", (255, 0, 0, 255))
    tc.add_faction("BLUE", (0, 0, 255, 255))

    tc.set_diplomacy_state(0, 1, tc.DIPLOMACY_STATE_WAR)

    tc.set_faction_controllable(0, False)
    tc.set_faction_controllable(1, False)


def setup_armies():

    global red_army_units
    global blue_army_units

    NROWS = 4
    NCOLS = math.ceil(ARMY_SIZE / NROWS)

    assert math.ceil(NROWS/2) * SPACING < MAP_HEIGHT//2
    assert math.ceil(NCOLS/2) * SPACING < MAP_WIDTH//2

    # (0,0) is the center of the map

    for r in range(int(-NROWS//2), int(NROWS//2 + NROWS % 2)):
        for c in range(int(-NCOLS//2), int(NCOLS//2 + NCOLS % 2)):

            x = -(r * SPACING) + 35
            z = c * SPACING
            y = tc.map_height_at_point(x, z)

            knight = rts.units.knight.Knight(
                "assets/models/knight", "knight.tcobj", "Knight")
            knight.pos = (float(x), float(y), float(z))
            knight.rotation = DIR_RIGHT
            knight.faction_id = 0
            knight.selection_radius = 3.25
            knight.selectable = True
            knight.vision_range = 35.0
            knight.hold_position()

            red_army_units += [knight]

            x = (r * SPACING) - 35.0
            z = c * SPACING
            y = tc.map_height_at_point(x, z)

            berz = rts.units.berzerker.Berzerker(
                "assets/models/berzerker", "berzerker.tcobj", "Berzerker")
            berz.pos = (float(x), float(y), float(z))
            berz.rotation = DIR_LEFT
            berz.faction_id = 1
            berz.selection_radius = 3.00
            berz.selectable = True
            berz.vision_range = 35.0
            berz.hold_position()

            blue_army_units += [berz]


def fixup_anim_combatable():

    def on_death(self, event):
        self.play_anim(self.death_anim(), mode=tc.ANIM_MODE_ONCE)
        self.register(tc.EVENT_ANIM_CYCLE_FINISHED,
                      am.AnimCombatable.on_death_anim_finish, self)

    def on_death_anim_finish(self, event):
        self.unregister(tc.EVENT_ANIM_CYCLE_FINISHED,
                        am.AnimCombatable.on_death_anim_finish)
        try:
            red_army_units.remove(self)
        except:
            pass
        try:
            blue_army_units.remove(self)
        except:
            pass

    am.AnimCombatable.on_death = on_death
    am.AnimCombatable.on_death_anim_finish = on_death_anim_finish


def start_war(user, event):

    if event[0] != tc.SDL_SCANCODE_W:
        return

    global war_on, red_army_units, blue_army_units
    if war_on:
        return
    war_on = True

    for unit in red_army_units:
        atk_pos = (-100, 0)
        unit.attack(atk_pos)

    for unit in blue_army_units:
        atk_pos = (+100, 0)
        unit.attack(atk_pos)


def toggle_pause(user, event):

    if event[0] == tc.SDL_SCANCODE_P:
        ss = tc.get_simstate()
        if ss == tc.G_RUNNING:
            tc.set_simstate(tc.G_PAUSED_UI_RUNNING)
        else:
            tc.set_simstate(tc.G_RUNNING)


setup_scene()
setup_armies()
fixup_anim_combatable()

perf_stats_win = psw.PerfStatsWindow()
perf_stats_win.show()

tc.register_event_handler(tc.SDL_KEYDOWN, start_war, None)
tc.register_ui_event_handler(tc.SDL_KEYDOWN, toggle_pause, None)
