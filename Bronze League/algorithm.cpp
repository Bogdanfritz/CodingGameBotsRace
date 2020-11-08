#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;


const int K_CHECKPOINT_RADIUS = 600;
const int K_RACE_ZONE_RADIUS = 200;
const int K_APEX_ZONE_RADIUS = 100;
const float K_APEX_DISTANCE = 0.25;
const int K_BOOST_COUNT = 1;
const int K_STEEP_ANGLE = 45;
const int K_ALIGNED_ANGLE = 10;

const int K_MAX_THRUST = 100;

const int K_COLLISION_FRAMES = 3;
const int K_SHIELD_RADIUS = 400;

const int K_UTURN_MAX_ANGLE = 15;
const float K_BRAKE_FROM_DISTANCE = 1000;

template<typename T>
constexpr T pi = T(3.1415926535897932385);

const static float RAD2DEGREE = 180 / pi<float>;

enum class ThrustChange
{
	Accelerate,
	Brake,
	Hold
};

enum class TurnType
{
	Invalid,
	RightTurn,
	LeftTurn,
	UTurn,
};

enum class PodState
{
	Thrusting,
	Braking
};

class Vector2
{
	float x;
	float y;

public:
	Vector2() : x(0), y(0) {}
	Vector2(float newX, float newY) : x(newX), y(newY) {}
	Vector2(const Vector2& pos) : x(pos.x), y(pos.y) {}

	void CerrPrint() const
	{
		cerr << x << " " << y << endl;
	}

	void RotateByAngle(float angle)
	{
		float sine = sin(angle);
		float cosine = cos(angle);
		float newX = cosine * x - sine * y;
		float newY = sine * x - cosine * y;
		x = newX;
		y = newY;
	}

	float GetDistance(const Vector2& other) const
	{
		float diffX = x - other.x;
		float diffY = y - other.y;
		return sqrt(diffX * diffX + diffY * diffY);
	}

