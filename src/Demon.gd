extends StaticBody2D

func _ready() -> void:
	pass


func _on_Awareness_body_exited(body: PhysicsBody2D) -> void:
	$AnimatedSprite.play("idle")


func _on_Awareness_body_entered(body: PhysicsBody2D) -> void:
	$AnimatedSprite.play("active")
