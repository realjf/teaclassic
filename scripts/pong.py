
import tc
import math
import traceback

############################################################
# Debugging setup                                          #
############################################################


def on_task_exc(user, event):
    traceback.print_exception(event[1], event[2], event[3])


tc.register_event_handler(tc.EVENT_SCRIPT_TASK_EXCEPTION, on_task_exc, None)

############################################################
# Global configs                                           #
############################################################

tc.load_map("assets/maps", "plain.tcmap")
tc.set_ambient_light_color((1.0, 1.0, 1.0))
tc.set_emit_light_color((1.0, 1.0, 1.0))
tc.set_emit_light_pos((1664.0, 1024.0, 384.0))
tc.disable_fog_of_war()
tc.set_minimap_size(0)
tc.disable_unit_selection()

############################################################
# Camera setup                                             #
############################################################

pong_cam = tc.Camera(mode=tc.CAM_MODE_FREE, position=(
    0.0, 175.0, 0.0), pitch=-90.0, yaw=180.0)
tc.set_active_camera(pong_cam)

############################################################
# Entities setup                                           #
############################################################

FIELD_WIDTH = 240
FIELD_HEIGHT = 120
PADDLE_HEIGHT = 16
PLAYER_SPEED = 2.5
COMPUTER_SPEED = 2.5
BALL_SPEED = 5.0

obelisks = []

# top, bot border
for i in range(-FIELD_WIDTH/2, FIELD_WIDTH/2, 4):
    top = tc.Entity("assets/models/props", "obelisk_2.tcobj", "border")
    top.pos = (-FIELD_HEIGHT/2.0, 0.0, i)
    obelisks += [top]

    bot = tc.Entity("assets/models/props", "obelisk_2.tcobj", "border")
    bot.pos = (FIELD_HEIGHT/2.0, 0.0, i)
    obelisks += [bot]

# left, right border
for i in range(-FIELD_HEIGHT/2, FIELD_HEIGHT/2, 4):
    left = tc.Entity("assets/models/props", "obelisk_2.tcobj", "border")
    left.pos = (i, 0.0, -FIELD_WIDTH/2.0)
    obelisks += [left]

    right = tc.Entity("assets/models/props", "obelisk_2.tcobj", "border")
    right.pos = (i, 0.0, FIELD_WIDTH/2.0)
    obelisks += [right]

ball = tc.Entity("assets/models/barrel", "barrel.tcobj", "ball")
ball.pos = (0.0, 0.0, 0.0)
ball.scale = (8.0, 8.0, 8.0)

left_paddle = tc.Entity("assets/models/props",
                        "wood_fence_1.tcobj", "left_paddle")
left_paddle.pos = (0.0, 0.0, -FIELD_WIDTH/2.0 * 0.8)
left_paddle.scale = (2.5, 2.5, 2.5)

right_paddle = tc.Entity("assets/models/props",
                         "wood_fence_1.tcobj", "right_paddle")
right_paddle.pos = (0.0, 0.0, FIELD_WIDTH/2.0 * 0.8)
right_paddle.scale = (2.5, 2.5, 2.5)

############################################################
# Gameplay loop                                            #
############################################################

player_score = 0
computer_score = 0


def intersect(ball, paddle):
    if ball.pos[2] < paddle.pos[2] - 3.0:
        return False
    if ball.pos[2] > paddle.pos[2] + 3.0:
        return False
    if ball.pos[0] < paddle.pos[0] - PADDLE_HEIGHT/2.0:
        return False
    if ball.pos[0] > paddle.pos[0] + PADDLE_HEIGHT/2.0:
        return False
    return True


class PlayerPaddleActor(tc.Task):
    def __run__(self):
        while True:
            scancode = self.await_event(tc.SDL_KEYDOWN)[0]
            if scancode == tc.SDL_SCANCODE_UP and left_paddle.pos[0] > -FIELD_HEIGHT/2.0 + PADDLE_HEIGHT/2.0:
                left_paddle.pos = left_paddle.pos[0] - \
                    PLAYER_SPEED, 0.0, left_paddle.pos[2]
            elif scancode == tc.SDL_SCANCODE_DOWN and left_paddle.pos[0] < FIELD_HEIGHT/2.0 - PADDLE_HEIGHT/2.0:
                left_paddle.pos = left_paddle.pos[0] + \
                    PLAYER_SPEED, 0.0, left_paddle.pos[2]


class ComputerPaddleActor(tc.Task):
    def __run__(self):
        while True:
            self.await_event(tc.EVENT_30HZ_TICK)
            if right_paddle.pos[0] > ball.pos[0] and right_paddle.pos[0] > -FIELD_HEIGHT/2.0 + PADDLE_HEIGHT/2.0:
                right_paddle.pos = right_paddle.pos[0] - \
                    COMPUTER_SPEED, 0.0, right_paddle.pos[2]
            if right_paddle.pos[0] < ball.pos[0] and right_paddle.pos[0] < FIELD_HEIGHT/2.0 - PADDLE_HEIGHT/2.0:
                right_paddle.pos = right_paddle.pos[0] + \
                    COMPUTER_SPEED, 0.0, right_paddle.pos[2]


class BallActor(tc.Task):
    def random_vel(self):
        x = 5 + tc.rand(5)
        z = 5 + tc.rand(5)
        mag = math.sqrt(x**2 + z**2)
        return [(x - 10.0) / mag * BALL_SPEED, (z - 10.0) / mag * BALL_SPEED]

    def __init__(self):
        self.velocity = self.random_vel()

    def __run__(self):
        while True:
            self.await_event(tc.EVENT_30HZ_TICK)
            ball.pos = (ball.pos[0] + self.velocity[0],
                        0.0, ball.pos[2] + self.velocity[1])
            if intersect(ball, left_paddle):
                self.velocity[1] *= -1.0
            if intersect(ball, right_paddle):
                self.velocity[1] *= -1.0
            if ball.pos[0] >= FIELD_HEIGHT/2.0 or ball.pos[0] <= -FIELD_HEIGHT/2.0:
                self.velocity[0] *= -1.0
            if ball.pos[2] >= FIELD_WIDTH/2.0:
                global player_score
                player_score += 1
                ball.pos = (0.0, 0.0, 0.0)
                self.velocity = self.random_vel()
            if ball.pos[2] <= -FIELD_WIDTH/2.0:
                global computer_score
                computer_score += 1
                ball.pos = (0.0, 0.0, 0.0)
                self.velocity = self.random_vel()


class UIActor(tc.Task):
    def __run__(self):
        while True:
            global player_score, computer_score
            self.await_event(tc.EVENT_UPDATE_START)
            tc.draw_text("PLAYER: {}, COMPUTER: {}".format(player_score, computer_score),
                         (25, 25, 250, 50), (255, 0, 0, 255))


PlayerPaddleActor().run()
ComputerPaddleActor().run()
BallActor().run()
UIActor().run()
