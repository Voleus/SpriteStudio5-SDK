//#include "stdafx.h"
#include <d3d9.h>
#include "d3dsimple.h"
//#include "Sprite.h"
//#include "../Misc/Task.h"
//#include "../Misc/Debug.h"

//extern	void	InitGameShader();
//extern	void	ExitGameShader();

/* �R���p�C���I�v�V���� */


//�X�^�e�B�b�N�ϐ�
CDirectDrawSimple*	CDirectDrawSimple::myInst = 0;
bool			CDirectDrawSimple::m_DeviceLost = false;

static	bool	IntervalImmediate = false;

/* ******************************************************** */
//! DirectDraw��������
/*!

@retval	���ہ@�i�@TRUE , FALSE )

*/
BOOL CDirectDrawSimple::Cleanup()
{
	SAFE_RELEASE(m_pd3dDev);
	SAFE_RELEASE(m_pD3D);

	return TRUE;
}
/* ******************************************************** */
//! DirectDraw�쐬���̃p�����[�^���쐬
/*!

*/
void	CDirectDrawSimple::CreateCaps()
{

	// �\���n�̃f�o�C�X�\�͂��擾
	D3DCAPS9	HardwareCaps;
	HRESULT	hr = GetD3D()->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &HardwareCaps);
	if(0 != (HardwareCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE))
	{	/* DirectX�����������������Ō��邱�Ƃ��\ */
		IntervalImmediate = false;	/* ����������DirectX�ɔC���� */
	} else {	/* DirectX�����������������Ō��邱�Ƃ��s�\ */
		IntervalImmediate = true;	/* �������������O�ōs�� */
	}

	::ZeroMemory(&m_d3dParam_window, sizeof(m_d3dParam_window));
	m_d3dParam_window.BackBufferWidth = m_dwWidth;
	m_d3dParam_window.BackBufferHeight = m_dwHeight;
	m_d3dParam_window.BackBufferFormat=D3DFMT_A8R8G8B8;
	m_d3dParam_window.BackBufferCount = 1;
	m_d3dParam_window.Windowed = TRUE;
	m_d3dParam_window.EnableAutoDepthStencil = TRUE;
	m_d3dParam_window.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dParam_window.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(true == IntervalImmediate)	m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	else							m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	::ZeroMemory(&m_d3dParam_full, sizeof(m_d3dParam_full));
	m_d3dParam_full.BackBufferWidth = m_dwWidth;
	m_d3dParam_full.BackBufferHeight = m_dwHeight;
	m_d3dParam_full.BackBufferFormat= D3DFMT_A8R8G8B8;
	m_d3dParam_full.BackBufferCount = 1;
	m_d3dParam_full.Windowed = FALSE;
	m_d3dParam_full.EnableAutoDepthStencil = TRUE;
	m_d3dParam_full.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dParam_full.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(true == IntervalImmediate)	m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	else							m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
}

