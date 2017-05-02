#pragma once
class CBussiness
{
public:
	static CBussiness *Initialize();
	static void Uninitialize();
	static CBussiness *GetInstance();

public:
	static CBussiness* m_pInstance;

public:
	CBussiness();
	~CBussiness();

private:
	void Init();
};

