#pragma once
#include "Tile.h"
#include "Pawn.h"

enum BallSize
{
	Small,
	Medium,
	Large
};
class Ball : public Actor
{
private:
	
	//radius
	int R;
	//initial ball Speed on X and y axis
	Vector2d* Speed = nullptr;

	//relative Location to paddle
	Vector2d* LocationRelToTile = nullptr;
	//ref to players paddle (players  Pawn)
	Pawn* Player = nullptr;
	//last hit edge that we should ignore at edge collision detection
	int IgnoreHitEdge = 0;
	//how much is the moving of the platform influencing the direction of the ball
	const float MovingDirectionInfluence = 0.15;
	//moving Direction (normalized)
	Vector2d* Direction = nullptr;

	Vector2d* hitNormal = nullptr;

	//if the ball is allowed to destry tiles
	bool CanDestroy;
	//time stamp of when the canDestroy variable was changed 
	float Time;
	//to ignore the player at the start of the game
	bool IgnorePlayer;

public:
	//if the ball should move with the paddle
	bool MoveWithTile = true;
	//ref to ticking actors list
	std::list<Actor*>* TickingActorsRef;


	Ball();
	Ball(Vector2d* location);

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	//detect if we are touching another actor
	virtual bool IsTouching(Actor* OtherObject) override;
	virtual void SetMesh(const char path[]) override;
	virtual void SetMesh(Sprite* mesh) override;
	//event called form Parent class to notify us that we are at worlds edge
	//and at which one
	virtual void IsAtEdge(int edgeCount) override;
	//bounce from a supraface
	void Bounce(Actor* OtherObject, Vector2d* Normal);
	//Block ball to paddle and move with it
	void LockBall();
	//determinate desired supraface normal of an actor
	Vector2d* GetHitNormal(Actor* OtherActor);
	//event called when the mouse is clicked
	void Clicked(FRMouseButton button, bool isRelease, Vector2d* mousePosition);
	void SetDirection(Vector2d* newDirection);
	void SetSpeed(Vector2d* newSpeed);
	void SetPawn(Pawn* Papa);
	Vector2d* GetDirection();
};