void	CDirectDrawSimple::DeviceReset()
{

	m_d3dParam_window.BackBufferWidth = m_dwWidth;
	m_d3dParam_window.BackBufferHeight = m_dwHeight;
	m_d3dParam_full.BackBufferWidth = m_dwWidth;
	m_d3dParam_full.BackBufferHeight = m_dwHeight;
	m_d3dParam_current.BackBufferWidth = m_dwWidth;
	m_d3dParam_current.BackBufferHeight = m_dwHeight;

	//�擾�ς݃o�b�t�@�̃����[�X
	SAFE_RELEASE( m_pBackBuffer );

	HRESULT	hr = m_pd3dDev->TestCooperativeLevel();

	if ( hr == S_OK || hr == D3DERR_DEVICENOTRESET )
	{
//		if ( hr == S_OK ) 
//			CommonLib::DEBUG_PRINTF( " TestCooperativeLevel : S_OK\n" );
//		if ( hr == D3DERR_DEVICENOTRESET ) 
//			CommonLib::DEBUG_PRINTF( " TestCooperativeLevel : D3DERR_DEVICENOTRESET\n" );


		//HRESULT hr;
		hr = m_pd3dDev->Reset( &m_d3dParam_current );

		if (FAILED(hr))
		{
			m_DeviceLost = true;
			if (hr == D3DERR_DEVICELOST)
			{
				//m_DeviceLost = true;
				return ;
			}else if( hr == D3DERR_DEVICENOTRESET ){
				//THROW_ERROR_MESSAGE( "Error : D3DERR_DEVICENOTRESET" );
				return ;					
			}else if ( hr == D3DERR_INVALIDCALL )
			{
				//THROW_ERROR_MESSAGE( "Error : D3DERR_INVALIDCALL" );
				return ;
			}else{
				//THROW_ERROR_MESSAGE( "Error : DirectX Reset" );
				return ;
			}
		}

		m_pd3dDev->GetBackBuffer(0, 0, 
			D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);

		m_currentRenderTarget = m_pBackBuffer;
		(m_RestoreDeviceCallback)();

	}
}

void	CDirectDrawSimple::ChangeDisplayMode( DISPLAYMODE mode )
{
	if ( mode != m_current_displaymode )
	{
		//D3DPRESENT_PARAMETERS d3dParam;
		if ( mode == DISPLAYMODE_WINDOW )
		{
			m_d3dParam_current = m_d3dParam_window;
//			CommonLib::DEBUG_PRINTF("change DISPLAYMODE_WINDOW\n" );
		}else if (mode == DISPLAYMODE_FULL ){
			m_d3dParam_current = m_d3dParam_full;
			GetWindowRect(m_hwnd, &g_rectWindow);
//			CommonLib::DEBUG_PRINTF("change DISPLAYMODE_FULL\n" );
		}

		DeviceReset();

		m_current_displaymode = mode;
		//�E�C���h�E�̈ʒu�A�傫���𒲐�
		if (mode == DISPLAYMODE_WINDOW)
		{
			SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			SetWindowPos(m_hwnd, HWND_NOTOPMOST,
			g_rectWindow.left, g_rectWindow.top,
			g_rectWindow.right - g_rectWindow.left,
			g_rectWindow.bottom - g_rectWindow.top,
			SWP_SHOWWINDOW);
		}

	}
}


/* ******************************************************** */
//! DirectDraw��������
/*!
@param	HWND
	�E�C���h�E�n���h��

@param	ScreenWidth
	�X�N���[�����������T�C�Y

@param	ScreenHeight
	�X�N���[�����������T�C�Y

@retval	���ہ@�i�@TRUE , FALSE )

*/

BOOL CDirectDrawSimple::Create( void* hWnd , int ScreenWidth , int ScreenHeight )
{
	m_dwWidth = ScreenWidth;
	m_dwHeight = ScreenHeight;

	HWND WindowHandle = (HWND)hWnd ;
	/* <<< DirectX9�C���^�t�F�[�X�̍쐬 >>> */
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if ( m_pD3D == NULL ) return FALSE;
	D3DDISPLAYMODE	d3ddm;

	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))){
		return FALSE;
	}

	D3DCAPS9	caps;
	// �f�o�C�X�̔\�͂��擾����
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);


	m_VertexShaderVersion = caps.VertexShaderVersion;
	m_PixelShaderVersion = caps.PixelShaderVersion;

//		m_VertexShaderVersion = 0;
//		m_PixelShaderVersion = 0;

	//�V�F�[�_�[�o�[�W����
	if( m_VertexShaderVersion < D3DVS_VERSION( 2, 0) ) {}
	if( m_PixelShaderVersion < D3DPS_VERSION( 2, 0) ) {}


	CreateCaps();
	//D3DPRESENT_PARAMETERS d3dParam;

	if ( m_current_displaymode == DISPLAYMODE_WINDOW )
	{
		m_d3dParam_current = m_d3dParam_window;
	}else{
		m_d3dParam_current = m_d3dParam_full;
	}

	D3DDEVTYPE	devType = D3DDEVTYPE_HAL;

