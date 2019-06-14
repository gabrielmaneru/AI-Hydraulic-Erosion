Student Name: Gabriel Mañeru
Project Name: Civilisation 7

Summary:
	Four civilisation are called to conquer the island. It's populated with a lot
	of innocent children and some rebels. The leaders of each civilisation will try
	to conquer as much people as possible. The rebels will try to use the art of dancing
	in order to revolte and free the childrens. The civilisation leaders will use their
	people as weapons throwing them against other civilisation. Rebels will try to help
	as much people as they can and flee the battle before is too late. Children will try
	to group up in "villages".
		
Opinion:
-	I really liked having the freedom to create any kind of AI with the little resources we had
	and the limitation of the framework.
	
-	However, I didn't liked the framework itself, it seams like being an old one that has
	been refactored a lot. Most of the logic is distributed along the whole project and there is
	no organisation. Ex: If I want to rotate the character I have to compute the direction
	vector creating and appling the rotation matrices in place, setting the vector and updating
	the	rendering instance. I whould be much simpler with a traditional Transform component.
	Also, the way math library of D3DX works is really annoying passing references and
	not having any operator

Difficulties:
	* I have a lot of problems deleting entities, so the bullets and the leaders will never
		died. You can try to activate DESTRUCTIVE_BEHAVIOUR in "L_BulletBehaviour.cpp" however
		it may lead to a lot of crashes.

Nodes:
	-Selector:
		· ConditionalSelector (IfElse interpretation);
		· HeuristicMove (Run randomly based on some variables);
	-Decorator:
		· AlwaysTrue;
	-Leaf:
		· MoveCloserFriend (Move Kids to their closer friend)
		· DetectEnemy (Detect close Soldiers)
		· RunAwayFromTarget (Run Away from previously detected Target)
		· HasMinions (Checks if the leader has available minions)
		· CheckFreeKids (Check if there are kids in the area)
		· BindFreeKids (Try to convert kids into minions of you team)
		· TravelNearSoldier (Move the minion towards the Soldier)
		· IsKidBinded (Checks if the Kid is free)
		· IsExploring (Checks if the minion is exploring or is waiting)
		· GrabMinion (Choose an minion that is ready and grabs it)
		· ThrowMinion (Throw the minion against the usurpor Soldier)
		· BulletBehaviour (Behaviour of the minion while being thrown)
		· Ready (Intermediate node to control minion pace)
		· Dance (The rebel dance and frees minions)
		
Tree:
	-Soldier
	-Kid
	-Dance(Rebel)
	
Extra Credit: none