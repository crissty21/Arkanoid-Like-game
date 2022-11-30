#include "Ball.h"

Ball::Ball()
{
	Actor(new Vector2d());
}

Ball::Ball(Vector2d* location)
{
	Actor(new Vector2d(location->X, location->Y));
}

void Ball::BeginPlay()
{
	//called when the game starts
	Actor::BeginPlay();
	Time = -10000;
	LocationRelToTile = new Vector2d;
	Speed = new Vector2d(500, 500);
	Player = new Pawn;
	Direction = new Vector2d;
	hitNormal = new Vector2d;
	CanDestroy = false;
	IgnorePlayer = true;
	MoveWithTile = true;
}


void Ball::Tick(float DeltaTime)
{
	Actor::Tick(DeltaTime);
	//if we need to move with the paddle, then we set the location relative to it
	if (MoveWithTile == true)
	{
		SetLocation(new Vector2d(Player->GetX() + LocationRelToTile->X, Player->GetY() + LocationRelToTile->Y));
	}
	//otherwise we set it relative to last position
	else
	{
		//we use PersonalDeltaTime to make the movement frame independent
		SetRelativeLocation(new Vector2d(Direction->X * Speed->X * PersonalDeltaTime, Direction->Y * Speed->Y * PersonalDeltaTime));
	}
	if (CanDestroy == false)
	{
		if (getTickCount() - Time > 10)
			//reset the ability to destroy blocks
			CanDestroy = true;
	}
	if (GetY() > Player->KillingZone)
	{
		//destroy this ball
		TickingActorsRef->push_back(this);
		//setSpriteSize(Mesh, 1, 1);
	}
}

void Ball::IsAtEdge(int edgeCount)
{
	//this is an event called from Actor Class when we hit an edge
	// edgeCount is the index of that edge
	
	//determinate edge normal

	if (IgnoreHitEdge == edgeCount)return;

	Vector2d* normal = new Vector2d(0,0);
	switch (edgeCount)
	{
	case 1:
		normal->Y = 1;
		break;
	case 2:
		normal->X = -1;
		break;
	case 3:
		normal->Y = -1;
		break;
	case 4:
		normal->X = 1;
		break;
	default:
		break;
	}
	
	//get reflection vector from that normal
	// d = d - (d * n * 2) * n
	Direction = Direction->Add((normal->Multiply(Direction->Dot(normal) * 2))->Negate())->Normalize();
	IgnoreHitEdge = edgeCount;
	if (IgnorePlayer)IgnorePlayer = false;
}


