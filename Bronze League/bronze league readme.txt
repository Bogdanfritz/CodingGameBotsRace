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
I still need to make some avoiding logic for enemies. Currently i'm loosing due to getting hit by enemies.
Elapsed 1 hr 30

I want to test if i can use my thrust to calculate my next position by using my previous logged position . It might not work because i need velocity and i think thrust is acceleration...
Not sure means i want to test it and check against coding game's debug vaue of Speed.
podPosition = deltaPosition * frameCount
deltaPosition = (podPositions[podPositions.size() -2 ] - position).Normalised()

pod speed vector might be the direction vector from my player to the destination * thrust. Late edit: * deltaTime
But i'm off by a small margin of units and i'm not sure why.
elapsed time 30 minutes 

So the first idea is incorrect because i can't account for the inertia of my pod. I still want to find a way using this kind of formula to represent my path so i can check the enemy path, of whom i don't know the thrust
So the thrust is the difference between the current position and the next position. Interesting.

Having the direction of movement of the enemy form his last two positions, i can presume
he's aiming for my next checkpoint and check if our paths would intersect by projecting several
points ahead. It's not the best solution but it's the best i could come up with with my experience.

I am making presumptions that the enemy is aiming for the same checkpoint as I am. But my collision prediction does not take into account the current movement vector, only the direction between the pod and the destination.
Hence... I'm presuming we're always going in a straight line, which we are not. So this idea is not properly defined at the moment either.

 It is not working and I consider I have used enough time for now trying to check collisions and predict my next positions.

 So for now I will focus on trying to smooth out racing and ignore the collisions.

 ------

 First part - updating my curent Algorithm
 Second part - possibly implement a racing strategy

 First part:
     If the movement angle is higher than 90 degrees, slow down for a few frames.
     I am now slowing at half speed when i pass the midpoint towards the next checkpoint and it slows down a bit but it's not enough to win.
Reached highest rank 14 824 / 13,643

Second part:
I have the angle of the turn calculated from a previous commit. But it doesn't know wether it's a left or right turn. I would like to determine that first.
Done. I learned this is what cross product is used for.
Elapsed time - 2 hours of trial and error then research on how to actally rotate vectors. I'm glad i got to find this information out. Now to use it..

The racing strategy i found was to determine three points on the track between each point:
Apex - a point inside the checkpoint after which i start aiming towards the next checkpoint
Turn point - the point where i start turning toward the apex
Brake point - the point where i start braking to prepare for the turn

This strategy will take into consideration the angle of the turn and wether it's a left or right turn. And it will only apply after the first lap.
I decided the apex will be at about 3/4 * radus from the checkpoint middle, at the angle of the turn /2 - to allow for maximum speed
Turn point will be at 10% distance from the checkpoint radius on the path to the previous checkpoint
Brake point will be at the halfway point between checkpoints 
These are for the first iteration. I will have to adjust the turn point and brake point to depend on the angle as well. - Probably 10 degrees ? maybe 15. will trial n error

These are three points. Since my ship is moving, i'll probably need them to have a radius of at least 100 units.
Elapsed time - 1 hours

I implemented the logic for the stragey and have found that i did not take into account the angle between my pod and the points.
In a perfect scenario, my movement direction would be the same as the path direction, but that's not the case most of the time.

So I think i should update the points dynamically, not statically once the first lap is done...
But before that i made some improvements to the code.

Now. 
What i need to care about is the direction angle of my movement towards the points.
I should check 
     preivous checkpoint to brake point angle vs my own - brake point.
     brake point - turnpoint vs my own - turn point
     turnpoint - apex vs my own - apex
For now will only check the angle and if it's too steep, i'll force it to the next one

After 3 hours of work, it kinda works, but the results seem worse than my alignment strategy. So I will keep using that one and improve it with what i've learned from this strategy.

2 more hours and the improvments don't work quite as well on the same test sample. but neither did my previous submit on the same test case.
     
1 hr later I've managed to get improvements by changing the steep angle to 45 which allows me to turn faster and also to adjust my path faster.
What i need to do next - adjust the angle of my target depending on the turn angle so that i don't loose as much momentum when i pass checkpoints

2 hrs work:
Once the first lap is finished, i'm now creating a set of two points on the previous and next path accordingly, on each checkpoint and use these new points as destinations
It works right now, but it's not good enough to finish a race properly because they can be too close together.
So i want to increase the angles a bit.