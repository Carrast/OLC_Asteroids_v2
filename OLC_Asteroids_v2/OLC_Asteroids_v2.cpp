// OLC_Asteroids_v2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <algorithm>

#include "olcConsoleGameEngine.h"

class Asteroids : public olcConsoleGameEngine {
public:
	Asteroids();
	~Asteroids() {}

private:
	struct sStars
	{
		float x;
		float y;
		float dx;
		float dy;
	};

	struct sSpaceObject
	{
		float x;
		float y;
		float dx;
		float dy;
		float angle;
		float scale;
		int nSize;
	};

	std::vector<sStars> vecStars;
	std::vector<sSpaceObject> vecAsteroids;
	sSpaceObject player;
	sSpaceObject throttle;
	std::vector<sSpaceObject> vecBullets;
	
	std::vector<std::pair<float, float>> vecModelShip;
	std::vector<std::pair<float, float>> vecModelThrottle;
	std::vector<std::pair<float, float>> vecModelAsteroids;

	float pi = 3.14159f;
	int nScore;
	int nDifficulty;
	bool bDead = false;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		// background stars
		vecStars.push_back({ 32.0f, 5.0f, -4.5f, 0.0f });
		vecStars.push_back({ 2.0f, 10.0f, -4.5f, 0.0f });
		vecStars.push_back({ 150.0f, 12.0f, -4.5f, 0.0f });
		vecStars.push_back({ 22.0f, 23.0f, -4.5f, 0.0f });
		vecStars.push_back({ 84.0f, 32.0f, -4.5f, 0.0f });
		vecStars.push_back({ 42.0f, 45.0f, -4.5f, 0.0f });
		vecStars.push_back({ 131.0f, 49.0f, -4.5f, 0.0f });
		vecStars.push_back({ 30.0f, 56.0f, -4.5f, 0.0f });
		vecStars.push_back({ 122.0f, 87.0f, -4.5f, 0.0f });
		vecStars.push_back({ 15.0f, 92.0f, -4.5f, 0.0f });
		vecStars.push_back({ 62.0f, 70.0f, -4.5f, 0.0f });

		// ship model
		vecModelShip =
		{
			{0.0f, -5.0f},
			{-2.5, +2.5f},
			{+2.5f, +2.5f}
		};

		// throttle model
		vecModelThrottle =
		{
			{0.0f, 4.4f},
			{-0.8f, +3.0f},
			{+0.8f, +3.0f}
		};

		// asteroid model
		int numSections = 20;
		for (int i = 0; i < numSections; i++)
		{
			float radius = ((float)std::rand() / (float)RAND_MAX) * 0.4f + 0.7f;
			float angle = ((float)i / (float)numSections) * 2.0f * pi;
			vecModelAsteroids.push_back(std::make_pair(radius * cosf(angle), radius * sinf(angle)));
		}

		ResetGame();
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// clear screen, background fill, reset every frame
		Fill(0, 0, m_nScreenWidth, m_nScreenHeight, PIXEL_SOLID, 0);

		// max out difficulty
		if (nDifficulty > 4)
			nDifficulty = 4;

		// background stars are moving
		for (auto& s : vecStars)
		{
			s.x += s.dx * fElapsedTime;
			s.y += s.dy * fElapsedTime;
			WrapCoordinates(s.x, s.y, s.x, s.y);
			Draw(s.x, s.y, PIXEL_SOLID, FG_DARK_GREY);
		}

