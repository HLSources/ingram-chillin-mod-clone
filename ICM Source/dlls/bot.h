
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
#ifdef __GNUC__
using namespace std;
#endif

#ifndef RHOBOT_H
#define RHOBOT_H

#include "game.h"
#include "bot_misc.h"

const int DISTANCE_FAR	= 700;
const int DISTANCE_NEAR	= 200;
const int DISTANCE_MAX = 8192; // this is actually incorrect because this is the length of the sides of the worldbox,
							   // not the diagonal through the middle from corner to corner

// ActionLook
const int SEARCH_DISTANCE = 256;

// SteerGroup
const int ESCORT_DISTANCE = 64;
const int FOLLOW_DISTANCE = 64;
const int GROUPING_DISTANCE = 128;

// HandleObstacles
const int WALL_ALIGN_DISTANCE = 32;
const int AVOID_AHEAD_DISTANCE = 256;
const float ALLOWED_TURNING_CHANGE = 20; // degrees per frame
const float YAW_VARIATION = .5;
const float SIDE_BUFFER_DISTANCE = 128;

typedef enum
{
	AIM_HEAD,
	AIM_BODY,
	AIM_SPLASH
} AIM;

typedef struct
{
	char *ammo_name;
	char *weapon_name;
	int max_carry;
} AmmoCheck;


//Scott:  Changed enums to allow bit-wise operators in bot_move.cpp
typedef enum
{
	NORTH = 1,		// (binary: 0001) NORTH = 1
	EAST,			// (binary: 0010) EAST = 2
	NORTHEAST,		// (binary: 0011) NORTHEAST = NORTH + EAST = 3
	SOUTH,			// (binary: 0100) SOUTH = 4
	SOUTHEAST = 6,	// (binary: 0110) SOUTHEAST = SOUTH + EAST = 6
	WEST = 8,		// (binary: 1000) WEST = 8
	NORTHWEST,		// (binary: 1001) NORTHWEST = NORTH + WEST = 9
	SOUTHWEST = 12,	// (binary: 1100) SOUTHWEST = SOUTH + WEST = 12
	UNKNOWN = 15	// (binary: 1111) UNKNOWN = 15 (All directions)
} DIRECTION;

typedef enum
{
	NONE,
	LEFT,
	RIGHT
} TURNING_DIRECTION;

typedef enum
{
	STEER_WANDER,
	STEER_WANT_ITEM,
	STEER_GROUP_ASSEMBLE,
	STEER_WANT_ENEMY,
	STEER_GROUP_BOT,
	STEER_GROUP_HUMAN,
	STEER_SURVIVAL
} STEER;

///////////////////////////////////////////////////////////////////////////////
// CBaseBotFightStyle Class
///////////////////////////////////////////////////////////////////////////////

class CBaseBotFightStyle
{
private:
	AIM			AimAt;
	BOOL		bHoldDownAttack;
	BOOL		bSecondaryFire;
	CBasePlayer	*pOwner;

	void	SetAimAt( const AIM newAim ) { AimAt = newAim; }
	void	SetHoldDownAttack( const BOOL newValue ) { bHoldDownAttack = newValue; }
	void	SetSecondaryFire( const BOOL newValue ) { bSecondaryFire = newValue; }

	void	RandomizeAimAtHead( const int AimAtHeadPropensity );
	void	RandomizeSecondaryFire( const int SecondaryFirePropensity );

	void	UseCrossbow( void );
	void	UseCrowbar( void );
	void	UseDuebel( void );
	void	UseEgon( void );
	void	UseGauss( void );
	void	UseGlock( void );
	void	UseHandGrenade( void );
	void	UseHornetGun( void );
	void	UseMP5( void );
	void	UsePython( void );
	void	UseRPG( void );
	void	UseSatchel( void );
	void	UseShotgun( void );
	void	UseSnark( void );
	void	UseTripMine( void );

public:

	CBaseBotFightStyle( CBasePlayer *pPlayer );
	~CBaseBotFightStyle();

	AIM		GetAimAt() { return AimAt; }
	BOOL	GetHoldDownAttack() { return bHoldDownAttack; }
	BOOL	GetSecondaryFire() { return bSecondaryFire; }

	void	DispatchWeaponUse( CBasePlayerItem *ActiveItem );
};

///////////////////////////////////////////////////////////////////////////////
// CBaseBotMemory Class
///////////////////////////////////////////////////////////////////////////////

