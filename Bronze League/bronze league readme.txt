Elapsed 2hr
Algorithm is not optimized yet. I need to take care of closely clumped together points.

Reached bronze league - unlocked collision.

Looking at my first race in the bronze league I got an idea. 
When i leave a checkpoint, i should thrust but not at full speed, until i'm aligned to the next checkpoint.
I tried to set the turning speed to half of maximum but it seems i lost way too much speed. trying 80%. I think this should depend on the angle to next point. hmm. no. the angle and distance.
if distance is high enough, i should be able to adjust direction. if it's close, adjusting direction is dificult due to inertia, so i will overshoot.

Modified the angle constant name to be called Steep angle instead of min thrust angle. Since steep angle is the angle i will check before slowing down.
Added methods for calculating the angle between to vectors.
Spent some time figuring out the proper formula.
Time elapsed 1 hr

Had tons of issues with the formula due to the input data being ints. I constructed my vector2 struct with ints and well.. that was obviously a bad idea. Updated Vector2 to use floats

I could not replicate the formula of angle to next objective. But i realised i might not be able to obtain the value yet, since i need data about the pod rotation.
     I was trying to calculate the angle between the pod and the direction towards the next checkpoint, but instead of the player position, i need his forward vector.
     So if i can't calculate the angle dynamically, the next idea i have is to try to hit the center of each checkpoint in the first lap, so i can save the angle between my pod entering the checkpoint and the next checkpoint.
     This would be my next best chance to get the angle between checkpoints... Or just calculate the angle between the actual checkpoints after the first lap is done.. Hmm..
Turns out my Substract method did not return the computed vector as well... So that was the second issue i had with the angle formula. Not it returns the proper angle.

First i will try to check the angle between the direction between player and next checkpoint, with the direction between the next 2 checkpoints and adjust my speed.
I saved the angle between each checkpoints based on their positions.
1 more hour to clean up everything.

So what I think I'm missing most right now is I don't know my velocity vector, so i can't adjust trajectory. At least now i know the turn angle of the checkpoint.
Not knowing my trajectory, even if i slow down, i won't be able to slow down from a proper distance.
The easiest way i can think of adjusting trajectory is to calculate the angle between the current position and the next checkpoint, versus the angle between the previous position and the next checkpoint.
If the difference between these two angles is too big, that means i'm going away from the checkpoint and i should slow down.

And it does not work as i want it. once aligned, the deviation is small, since my speed is constant. so there is no point in this check.
I thought of a better option.
if i check the deviation of my ship versus the direction vector between the previous checkpoint (the last passed) and the target checkpoint, I could adjust my checkpoint goal so that 
my pod would end up running on the vector between these points. Speed remains a question though.

So if the deviation angle is higher than a constant, i'm aiming for the middle point on the current segment of checkpoints. Hmm, middle point is not right. I should aim for...
The middle point betwen how far i progressed between the 2 checkpoints and the next checkpoint. So i need to project the pod vector position on the segment.
Time spent 1 hour 30

Issues encountered 
     - math formulas.
     My projection formula works. My midpoint formula works. So i must be using the wrong inputs
The inputs were wrong because i understood substracting vectors means looking at the vector between those two vectors, so I thought my origin is moved... Thus i tried to move my position as well.
Scratching all that, i relaxed and cleaned up an unnecesarry substraction.

Aand i don't even need to project anything. I just need the distance between the pod and next checkpoint multiplied by the unit vector between the checkpoints + the previous checkpoint. 
Will check it now. And yes. finally, a success! 
     now the problem is the closer i get, the smaller the distance becomes, so after passing the half point mark, i'm actually target points behind me. 
          Solution - reversed the vectors of the path. Instead of Previous - next, i'm taking the Next to previous vector, normalize it and add the next checkpoint position.
Aaaand it Works! Kind of. I managed to win one attempt in the test run for Bronze League. Will try several more to check for bugs then try out the arena.
Elapsed 1 hr