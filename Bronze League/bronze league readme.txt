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