class CBaseBotMemory
{
private:
	float	EnemyFirstSeenTime;
	BOOL	bEnemyInSight;
	Vector	EnemyLastSeenPosition;
	float	EnemyLastSeenTime;

	Vector	Prev1EnemyVelocity;
	Vector	Prev1LookAtVec;
	Vector	Prev1Velocity;
	Vector	Prev2EnemyVelocity;
	Vector	Prev2LookAtVec;
	Vector	Prev2Velocity;
	Vector	Prev3EnemyVelocity;
	Vector	Prev3LookAtVec;
	Vector	Prev3Velocity;
	Vector	Prev4EnemyVelocity;
	Vector	Prev4LookAtVec;
	Vector	Prev4Velocity;
	Vector	PrevOrigin;

	void	SetEnemyFirstSeenTime( const float newTime ) { EnemyFirstSeenTime = newTime; }
	void	SetEnemyInSight( const BOOL newValue ) { bEnemyInSight = newValue; }
	void	SetEnemyLastSeenPosition( const Vector &newVec ) { EnemyLastSeenPosition = newVec; }
	void	SetEnemyLastSeenTime( const float newTime ) { EnemyLastSeenTime = newTime; }
	void	SetPrev1EnemyVelocity( const Vector &newVec ) { Prev1EnemyVelocity = newVec; }
	void	SetPrev1LookAtVec( const Vector &newVec ) { Prev1LookAtVec = newVec; }
	void	SetPrev1Velocity( const Vector &newVec ) { Prev1Velocity = newVec; }
	void	SetPrev2EnemyVelocity( const Vector &newVec ) { Prev2EnemyVelocity = newVec; }
	void	SetPrev2LookAtVec( const Vector &newVec ) { Prev2LookAtVec = newVec; }
	void	SetPrev2Velocity( const Vector &newVec ) { Prev2Velocity = newVec; }
	void	SetPrev3EnemyVelocity( const Vector &newVec ) { Prev3EnemyVelocity = newVec; }
	void	SetPrev3LookAtVec( const Vector &newVec ) { Prev3LookAtVec = newVec; }
	void	SetPrev3Velocity( const Vector &newVec ) { Prev3Velocity = newVec; }
	void	SetPrev4EnemyVelocity( const Vector &newVec ) { Prev4EnemyVelocity = newVec; }
	void	SetPrev4LookAtVec( const Vector &newVec ) { Prev4LookAtVec = newVec; }
	void	SetPrev4Velocity( const Vector &newVec ) { Prev4Velocity = newVec; }
	void	SetPrevOrigin( const Vector &newVec ) { PrevOrigin = newVec; }

public:

	CBaseBotMemory();
	~CBaseBotMemory();

	float	GetEnemyFirstSeenTime() { return EnemyFirstSeenTime; }
	float	GetEnemyInSight() { return bEnemyInSight; }
	Vector	GetEnemyLastSeenPosition() { return EnemyLastSeenPosition; }
	float	GetEnemyLastSeenTime() { return EnemyLastSeenTime; }
	Vector	GetPrev1EnemyVelocity() { return Prev1EnemyVelocity; }
	Vector	GetPrev1LookAtVec() { return Prev1LookAtVec; }
	Vector	GetPrev1Velocity() { return Prev1Velocity; }
	Vector	GetPrev2EnemyVelocity() { return Prev2EnemyVelocity; }
	Vector	GetPrev2LookAtVec() { return Prev2LookAtVec; }
	Vector	GetPrev2Velocity() { return Prev2Velocity; }
	Vector	GetPrev3EnemyVelocity() { return Prev3EnemyVelocity; }
	Vector	GetPrev3LookAtVec() { return Prev3LookAtVec; }
	Vector	GetPrev3Velocity() { return Prev3Velocity; }
	Vector	GetPrev4EnemyVelocity() { return Prev4EnemyVelocity; }
	Vector	GetPrev4LookAtVec() { return Prev4LookAtVec; }
	Vector	GetPrev4Velocity() { return Prev4Velocity; }
	Vector	GetPrevOrigin() { return PrevOrigin; }

	void	EnemyInSight( CBaseEntity *pEnemy );
	void	EnemyOutOfSight( void );
	void	EnemySighted( CBaseEntity *pEnemy );
	void	EndFrameUpdate( CBasePlayer *pOwner );
	
};

///////////////////////////////////////////////////////////////////////////////
// CBaseBotStats Class
///////////////////////////////////////////////////////////////////////////////

