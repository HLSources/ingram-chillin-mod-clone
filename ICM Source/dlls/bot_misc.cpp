
/**************************************************
 * Rho-Bot Source Code                            *
 * by Eric Bieschke (rhobot@botepidemic.com)      *
 * http://www.botepidemic.com/rhobot/             *
 **************************************************
 * You are free to do whatever you want with this *
 * source. All I ask is that (A) you tell me what *
 * you do with it so that I can check it out, (B) *
 * you credit me for any of my code that you use  *
 * in your project, and (C) if you are now, or    *
 * later become involved in the gaming industry   *
 * and think I would be a valuable contribution   *
 * to the team, contact me.   =]                  *
 **************************************************
 * If you have questions about the source, please *
 * don't hesitate to give me a ring. If you make  *
 * improvements to this source that you'd like to *
 * share with others, please let me know and I'll *
 * release your changes with the next source code *
 * release (you will be fully credited for all of *
 * your work). If I inadvertantly did not credit  *
 * either botman or Rich Whitehouse for code taken*
 * from their respective open-source bots, I      *
 * apologize (I did my best to comment what was   *
 * theirs), and if you let me know, I will credit *
 * them in the next source release.               *
 **************************************************/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "bot.h"
#include "bot_misc.h"
#ifdef _WIN32
#include <fstream.h>
#else
#include <fstream>
#endif
#include <string.h>

extern CBasePlayer *CBasePlayerByIndex( int playerIndex ); // client.cpp

CBaseBotGlobals BotGlobals;

BOOL skin_used[MAX_BOT_SKINS] = {
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE
};

//Scott: Style change.  Did not like pointer reference.
//       changed "char *bot_skins[MAX_BOT_SKINS]" to "bot_skins[MAX_BOT_SKINS][64]".
char bot_skins[MAX_BOT_SKINS][64] = {
	"barney",
	"barney",
	"gina",
	"gordon",
	"gman",
	"gman",
	"helmet",
	"hgrunt",
	"nmgrunt",
	"recon",
	"recon",
	"robo",
	"scientist",
	"scientist",
	"zombie",
	"zombie"
};

BOOL name_used[MAX_BOT_NAMES] = {
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE
};

//Scott: Style change.  Did not like pointer reference.
//        changed "char *bot_names[MAX_BOT_NAMES]" to "bot_names[MAX_BOT_NAMES]".
//Scott: List Change.  Removed a few names that I did not like, added some of
//       players that I have played against
char bot_names[MAX_BOT_NAMES][64] = {
	"1000101",			// Lester Lee
	"Ben",				// Benjamin Matasar
	"BoBoKiLLeR",		// Clan 2
	"botman",			// Demi-God
	"Carmack",			// God
	"Chandler Bing",	// Raymond Wong
	"Coffee",			// Demi-God
	"crimrOw",			// Clan No Limit
	"Darkblade",		// Clan Paranoia
	"DarkFig",			// Clan Paranoia
	"DeaDPooL",			// Clan Paranoia
	"Deltha O'Neal",	// Cal Football
	"Eraserman",		// Demi-God
    "Eric",				// Eric Bieschke
	"Grym Reaper",		// Clan Paranoia
	"gumby",			// Clan 2 (Ben)
	"Oberon",			// Eric Alias #2
	"incognito",		// Secret #1
	"Jackal",			// Eric Alias #1
	"J!mmyPop",			// Clan No Limit
	"kastral",			// Clan No Limit
	"kevlar",			// Clan No Limit
	"Mace",				// Clan Paranoia
	"NARLOTEP",			// Secret #2
	"Starbreaker",		// Rho-OZ programmer
	"Vacindak",			// Rho-Bot Contributor
	"Oski",				// Go Cal! (UC Berkeley)
	"phewl",			// Clan 2
	"Scooter",			// Scott
	"Professor Harvey",	// God
	"Shpen",			// CGD (Eric)
	"Slippy",			// CGD
	"Cutter",			// Scott's Brother
	"Dr Deb",			// Sunlit Surf Admin
	"Capt. John",		// Sunlit Surf Admin
	"Commando Dad",		// Sunlit Surf Admin
	"Lazarus Long",		// Sunlit Surf Admin
};

respawn_t bot_respawn[32] = {
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""},
   {FALSE,""}, {FALSE,""}, {FALSE,""}, {FALSE,""}
};

///////////////////////////////////////////////////////////////////////////////
// CBaseBotGlobals Constructor/Destructor
///////////////////////////////////////////////////////////////////////////////

