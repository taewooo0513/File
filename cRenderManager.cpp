#include "DXUT.h"
#include "cRenderManager.h"

cRenderManager::cRenderManager()
{
	Loader = new cMeshLoader;
	Device->CreateVertexBuffer(sizeof(sVerTex)*4,D3DUSAGE_WRITEONLY,sVerTex::FVF,D3DPOOL_DEFAULT,&vb,NULL);
	
	
	sVerTex * pVerTices = nullptr;

	vb->Lock(0,0,(void**)&pVerTices,0);

	pVerTices[0].Pos = Vec3(-0.5, 0.5, 0);
	pVerTices[1].Pos = Vec3(0.5, 0.5, 0);
	pVerTices[2].Pos = Vec3(-0.5, -0.5, 0);
	pVerTices[3].Pos = Vec3(0.5, -0.5, 0);

	pVerTices[0].uv = Vec2(0, 0);
	pVerTices[1].uv = Vec2(1, 0);
	pVerTices[2].uv = Vec2(0, 1);
	pVerTices[3].uv = Vec2(1, 1);

	pVerTices[0].normal = Vec3(0,0,0);
	pVerTices[1].normal = Vec3(0, 0, 0);
	pVerTices[2].normal = Vec3(0, 0, 0);
	pVerTices[3].normal = Vec3(0, 0, 0);

	vb->Unlock();
	

	Device->CreateIndexBuffer(sizeof(WORD) * 6, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL);
	void * pIndice = NULL;
	
	WORD idx[] = { 1,2,0,1,3,2 };

	ib->Lock(0, 0, &pIndice, 0);
	memcpy(pIndice, idx, sizeof(WORD) * 6);
	ib->Unlock();
	

	DWORD* Attribute = nullptr;
	

	
	ZeroMemory(&light,sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_POINT;
	light.Diffuse.r = 0.5f;
	light.Diffuse.g = 0.5f;
	light.Diffuse.b = 0.5f;
	light.Range =10000;
	light.Attenuation0 = 0.00000001f;
	light.Position.x = 0;
	light.Position.y = 100;
	light.Position.z = 0;

	Device->SetLight(1,&light);
	Device->LightEnable(1,TRUE);
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	Device->SetRenderState(D3DRS_AMBIENT,0x00202020);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetFVF(sVerTex::FVF);
	Device->SetStreamSource(0, vb, 0, sizeof(sVerTex));
	Device->SetIndices(ib);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60), 16.f / 9.f, 1.f, 50000.f);
	Device->SetTransform(D3DTS_PROJECTION, &matProj);


}

cRenderManager::~cRenderManager()
{
	vb->Release();
	ib->Release();
	for (auto iter : mMeshs)
	{
		SAFE_DELETE(iter.second);
	}
	mMeshs.clear();
}

Mesh * cRenderManager::LoadObj(const string & key, const string & path)
{
	auto find = mMeshs.find(key);
	if (find != mMeshs.end())
	{
		return find->second;
	}
	Mesh*mesh = new Mesh;
	Loader->ObjLoad(mesh, path);
	mMeshs[key] = mesh;
	return mesh;
}

Mesh * cRenderManager::FindObj(const string & key)
{
	auto find = mMeshs.find(key);
	if (find == mMeshs.end())
	{
		return nullptr;
	}
	return find->second;
}

void cRenderManager::Cam(Vec2 Pos)
{
	D3DXMATRIXA16 matCam;
	CamPos = D3DXVECTOR3(Pos.x, Pos.y, -1000);
	CamAt = D3DXVECTOR3(Pos.x, Pos.y, 0);
	CamUp = D3DXVECTOR3(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&matCam, &CamPos, &CamAt, &CamUp);

	Device->SetTransform(D3DTS_VIEW, &matCam);
}

void cRenderManager::MeshRender(Mesh * mesh, D3DXVECTOR3 Pos , D3DXVECTOR3 size, D3DXVECTOR3 Rot)
{

	D3DXMATRIXA16 matP, matS,matR, matR1, matR2, matR3, matW;
	D3DXMatrixTranslation(&matP, Pos.x,Pos.y,Pos.z);
	D3DXMatrixScaling(&matS,size.x,size.y,size.z);
	D3DXMatrixRotationX(&matR1,Rot.x);
	D3DXMatrixRotationY(&matR2, Rot.y);
	D3DXMatrixRotationZ(&matR3, Rot.z);
	matR = matR1 * matR2*matR3;
	matW = matR*matP;
	Device->SetTransform(D3DTS_WORLD,&matW);
	for (int i = 0; i < mesh->vMaterial.size(); ++i)
	{
		auto mat = mesh->vMaterial[i];


		Device->SetMaterial(&mat->material);

		if (mat->DiffuseMap != nullptr)
			Device->SetTexture(0, mat->DiffuseMap->texturePtr);
		else
			Device->SetTexture(0, nullptr);

		mesh->LPD3DMesh->DrawSubset(i);
	}


}

void cRenderManager::Render(cTexture * texture, D3DXVECTOR2 Pos, D3DXVECTOR2 size, float rot)
{
	D3DXMATRIXA16 matP, matS, matR, matW;
	D3DXMatrixTranslation(&matP, Pos.x, Pos.y, 0);
	D3DXMatrixScaling(&matS, texture->info.Width*size.x,texture->info.Height* size.y, 0);

	D3DXMatrixRotationZ(&matR,rot);
	matW = matS * matR*matP;

	Device->SetTransform(D3DTS_WORLD, &matW);
	Device->SetTexture(0,texture->texturePtr);
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

}