class CBaseBotStats
{ // TODO: eventually we can use the SetTraits to auto-adjust difficulty midgame
private:

	// could use string_t with MAKE_STRING, ALLOC_STRING, & STRING instead
	char	Name[64];
	char	Model[64];
//	char	ChatFile[32];
	char	TopColor[4];
	char	BottomColor[4];

	float	PrefCrossbow;
	float	PrefCrowbar;
	float	PrefHandGrenade;
	float	PrefMP5;
	float	PrefRPG;

	float	TraitAccuracy; // traits are all [1,100]
	float	TraitAggression;
//	float	TraitChatPropensity;
	float	TraitJumpPropensity;
	float	TraitPerception;
	float	TraitReflexes;

	void	SetName( char *newString ) { strcpy( Name, newString ); }
	void	SetModel( char *newString ) { strcpy( Model, newString ); }
//	void	SetChatFile( char *newString ) { strcpy( ChatFile, newString ); }
	void	SetTopColor( char *newString ) { strcpy( TopColor, newString ); }
	void	SetBottomColor( char *newString ) { strcpy( BottomColor, newString ); }
	void	SetPrefCrossbow( const float newPref ) { PrefCrossbow = newPref; }
	void	SetPrefCrowbar( const float newPref ) { PrefCrowbar = newPref; }
	void	SetPrefHandGrenade( const float newPref ) { PrefHandGrenade = newPref; }
	void	SetPrefMP5( const float newPref ) { PrefMP5 = newPref; }
	void	SetPrefRPG( const float newPref ) { PrefRPG = newPref; }
	void	SetTraitAccuracy( const float newTrait ) { TraitAccuracy = newTrait; }
	void	SetTraitAggression( const float newTrait ) { TraitAggression = newTrait; }
//	void	SetTraitChatPropensity( const float newTrait ) { TraitChatPropensity = newTrait; }
	void	SetTraitJumpPropensity( const float newTrait ) { TraitJumpPropensity = newTrait; }
	void	SetTraitPerception( const float newTrait ) { TraitPerception = newTrait; }
	void	SetTraitReflexes( const float newTrait ) { TraitReflexes = newTrait; }

public:

	CBaseBotStats();
	~CBaseBotStats();

	char	*GetName() { return Name; }
	char	*GetModel() { return Model; }
//	char	*GetChatFile() { return ChatFile; }
	char	*GetTopColor() { return TopColor; }
	char	*GetBottomColor() { return BottomColor; }
	float	GetPrefCrossbow() { return PrefCrossbow; }
	float	GetPrefCrowbar() { return PrefCrowbar; }
	float	GetPrefHandGrenade() { return PrefHandGrenade; }
	float	GetPrefMP5() { return PrefMP5; }
	float	GetPrefRPG() { return PrefRPG; }
	float	GetTraitAccuracy() { return FDifficultyAdjusted( TraitAccuracy ); }
	float	GetTraitAggression() { return TraitAggression; }
	float	GetTraitJumpPropensity() { return FDifficultyAdjusted( TraitJumpPropensity * ( bot_jump.value / 100 ) ); }
//	float	GetTraitJumpPropensity() { return FDifficultyAdjusted( TraitJumpPropensity * ( BotGlobals.JumpPropensity / 100 ) ); }
//	float	GetTraitChatPropensity() { return ( TraitChatPropensity * ( BotGlobals.ChatPropensity / 100 ) ); }
	float	GetTraitPerception() { return FDifficultyAdjusted( TraitPerception ); }
	float	GetTraitReflexes() { return FDifficultyAdjusted( TraitReflexes ); }

	float	FDifficultyAdjusted( float currentTrait );
	float   FindWeaponDesire( CBasePlayerItem *pWeapon, float DistanceToEnemy );
	BOOL	ReadBotFile( char *filename );

};

///////////////////////////////////////////////////////////////////////////////
// CBaseBot Class
///////////////////////////////////////////////////////////////////////////////

class CBaseBot : public CBasePlayer
{
protected:
	BOOL		bCalledAimThisFrame;
	Vector		DesiredVelocity;
	BOOL		bGoUpOnLadder;
	Vector		LookAtBiasVec;
	Vector		LookAtVec;
	float		MovedDistance;
	float		MoveForward;
	float		MoveStrafe;
	float		MoveVertical;
	float		MSec;
	int			MSecCounter;
	//Vacindak:  Memory Leak Fix
	EHANDLE		pEnemy;
	EHANDLE		pGoal;
	//End Fix
	DIRECTION	RoamDirection;
	int			SteerCallCounter;
	STEER		SteerCallPrecedence;
	float		TimeGoalCheck;
	float		TimeGoalCheckDelay;
	float		TimeMSecCheck;
	TURNING_DIRECTION	TurningDirection;
	BOOL		bWantToBeInCombat;

