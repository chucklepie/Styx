extends KinematicBody2D

onready var last_position = position

const PLAYER_SPEED = 100

var current_animation=""

func _ready() -> void:
	pass # Replace with function body.

func _physics_process(delta: float) -> void:
	MovePlayer(delta)
	
func MovePlayer(delta: float):
	var x = 0
	var y = 0
	
	x = -int(Input.is_action_pressed("ui_left"))
	x += int(Input.is_action_pressed("ui_right"))
	y = int(Input.is_action_pressed("ui_down"))
	y -= int(Input.is_action_pressed("ui_up"))
	
	var dir=Vector2(x,y).normalized()
	if x!=0 && y!=0:
		move_and_slide(dir*PLAYER_SPEED)
	else:
		move_and_collide(dir*delta*PLAYER_SPEED)
		
	if dir.x<0:
		$AnimatedSprite.flip_h=true
	if dir.x>0:
		$AnimatedSprite.flip_h=false
		
	if dir==Vector2.ZERO:
		if current_animation!="idle":
			$AnimatedSprite.play("idle")
	elif current_animation!="walk":
		$AnimatedSprite.play("walk")
		