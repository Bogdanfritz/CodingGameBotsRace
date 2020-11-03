#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

enum class PodState
{
	Thrusting,
	Braking
};

struct Vector2
{
	int x;
	int y;
	Vector2(int newX, int newY) : x(newX), y(newY) {}

	static int GetDistance(const Vector2& vector1, const Vector2& vector2)
	{
		int diffX = vector1.x - vector2.x;
		int diffY = vector1.y - vector2.y;
		return (int)sqrt(diffX * diffX + diffY * diffY);
	}
};

struct Checkpoint
{
	Vector2 position;
	int distanceFromPrevious = 0;

	Checkpoint() : position(0, 0) {}
};

struct SpeedBoost
{
	int count = 0;
	bool canUseBoost = false;
	bool shouldUseBoost = false;
	void UseBoost() { count--; }
	void TryAllowBoost()
	{
		if (count > 0)
		{
			canUseBoost = true;
		}
	}
	void Boost()
	{
		cout << "BOOST" << endl;
		shouldUseBoost = false;
	}
};

class Solution
{
	int K_MIN_THRUST_ANGLE = 10;
	int K_MIN_BOOST_ANGLE = 10;
	float K_BRAKE_FROM_DISTANCE = 1000;

	Vector2 position;
	int nextX = 0;
	int nextY = 0;
	int thrust = 0;
	int nextCheckpointAngle = 0;
	int distanceToNextCheckpoint = 0;
	int maxDistanceCheckpointIndex = 0;

	vector<Checkpoint> checkpoints;
	int currentCheckpointIndex = -1;

	bool firstLapFinished = false;
	bool firstLapUpdateDone = false;
	SpeedBoost speedBoost;
	PodState state = PodState::Thrusting;

	void PrintCurrentChoice()
	{
		cout << nextX << " " << nextY << " ";
		if (speedBoost.shouldUseBoost)
		{
			speedBoost.Boost();
		}
		else
		{
			cout << thrust << endl;
		}
	}
	void AddNewCheckpoint(int x, int y)
	{
		Vector2 previousCheckpointPosition = position;
		if (checkpoints.size() != 0)
		{
			previousCheckpointPosition = checkpoints[currentCheckpointIndex].position;
		}
		Checkpoint newCheckpoint;
		newCheckpoint.position.x = x;
		newCheckpoint.position.y = y;
		int distanceFromPrevious = Vector2::GetDistance(newCheckpoint.position, previousCheckpointPosition);
		newCheckpoint.distanceFromPrevious = distanceFromPrevious;

		checkpoints.push_back(newCheckpoint);
		currentCheckpointIndex++;

		if (checkpoints.size() != 0 && distanceFromPrevious > checkpoints[maxDistanceCheckpointIndex].distanceFromPrevious)
		{
			maxDistanceCheckpointIndex = currentCheckpointIndex;
		}
	}
	void CheckNextCheckpoint(int x, int y)
	{
		if (nextX != x || nextY != y)
		{
			int checkpointIndex = FindCheckpoint(x, y);
			if (checkpointIndex == -1)
			{
				AddNewCheckpoint(x, y);
			}
			else
			{
				firstLapFinished = true;
				currentCheckpointIndex = checkpointIndex;
			}
			nextX = x;
			nextY = y;
		}
	}
	int FindCheckpoint(int x, int y)
	{
		//liniar approach for now
		for (int index = 0; index < checkpoints.size(); index++)
		{
			if (checkpoints[index].position.x == x && checkpoints[index].position.y == y)
			{
				return index;
			}
		}
		return -1;
	}

	void UpdateFirstLap()
	{
		speedBoost.TryAllowBoost();
		firstLapUpdateDone = false;
	}

	void TryBoost()
	{
		if (FacingTowardsCheckpoint() && speedBoost.canUseBoost && currentCheckpointIndex == maxDistanceCheckpointIndex)
		{
			speedBoost.count--;
			speedBoost.canUseBoost = speedBoost.count > 0;
			speedBoost.shouldUseBoost = true;
		}
	}

	bool FacingTowardsCheckpoint()
	{
		return nextCheckpointAngle < K_MIN_THRUST_ANGLE&& nextCheckpointAngle > -K_MIN_THRUST_ANGLE;
	}

	bool ShouldBrake()
	{
		bool facingTowardsCheckpoint = FacingTowardsCheckpoint();
		bool inBrakeDistance = IsInBrakeDistanceToCheckpoint();
		cerr << !facingTowardsCheckpoint << " " << inBrakeDistance << endl;
		return !facingTowardsCheckpoint || inBrakeDistance;
	}

	bool IsInBrakeDistanceToCheckpoint()
	{
		int maxDistance = checkpoints[currentCheckpointIndex].distanceFromPrevious;
		if (maxDistance == 0)
		{
			cerr << "ERROR : distance from previous point is zero!" << endl;
			return false;
		}
		return distanceToNextCheckpoint < K_BRAKE_FROM_DISTANCE;
	}

	bool ShouldThrust()
	{
		return FacingTowardsCheckpoint() && !IsInBrakeDistanceToCheckpoint();
	}

	void ChanceState(PodState newState)
	{
		state = newState;
	}

	void UpdateState()
	{
		switch (state)
		{
		case PodState::Thrusting:
		{
			if (ShouldBrake())
			{
				ChanceState(PodState::Braking);
			}
			else
			{
				thrust = 100;
			}
			break;
		}
		case PodState::Braking:
		{
			thrust = 0;
			if (ShouldThrust())
			{
				ChanceState(PodState::Thrusting);
			}
			break;
		}
		}
	}
public:

	Solution() : position(0, 0) { speedBoost.count = 1; }

	void UpdateData(int x, int y, int targetX, int targetY, int distanceToTarget, int angleToTarget)
	{
		position.x = x;
		position.y = y;
		nextCheckpointAngle = angleToTarget;
		distanceToNextCheckpoint = distanceToTarget;

		CheckNextCheckpoint(targetX, targetY);
	}
	void UpdateLogic()
	{
		if (firstLapFinished && !firstLapUpdateDone)
		{
			UpdateFirstLap();
		}

		UpdateState();
		TryBoost();
		PrintCurrentChoice();
	}
};

int main()
{
	Solution solution;
	// game loop
	while (1) {
		int x;
		int y;
		int nextCheckpointX; // x position of the next check point
		int nextCheckpointY; // y position of the next check point
		int nextCheckpointDist; // distance to the next checkpoint
		int nextCheckpointAngle; // angle between your pod orientation and the direction of the next checkpoint
		cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; cin.ignore();
		int opponentX;
		int opponentY;
		cin >> opponentX >> opponentY; cin.ignore();

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// You have to output the target position
		// followed by the power (0 <= thrust <= 100)
		// i.e.: "x y thrust"

		solution.UpdateData(x, y, nextCheckpointX, nextCheckpointY, nextCheckpointDist, nextCheckpointAngle);
		solution.UpdateLogic();
	}
}