#pragma once

#include "Framework.h"
#include "Vector2d.h"
#include <list>
#include <cstdlib>
#include <iostream>

class Actor {
private:
	//Window scale Raport
	Vector2d* ScaleRaport = nullptr;
protected:
	float PersonalDeltaTime;
	//resize mesh to this scales
	Vector2d* MeshScaleResize = nullptr;

	Vector2d* Location = nullptr;
	Vector2d* Size = nullptr;
	Vector2d* Normal = nullptr;

	//the sprite to be drawn in world
	Sprite* Mesh =nullptr;
public:
	 
	Actor(Vector2d* location);
	Actor();
	virtual void Tick(float DeltaTime);
	virtual void BeginPlay();
	virtual void SetMesh(Sprite* mesh);
	//returns true if this actor is touching the other actor
	virtual bool IsTouching(Actor* OtherObject);
	virtual void SetMesh(const char path[]);
	//get the mesh pointer
	Sprite* GetMesh();

	//event callend when an actor is at worlds edge
	//it returns hit normal with that edge
	//edge Count:
	/*	1
	  4	  2
		3 */
	virtual void IsAtEdge(int edgeCount);

	//returns for Location
	int GetX();
	int GetY();
	Vector2d* GetWorldLocation();
	//seters for Location
	void SetLocation(Vector2d* newLocation);
	void SetRelativeLocation(Vector2d* newLocation);

	//returns for Size
	int GetWidth();
	int GetHeight();
	Vector2d* GetSize();

	//working with size
	void UpdateSize();
	void ResizeMesh(Vector2d* raport);
	void SetMeshScaleResize(Vector2d* raport);
	void SetRaport(Vector2d* raport);

	int Minim(int a, int b);
	int Maxim(int a, int b);
};