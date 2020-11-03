League time - 3 hour 15 minutes
Upgraded to wood league 1. obtained boost.
I'm unhappy with my previous solution. I'm thinking of slowing down just before I reach a checkpoint to allow myself to rotate

First i will shorten the rotation window from 90 degrees to 45. Seems to be a faster now.

I am thinking of racing cars. I don't have any knowledge before the race of the track, so the first lap will have to be about "learning" the track (storing each checkpoint and finding a strategy).

There seem to be some racing strategies which i might be able to use.

Second choice i made was to pack my solution into a class for easier future iterations.

Time elapsed so far: 1 hour 15 minutes

Updated boost to be used starting with the second lap, on the longest distance checkpoint.

Issues encountered 
	- Having multiple booleans for the boost was annoying so i put it in a structure
	- AddNewCheckpoint default value was the next checkpoint resulting in a zero distance for the first checkpoint 

Even with the boost, it takes my pod too long to rotate when reaching a new checkpoint.
I lowered the brake angle to 10 degrees. Still not good enough.
I remembered the problem is with the braking before reaching a checkpoint, so i created a treshold to brake at 70% of the distance between checkpoints.

Issues encountered 
	- First index was 1 so i was going out of range accessing the current checkpoint
	- I can't brake unless my thrust is higher than zero. Before adding this check, my pod woudn't move at all
	- With the check, not the condition is on and off every frame so it's not working as inteded. It should keep braking until reaching checkpoint and realigning. I am thinking of adding a state machine for this with Thrusting and Braking as values.

Time elapsed 1 hour.

Moved braking condintions in ShouldBrake method

Issued encountered 
	- Finding the proper conditions to brake and thrust
	- My distance condition was wrong. Changed it to 1 - currentDistance/maxDistance < tresholdConstant
	- After changing the brake condition, it was still wrong
	- The distance condition was wrong again. it should check if > tresholdConstant
	- Pod still not moving because thrust is 0
	- Angle to checkpoint check became invalid when moved into it's own method
	- Fixed and when retesting, the pod brakes after moving out
	- Division had ints as parameters. Converted to floats
	- Braking doesn't work as intended. It should break before reaching the target, but it won't always go in a straight line from the previous checkpoint.
	
Time elapsed 30 minutes
Todo: update break based on distance remaining to target - constant value perhaps?
Keep moving after checkpoint but at slower pace?	

I realised making the turning window smaller is not actually making me turn faster. 
Instead it takes longer until i thrust while rotating to new checkpoint. Raising the angle back to 90 degrees made me clear wood league 1.

Time elapsed 15 minutes
My braking solution before checkpoints is not working properly because i'm changing the state and updating the thrust next frame. So I made the thrust be updated when the state changed.
Updated thrust treshold constant to 600 to make sure i pass  units.
Encountered issue:
	- There seems to be a higher speed of clearing the race with the threshold at 1000, but there is an edge case where my pod gets stuck because due to speedy turns it doesn't reach the checkpoint.
I added an edge case when I detect not getting any closer to the destination for 1 frame to thrust in that direction. Since I usually get stuck next to checkpoints one thrust should be enough.
It is not enough.
I will keep the stuck logic for now in case it arises later.

The problem is indeed cases where checkpoints are close together.
Letting it run for a while i got promoted to Bronze League. 

Time elapsed: 15 minutes.