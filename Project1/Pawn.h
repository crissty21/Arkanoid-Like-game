#pragma once
#include "Actor.h"
#include <iostream>

enum State
{
	Pressed,
	Hold,
	Release
};

class  Pawn: public Actor
{
private:
	float movingDir ;
	float MovingSpeed;
public:
	bool getReadyToCatchTheBall = false;
	//change this variable to true to end the game;
	bool GameOver = false;
	int KillingZone;
	Pawn();
	Pawn(Vector2d* location);
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void ReactToInput(FRKey key, State state);
	void AdjustMovingSpeed(float Raport);
	float GetMovingDir();
};
