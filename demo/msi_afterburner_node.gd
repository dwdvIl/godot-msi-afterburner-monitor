extends MSIAfterburnerNode
## Example script for MSIAfterburnerGodot addon

@export var text1:Label
@export var text2:Label
@export var text3:Label

func _on_timer_timeout() -> void:
	# Poll data
	if not poll():
		print("MSIAfterburner not running")
	
	# Do not process if there's no data
	if data.is_empty():
		return
	
	# Reset texts for display
	text1.text = ""
	text2.text = ""
	text3.text = ""
	
	var index = 0
	for v in data:
		# Switch labels after 32 lines
		var curText:Label
		
		if index < 32:
			curText = text1
		elif index < 64:
			curText = text2
		else:
			curText = text3
		index += 1
		
		# Data visualization
		
		if data[v] is Dictionary:
			# For numeric values:
			# value:float, maxLimit:float, minLimit:float, unit:String
			var value:float = data[v].value
			if value != -1:
				curText.text += v + ": " + str(roundf(value * 10) / 10) + data[v].unit + "\n"
			else:
				# -1 means no data
				curText.text += v + ": " + "<No data>\n"
		else:
			# For string values, such as gpu names
			curText.text += v + ": " + data[v] + "\n"
			
	# Properties can be accessed by their plain name
	print(data[&"CPU usage"].value)
