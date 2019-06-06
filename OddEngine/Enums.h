#pragma once

/*
	Undeclared = I don't know yet
	Input = User input detected
	Timed = Something on a timer or on next game loop
	System = External event triggered by windows (Device attach, Graphics device crash)
	GameObject = Virtual in game object triggered the event system (UI Elements)
*/
enum EventType { Undeclared, Input, Timed, System, GameObject };