#if 1	/* MEMO: �h���C�o�Ƀ��\�[�X�Ǘ���C���� */
	m_Vertextype = D3DCREATE_MIXED_VERTEXPROCESSING;
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
//									  D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParam, &m_pd3dDev)))
									D3DCREATE_MIXED_VERTEXPROCESSING, &m_d3dParam_current, &m_pd3dDev)))
	{

		m_Vertextype = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
										D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dParam_current, &m_pd3dDev)))
		{
			// �ʖڂȂ�\�t�g�E�F�A�ŏ�������
			m_Vertextype = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											&m_d3dParam_current, &m_pd3dDev)))
			{
				return FALSE;
			}else{
			}
		}
	}
#else	/* MEMO: DirectX�̃}�l�[�W���Ƀ��\�[�X�Ǘ���C���� */
	m_Vertextype = D3DCREATE_MIXED_VERTEXPROCESSING;
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
//									  D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParam, &m_pd3dDev)))
									D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &m_d3dParam_current, &m_pd3dDev)))
	{

		m_Vertextype = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
										D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &m_d3dParam_current, &m_pd3dDev)))
		{
			// �ʖڂȂ�\�t�g�E�F�A�ŏ�������
			m_Vertextype = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
											&m_d3dParam_current, &m_pd3dDev)))
			{
				return FALSE;
			}else{
			}
		}
	}
#endif

	m_pd3dDev->Clear(0L, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x00000000, 1.0f, 0L);

	m_pd3dDev->SetVertexShader(0);
	m_pd3dDev->SetPixelShader(0);

	m_pd3dDev->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	m_pd3dDev->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_MODULATE);
	m_pd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_pd3dDev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	m_pd3dDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	m_pd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pd3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  //SRC�̐ݒ�
	m_pd3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//DEST�̐ݒ�
		
	m_pd3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);	//mipmap�؂�ւ�����filter
	m_pd3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);	//�g�厞��filter
	m_pd3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);	//�k������filter
		
//		m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
//		m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_pd3dDev->GetBackBuffer(0, 0, 
			D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);

	m_currentRenderTarget = m_pBackBuffer;


    D3DVIEWPORT9 vp = {0,0, m_dwWidth,m_dwHeight, 0,1};
    m_pd3dDev->SetViewport(&vp);

    // Set D3D matrices
    D3DMATRIX matId = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    m_pd3dDev->SetTransform(D3DTS_WORLD, &matId);
    m_pd3dDev->SetTransform(D3DTS_VIEW, &matId);
    D3DMATRIX matProj = { (float)m_dwHeight/m_dwWidth,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    m_pd3dDev->SetTransform(D3DTS_PROJECTION, &matProj);

    // Disable lighting and culling
    m_pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	return TRUE;
}
	
LPDIRECT3DSURFACE9 CDirectDrawSimple::getBackBuffer()
{
	if ( m_pBackBuffer  == 0)
	{
		m_pd3dDev->GetBackBuffer(0, 0, 
				D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	}
	return m_pBackBuffer;
}

BOOL	CDirectDrawSimple::DeviceChk()
{

	if (m_DeviceLost)
	{
		HRESULT hr;
		Sleep(100); // 0.1�b�҂�

		DeviceReset();

		hr  = m_pd3dDev->TestCooperativeLevel();
		if (FAILED(hr))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				return TRUE; // �f�o�C�X�͂܂������Ă���
			}
			return FALSE;	// �f�o�C�X�̕����Ɏ��s
		}

		m_DeviceLost = false;
		return TRUE;
	}
	return TRUE;	// �ꉞTRUE��Ԃ��Ă���
}

