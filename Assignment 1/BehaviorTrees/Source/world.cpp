/* Copyright Steve Rabin, 2013. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#include <Stdafx.h>

//#define UNIT_TESTING

World::World(void)
: m_initialized(false)
{

}

World::~World(void)
{
}

void World::InitializeSingletons( void )
{
	//Create Singletons
	m_clock = &g_clock;
	m_database = &g_database;
	m_msgroute = &g_msgroute;
	m_debuglog = &g_debuglog;
	m_random = &g_random;
}

void World::Initialize( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent )
{
	if(!m_initialized)
	{
		g_trees.Initialize();		// initialize behavior tree

		(new Soldier())->GetBody().SetPos(D3DXVECTOR3{ 0.0f, 0.0f, 0.0f });
		(new Soldier())->GetBody().SetPos(D3DXVECTOR3{ 0.0f, 0.0f, 5.f });
		(new Soldier())->GetBody().SetPos(D3DXVECTOR3{ 5.f, 0.0f, 0.0f });
		(new Soldier())->GetBody().SetPos(D3DXVECTOR3{ 5.f, 0.0f, 5.f });

		m_initialized = true;
	}
}


void World::PostInitialize()
{
	g_database.Initialize();
}


void World::Update(float dt)
{
	g_clock.MarkTimeThisTick();
	g_database.Update();
	g_trees.Update(dt);
}

void World::Animate( double dTimeDelta )
{
	g_database.Animate( dTimeDelta );
}

void World::AdvanceTimeAndDraw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	g_database.AdvanceTimeAndDraw( pd3dDevice, pViewProj, dTimeDelta, pvEye );
}

void World::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return( g_database.RestoreDeviceObjects( pd3dDevice ) );
}

void World::InvalidateDeviceObjects( void )
{
	g_database.InvalidateDeviceObjects();
}
