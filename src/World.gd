extends Node

# Declare member variables here. Examples:
# var a: int = 2
# var b: String = "text"

onready var spider=preload("res://src/Spider.tscn")
onready var sfx=$SFX

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	$Level01.position=Vector2(5,5)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta: float) -> void:
#	pass

func _on_Area2D_body_entered(body: PhysicsBody2D) -> void:
	for n in get_children():
		if n.is_in_group("spider_spawner"):
			spawn_spiders(n.position,10)
			
			
func spawn_spiders(position, amount):
	print("spawning " + str(amount) + " spiders at " + str(position.x) + "," + str(position.y))
	var audio = sfx.get_node("MammySpawn")
	audio.play()
	for s in range(amount):
		var new_spider = spider.instance()
		add_child(new_spider)
		new_spider.position=position