bool FindPoint(int x1, int y1, int x2, int y2, Vector2d* point)
{
	//check to see if a given point is inside a rectangle
	return	point->X > x1 && 
			point->X < x2 &&
			point->Y > y1 &&
			point->Y < y2;
}
bool Ball::IsTouching(Actor* otherActor)
{
	//this function checks if the ball is touching the other actor
	//if the ball can't destry, then we exit the function
	if (CanDestroy == false) return false;

	bool hit = false;

	//ball center
	int centerX = GetX() + R;
	int centerY = GetY() + R;

	//first we do a simple and cheap rectangle to rectangle collision detection
	if (Actor::IsTouching(otherActor))
	{
		//if we are touching the other actor, we do a more complex and more expensive circle collision detection
		float lineX = centerX, 
			  lineY = centerY;

		//we need first to determine which side of the rectangle is closer to us
		if (centerX < otherActor->GetX())
		{
			//left side
			lineX = otherActor->GetX();
		}
		else if (centerX > otherActor->GetX() + otherActor->GetWidth())
		{
			//right side
			lineX = otherActor->GetX() + otherActor->GetWidth();
		}

		if (centerY < otherActor->GetY())
		{
			//top side
			lineY = otherActor->GetY();
		}
		else if (centerY > otherActor->GetY() + otherActor->GetHeight())
		{
			//bottom side
			lineY = otherActor->GetY() + otherActor->GetHeight();
		}

		//we get the distances to corners
		float distX = centerX - lineX;
		float distY = centerY - lineY;
		//and then the distance to the edge
		float distance = sqrt(distX * distX + distY * distY);

		//if the distance to the edge is less or equal to radius, then we have a hit
		if (distance <= R)
		{
			hit = true;
		}
	}

	if (hit)
	{
		if (IgnorePlayer)
		{
			if (otherActor == Player)
			{
				return false;
			}
			else
			{
				IgnorePlayer = false;
			}
		}
		else
		{
			if (otherActor == Player)
			{
				if (Player->getReadyToCatchTheBall)
				{
					//if we made it here, we won
					//reset Game
					Player->GameOver = true;

				}
				IgnorePlayer = true;
				//there is a 15% change that the ball will get stuck on paddle
				if (rand() % 100 < 15)
				{
					MoveWithTile = true;
					LocationRelToTile->X = GetX() - Player->GetX();
					LocationRelToTile->Y = -GetHeight();
				}
			}
		}
		//reset hit edge ignore
		IgnoreHitEdge = 0;

		Tile* otherActorAsTile = dynamic_cast<Tile*>(otherActor);
		if (otherActorAsTile != nullptr)
		{
			//destroy the tile
			otherActorAsTile->Disapear();
		}
		//disable for a brief period of time the ability to destroy other tiles
		// we do this to protect the neighboards of the just destroyed tile
		CanDestroy = false;
		Time = getTickCount();
		
		//get hit normal
		hitNormal = GetHitNormal(otherActor);
		//and bounce off that normal
		Bounce(otherActor, hitNormal);
	}
	//return hit result
	return hit;
}
void Ball::Bounce(Actor* OtherObject, Vector2d* hitNormal)
{
	//bounce back from a normal
	//if we hit something
	Vector2d* newDirection = new Vector2d;

	if (OtherObject == Player)
	{
		//first we test to see where we hit the paddle
		// if we didn't hit it on the side
		if (hitNormal->X == 0)
		{
			//if we hit the paddle we will act like we hit a semi-cercle (a moon like shape)
			// we will add a small deviation to left/right
			// we'll take in consideration if the paddle is moving or not
			
			//deviation caused by the movement of paddle
			float movingDir = Player->GetMovingDir() * MovingDirectionInfluence;

			//deviation caused by the position where we hit the paddle
			float paddleCenterX = Player->GetX() + Player->GetWidth() / 2;
			float myCenterX = GetX() + GetWidth() / 2;
			float deviation = myCenterX - paddleCenterX;
			float deltaDeviation = deviation / (Player->GetWidth() / 2);
			//we now have a value between 0 and 1
			deltaDeviation = deltaDeviation * 0.3;

			//we now have a value 0 - +-0.2 (max +-45 deg. deviation)
			hitNormal->X = deltaDeviation + movingDir;
			hitNormal->Y = -1;
		}
		else
		{
			//if we hit the paddle on the side, we'll act like we hit a 45 deg surface
			
			//we shape the edges like a trapezoid 
			/* something like this
				______
			   /	  \
			*/
			//it will still be redered as a rectangle ***
			hitNormal->Y = -1;
			//check on which side we hit
			if (GetX() < Player->GetX())
			{
				hitNormal->X = -1;
			}
			else
			{
				hitNormal->X = 1;
			}
		}
		hitNormal = hitNormal->Normalize();
		
		//if the paddle and the ball are moving in the same directio, we need to make sure that the ball can bounce off the angle we created
		if (hitNormal->X * Direction->X > 0)
		{
			//the best aproach would be to calculate the angle, but we'll just make sure the ball angle isn't to small
			if (Direction->X > 0.94 || Direction->X < -0.94)
			{
				hitNormal->X = 0;
				hitNormal->Y = -1;
			}
		}
	}

	//get reflection Vector
	newDirection = Direction->Add((hitNormal->Multiply(Direction->Dot(hitNormal) * 2))->Negate());


	//we do the following to get the ball unstuck
	if (newDirection->Y < 0.3 && newDirection->Y > -0.3)
	{
		if (newDirection->Y < 0)
		{
			newDirection->Y = -0.3;
		}
		else
		{
			newDirection->Y = 0.3;
		}
		//yes i know that normalization is expensive on processor - sorry
		newDirection = newDirection->Normalize();

	}
	Direction = new Vector2d(newDirection->X,newDirection->Y);
}

