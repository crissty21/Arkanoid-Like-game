
#include "Actor.h"

Actor::Actor(Vector2d* location)
{
	Location = location;
}
Actor::Actor()
{
	Actor(new Vector2d(0, 0));
}

void Actor::Tick(float DeltaTime)
{
	PersonalDeltaTime = DeltaTime;
	if (Mesh)
	{
		drawSprite(Mesh, Location->X, Location->Y);
	}
}

void Actor::BeginPlay()
{
	ScaleRaport = new Vector2d;
	MeshScaleResize = new Vector2d(1, 1);
	Location = new Vector2d();
	Size = new Vector2d();
	Normal = new Vector2d();
}

void Actor::SetLocation(Vector2d* newLocation)
{
	if (newLocation == nullptr)
	{
		std::cout << "Error Vector2d null in Actor Class!";
		return;
	}

	if (Size == nullptr)
	{
		UpdateSize();
	}

	int ScreenW, ScreenH;
	getScreenSize(ScreenW, ScreenH);
	//checking to see if we are not at the edge
	if (newLocation->X + Size->X >= ScreenW)
	{
		newLocation->X = ScreenW - Size->X - 1;
		//call event isAtEdge with the edge number 
		IsAtEdge(2);
	}
	else if (newLocation->X < 0)
	{
		newLocation->X = 0;
		IsAtEdge(4);
	}

	if (newLocation->Y + Size->Y >= ScreenH)
	{
		newLocation->Y = ScreenH - Size->Y - 1;
		IsAtEdge(3);
	}
	else if (newLocation->Y < 0)
	{
		newLocation->Y = 0;
		IsAtEdge(1);
	}

	Location->X = newLocation->X;
	Location->Y = newLocation->Y;
}

void Actor::SetRelativeLocation(Vector2d* newLocation)
{	
	if (newLocation == nullptr)
	{
		std::cout << "Error Vector2d null in Actor Class!";
		return;
	}
	SetLocation(new Vector2d(Location->X + newLocation->X,Location->Y + newLocation->Y));
}


void Actor::UpdateSize()
{
	if (Mesh == nullptr)
	{
		std::cout << "Error null Mesh in Actor class!";
		return;
	}
	int w, h;
	getSpriteSize(Mesh, w, h);
	Size->X = w;
	Size->Y = h;
}

void Actor::SetMesh(const char path[])
{
	//create a new mesh from a path 
	Mesh = createSprite(path);
	if (Mesh == nullptr)
	{
		//we do this in case we can't open the file
		char pathAltered[50] = ".";
		strcat_s(pathAltered, path);
		Mesh = createSprite(pathAltered);
		if (Mesh == nullptr)
		{
			std::cout << "Error creating Mesh in Actor class!";
			return;
		}
	}
	int w, h;
	getSpriteSize(Mesh, w, h);
	Size = new Vector2d(w, h);
}



void Actor::ResizeMesh(Vector2d* raport)
{
	if (Mesh == nullptr)
	{
		std::cout << "Error null Mesh in Actor class!";
		return;
	}
	int w, h;
	getSpriteSize(Mesh, w, h);

	w = w * raport->X;
	h = h * raport->Y;
	Size->X = w;
	Size->Y = h;

	setSpriteSize(Mesh, w, h);
}

bool Actor::IsTouching(Actor* OtherObject)
{
	//checking colision between two rectangles 
	int rec1[] = { Location->X,
				   Location->Y,
				   Location->X + GetWidth(),
				   Location->Y + GetHeight() };
	int rec2[] = { OtherObject->Location->X,
				   OtherObject->Location->Y,
				   OtherObject->Location->X + OtherObject->GetWidth(),
				   OtherObject->Location->Y + OtherObject->GetHeight() };

	bool widthIsPositive = Minim(rec1[2], rec2[2]) > Maxim(rec1[0], rec2[0]);
	bool heightIsPositive = Minim(rec1[3], rec2[3]) > Maxim(rec1[1], rec2[1]);

	return (widthIsPositive && heightIsPositive);
}

void Actor::SetMeshScaleResize(Vector2d* raport)
{
	MeshScaleResize->X = raport->X;
	MeshScaleResize->Y = raport->Y;
}

void Actor::SetRaport(Vector2d* raport)
{
	ScaleRaport->X = raport->X;
	ScaleRaport->Y = raport->Y;
}

Sprite* Actor::GetMesh()
{
	return Mesh;
}

int Actor::GetWidth()
{
	return Size->X;
}

int Actor::GetHeight()
{
	return Size->Y;
}

void Actor::IsAtEdge(int edgeCount){}

int Actor::GetX()
{
	return Location->X;
}

int Actor::GetY()
{
	return Location->Y;
}

Vector2d* Actor::GetWorldLocation()
{
	return Location;
}

Vector2d* Actor::GetSize()
{
	return Size;
}

void Actor::SetMesh(Sprite* mesh)
{
	//setting the mesh to a mesh allready created
	if (mesh == nullptr)
	{
		std::cout << "Error null Mesh in Actor class!";
		return;
	}
	Mesh = mesh;
	int w, h;
	getSpriteSize(Mesh, w, h);
	Size = new Vector2d(w, h);
}

int Actor::Minim(int one, int two)
{
	if (one < two)
		return one;
	else return two;
}

int Actor::Maxim(int one, int two)
{
	if (one > two)
		return one;
	else return two;
}
