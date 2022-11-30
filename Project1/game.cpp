
#include "Ball.h"


class World : public Framework {

private:
	//a list with up to 2 keys that can be pressed at the same time
	std::list<FRKey>* PressedKeys = new std::list<FRKey>;
	//a list with all actors 
	//we use it to make them tick
	std::list<Actor*>* TickingActors = new std::list<Actor*>;

	Vector2d* MouseCoord = new Vector2d;

	//paddle Object (Player)
	Pawn* PlayerPawn = new Pawn();
	//a list with all the balls in the world (try not to laugh)
	std::list<Ball*>* Balls = new std::list<Ball*>;


	//variables used to set screen Dim from console
	unsigned int ScreenWidth = 800, ScreenHeight = 600;
	//variables used for adjusting positions and scales 
	Vector2d* ScaleRaport = new Vector2d;

	float InitialBallSpeed;

	//variable used to determine DeltaTime (time between Frames) in order to make the game frame independent
	float LastFrameTime;

public:

	//a list we all the actors that need to be remove at the end of this frame
	std::list<Actor*>* TickingActorsToRemove = new std::list<Actor*>;
	//the number of tiles that can be destroyed 
	int TilesThatCanBeDestroyed;
	int Points;

	bool WeWantToContinue = false;

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = ScreenWidth;
		height = ScreenHeight;
		fullscreen = false;
	}

	virtual bool Init() {

		//initialize random seed for time
		srand(time(NULL));

		LastFrameTime = 0;
		Points = 0;

		//calculating the raports of scale
		ScaleRaport->X = ScreenWidth / 800.f;
		ScaleRaport->Y = ScreenHeight / 600.f;
		InitialBallSpeed = 400;
		TilesThatCanBeDestroyed = 0;


		Ball* ball = new Ball();

		//add actors to ticking list
		TickingActors->push_back(PlayerPawn);
		TickingActors->push_back(ball);

		PlayerPawn->BeginPlay();
		PlayerPawn->SetMesh(".\\data\\Tiles\\Tile-1.png");
		PlayerPawn->ResizeMesh(ScaleRaport->Multiply(0.2));
		PlayerPawn->AdjustMovingSpeed(ScaleRaport->X);
		//half on width
		//10% on hight
		PlayerPawn->SetLocation(new Vector2d(ScreenWidth / 2 - PlayerPawn->GetWidth() / 2, ScreenHeight - ScreenHeight * 0.1));
		PlayerPawn->KillingZone = PlayerPawn->GetY() + PlayerPawn->GetHeight();

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				Tile* tile;
				if (i == 0)
				{
					tile = new Tile(9);
					tile->SetCanBeDestroyed(false);
				}
				else
				{
					tile = new Tile;
					TilesThatCanBeDestroyed++;
				}
				tile->RefToPoins = &Points;
				tile->BeginPlay();
				tile->ResizeMesh(ScaleRaport->Multiply(0.15));
				tile->SetMeshScaleResize(ScaleRaport->Multiply(0.15));
				tile->TilesLeft = &TilesThatCanBeDestroyed;
				tile->TickingActorsRef = TickingActorsToRemove;
				
				int coordX, coordY;
				//%10 leave empty
				coordX = ScreenWidth * 0.1;
				coordX += j * tile->GetWidth();
				coordY = ScreenHeight * 0.25;
				coordY += i * tile->GetHeight();
				tile->SetLocation(new Vector2d(coordX, coordY));

				TickingActors->push_back(tile);
			}
		}
		
		ball->BeginPlay();
		ball->SetMesh(".\\data\\Balls\\Ball-Medium.png");
		ball->SetSpeed(ScaleRaport->Multiply(InitialBallSpeed));
		ball->SetMeshScaleResize(ScaleRaport->Multiply(0.9));
		ball->ResizeMesh(ScaleRaport->Multiply(0.9));
		ball->SetLocation(new Vector2d(PlayerPawn->GetX() + PlayerPawn->GetWidth() / 2 - ball->GetWidth() / 2, PlayerPawn->GetY() - ball->GetHeight()));
		ball->SetPawn(PlayerPawn);
		ball->TickingActorsRef = TickingActorsToRemove;
		Balls->push_back(ball);

	
		std::list<Actor*>::iterator it;
		for (it = TickingActors->begin(); it != TickingActors->end(); ++it)
		{
			(*it)->SetRaport(ScaleRaport);
		}
		return true;
	}

	virtual void Close() {
		WeWantToContinue = PlayerPawn->GameOver;
	}

	virtual bool Tick() {
		//calculate DeltaTime
		float currentFrameTime = getTickCount();
		float DeltaTime = currentFrameTime - LastFrameTime;
		LastFrameTime = currentFrameTime;
		//Transform form milliseconds to seconds
		DeltaTime = DeltaTime / 1000;

		bool thereAreTilesLeft = false;

		std::list<Actor*>::iterator it;
		std::list<Ball*>::iterator ball;
		for (it = TickingActors->begin(); it != TickingActors->end(); ++it)
		{
			(*it)->Tick(DeltaTime);

			//if it is a ball, we continue
			if (dynamic_cast<Ball*>(*it))continue;
			if (!thereAreTilesLeft && dynamic_cast<Tile*>(*it))thereAreTilesLeft = true;

			//otherwise we check colisions with the balls
			for (ball = Balls->begin(); ball != Balls->end(); ++ball)
			{
				//I laughed way to much at this line of code... 
				(*ball)->IsTouching(*it);
			}
		}
		if (thereAreTilesLeft == false)
		{
			PlayerPawn->getReadyToCatchTheBall = true;
		}
		if (!PressedKeys->empty())
		{
			//we send all pressed keys to the Player pawn to deal with them as it pleases
			std::list<FRKey>::iterator it;
			for (it = PressedKeys->begin(); it != PressedKeys->end(); ++it) {
				PlayerPawn->ReactToInput(*it, State::Hold);
			}
		}
		//we remove for TickingActors all actors that were deleted this tick
		for (it = TickingActorsToRemove->begin(); it != TickingActorsToRemove->end(); ++it)
		{
			TickingActors->remove(*it);
			Ball* itAsBall = dynamic_cast<Ball*>(*it);
			if (itAsBall)
			{
				Balls->remove(itAsBall);
				if (Balls->empty())
				{
					PlayerPawn->GameOver = true;
					return true;
				}
			}
		}
		TickingActorsToRemove->clear();
		return PlayerPawn->GameOver;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		//allways know where the mouse is
		MouseCoord->X = x;
		MouseCoord->Y = y;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		//we send the click event to all balls 
		std::list<Ball*>::iterator ball;
		for (ball = Balls->begin(); ball != Balls->end(); ++ball)
		{
			(*ball)->Clicked(button, isReleased, MouseCoord);
		}
		if (button == FRMouseButton::RIGHT && isReleased)
		{
			if (Points >= 20)
			{
				if (rand() % 2 == 0)
				{
					TeleportBall();
				}
				else
				{
					MakeThree();
				}
			Points -= 20;
			}
		}
	}

	virtual void onKeyPressed(FRKey k) {
		//if we have less than 2 keys pressed we add current key to the list of pressed keys
		if (PressedKeys->size() < 2)
		{
			PressedKeys->push_back(k);
		}
	}

	virtual void onKeyReleased(FRKey k) {
		//we remove this key from the list 
		PressedKeys->remove(k);
		PlayerPawn->ReactToInput(k, State::Release);
	}

	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}

	void TeleportBall()
	{
		int chosenBall = rand() % Balls->size();
		std::list<Ball*>::iterator itBall;
		for (itBall = Balls->begin(); itBall != Balls->end(); ++itBall)
		{
			if (chosenBall == 0)
			{
				Vector2d* newLocation = new Vector2d;
				newLocation->X = rand() % ScreenWidth;
				newLocation->Y = rand() % (ScreenHeight / 2);
				(*itBall)->SetLocation(newLocation);
				//we have a random location, now we need to test if it is free 
				bool isFree = false;
				//random value -1 or 1
				int directionToChech = rand() % 2;
				if (directionToChech == 0)directionToChech--;

				std::list<Actor*>::iterator it;
				while (!isFree)
				{
					isFree = true;
					for (it = TickingActors->begin(); it != TickingActors->end(); ++it)
					{
						//if it is a ball, we continue
						if (dynamic_cast<Ball*>(*it))continue;
						if ((*itBall)->Actor::IsTouching(*it))
						{
							isFree = false;
							break;
						}
					}
					if (!isFree)
					{//																	  this will make sure we'll allways find a position
						(*itBall)->SetRelativeLocation(new Vector2d(directionToChech*20, directionToChech/10));
					}
				}
				return;
			}
			else
			{
				chosenBall--;
			}
		}
	}

	void MakeThree()
	{
		//function called when we activate the ability make three
		Ball* ball = nullptr; ball = new Ball();
		std::list<Ball*>* TemporaryBalls = new std::list<Ball*>;
		std::list<Ball*>::iterator itBall;
		for (itBall = Balls->begin(); itBall != Balls->end(); ++itBall)
		{
			for (int i = 0; i < 2; i++) 
			{
				//each ball we split in 3 (we create 2 more)
				ball = new Ball();
				ball->BeginPlay();
				//we get the image from the main ball
				ball->SetMesh((*itBall)->GetMesh());
				ball->SetSpeed(ScaleRaport->Multiply(InitialBallSpeed));
				ball->SetMeshScaleResize(ScaleRaport->Multiply(0.9));
				ball->SetLocation((*itBall)->GetWorldLocation());
				ball->SetPawn(PlayerPawn);
				ball->MoveWithTile = false;
				ball->TickingActorsRef = TickingActorsToRemove;

				//we set its direction with a 30deg deviation 
				//here we don't care if it gets stuck
			
				Vector2d* futureLocation = new Vector2d;
				futureLocation->X = (*itBall)->GetDirection()->X;
				futureLocation->Y = (*itBall)->GetDirection()->Y;
				
				float angle = 0.524; //30 deg in rad

				//we rotate the vector using the euclidian rotation alg 
				if (i == 1)
					angle = -angle;

				float cs = std::cos(angle);
				float sn = std::sin(angle);
				
				int direction = 1;

				float px = futureLocation->X * cs - futureLocation->Y * sn;
				float py = futureLocation->X * sn + futureLocation->Y * cs;

				futureLocation->X = px;
				futureLocation->Y = py;

				ball->SetDirection(futureLocation->Normalize());
				
				TemporaryBalls->push_back(ball);
				TickingActors->push_back(ball);
			}
		}
		for (itBall = TemporaryBalls->begin(); itBall != TemporaryBalls->end(); ++itBall)
		{
			//we add the new created balls to the balls list 
			Balls->push_back(*itBall);
		}
		TemporaryBalls->clear();
	}
	void SetDimensions(int W, int H)
	{
		ScreenWidth = W;
		ScreenHeight = H;
	}
};

int main(int argc, char* argv[])
{
	int w = 800, h = 600;
	World* world = new World;
	do {
		if (argc > 2)
		{
			//if we have commands from command line
			if (strcmp(argv[1], "-window") == 0)
			{
				char firstDim[20] = {};
				strcat_s(firstDim, argv[2]);
				w = atoi(firstDim);
				if (w == 0)
				{
					std::cout << "Invalid Command";
				}
				else
				{
					char secondDim[20] = {};
					strcat_s(secondDim, strchr(argv[2], 'x') + 1);
					h = atoi(secondDim);
					if (h == 0)
					{
						std::cout << "Invalid Command";
					}
				}
			}
		}
		world->SetDimensions(w,h);
		run(world);
	}
	while(world->WeWantToContinue);
}