Vector2d* Ball::GetHitNormal(Actor* OtherActor)
{
	//this function will return the hit normal between the ball and other actor
	//first we determinate what side of the actor we hit
	//we do that by projecting the combined area on axis, and determining which projection is closer to the maximul projection lenght

	//maximum area projection on X and Y
	float maxH = OtherActor->GetHeight() + GetHeight();
	float maxW = OtherActor->GetWidth() + GetWidth();

	//Actual Projection
	float actualH, actualW;
	float deltaH, deltaW;
	
	bool up, left;
	if (GetY() > OtherActor->GetY())
	{
		//we are under
		actualH = GetY() - OtherActor->GetY() + GetHeight();
		up = false;
	}
	else
	{
		//we are above
		actualH = OtherActor->GetY() - GetY() + OtherActor->GetHeight();
		up = true;
	}
	deltaH = maxH - actualH;

	if (GetX() > OtherActor->GetX())
	{
		//we are to the right
		actualW = GetX() - OtherActor->GetX() + GetWidth();
		left = false;
	}
	else
	{
		//we are to the left
		actualW = OtherActor->GetX() - GetX() + OtherActor->GetWidth();
		left = true;
	}
	deltaW = maxW - actualW;

	if (deltaH < deltaW)
	{
		if (up) return new Vector2d(0, -1);
		else return new Vector2d(0, 1);
	}
	else
	{
		if (left) return new Vector2d(-1, 0);
		else return new Vector2d(1, 0);
	}

	return new Vector2d;
}

void Ball::LockBall()
{
	MoveWithTile = true;
	LocationRelToTile = new Vector2d(GetX() - Player->GetX(), GetY() - Player->GetY());
}


void Ball::Clicked(FRMouseButton button, bool isRelease, Vector2d* mousePosition)
{
	if (button == FRMouseButton::LEFT && isRelease)
	{
		if (MoveWithTile)
		{
			IgnorePlayer = true;
			MoveWithTile = false;
			//set Direction towards the mouse cursor
			Direction = Location->GetLookAtPosition(mousePosition);
			//if the angle is less than 18 deg we set it to 18 deg
			//the vector (0.95, 0.32) is a 18 deg normalized vector;
			if (Direction->Y > -0.32)
			{
				Direction->Y = -0.32;
				if (Direction->X > 0)
				{
					Direction->X = 0.95;
				}
				else
				{
					Direction->X = -0.95;
				}
			}
		}
	}	
}

void Ball::SetSpeed(Vector2d* newSpeed)
{
	Speed->X = newSpeed->X;
	Speed->Y = newSpeed->Y;
}

void Ball::SetPawn(Pawn* Papa)
{
	//set Player Pawn ref
	Player = Papa;
	LocationRelToTile = new Vector2d(GetX() - Papa->GetX(), GetY() - Papa->GetY());
}

Vector2d* Ball::GetDirection()
{
	return Direction;
}

void Ball::SetMesh(const char path[])
{
	Actor::SetMesh(path);
	int width;
	getSpriteSize(Mesh, R, width);
	//radius 
	R = R / 2;
}

void Ball::SetMesh(Sprite* mesh)
{
	Actor::SetMesh(mesh);
	int width;
	getSpriteSize(Mesh, R, width);
	//radius 
	R = R / 2;
}

void Ball::SetDirection(Vector2d* newDirection)
{
	Direction = newDirection->Normalize();
}