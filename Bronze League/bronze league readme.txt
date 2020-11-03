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