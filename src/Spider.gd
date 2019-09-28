extends KinematicBody2D

const SPEED = 100
var real_speed = SPEED
var collision : KinematicCollision2D = null
enum {UP,DOWN,LEFT,RIGHT}
var current_direction=-1
var movement = Vector2.ZERO
var dead=false

func _ready() -> void:
	randomize()
	real_speed *= rand_range(0.5,1.5)
	$AnimatedSprite.play("move")
	
func _process(delta: float) -> void:
	if dead:
		return
	if current_direction == -1:
		current_direction = get_new_direction()

	movement.x=0
	movement.y=0
	match current_direction:
		UP:
			movement.y=-1
		DOWN:
			movement.y=1
		LEFT:
			movement.x=-1
		RIGHT:
			movement.x=1
	
	#this is where badness happens
	collision = move_and_collide(movement*delta*real_speed)
	if collision:
		current_direction = -1
	
func get_new_direction():
	var d = randi()%4
	return d

func death():
	dead=true
	$DeathTimer.start()
	$AnimatedSprite.play("dead")

func _on_DeathTimer_timeout() -> void:
	queue_free()