	float DotProduct(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	int PositionSign(const Vector2& other) const
	{
		int crossProduct = other.x * y - other.y * x;
		return crossProduct > 0 ? 1 : -1;
	}

	float Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	Vector2 Midpoint(const Vector2& other) const
	{
		Vector2 midpoint = ((*this + other) / 2);
		return midpoint;
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

	int GetAngle(const Vector2& other) const
	{
		Vector2 normalized1 = GetNormalized();
		Vector2 normalized2 = other.GetNormalized();
		float dot = normalized1.DotProduct(normalized2);
		float theta = acos(dot);
		return  theta * RAD2DEGREE;
	}

	Vector2 ProjectOn(const Vector2& other) const
	{
		float scalarProjection = DotProduct(other) / other.Magnitude();
		Vector2 projection = other.GetNormalized() * scalarProjection;
		return projection;
	}

	bool operator != (const Vector2& other) const
	{
		return x != other.x || y != other.y;
	}

	bool operator == (const Vector2& other) const
	{
		return x == other.x && y == other.y;
	}

	Vector2 operator +(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 operator -(const Vector2& other) const
	{
		return Vector2(other.x - x, other.y - y);
	}

	Vector2 operator /(float scalar) const
	{
		return Vector2(x / scalar, y / scalar);
	}

	Vector2 operator *(float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}

	void SetPosition(const Vector2& other)
	{
		x = other.x;
		y = other.y;
	}
	void SetPosition(float newX, float newY)
	{
		x = newX;
		y = newY;
	}

	bool IntersectingCircle(const Vector2& other, float myRadius, float otherRadius) const
	{
		float distance = GetDistance(other);
		return distance <= myRadius + otherRadius;
	}

	float GetX() const { return x; }
	float GetY() const { return y; }
};

class RaceData
{
	int turnAngle;
	TurnType turnType;
	Vector2 brakePoint;
	Vector2 turnPoint;
	Vector2 apex;
	bool reachedBrakePoint;
	bool reachedTurnPoint;
	bool reachedApex;
public:

	int GetTurnAngle() { return turnAngle; }
	TurnType GetTurnType() { return turnType; }
	Vector2 GetTurnPoint() { return turnPoint; }
	Vector2 GetBrakePoint() { return brakePoint; }
	Vector2 GetApex() { return apex; }

	void ComputeTurnAngle(const Vector2& previousPath, const Vector2& nextPath)
	{
		turnAngle = previousPath.GetAngle(nextPath);
	}

	void ComputeTurnType(const Vector2& previousPath, const Vector2& nextCheckpoint)
	{
		int turnSign = nextCheckpoint.PositionSign(previousPath);
		if (turnAngle < K_UTURN_MAX_ANGLE || turnAngle > 180 - K_UTURN_MAX_ANGLE)
		{
			turnType = TurnType::UTurn;
		}
		else if (turnSign > 0)
		{
			turnType = TurnType::LeftTurn;
		}
		else
		{
			turnType = TurnType::RightTurn;
		}
	}

	void ComputeApex(const Vector2& origin, const Vector2& nextPoint)
	{
		Vector2 direction = (nextPoint - origin).GetNormalized();
		apex = direction * K_CHECKPOINT_RADIUS * K_APEX_DISTANCE;
		apex = apex - origin;
	}

	void ComputeBrakePoint(const Vector2& checkpointPosition, const Vector2& previousPoint)
	{
		//Todo apply angle
		brakePoint = (previousPoint - checkpointPosition).GetNormalized() * checkpointPosition.GetDistance(previousPoint) * 0.5 - checkpointPosition; // Could have used midpoint but i might want to change this value later
	}

	void ComputeTurnPoint(const Vector2& checkpointPosition, const Vector2& previousPoint)
	{
		//Todo - apply angle
		turnPoint = (previousPoint - checkpointPosition).GetNormalized() * checkpointPosition.GetDistance(previousPoint) * 0.35 - checkpointPosition;
	}

	void ComputeData(const Vector2& checkpointPosition, const Vector2& previousPoint, const Vector2& nextPoint)
	{

		Vector2 prev = previousPoint - checkpointPosition;
		Vector2 next = nextPoint - checkpointPosition;
		ComputeTurnAngle(prev, next);
		ComputeTurnType(prev, next);
		ComputeApex(checkpointPosition, nextPoint);
		ComputeBrakePoint(checkpointPosition, previousPoint);
		ComputeTurnPoint(checkpointPosition, previousPoint);
		CleanCache();
	}

	void CleanCache()
	{
		reachedBrakePoint = false;
		reachedTurnPoint = false;
		reachedApex = false;
	}

	void SetNextDestination(const Vector2& podPosition, Vector2& destination, const Vector2& nextCheckpointPosition, ThrustChange& thrustChange)
	{
		float distanceToCheckpoint = podPosition.GetDistance(nextCheckpointPosition);
		float distanceToBrakePoint = podPosition.GetDistance(brakePoint);
		float distanceToTurnPoint = podPosition.GetDistance(turnPoint);
		float distanceToApex = podPosition.GetDistance(apex);
		if (!reachedBrakePoint)
		{
			ComputeDestination(podPosition, brakePoint, turnPoint, destination, reachedBrakePoint);
			if (destination == turnPoint && distanceToCheckpoint < brakePoint.GetDistance(nextCheckpointPosition))
			{
				cerr << "going for turnpoint 1" << endl;
				thrustChange = ThrustChange::Brake;
			}
			else if (destination == brakePoint)
			{
				cerr << "going for brakepoint" << endl;
				thrustChange = ThrustChange::Accelerate;
			}
		}
		else if (!reachedTurnPoint)
		{
			cerr << "going for turnpoint 2" << endl;
			ComputeDestination(podPosition, turnPoint, apex, destination, reachedTurnPoint);
		}
		else
		{
			cerr << "going for apex" << endl;
			ComputeDestination(podPosition, apex, nextCheckpointPosition, destination, reachedApex);
			if (destination == nextCheckpointPosition)
			{
				thrustChange = ThrustChange::Accelerate;
			}
			reachedApex = false;
		}
	}

	void ComputeDestination(const Vector2& podPosition, const Vector2& closestRacePoint, const Vector2& nextRacePoint, Vector2& destination, bool& reachedClosestRacePoint)
	{
		if (!reachedClosestRacePoint)
		{
			bool aimingTowardsApex = closestRacePoint == apex;
			if (!aimingTowardsApex)
			{
				Vector2 directionToClosest = closestRacePoint - podPosition;
				Vector2 directionOfPath = nextRacePoint - closestRacePoint;
				float angle = directionToClosest.GetAngle(directionOfPath);
				if (angle > K_ALIGNED_ANGLE)
				{
					reachedClosestRacePoint = true;
					destination = nextRacePoint;
					return;
				}
			}
			float radius = aimingTowardsApex ? K_APEX_ZONE_RADIUS : K_RACE_ZONE_RADIUS;
			if (podPosition.IntersectingCircle(closestRacePoint, K_SHIELD_RADIUS, radius))
			{
				reachedClosestRacePoint = true;
				destination = nextRacePoint;
			}
			else
			{
				destination = closestRacePoint;
			}
		}
		else
		{
			destination = nextRacePoint;
		}
	}
};

class Checkpoint
{
	Vector2 position;
	float distanceToNext = 0;
	int turnSign = 0;
	RaceData raceData;
public:
	Vector2 GetPosition() { return position; }
	void SetPosition(Vector2 pos) { position = pos; }

	float GetDistanceToNext() { return distanceToNext; }
	int GetTurnAngle() { return raceData.GetTurnAngle(); }
	TurnType GetTurnType() { return raceData.GetTurnType(); }

	Checkpoint(const Vector2& newPos) : position(newPos), distanceToNext(0) {}

	void SetDistanceToNext(const Vector2& newPosition)
	{
		distanceToNext = position.GetDistance(newPosition);
	}

	void ComputeRaceData(const Vector2& previousCheckpoint, const Vector2& nextCheckpoint)
	{
		raceData.ComputeData(position, previousCheckpoint, nextCheckpoint);
	}

	void CerrPrint()
	{
		/*
		cerr << "turn angle: " << raceData.GetTurnAngle() << " turnType: ";
		if (raceData.GetTurnType() == TurnType::UTurn)
		{
			cerr << "UTurn";
		}
		else if (raceData.GetTurnType() == TurnType::LeftTurn)
		{
			cerr << "Left turn";
		}
		else if(raceData.GetTurnType() == TurnType::RightTurn)
		{
			cerr << "Right turn";
		}
		else
		{
			cerr << "Error. Print requested before track data has been computed.";
		}
		cerr << endl;*/
		cerr << " apex at: "; raceData.GetApex().CerrPrint();
		cerr << " turnpoint at: "; raceData.GetTurnPoint().CerrPrint();
		cerr << " brakepoint at: "; raceData.GetBrakePoint().CerrPrint();
	}

	bool PassedApex()
	{
		if (position.IntersectingCircle(raceData.GetApex(), K_SHIELD_RADIUS, K_RACE_ZONE_RADIUS))
		{
			return true;
		}
		return false;
	}

	bool PassedBrakePoint()
	{
		if (position.IntersectingCircle(raceData.GetBrakePoint(), K_SHIELD_RADIUS, K_RACE_ZONE_RADIUS))
		{
			return true;
		}
		return false;
	}

	bool PassedTurnPoint()
	{
		if (position.IntersectingCircle(raceData.GetTurnPoint(), K_SHIELD_RADIUS, K_RACE_ZONE_RADIUS))
		{
			return true;
		}
		return false;
	}

	void SetNextDestination(const Vector2& podPosition, Vector2& destination, const Vector2& nextCheckpointPosition, ThrustChange& thrustChange)
	{
		raceData.SetNextDestination(podPosition, destination, nextCheckpointPosition, thrustChange);
	}

	void CleanDataCache()
	{
		raceData.CleanCache();
	}
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
	Vector2 position;
	Vector2 enemyPosition;
	Vector2 destination;
	int thrust = 0;
	int suggestedThrust = K_MAX_THRUST;
	int nextCheckpointAngle = 0;
	int distanceToNextCheckpoint = 0;
	float angleToCurrentTarget = 0;

	int maxDistanceCheckpointIndex = 0;

	vector<Checkpoint> checkpoints;
	vector<Vector2> podPositions;
	int currentCheckpointIndex = -1;
	bool reachedNewCheckpoint = false;

	bool firstLapFinished = false;
	bool firstLapUpdateDone = false;
	SpeedBoost speedBoost;
	PodState state = PodState::Thrusting;

	void PrintCurrentChoice()
	{
		cout << (int)destination.GetX() << " " << (int)destination.GetY() << " ";
		if (speedBoost.shouldUseBoost)
		{
			speedBoost.Boost();
		}
		else
		{
			cout << thrust << endl;
		}
	}

	void UpdateDataBetweenCheckpoints()
	{
		if (checkpoints.size() <= 1)
		{
			return;
		}
		size_t checkpointsCount = checkpoints.size();

		for (size_t index = 0; index < checkpointsCount; index++)
		{
			size_t nextIndex = (index == (checkpointsCount - 1)) ? 0 : index + 1;
			checkpoints[index].SetDistanceToNext(checkpoints[nextIndex].GetPosition());
			if (checkpoints[index].GetDistanceToNext() > checkpoints[maxDistanceCheckpointIndex].GetDistanceToNext())
			{
				maxDistanceCheckpointIndex = index;
			}

			size_t prevIndex = (index == 0) ? checkpointsCount - 1 : index - 1;
			checkpoints[index].ComputeRaceData(checkpoints[prevIndex].GetPosition(), checkpoints[nextIndex].GetPosition());
		}
	}

	void CleanDataRaceCache()
	{
		if (checkpoints.size() <= 1)
		{
			return;
		}
		size_t checkpointsCount = checkpoints.size();

		for (size_t index = 0; index < checkpointsCount; index++)
		{
			checkpoints[index].CleanDataCache();
		}
	}

	void AddNewCheckpoint(float x, float y)
	{
		Vector2 newPosition(x, y);
		Checkpoint newCheckpoint(newPosition);
		checkpoints.push_back(newCheckpoint);
		currentCheckpointIndex++;
	}

	void CheckNextCheckpoint(float x, float y)
	{
		if (checkpoints.size() == 0)
		{
			AddNewCheckpoint(x, y);
		}
		Vector2 checkpointPosition = Vector2(x, y);

		if (checkpointPosition != checkpoints[currentCheckpointIndex].GetPosition())
		{
			int checkpointIndex = FindCheckpoint(checkpointPosition);
			if (checkpointIndex == -1)
			{
				AddNewCheckpoint(x, y);
			}
			else
			{
				firstLapFinished = true;
				currentCheckpointIndex = checkpointIndex;
				reachedNewCheckpoint = true;
			}
		}
	}

	int FindCheckpoint(const Vector2& newCheckpoint)
	{
		//liniar approach for now
		for (int index = 0; index < checkpoints.size(); index++)
		{
			if (checkpoints[index].GetPosition() == newCheckpoint)
			{
				return index;
			}
		}
		return -1;
	}

	void UpdateFirstLap()
	{
		UpdateDataBetweenCheckpoints();
		speedBoost.TryAllowBoost();
		firstLapUpdateDone = true;
	}

	void TryBoost()
	{
		int prevIndex = (currentCheckpointIndex == 0) ? checkpoints.size() - 1 : currentCheckpointIndex - 1; // check against prev index because my checkpoints indices start at 1 instead of start line
		if (AlignedAngle(nextCheckpointAngle) && speedBoost.canUseBoost && prevIndex == maxDistanceCheckpointIndex)
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

	bool AlignedAngle(float angle)
	{
		return angle < K_ALIGNED_ANGLE&& angle > -K_ALIGNED_ANGLE;
	}

	bool SteepAngle(float angle)
	{
		return angle > K_STEEP_ANGLE || angle < -K_STEEP_ANGLE;
	}

	bool ShouldBrake()
	{
		bool facingTowardsCheckpoint = FacingTowardsCheckpoint();
		return !facingTowardsCheckpoint;
	}

	bool ShouldThrust()
	{
		return FacingTowardsCheckpoint();
	}

	void UpdateThrust()
	{
		thrust = suggestedThrust;
	}

	void UpdateBraking()
	{
		thrust = 0;
	}

	void ChangeState(PodState newState)
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

	void UpdateState()
	{
		switch (state)
		{
		case PodState::Thrusting:
		{
			if (ShouldBrake())
			{
				ChangeState(PodState::Braking);
			}
			else
			{
				UpdateThrust();
			}
			break;
		}
		case PodState::Braking:
		{
			if (ShouldThrust())
			{
				ChangeState(PodState::Thrusting);
			}
			break;
		}
		}
	}

	float GetAngleWithPosition(size_t prevCheckpointIndex, size_t nextCheckpointIndex)
	{
		float angle = 0;
		if (checkpoints.size() > 1)
		{
			Vector2 previousCheckpoint = checkpoints[prevCheckpointIndex].GetPosition();
			Vector2 nextCheckpoint = checkpoints[nextCheckpointIndex].GetPosition();
			Vector2 currentPath = nextCheckpoint - previousCheckpoint;
			angle = position.GetAngle(currentPath);
		}
		return angle;
	}

	float GetAngleWithDestination(size_t prevCheckpointIndex, size_t nextCheckpointIndex)
	{
		float angle = 0;
		if (checkpoints.size() > 1)
		{
			Vector2 previousCheckpoint = checkpoints[prevCheckpointIndex].GetPosition();
			Vector2 nextCheckpoint = checkpoints[nextCheckpointIndex].GetPosition();
			Vector2 currentPath = nextCheckpoint - previousCheckpoint;
			Vector2 moveDirection = destination - position;
			angle = moveDirection.GetAngle(currentPath);
		}
		return angle;
	}

	void AdjustTrajectory()
	{
		Vector2 newTarget;
		size_t prevIndex = (currentCheckpointIndex == 0) ? checkpoints.size() - 1 : currentCheckpointIndex - 1;
		angleToCurrentTarget = GetAngleWithPosition(prevIndex, currentCheckpointIndex);
		float movementAngle = GetAngleWithDestination(prevIndex, currentCheckpointIndex);

		Vector2 previousCheckpoint = checkpoints[prevIndex].GetPosition();
		Vector2 nextCheckpoint = checkpoints[currentCheckpointIndex].GetPosition();
		Vector2 currentPath = nextCheckpoint - previousCheckpoint;
		float distanceToNextCheckpoint = position.GetDistance(nextCheckpoint);
		float distanceBetweenCheckpoints = previousCheckpoint.GetDistance(nextCheckpoint);

		if (angleToCurrentTarget > K_ALIGNED_ANGLE && movementAngle > K_ALIGNED_ANGLE)
		{
			Vector2 pathUnitVector = currentPath.GetNormalized();
			pathUnitVector = pathUnitVector * (min(distanceBetweenCheckpoints / 2, distanceToNextCheckpoint / 2)) + nextCheckpoint;
			newTarget = pathUnitVector;

			if ((SteepAngle(angleToCurrentTarget) || SteepAngle(movementAngle)) && distanceToNextCheckpoint < distanceBetweenCheckpoints / 2)
			{
				suggestedThrust = K_MAX_THRUST / 2;
			}
			else
			{
				suggestedThrust = K_MAX_THRUST;
			}
		}
		else
		{
			if (distanceToNextCheckpoint < K_CHECKPOINT_RADIUS + K_SHIELD_RADIUS * 2)
			{
				suggestedThrust = K_MAX_THRUST / 2;
			}
			else
			{
				suggestedThrust = K_MAX_THRUST;
			}
			newTarget = checkpoints[currentCheckpointIndex].GetPosition();
		}
		destination = newTarget;
	}

	void AdjustTrajectoryImproved()
	{
		Vector2 newTarget;
		size_t prevIndex = (currentCheckpointIndex == 0) ? checkpoints.size() - 1 : currentCheckpointIndex - 1;
		size_t nextIndex = (currentCheckpointIndex == (checkpoints.size() - 1)) ? 0 : currentCheckpointIndex + 1;
		if (position.GetDistance(checkpoints[currentCheckpointIndex].GetPosition()) > K_CHECKPOINT_RADIUS + K_SHIELD_RADIUS)
		{
			nextIndex = currentCheckpointIndex;
		}
		else
		{
			prevIndex = currentCheckpointIndex;
		}
		angleToCurrentTarget = GetAngleWithDestination(prevIndex, nextIndex);
		if (angleToCurrentTarget > K_ALIGNED_ANGLE)
		{
			Vector2 previousCheckpoint = checkpoints[prevIndex].GetPosition();
			Vector2 nextCheckpoint = checkpoints[nextIndex].GetPosition();
			Vector2 currentPath = nextCheckpoint - previousCheckpoint;

			Vector2 pathUnitVector = currentPath.GetNormalized();
			float distanceToNextCheckpoint = position.GetDistance(nextCheckpoint);
			float distanceBetweenCheckpoints = previousCheckpoint.GetDistance(nextCheckpoint);
			pathUnitVector = pathUnitVector * (min(distanceBetweenCheckpoints / 2, distanceToNextCheckpoint / 2)) + nextCheckpoint;
			newTarget = pathUnitVector;

			if (SteepAngle(angleToCurrentTarget) && distanceToNextCheckpoint < distanceBetweenCheckpoints / 2)
			{
				suggestedThrust = K_MAX_THRUST / 2;
			}
			else
			{
				suggestedThrust = K_MAX_THRUST;
			}

		}
		else
		{
			newTarget = checkpoints[nextIndex].GetPosition();
			suggestedThrust = K_MAX_THRUST;
		}
		destination = newTarget;
	}

	/* Not used
	void UpdateSuggestedThrust(ThrustChange thrustChange)
	{
		switch (thrustChange)
		{
			case ThrustChange::Accelerate:
			{
				suggestedThrust = K_MAX_THRUST;
				break;
			}
			case ThrustChange::Brake:
			{
				suggestedThrust = K_MAX_THRUST / 2;
				break;
			}
		}
	}
	*/

	/* Not used
	void AdjustTrajectoryRace()
	{
		size_t prevIndex = (currentCheckpointIndex == 0) ? checkpoints.size() - 1 : currentCheckpointIndex - 1;
		size_t nextIndex = (currentCheckpointIndex == (checkpoints.size() - 1)) ? 0 : currentCheckpointIndex + 1;
		Vector2 newTarget;
		ThrustChange thrustChange = ThrustChange::Hold;
		if (reachedNewCheckpoint)
		{
			Vector2 nextCheckpointPosition = checkpoints[currentCheckpointIndex].GetPosition();

			checkpoints[currentCheckpointIndex].SetNextDestination(position, newTarget, nextCheckpointPosition, thrustChange);
			if (newTarget == nextCheckpointPosition)
			{
				reachedNewCheckpoint = false;
				if (nextCheckpointPosition == checkpoints[0].GetPosition())
				{
					CleanDataRaceCache();
				}
				AdjustTrajectoryRace();
			}
			else
			{
				destination = newTarget;
				UpdateSuggestedThrust(thrustChange);
			}
		}
		else
		{
			checkpoints[prevIndex].SetNextDestination(position, newTarget, checkpoints[nextIndex].GetPosition(), thrustChange);
			destination = newTarget;
			UpdateSuggestedThrust(thrustChange);
		}
	}

	bool CheckColission(float distance)
	{
		bool collision = false;
		if (distance <= K_SHIELD_RADIUS * 2)
		{
			collision = true;
		}
		return collision;
	}

	bool CouldCollideWithEnemy(double deltaTime)
	{
		bool collisionPossible = false;

		Vector2 directionToDestination = (destination - position).GetNormalized();
		Vector2 presumedEnemyDirection = (checkpoints[currentCheckpointIndex].GetPosition() - enemyPosition).GetNormalized();

		for (int i = 1; i <= K_COLLISION_FRAMES; ++i)
		{
			Vector2 newPos = directionToDestination * i * thrust + position;
			Vector2 newEnemyPos = presumedEnemyDirection * i * thrust + enemyPosition; // presume enemy is going at the same speed
			float distanceToEnemy = newPos.GetDistance(newEnemyPos);
			collisionPossible = CheckColission(distanceToEnemy);
			if (collisionPossible)
			{
				break;
			}
		}

		return collisionPossible;
	}
	*/
public:

	Solution() : position(0, 0) { speedBoost.count = K_BOOST_COUNT; }

	void UpdateData(float x, float y, float targetX, float targetY, int distanceToTarget, int angleToTarget, float enemyX, float enemyY)
	{
		position.SetPosition(x, y);
		podPositions.push_back(position);

		enemyPosition.SetPosition(enemyX, enemyY);

		nextCheckpointAngle = angleToTarget;
		distanceToNextCheckpoint = distanceToTarget;
		CheckNextCheckpoint(targetX, targetY);

	}

	void UpdateLogic(double deltaTime)
	{
		if (firstLapFinished)
		{
			if (!firstLapUpdateDone)
			{
				UpdateFirstLap();
			}
			//AdjustTrajectoryImproved();
			//UpdateState();
		}
		//else
		{
			AdjustTrajectory();
			UpdateState();
		}
		TryBoost();
		PrintCurrentChoice();
	}
};

int main()
{
	Solution solution;
	auto previousFrameTime = std::chrono::high_resolution_clock::now();
	// game loop
	while (1) {
		auto frameTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = frameTime - previousFrameTime;
		double deltaTIme = elapsed.count();
		previousFrameTime = frameTime;

		float x;
		float y;
		float nextCheckpointX; // x position of the next check point
		float nextCheckpointY; // y position of the next check point
		int nextCheckpointDist; // distance to the next checkpoint
		int nextCheckpointAngle; // angle between your pod orientation and the direction of the next checkpoint
		cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; cin.ignore();
		float opponentX;
		float opponentY;
		cin >> opponentX >> opponentY; cin.ignore();

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// You have to output the target position
		// followed by the power (0 <= thrust <= 100)
		// i.e.: "x y thrust"

		solution.UpdateData(x, y, nextCheckpointX, nextCheckpointY, nextCheckpointDist, nextCheckpointAngle, opponentX, opponentY);
		solution.UpdateLogic(deltaTIme);
	}
}