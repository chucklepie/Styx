extends KinematicBody2D

onready var last_position = position

const PLAYER_SPEED = 100

var current_animation=""
var beam_duration=10
var cooldown=0.1
var can_shoot=true
var hit=null

func _ready() -> void:
	pass
	#$Laser.remove_point(1)

func _physics_process(delta: float) -> void:
	$Laser.remove_point(1)
	MovePlayer(delta)
	GunCheck()
	
func GunCheck():
	#ensure length is clipped to the nearest collision
	var gunsize
	var hitobject
	var hitpos
	var tilepos
	if $GunRaycast.is_colliding():
		hitpos=$GunRaycast.get_collision_point()
		hitobject=$GunRaycast.get_collider()
		get_node("../Label").text="%s,%s" % [str(hitpos),hitpos]
		$Laser.add_point(transform.xform_inv((hitpos)))
		#$Laser.add_point(hitpos)
	if hitobject is TileMap:
		tilepos=get_parent().get_node("Level01").world_to_map(hitpos)
		print(hitpos-position)
		#print(tilepos)
		#print(hitpos)
	else:
		gunsize=$GunRaycast.get_collision_point()-$GunPosition.position
		print(hitobject.name)
		print(gunsize.length())
	
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
		