	void		SetCalledAimThisFrame( const BOOL newValue ) { bCalledAimThisFrame = newValue; }
	void		SetDesiredVelocity( const Vector &newVec ) { DesiredVelocity = newVec; }
	void		SetEnemy( CBaseEntity *newEnemy ) { pEnemy = newEnemy; }
	void		SetGoal( CBaseEntity *newGoal ) { pGoal = newGoal; }
	void		SetGoUpOnLadder( const BOOL newValue ) { bGoUpOnLadder = newValue; }
	void		SetLookAtBiasVec( const Vector &newVec ) { LookAtBiasVec = newVec; }
	void		SetLookAtVec( const Vector &newVec ) { LookAtVec = newVec; }
	void		SetMovedDistance( const float newValue ) { MovedDistance = newValue; }
	void		SetMoveForward( const float newValue ) { MoveForward = newValue; }
	void		SetMoveStrafe( const float newValue ) { MoveStrafe = newValue; }
	void		SetMoveVertical( const float newValue ) { MoveVertical = newValue; }
	void		SetMSec( const float newValue ) { MSec = newValue; }
	void		SetMSecCounter( const int newValue ) { MSecCounter = newValue; }
	void		SetRoamDirection( const DIRECTION newDirection ) { RoamDirection = newDirection; }
	void		SetSteerCallCounter( const int newValue ) { SteerCallCounter = newValue; }
	void		SetSteerCallPrecedence( const STEER newValue ) { SteerCallPrecedence = newValue; }
	void		SetTimeGoalCheck( const float newValue ) { TimeGoalCheck = newValue; }
	void		SetTimeGoalCheckDelay( const float newValue ) { TimeGoalCheckDelay = newValue; }
	void		SetTimeMSecCheck( const float newValue ) { TimeMSecCheck = newValue; }
	void		SetTurningDirection( const TURNING_DIRECTION newValue ) { TurningDirection = newValue; }
	void		SetWantToBeInCombat( const BOOL newValue ) { bWantToBeInCombat = newValue; }

public:

	CBaseBotFightStyle	FightStyle;
	CBaseBotMemory	Memory;
	CBaseBotStats	Stats;

	CBaseBot();
	~CBaseBot();

	BOOL		GetCalledAimThisFrame() { return bCalledAimThisFrame; }
	Vector		GetDesiredVelocity() { return DesiredVelocity; }
	//Vacindak:  Memory Leak Fix.
	EHANDLE		GetEnemy() { return pEnemy; }
	EHANDLE		GetGoal() { return pGoal; }
	//End Memory Leak Fix
	BOOL		GetGoUpOnLadder() { return bGoUpOnLadder; }
	Vector		GetLookAtBiasVec() { return LookAtBiasVec; }
	Vector		GetLookAtVec() { return LookAtVec; }
	float		GetMovedDistance() { return MovedDistance; }
	float		GetMoveForward() { return MoveForward; }
	float		GetMoveStrafe() { return MoveStrafe; }
	float		GetMSec() { return MSec; }
	int			GetMSecCounter() { return MSecCounter; }
	float		GetMoveVertical() { return MoveVertical; }
	DIRECTION	GetRoamDirection() { return RoamDirection; }
	//Scott: GetServerMaxSpeed now return the max speed the bot is allowed to move
	float		GetServerMaxSpeed() { return	bot_speed.value; }
	int			GetSteerCallCounter() { return SteerCallCounter; }
	STEER		GetSteerCallPrecedence() { return SteerCallPrecedence; }
	float		GetTimeGoalCheck() { return TimeGoalCheck; }
	float		GetTimeGoalCheckDelay() { return TimeGoalCheckDelay; }
	float		GetTimeMSecCheck() { return TimeMSecCheck; }
	TURNING_DIRECTION	GetTurningDirection() { return TurningDirection; }
	BOOL		GetWantToBeInCombat() { return bWantToBeInCombat; }

//	void		ActionChat( void );
	DIRECTION	ActionChooseDirection( void );
	void		ActionChooseGoal( void );
	void		ActionChooseWeapon( void );
	void		ActionLook( int SearchDistance = SEARCH_DISTANCE );
	void		ActionOpenFire( void );
	BOOL		ActionReload( void );
	void		ActionSpeak( char *pText );

