#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

template<typename T>
constexpr T pi = T(3.1415926535897932385);

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
	float x;
	float y;
	Vector2(float newX, float newY) : x(newX), y(newY) {}

	static int GetDistance(const Vector2& vector1, const Vector2& vector2)
	{
		float diffX = vector1.x - vector2.x;
		float diffY = vector1.y - vector2.y;
		return (int)sqrt(diffX * diffX + diffY * diffY);
	}

	static float DotProduct(const Vector2& vector1, const Vector2& vector2)
	{
		return vector1.x * vector2.x + vector1.y * vector2.y;
	}
	static float Determinant(const Vector2& vector1, const Vector2& vector2)
	{
		return vector1.x * vector2.y - vector1.y * vector2.x;
	}

	float Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	Vector2 GetNormalized() const
	{
		Vector2 normalized(0, 0);
		float mag = Magnitude();
		mag = 1 / mag;
		normalized.x = x * mag;
		normalized.y = y * mag;
		return normalized;
	}

	static Vector2 Substract(const Vector2& vector1, const Vector2& vector2)
	{
		Vector2 substractedVector = Vector2(vector2.x - vector1.x, vector2.y - vector1.x);
	}

	static int GetAngle(const Vector2& vector1, const Vector2& vector2)
	{
		Vector2 v2Normalized = vector2.GetNormalized();
		float dot = DotProduct(vector1, v2Normalized);
		float theta = acos(dot / (vector1.Magnitude()));
		cerr << "theta = " << theta << endl;
		return  theta * 180 / pi<float>;
		//float determinant = Determinant(vector1, vector2);
		//return atan2(determinant, dot) * 180 * pi<float>;
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
	int K_STEEP_ANGLE = 90;
	int K_ALIGNED_ANGLE = 30;
	int K_MIN_BOOST_ANGLE = 10;
	float K_BRAKE_FROM_DISTANCE = 1000;
	int K_MAX_STUCK_FRAMES = 1;

	Vector2 position;
	float nextX = 0;
	float nextY = 0;
	int thrust = 0;
	int nextCheckpointAngle = 0;
	int distanceToNextCheckpoint = 0;
	int maxDistanceCheckpointIndex = 0;
	int framesStuckBraking = 0;

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
	void AddNewCheckpoint(float x, float y)
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

	void CheckNextCheckpoint(float x, float y)
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
	int FindCheckpoint(float x, float y)
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
		return nextCheckpointAngle < K_STEEP_ANGLE&& nextCheckpointAngle > -K_STEEP_ANGLE;
	}

	bool AlignedToNextCheckpoint()
	{
		return nextCheckpointAngle < K_ALIGNED_ANGLE&& nextCheckpointAngle > -K_ALIGNED_ANGLE;
	}

	bool ShouldBrake()
	{
		bool facingTowardsCheckpoint = FacingTowardsCheckpoint();
		bool inBrakeDistance = IsInBrakeDistanceToCheckpoint();
		return !facingTowardsCheckpoint || inBrakeDistance;
	}

	bool IsInBrakeDistanceToCheckpoint()
	{
		return distanceToNextCheckpoint < K_BRAKE_FROM_DISTANCE;
	}

	bool ShouldThrust()
	{
		return FacingTowardsCheckpoint() && !IsInBrakeDistanceToCheckpoint();
	}

	void ChanceState(PodState newState)
	{
		state = newState;
		switch (state)
		{
		case PodState::Thrusting:
		{
			UpdateThrust();
			break;
		}
		case PodState::Braking:
		{
			UpdateBraking();
			break;
		}
		}
	}

	void UpdateThrust()
	{
		if (!AlignedToNextCheckpoint())
		{
			thrust = 50;
		}
		else
		{
			thrust = 100;
		}
	}

	void UpdateBraking()
	{
		thrust = 0;
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
				UpdateThrust();
			}
			break;
		}
		case PodState::Braking:
		{
			if (ShouldThrust() || framesStuckBraking >= K_MAX_STUCK_FRAMES)
			{
				ChanceState(PodState::Thrusting);
			}
			break;
		}
		}
	}

	void CheckForStuckFrames(int distanceToTarget)
	{
		framesStuckBraking = distanceToNextCheckpoint == distanceToTarget ? framesStuckBraking + 1 : 0;
	}

public:

	Solution() : position(0, 0) { speedBoost.count = 1; }

	void UpdateData(float x, float y, float targetX, float targetY, int distanceToTarget, int angleToTarget)
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

		cerr << Vector2::GetAngle(position, checkpoints[currentCheckpointIndex].position) << " " << nextCheckpointAngle << endl;

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
		float x;
		float y;
		float nextCheckpointX; // x position of the next check point
		float nextCheckpointY; // y position of the next check point
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