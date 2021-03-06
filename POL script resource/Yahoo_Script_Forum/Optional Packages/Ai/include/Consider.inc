///////////////////////////////////////////////////////////
// Consider.inc
//
// Main consider processing.  DO NOT EDIT THIS FILE!!!
//
// Creator: Mercy
// Contact: mkow@sympatico.ca
//
// Created: 12-Sept-2000
// Updated: 12-Sept-2000
//
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Purpose: Something entered my space.
// Params:  opponent		opponent of the disturbance.
// Returns: 1	Ready to fight!
//			0 	Don't fight it.
///////////////////////////////////////////////////////////
function ConsiderCombat(byref opponent)
	// Run custom hook function.
	if (Consider(opponent))
		return 1;
	endif

	// Only evil critters attack first, so only consider
	// if I am evil, otherwise I don't attack!
	if (mydesc.alignment != "evil")
		return 0;
	endif

	// Do main consider functions.
	if (opponent.npctemplate) // NPC
		if (!CanGroupWith(opponent) and ai.AttackNpc)
			return CanKillIt(opponent);
		endif
	else // Player
		return CanKillIt(opponent);
	endif

	return 0;
endfunction

///////////////////////////////////////////////////////////
// Purpose: Tells if I can kill it.
// Params:  opponent		Ref to mobile.
// Returns: 1		I think I can.
//			0		Too tough for me!
///////////////////////////////////////////////////////////
function CanKillIt(byref opponent)
	var hisstats := opponent.strength + opponent.dexterity + opponent.intelligence;
	var mystats := me.strength + me.dexterity + me.intelligence;

	// Kamikazi fighter! Fight anything, no matter its strength.
	if (ai.Bravery = -1)
		return 1;
	endif

	// If I am grouped, what the hell, attack!
	if (mygroup.size() > 0)
		return 1;
	endif

	if (ai.Bravery = 0)	// Not so brave, attack only
		if (mystats >= hisstats)
			return 1;
		endif
	else // I'm have bravery, but how tough is he?
		var himtougher := 100 - (mystats * 100 / hisstats);

		if (himtougher > ai.Bravery) // I'm not brave enough, move away in fear.
			MoveAwayFrom(opponent);
		else // I'm confident enough to attack.
			return 1;
		endif
	endif

	return 0;
endfunction