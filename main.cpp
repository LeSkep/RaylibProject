#include "raylib.h"
#include <iostream>

enum PlayerState
{
	IDLE,
	WALKING,
	RUNNING
};

struct Player {
	Vector2 position;
	float speed;
	float runSpeedMultiplier;
	Texture2D playerIdleTexture; 
	Texture2D playerWalkTexture;
	Texture2D playerRunTexture; 
	float SPRITE_WIDTH; 
	float SPRITE_HEIGHT; 

	PlayerState playerState = IDLE;
	bool isMoving;
	bool isFacingLeft;
}; 



void updatePlayerMovement(Player& player, float dt, Rectangle& sourceRect, int& currentFrame, int&frameCounter, int frameSpeed, int idleFrameSpeed)
{
	float dx = 0.0f;
	float dy = 0.0f;

	// Collect input
	if (IsKeyDown(KEY_W)) dy -= 1.0f;
	if (IsKeyDown(KEY_S)) dy += 1.0f;
	if (IsKeyDown(KEY_A)) dx -= 1.0f;
	if (IsKeyDown(KEY_D)) dx += 1.0f;

	// Normalize diagonal movement so speed is consistent
	if (dx != 0 && dy != 0)
	{
		float length = sqrtf(dx * dx + dy * dy);
		dx /= length;
		dy /= length;
	}

	bool isMoving = (dx != 0 || dy != 0); 
	bool isRunning = isMoving && IsKeyDown(KEY_LEFT_SHIFT); 


	if (isRunning) 
	{
		player.playerState = RUNNING;
	}
	else if (isMoving) 
	{
		player.playerState = WALKING; 
	}
	else
	{
		player.playerState = IDLE; 
	} 

	float currentSpeed = player.speed; 

	if (player.playerState == RUNNING) { 
		currentSpeed = player.speed * player.runSpeedMultiplier;   // This is the key line 
	} 

	// Apply movement
	player.position.x += dx * currentSpeed * dt;   
	player.position.y += dy * currentSpeed * dt;  

	// Set facing direction
	if (dx < 0) player.isFacingLeft = true; 
	else if (dx > 0) player.isFacingLeft = false; 

	if (player.playerState == WALKING || player.playerState == RUNNING) 
	{
		frameCounter++; 

		if (frameCounter >= (60 / frameSpeed)) 
		{ 
			frameCounter = 0; 
			if (player.playerState == WALKING)
			{
				currentFrame = (currentFrame + 1) % 4; 
				sourceRect.x = currentFrame * player.SPRITE_WIDTH; 
			}
				
			else if (player.playerState == RUNNING) 
			{
				currentFrame = (currentFrame + 1) % 5;
				sourceRect.x = currentFrame * player.SPRITE_WIDTH; 
			}
			
		}
		// Choose sprite row based on direction
		if (dy < 0 && dx == 0)           // Up 
			sourceRect.y = player.SPRITE_HEIGHT * 4; 
		else if (dy > 0 && dx == 0)      // Down 
			sourceRect.y = 0.0f; 
		else if (dx < 0 && dy == 0)      // Left 
			sourceRect.y = player.SPRITE_HEIGHT * 2; 
		else if (dx > 0 && dy == 0)      // Right 
			sourceRect.y = player.SPRITE_HEIGHT * 2; 
		else if (dy < 0 && dx < 0)       // Up-Left 
			sourceRect.y = player.SPRITE_HEIGHT * 3; 
		else if (dy < 0 && dx > 0)       // Up-Right 
			sourceRect.y = player.SPRITE_HEIGHT * 3; 
		else if (dy > 0 && dx < 0)       // Down-Left 
			sourceRect.y = player.SPRITE_HEIGHT * 1; 
		else if (dy > 0 && dx > 0)       // Down-Right 
			sourceRect.y = player.SPRITE_HEIGHT * 1;  
	}
	else if (player.playerState == IDLE)
	{
		frameCounter++; 

		if (frameCounter >= (60 / idleFrameSpeed))  
		{  
			frameCounter = 0; 
			currentFrame = (currentFrame + 1) % 4;
			sourceRect.x = currentFrame * player.SPRITE_WIDTH;
		} 
	}

} 