		// player movement
		// steer
		if (m_keys[VK_LEFT].bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (m_keys[VK_RIGHT].bHeld)
			player.angle += 5.0f * fElapsedTime;

		// thrust
		if (m_keys[VK_UP].bHeld)
		{
			player.dx += sinf(player.angle) * 25.0f * fElapsedTime;
			player.dy += -cosf(player.angle) * 25.0f * fElapsedTime;
		}

		// monitor velocity
		DrawString(2, 2, L"player.dx.: " + std::to_wstring(player.dx));
		DrawString(2, 3, L"player.dy.: " + std::to_wstring(player.dy));

		// give speed to the ship
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		// check collision of ship with asteroids
		for (auto& a : vecAsteroids)
		{
			if (IsPointInCircle(a.x, a.y, (float)a.nSize, player.x, player.y))
			{
				bDead = true;
				ResetGame();
			}
		}

		// bullets
		if (m_keys[VK_SPACE].bReleased)
		{
			vecBullets.push_back({ player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0, 0 });
		}

		// update and draw bullets
		for (auto& b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			Draw(b.x, b.y, PIXEL_SOLID, FG_RED);
		}

		// check collision with asteroids
		std::vector<sSpaceObject> newAsteroids;

		for (auto& b : vecBullets)
		{
			for (auto& a : vecAsteroids)
			{
				if (IsPointInCircle(a.x, a.y, (float)a.nSize, b.x, b.y))
				{
					// hit! remove bullet from gamespace
					nScore += 100;
					b.x = -100;

					// create 2 more smaller asteroids!
					if (a.nSize > 4)
					{
						float nx1 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
						float ny1 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
						float nx2 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
						float ny2 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
						newAsteroids.push_back({a.x, a.y, nx1, ny1, a.angle, a.scale, a.nSize / 2});
						newAsteroids.push_back({a.x, a.y, nx2, ny2, a.angle, a.scale, a.nSize / 2});
					}

					// remove original asteroid from gamespace
					a.x = -100;
				}
			}
		}

		for (auto a : newAsteroids)
		{
			vecAsteroids.push_back(a);
		}
		newAsteroids.clear();

		// level complete? get 2 more asteroids!
		if (vecAsteroids.empty())
		{
			nScore += 1000;
			nDifficulty += 2;
			float nx1 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
			float ny1 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
			float nx2 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
			float ny2 = ((float)std::rand() / (float)RAND_MAX) * 10.0f * nDifficulty - 5.0f * nDifficulty;
			vecAsteroids.push_back({player.x + 50.0f * sinf(player.angle), player.y + 50.0f * cosf(player.angle), nx1, ny1, 0.0f, 1.0f, (int)16 });
			vecAsteroids.push_back({player.x - 50.0f * sinf(player.angle), player.y - 50.0f * cosf(player.angle), nx1, ny1, 0.0f, 1.0f, (int)16 });
		}

		// remove off screen bullets
		if (vecBullets.size() > 0)
		{
			auto iterator = std::remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.x > ScreenWidth() || o.y < 1 || o.y > ScreenHeight()); });
			if (iterator != vecBullets.end())
			{
				vecBullets.erase(iterator);
			}
		}

		// remove off screen asteroids
		if (vecAsteroids.size() > 0)
		{
			auto iterator = std::remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x < 0); });
			if (iterator != vecAsteroids.end())
			{
				vecAsteroids.erase(iterator);
			}
		}

		// monitor bullets that are on gamespace
		//int numBullets = std::count_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject b) { return (b.dx != 0 || b.dy != 0); });
		int numBullets = vecBullets.size();
		DrawString(2, 5, L"numBullets: " + std::to_wstring(numBullets));
		DrawString(60, 2, L"nScore: " + std::to_wstring(nScore));

		// draw asteroids
		for (auto& a : vecAsteroids)
		{
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 2.0f * fElapsedTime;
			WrapCoordinates(a.x, a.y, a.x, a.y);
			DrawWireFrameModel(vecModelAsteroids, a.x, a.y, a.angle, (float)a.nSize, PIXEL_SOLID, FG_YELLOW);
		}

		// draw ship & throttle
		WrapCoordinates(player.x, player.y, player.x, player.y);
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle, player.scale, PIXEL_SOLID, FG_WHITE);
		if (m_keys[VK_UP].bHeld)
		{
			DrawWireFrameModel(vecModelThrottle, player.x, player.y, player.angle, player.scale, PIXEL_SOLID, FG_RED);
		}

		return true;
	}

// special functions
protected:
	void ResetGame()
	{
		vecBullets.clear();
		vecAsteroids.clear();
		
		nScore = 0;
		nDifficulty = 0;
		bDead = false;

		// initiate player position
		player.x = ScreenWidth() / 2;
		player.y = ScreenHeight() / 2;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;
		player.scale = 1.0f;

		// initiate asteroids
		vecAsteroids.push_back({ 120.0f, 35.0f, 2.0f, -8.0f, 0.0f, 1.0f, (int)16 });
		vecAsteroids.push_back({ 25.0f, 50.0f, -8.0f, 3.0f, 0.0f, 1.0f, (int)16 });
	}

	bool IsPointInCircle(float cx, float cy, float r, float x, float y)
	{
		float distance = std::sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
		if (distance <= r)
			return true;
		if (distance > r)
			return false;
	}

	void WrapCoordinates(float ix, float iy, float& ox, float& oy)
	{
		ox = ix;
		oy = iy;

		if (ix < 0.0f)
			ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth())
			ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)
			oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight())
			oy = iy - (float)ScreenHeight();
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}

	void DrawWireFrameModel(const std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float a = 0.0f, float s = 1.0f, short c = 0x2588, short col = 0x000F)
	{
		// first = x
		// second = y

		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int size = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(size);

		// rotate
		for (int i = 0; i < size; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(a) - vecModelCoordinates[i].second * sinf(a);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(a) + vecModelCoordinates[i].second * cosf(a);
		}

		// rescale (if needed)
		for (int i = 0; i < size; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// translate
		for (int i = 0; i < size; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// draw
		for (int i = 0; i < size + 1; i++)
		{
			int j = i + 1;
			DrawLine(	vecTransformedCoordinates[i % size].first, vecTransformedCoordinates[i % size].second,
						vecTransformedCoordinates[j % size].first, vecTransformedCoordinates[j % size].second,
						c, col);
		}
	}
};

Asteroids::Asteroids()
{
	m_sAppName = L'Asteroids';
}

int main()
{
	// Use olcConsoleGameEngine derived app
	Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