HRESULT	CDirectDrawSimple::BeginScene()
{
 	HRESULT	hr = S_OK;
	hr  = m_pd3dDev->TestCooperativeLevel();

	if ( hr == S_OK )
	{
		if ( m_pause ) return S_FALSE;

		hr = m_pd3dDev->Clear( 0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					D3DCOLOR_XRGB(bgcolorR,bgcolorG,bgcolorB), 1.0f, 0 );

		return m_pd3dDev->BeginScene();
	}else{
		//���߂������Ƃ�
		if ( hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
		{
			m_DeviceLost = true;
			DeviceChk();
			return S_FALSE;
		}else{
			//�\�����ʃG���[
			//CommonLib::ThrowErrorMessage( 0 , "DirectX : �\�����ʃG���[\n" );
		}
	}

	return S_FALSE;
}


HRESULT	CDirectDrawSimple::EndScene()
{
 	HRESULT	hr = S_OK;
	if ( m_pause ) return S_FALSE;


	hr = m_pd3dDev->EndScene();

	//�o�b�N�o�b�t�@����v���C�}���o�b�t�@�֓]��
	if(true == IntervalImmediate)
	{	/* ���O�Ő�������������ꍇ */
		D3DRASTER_STATUS	RasterStatus;
		for( ; ; )
		{
			if( D3D_OK == m_pd3dDev->GetRasterStatus(0, &RasterStatus) )
			{	/* �擾���� */
				if(TRUE == RasterStatus.InVBlank)	break;	/* ���[�v����E�o */
				/* MEMO: �����ő��̃X���b�h�ɏ����������悤�Ƃ���Sleep���Ȃ����ƁB */
				/*       �E�B���h�E���쐬���Ă���X���b�h�Ȃ̂ŁA���̃X���b�h���Q��ƁAWndProc�܂ŐQ�Ă��܂�Sleep���Ă���ԃ��b�Z�[�W���������ꂸOS�S�̂̏������x�܂ŗ����� */
			}
		}
	}
 	if(FAILED(hr = m_pd3dDev->Present( NULL, NULL, NULL, NULL )))
 	{
		//DeviceChk();
 	}
		
	return hr;
}

/*
void	Setup2DMatrix( int width , int height , float center_posx , float center_posy )
{

	CDirectDraw* inst = CDirectDraw::GetInst();

	D3DXMATRIX      matWorld;
	D3DXMATRIX      matView;
	D3DXMATRIX      matProj;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matProj );

	float   x   = ( 2.0f / width );
	float   y   = ( -2.0f / height);
	float   z   =  1.0f;
	float   u   = -1.0f;
	float   v   =  1.0f;

	matProj._11 = x;
	matProj._22 = y;
	matProj._33 = z;
	matProj._41 = u;
	matProj._42 = v;

	D3DXMatrixTranslation( &matView ,(width/2) - center_posx , 
							(height/2) - center_posy , 0.0f );

	GetD3DDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	GetD3DDevice()->SetTransform(D3DTS_VIEW,&matView);
	GetD3DDevice()->SetTransform(D3DTS_PROJECTION,&matProj);
}

void	Setup2DMatrix( int width , int height )
{
	CDirectDraw* inst = CDirectDraw::GetInst();

	D3DXMATRIX      matWorld;
	D3DXMATRIX      matView;
	D3DXMATRIX      matProj;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matProj );

	float   x   =  2.0f / width;
	float   y   = -2.0f / height;

	float   z   =  1.0f;
	float   u   = -1.0f;
	float   v   =  1.0f;

	matProj._11 = x;
	matProj._22 = y;
	matProj._33 = z;
	matProj._41 = u;
	matProj._42 = v;

	GetD3DDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	GetD3DDevice()->SetTransform(D3DTS_VIEW,&matView);
	GetD3DDevice()->SetTransform(D3DTS_PROJECTION,&matProj);
}

*/