int main()
{

	const int screenWidth = 800;
	const int screenHeight = 600;
	 
	InitWindow(screenWidth, screenHeight, "Finally?"); 
	SetTargetFPS(60); 

	Player player; 
	player.position = { screenWidth / 2.0f, screenHeight / 2.0f }; 
	player.speed = 120.0f; 
	player.runSpeedMultiplier = 1.4f;
	player.playerWalkTexture = LoadTexture("./16x16/16x16 Walk-Sheet.png");
	player.playerIdleTexture = LoadTexture("./16x16/16x16 Idle-Sheet.png"); 
	player.playerRunTexture = LoadTexture("./16x16/16x16 Run-Sheet.png"); 
	player.SPRITE_WIDTH = 24.0f; 
	player.SPRITE_HEIGHT = 24.0f; 
	player.isFacingLeft = false;

	

	// Defining the origin point on the sprite sheet to draw the correct sprite  
	Rectangle sourceRect = { 0, 0, player.SPRITE_WIDTH, player.SPRITE_HEIGHT }; 

	int frameCounter = 0; 
	int currentFrame = 0; 
	int frameSpeed = 6; 
	int idleFrameSpeed = 4;
	
	if (IsTextureValid(player.playerIdleTexture) && IsTextureValid(player.playerWalkTexture) && IsTextureValid(player.playerRunTexture))
	{
		std::cout << "Texture Loaded Successfully!" << std::endl; 
	}
	else
	{
		std::cout << "Failed to load texture!" << std::endl; 
	} 


	while (!WindowShouldClose()) 
	{ 
		float dt = GetFrameTime();  
		
		// Function to update the player movement based on input and dt 
		updatePlayerMovement(player, dt, sourceRect, currentFrame, frameCounter, frameSpeed, idleFrameSpeed);

		BeginDrawing();
		// Clearing background before drawing
		ClearBackground(RAYWHITE);

		// Drawing the fps counter in the top left of the screen
		DrawFPS(10, 10);


		// Defining the destination rectangle to draw the sprite at the player's position with a scale of 2.0f 
		Rectangle destRect = { player.position.x, player.position.y, sourceRect.width * 2.0f, sourceRect.height * 2.0f }; 
		Rectangle drawRect = sourceRect;

		if (player.isFacingLeft) 
		{
			drawRect.width = -drawRect.width; // Flip the sprite horizontally if facing left  
			drawRect.x += sourceRect.width; // Adjust the x position of the source rectangle to account for the flip 
			player.isFacingLeft = true;
		}
		else if (!player.isFacingLeft)
		{
			drawRect.width = player.SPRITE_WIDTH;
			drawRect.x = currentFrame * player.SPRITE_WIDTH; // Reset the x position of the source rectangle to the current frame if facing right 
			player.isFacingLeft = false; 
		}
		
		

		// Drawing the player texture at the player's position with the specified sprite width and height  
		if (player.playerState == IDLE)
		{
			DrawTexturePro(player.playerIdleTexture, drawRect, destRect, Vector2{ destRect.width / 2.0f, destRect.height / 2.0f }, 0.0f, WHITE);
		}
		else if (player.playerState == WALKING)
		{
			DrawTexturePro(player.playerWalkTexture, drawRect, destRect, Vector2{ destRect.width / 2.0f, destRect.height / 2.0f }, 0.0f, WHITE); 
		}
		else if (player.playerState == RUNNING)
		{
			DrawTexturePro(player.playerRunTexture, drawRect, destRect, Vector2{ destRect.width / 2.0f, destRect.height / 2.0f }, 0.0f, WHITE); 
		}
		
		

		DrawText("Finally, light", 10, 30, 30, LIME); 

		EndDrawing();
	}
	// Unloading the texture so it doesn't stay loaded in memory causing performance issues 
	UnloadTexture(player.playerWalkTexture);  
	UnloadTexture(player.playerIdleTexture); 
	CloseWindow();
	

	return 0;

}





