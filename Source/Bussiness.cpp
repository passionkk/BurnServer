#include "Bussiness.h"
#include "CommonDefine.h"
#include "HttpServerModule.h"

CBussiness* CBussiness::m_pInstance = NULL;

CBussiness::CBussiness()
{
}

CBussiness::~CBussiness()
{
}

CBussiness * CBussiness::Initialize()
{
	return CBussiness::GetInstance();
}

void CBussiness::Uninitialize()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CBussiness * CBussiness::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CBussiness;
		m_pInstance->Init();
	}

	return m_pInstance;
}

void CBussiness::Init()
{
	HttpServerModule::Initialize();
}