CBaseBotGlobals::CBaseBotGlobals():
AutoAdjCheckTime( 12 )
//AutoAdjustDifficulty( 1 ), //starbreaker - changed AutoAdjustDifficulty from BOOL to int
//ChatPropensity( 20 ),
//DifficultyLevel( 50 ),
//JumpPropensity( 50 ),
//NumBotsDefault( 16 )
{
	strcpy( InitialMap, "boot_camp" );
	strcpy( LastUsedBotName, "NoBotLastUsed" );
}

CBaseBotGlobals::~CBaseBotGlobals()
{
}

///////////////////////////////////////////////////////////////////////////////
// HandleAutoAdjDifficulty
///////////////////////////////////////////////////////////////////////////////

void CBaseBotGlobals::HandleAutoAdjDifficulty( void )
{
	if ( (bot_skill_aa.value) > 0  ) //starbreaker - now use cvar value
	{
		if ( AutoAdjCheckTime < gpGlobals->time )
		{
			float BotFragTotal		= 1; // starting at zero so we don't divide by zero
			float BotDeathTotal		= 1;
			float HumanFragTotal	= 1;
			float HumanDeathTotal	= 1;

			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CBasePlayer *pPlayer = CBasePlayerByIndex( i );

				if ( pPlayer )
				{
					if ( pPlayer->IsBot() )
					{
						BotFragTotal	+= pPlayer->pev->frags;
						BotDeathTotal	+= pPlayer->m_iDeaths;
					}
					else if ( !pPlayer->IsBot() && pPlayer->IsNetClient() )
					{
						HumanFragTotal	+= pPlayer->pev->frags;
						HumanDeathTotal	+= pPlayer->m_iDeaths;
					}
				}
			}

			if ( ( BotFragTotal / BotDeathTotal ) < ( HumanFragTotal / HumanDeathTotal ) )
			{
				IncreaseDifficulty();
			}
			else if ( ( BotFragTotal / BotDeathTotal ) > ( HumanFragTotal / HumanDeathTotal ) )
			{
				DecreaseDifficulty();
			}

			AutoAdjCheckTime += 12; // AutoAdj every 12 seconds
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// ReadRhobotCfg
///////////////////////////////////////////////////////////////////////////////
//Starbreaker removing rhobot.cfg

//BOOL CBaseBotGlobals::ReadRhobotCfg( void )
//{

//starbreaker make rhobot.cfg into cvars
//NumBotsDefault = (bot_number.value);
//AutoAdjustDifficulty = (bot_skill_aa.value);
//DifficultyLevel = (bot_skill.value);
//JumpPropensity = (bot_jump.value);

//		return TRUE;
//}

//starbreaker end
///////////////////////////////////////////////////////////////////////////////
// ChooseRandomName
///////////////////////////////////////////////////////////////////////////////

char *ChooseRandomName()  
{
	int i;
	int name_index = RANDOM_LONG(0, (MAX_BOT_NAMES - 1));
	for (i = 0; i < MAX_BOT_NAMES; i++) // checking if all names are already in use
	{
		if ( name_used[i] == FALSE )
		{
			break;
		}
	}

	//Scott:	Fix. This loop was imbedded in the loop above.
	//			Prevented user from using MAX_BOT_NAMES random bot names.

	if (i == MAX_BOT_NAMES)
	{
		for (i = 0; i < MAX_BOT_NAMES; i++)
		{
			name_used[i] = FALSE;
		}
	}
	
	while ( name_used[name_index] == TRUE)
	{
		name_index++;

		if (name_index == MAX_BOT_NAMES)
		{
			name_index = 0;
		}
	}

	name_used[name_index] = TRUE;
	return bot_names[name_index];
}

///////////////////////////////////////////////////////////////////////////////
// ChooseRandomSkin
///////////////////////////////////////////////////////////////////////////////

char *ChooseRandomSkin( void )
{
	int skin_index = RANDOM_LONG( 0, (MAX_BOT_SKINS - 1) );
	int i;

	for (i = 0; i < MAX_BOT_SKINS; i++) // checking if all skins are already in use
	{
		if ( skin_used[i] == FALSE )
		{
			break;
		}
	}

	//Scott:	Fix. This loop was imbedded in the loop above.
	//			Prevented user from using MAX_BOT_SKINS random bot skins.

	if ( i == MAX_BOT_SKINS )
	{
		for (i = 0; i < MAX_BOT_SKINS; i++)
		{
			skin_used[i] = FALSE;
		}
	}
	
	while ( skin_used[skin_index] == TRUE )
	{
		skin_index++;

		if ( skin_index == MAX_BOT_SKINS )
		{
			skin_index = 0;
		}
	}

	skin_used[skin_index] = TRUE;

	return bot_skins[skin_index];
}

///////////////////////////////////////////////////////////////////////////////
// UseBotCycle - returns the name of the next bot to add (returns NULL if no more left)
///////////////////////////////////////////////////////////////////////////////

//Scott:  Pass integer to reset file.  Now each level starts with the 
//        same bots as listed in the botcycle.txt
//BOOL UseBotCycle( void )
BOOL UseBotCycle( int NumBots )
{
	ifstream	botCycleFile( "icm/botcycle.txt" );
	char		line[MAX_LINE_LENGTH];
	char		lastLastUsedBot[MAX_LINE_LENGTH];
	strcpy( lastLastUsedBot, BotGlobals.LastUsedBotName );

	if ( botCycleFile.bad() )
	{
		return FALSE;
	}
	
	else
	{
		if (NumBots == 0) // Beginning of level or no bots
		{
			botCycleFile.seekg(0); // Reset file
		}

		botCycleFile.getline( line, sizeof(line) ); // slurp first line
		if (line[strlen(line)-1] == '\r') //dos files in linux
			line[strlen(line)-1]='\0'; 
		if ( FStrEq( lastLastUsedBot, "NoBotLastUsed" ) )
		{
			strcpy( BotGlobals.LastUsedBotName, line );
		}
		else
		{
			BOOL NextBotIsIt = FALSE;

			while ( sizeof(line) != 0 && !FStrEq( line, "\0" ) )
			{
				if ( NextBotIsIt )
				{
					strcpy( BotGlobals.LastUsedBotName, line );
					break;
				}

				if ( FStrEq( line, lastLastUsedBot ) )
				{
					NextBotIsIt = TRUE;
				}

				botCycleFile.getline( line, sizeof(line) );
				if (line[strlen(line)-1] == '\r') //dos files in linux
					line[strlen(line)-1]='\0';
			}
		}

		botCycleFile.close();

		if ( FStrEq( BotGlobals.LastUsedBotName, lastLastUsedBot ) )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// originally from Rich Whitehouse's 'Bot Frame'  (hacked by Eric & Ben)
///////////////////////////////////////////////////////////////////////////////

void BotClientPutInServer( edict_t *pEntity, BOOL customBot )
{
	char bot_skin[64] = "gordon";
//Scott: Style change.  Did not like pointer reference.
//	char *color_for_top = "255"; 
//	char *color_for_bottom = "255";
	char color_for_top[4] = "255"; 
	char color_for_bottom[4] = "255";

	CBaseBot *pPlayer;

	entvars_t *pev = &pEntity->v;

	pPlayer = GetClassPtr((CBaseBot *)pev); //Link bot object to the edict

	pPlayer->Spawn();
	pev->flags |= FL_FAKECLIENT; // bot is fakeclient

	if ( customBot )
	{
		char customBotFile[20 + MAX_LINE_LENGTH] = "icm/bots/default.bot";
		sprintf( customBotFile, "icm/bots/%s.bot", BotGlobals.LastUsedBotName );

		if ( pPlayer->Stats.ReadBotFile( customBotFile ) )
		{
			g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer(pEntity), "name", pPlayer->Stats.GetName() );
			strcpy( bot_skin, pPlayer->Stats.GetModel() );
			strcpy( color_for_bottom, pPlayer->Stats.GetBottomColor() );
			strcpy( color_for_top, pPlayer->Stats.GetTopColor() );
		}
		else // randomize
		{
			strcpy( bot_skin, ChooseRandomSkin() );

			int myColor = RANDOM_LONG(1,255);
			sprintf( color_for_bottom, "%d", myColor );

			myColor = RANDOM_LONG(1,255);
			sprintf( color_for_top, "%d", myColor );
		}
	}
	else // randomize
	{
		strcpy( bot_skin, ChooseRandomSkin() );

		int myColor = RANDOM_LONG(1,255);
		sprintf( color_for_bottom, "%d", myColor );

		myColor = RANDOM_LONG(1,255);
		sprintf( color_for_top, "%d", myColor );
	}

	// talking with the engine
	g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(),
		g_engfuncs.pfnGetInfoKeyBuffer(pEntity), "model", bot_skin );
	g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(),
		g_engfuncs.pfnGetInfoKeyBuffer(pEntity), "topcolor", color_for_top);
	g_engfuncs.pfnSetClientKeyValue( pPlayer->entindex(),
		g_engfuncs.pfnGetInfoKeyBuffer(pEntity), "bottomcolor", color_for_bottom );

	g_pGameRules->PlayerThink( pPlayer );

}

///////////////////////////////////////////////////////////////////////////////
// originally from Rich Whitehouse's 'Bot Frame'  (hacked by Eric & Ben)
///////////////////////////////////////////////////////////////////////////////

//Scott:  Routine now passes an integer into the routine to determine if
//        name and skin array should be reset.
//void BotConnect( edict_t *aEntity)
void BotConnect( int NumBots)
{
	edict_t *pEntity;
	BOOL bPickedCustomBot;

	if (NumBots == 0) // reset names, skins (previously in multiplay_gamerules)
	{
		int i;
		for ( i = 0; i < MAX_BOT_SKINS; i++ )
		{
			skin_used[i] = FALSE;
		}

		for ( i = 0; i < MAX_BOT_NAMES; i++ )
		{
			name_used[i] = FALSE;
		}

		strcpy( BotGlobals.LastUsedBotName, "NoBotLastUsed" );
	}

	bPickedCustomBot = UseBotCycle(NumBots);
 
	if (!bPickedCustomBot) { //always use botcycle
		strcpy( BotGlobals.LastUsedBotName, "NoBotLastUsed" );
		bPickedCustomBot = UseBotCycle(NumBots);
	}
//Scott: Style change.  Did not like pointer reference.
//	char *name = "123456789012345678901234567890123456789012345678901234567890";
	char name[64] = "123456789012345678901234567890123456789012345678901234567890123";

	//Scott: Fixed length copies to prevent overwrite of "\0" in name.
	if ( bPickedCustomBot )
	{
		strncpy( name, BotGlobals.LastUsedBotName,63 );
	}
	else
	{
		strncpy( name, ChooseRandomName(), 63 );
	}

	// Scott:  from botman's bot10.  Helps with dedicated server.

	int i, j;
	int length = strlen(name);

	// remove any illegal characters from name...
	for (i = 0; i < length; i++)
	{
		if ((name[i] < ' ') || (name[i] > '~') || (name[i] == '"'))
		{
			for (j = i; j < length; j++)  // shuffle chars left (and null)
				name[j] = name[j+1];
			length--;
		}               
	}

	pEntity = g_engfuncs.pfnCreateFakeClient( name );

	if ( !pEntity )
	{
//Scott:Change print so that dedicated server will work.
//		ClientPrint(&aEntity->v, HUD_PRINTNOTIFY, UTIL_VarArgs( "There are no free client slots. Set maxplayers to a higher value.\n"));
		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, 
			UTIL_VarArgs( "There are no free client slots. Set maxplayers to a higher value.\n"));
		if (IS_DEDICATED_SERVER())
			printf("There are no free client slots. Set maxplayers to a higher value.\n");
		return;
	}

//Scott:  With botman fix above, this line is no longer needed!
//	pEntity->v.netname = MAKE_STRING( name );

	BotClientPutInServer( pEntity, bPickedCustomBot );

//Scott: Needed to fix kicking bot.

	CBaseEntity *someEntity = CBaseEntity::Instance(pEntity);

	for ( i = 0; i < MAX_BOTS; i++ )
	{
		if ( !bot_respawn[i].is_used )
		{
			bot_respawn[i].is_used = TRUE;
			strcpy( bot_respawn[i].name, name );
			someEntity->SpawnIndex = i;
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// TakeDamage - turn bot toward attacker, set as enemy
//              Scott: Adapted from botman's Bot10
///////////////////////////////////////////////////////////////////////////////

int CBaseBot::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker,
			float flDamage, int bitsDamageType )
{
	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);
	int ret_damage;

	// do the damage first...
	ret_damage = CBasePlayer::TakeDamage( pevInflictor, pevAttacker, flDamage,
	                                      bitsDamageType );

	// if the bot injures itself, or by a nonplayer,
	// do not turn to attack...
	if (pAttacker == this || !pAttacker->IsPlayer() ) return ret_damage;

	// if the bot doesn't have an enemy and someone is shooting at it then
	// turn in the attacker's direction...
	if (GetEnemy() == NULL || ( ( gpGlobals->time - Memory.GetEnemyLastSeenTime() ) > 1 ) )
	{
		SetEnemy(NULL); // Make sure enemy is reset

		// face the attacker...
		SetLookAtVec( pAttacker->pev->origin - pev->origin );

		// set enemy if not a teammate
		if (pAttacker->IsAlive() ||
			g_pGameRules->PlayerRelationship( this, pAttacker ) != GR_NOTTEAMMATE )
		{
			SetWantToBeInCombat( TRUE );
			SetEnemy (pAttacker);
		}

		if ( GetEnemy() != NULL)
		{
//			ActionSpeak("I've found an enemy.");

			ActionChooseWeapon();

			SetLookAtBiasVec( Vector( RANDOM_FLOAT(-1,1), RANDOM_FLOAT(-1,1), RANDOM_FLOAT(-1,1) ) );

			Memory.EnemySighted( GetEnemy() );
		}
	}

	return ret_damage;
}
