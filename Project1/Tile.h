#pragma once

#include "Actor.h"

class Tile : public Actor
{
private:

	const char colors[10][10] = { "Blue", "Brown", "Darkgreen","Green","Lightblue","Mangenta","Orange","Red","Yellow","Grey"};
	int Color;
	bool CanBeDestroyed = true;
	bool visible;

public:
	int* TilesLeft;
	int* RefToPoins;
	std::list<Actor*>* TickingActorsRef;
	Tile();
	Tile(int color);
	Tile(Vector2d* location);
	Tile(Vector2d* location, int color);
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay();

	void SetImage(int color);
	void Disapear();
	void SetCanBeDestroyed(bool newValue);
	bool CanItBeDestroyed();
	bool IsNextTo(Actor* OtherActor);
};