	void		AimAtBias( void );
	void		AimAtEnemy( void );
	void		AimAtEntity( CBaseEntity *pEntity );
	void		AimWander( void );

	void		BotThink( void );

	BOOL		CheckHasDecentWeapon( void );
	BOOL		CheckNotice( CBaseEntity *pEntity );
	BOOL		CheckPathObstructedFront( Vector &IdealVelocity, TraceResult &tr );
	BOOL		CheckPathObstructedSides( Vector &IdealVelocity, TraceResult &tr, TraceResult &tr2 );

	BOOL		CheckWallOnLeft( void );
	BOOL		CheckWallOnRight( void );

	BOOL		CheckVisible( CBaseEntity *pEntity );
	//Scott: check to see if bot has been kicked
	BOOL		CheckBotKick();

	void		HandleMovement( void );
	Vector		HandleObstacles( Vector &IdealVelocity );
	void		HandleTime( void );

	void		SteerGroupAlign( void );
	void		SteerGroupCohesion( void );
	void		SteerGroupEscort( CBaseEntity *escortTarget );
	void		SteerGroupFlock( void );
	void		SteerGroupFollow( CBaseEntity *followTarget );
	void		SteerGroupSeparation( void );

	void		SteerPrimitiveEvade( CBaseEntity *evadeTarget );
	void		SteerPrimitiveFlee( CBaseEntity *fleeTarget );
	void		SteerPrimitiveFlee( const Vector &fleeTarget );
	void		SteerPrimitiveOnLadder( void );
	void		SteerPrimitivePursue( CBaseEntity *pursueTarget, Vector offset = Vector(0,0,0) );
	void		SteerPrimitiveSeek( CBaseEntity *seekTarget );
	void		SteerPrimitiveSeek( const Vector &seekTarget );
	void		SteerPrimitiveWander( float WanderRate = 20 );
	void		SteerPrimitiveWanderImproved( void );

	// TODO: these SteerSafes are near duplicates
	void		SteerSafeGroupAlign( STEER precedence = STEER_GROUP_ASSEMBLE );
	void		SteerSafeGroupCohesion( STEER precedence = STEER_GROUP_ASSEMBLE );
	void		SteerSafeGroupEscort( CBaseEntity *escortTarget, STEER precedence = STEER_GROUP_HUMAN );
	void		SteerSafeGroupFlock( STEER precedence = STEER_GROUP_BOT );
	void		SteerSafeGroupFollow( CBaseEntity *followTarget, STEER precedence = STEER_GROUP_HUMAN );
	void		SteerSafeGroupSeparation( STEER precedence = STEER_GROUP_ASSEMBLE );

	void		SteerSafeEvade( CBaseEntity *evadeTarget, STEER precedence = STEER_SURVIVAL );
	void		SteerSafeFlee( CBaseEntity *fleeTarget, STEER precedence = STEER_SURVIVAL );
	void		SteerSafeFlee( const Vector &fleeTarget, STEER precedence = STEER_SURVIVAL );
	void		SteerSafeOnLadder( STEER precedence = STEER_SURVIVAL );
	void		SteerSafePursue( CBaseEntity *pursueTarget, STEER precedence = STEER_WANT_ENEMY, Vector offset = Vector(0,0,0) );
	void		SteerSafeSeek( CBaseEntity *seekTarget, STEER precedence = STEER_WANT_ITEM );
	void		SteerSafeSeek( const Vector &seekTarget, STEER precedence = STEER_WANT_ITEM );
	void		SteerSafeWander( float WanderRate = 20, STEER precedence = STEER_WANDER );
	void		SteerSafeWanderImproved( STEER precedence = STEER_WANDER );

	void		ThinkDead( void );
	void		ThinkMood( void );
	void		ThinkStart( void );
	void		ThinkSteering( void );
	void		ThinkValidateEnemy( void );
	void		ThinkValidateGoal( void );

	int			Classify( void ) { return CLASS_PLAYER; }
	virtual	BOOL IsPlayer( void ) { return TRUE; }
	virtual	BOOL IsBot( void ) { return TRUE; }
	virtual BOOL IsNetClient( void ) { return FALSE; }

	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker,
	                float flDamage, int bitsDamageType );

};

#endif /*RHOBOT_H*/

