#include "Tile.h"

Tile::Tile()
{
	Actor::Actor(new Vector2d);
	//set the image to a random color
	Color = rand() % 9;
}

Tile::Tile(Vector2d* location)
{
	Actor::Actor(location);	
	Color = (rand() % 9);
}

Tile::Tile(int color)
{
	Actor::Actor(new Vector2d);
	Color = color;
}

Tile::Tile(Vector2d* location, int color)
{
	Actor::Actor(location);
	Color = color;
}

void Tile::BeginPlay()
{
	Actor::BeginPlay();
	SetImage(Color);
	visible = true;
}

void Tile::Tick(float DeltaTime)
{
	if (visible)
	{
		Actor::Tick(DeltaTime);
	}
}
void Tile::SetImage(int color)
{
	char aux[] = ".\\data\\Bricks\\";
	char png[] = ".png";
	char path[30] = "";
	strcat_s(path, aux);
	strcat_s(path, colors[color]);
	strcat_s(path, png);
	SetMesh(path);
}
void Tile::Disapear()
{
	//called when we want the tile destroyed
	if (CanBeDestroyed)
	{
		visible = false;
		if (TickingActorsRef == nullptr)
		{
			std::cout << "Null Pointer TickingActorsRef in tile class";
			return;
		}
		TickingActorsRef->push_back(this);
		(*TilesLeft)--;
		(*RefToPoins)++;
	}
	else if (*TilesLeft <= 0)
	{
		SetImage(rand() % 9);
		ResizeMesh(MeshScaleResize);
		CanBeDestroyed = true;
	}
	
}

void Tile::SetCanBeDestroyed(bool newValue)
{
	CanBeDestroyed = newValue;
}

bool Tile::CanItBeDestroyed()
{
	return CanBeDestroyed;
}

bool Tile::IsNextTo(Actor* OtherObject)
{
	//it is a rectangle to rectangle collision detection with one of them enlarged by one pixel on each side

	if (OtherObject == nullptr)return false;
	int rec1[] = {	Location->X - 1,
					Location->Y - 1,
					Location->X + GetWidth() + 1,
					Location->Y + GetHeight() + 1 };

	int rec2[] = {	OtherObject->GetX(),
					OtherObject->GetY(),
					OtherObject->GetX() + OtherObject->GetWidth(),
					OtherObject->GetY() + OtherObject->GetHeight()};

	bool widthIsPositive = Minim(rec1[2], rec2[2]) > Maxim(rec1[0], rec2[0]);
	bool heightIsPositive = Minim(rec1[3], rec2[3]) > Maxim(rec1[1], rec2[1]);

	return (widthIsPositive && heightIsPositive);
}
