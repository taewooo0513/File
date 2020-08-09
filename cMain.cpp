#include "DXUT.h"
#include "cMain.h"

cMain::cMain()
{
}

cMain::~cMain()
{
}

void cMain::Init()
{
	IMAGE->AddImage("f","./resource/Object001CompleteMap.png");
	mesh =  RENDER->LoadObj("B","./resource/c.obj");
	image = IMAGE->FindImage("f");

}

void cMain::Update()
{
}

void cMain::Render()
{
	RENDER->Cam(Vec2(0,0));
	if (DXUTIsKeyDown(VK_UP))
	{
		pos.y += 60;
	}
	if (DXUTIsKeyDown(VK_RIGHT))
	{
		pos.x += 60;

	}
	if (DXUTIsKeyDown(VK_LEFT))
	{
		pos.x -= 60;

	}
	
	if (DXUTIsKeyDown(VK_DOWN))
	{
		pos.y -= 60;



	}
	
	
	
	//RENDER->Render(image,Vec2(0,0),Vec2(1,1),0);
	RENDER->MeshRender(mesh,pos,Vec3(1,1,1),Vec3(rot,rotL,rotR));
}

void cMain::Release()
{
	cGameManager::ReleaseInstance();
	cRenderManager::ReleaseInstance();
	cImageManager::ReleaseInstance();
	cMeshLoader::ReleaseInstance();
}

void cMain::ResetDevice()
{
}

void cMain::LostDevice()
{
}
