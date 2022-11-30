#include "Pawn.h"

Pawn::Pawn()
{
	Actor(new Vector2d());
}

Pawn::Pawn(Vector2d* location)
{
	Actor(new Vector2d(location->X, location->Y));
}

void Pawn::BeginPlay()
{
	Actor::BeginPlay();
	movingDir = 0;
	MovingSpeed = 500;
	getReadyToCatchTheBall = false;
	GameOver = false;
}

void Pawn::Tick(float DeltaTime)
{
	Actor::Tick(DeltaTime);
}

void Pawn::ReactToInput(FRKey key, State state)
{
	if (state == Hold)
	{
		if (key == FRKey::RIGHT)
		{
			movingDir = 1;
			//we use PersonalDeltaTime to make the movement frame independent
			SetRelativeLocation(new Vector2d(1 * PersonalDeltaTime * MovingSpeed, 0));
		}
		else if (key == FRKey::LEFT)
		{
			movingDir = -1;
			//we use PersonalDeltaTime to make the movement frame independent
			SetRelativeLocation(new Vector2d(-1 * PersonalDeltaTime * MovingSpeed, 0));
		}
	}
	else if (state == Release)
	{
		movingDir = 0;
	}

}

void Pawn::AdjustMovingSpeed(float Raport)
{
	MovingSpeed = MovingSpeed * Raport;
}

float Pawn::GetMovingDir()
{
	return movingDir;
}

