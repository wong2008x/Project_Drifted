// Graphic2Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Graphic2Project.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void createSphere(float fRadius, UINT uSlices, UINT uStacks);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHIC2PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHIC2PROJECT));

    MSG msg;
	mTimer.Restart();
    // Main message loop:
    while (true)//GetMessage(&msg, nullptr, 0, 0))
    {
		
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		if (msg.message == WM_QUIT)
			break;
		
		mTimer.Signal();
		Update();
		Render();

		
    }
	CleanupDevice();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHIC2PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHIC2PROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   

   RECT mWinR;
   GetClientRect(hWnd, &mWinR);

   //my codes here   
   D3D_DRIVER_TYPE driverTypes[] =
   {
	   D3D_DRIVER_TYPE_HARDWARE,
	   D3D_DRIVER_TYPE_WARP,
	   D3D_DRIVER_TYPE_REFERENCE,
   };
   UINT numDriverTypes = ARRAYSIZE(driverTypes);

   D3D_FEATURE_LEVEL featureLevels[] =
   {
	   D3D_FEATURE_LEVEL_11_1,
	   D3D_FEATURE_LEVEL_11_0,
	   D3D_FEATURE_LEVEL_10_1,
	   D3D_FEATURE_LEVEL_10_0,
   };
   UINT numFeatureLevels = ARRAYSIZE(featureLevels);

   DXGI_SWAP_CHAIN_DESC swap;
   ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
   swap.BufferCount = 1;
   swap.OutputWindow = hWnd;
   swap.Windowed = true;
   swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
   swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   swap.BufferDesc.Width = mWinR.right - mWinR.left;
   swap.BufferDesc.Height = mWinR.bottom - mWinR.top;
   swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swap.SampleDesc.Count = 1;
   aspectRatio = swap.BufferDesc.Width / static_cast<float>(swap.BufferDesc.Height);
 
   HRESULT hr;
   // ----Debug Mode change later

   UINT createDeviceFlags = 0;
#ifdef _DEBUG
   createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

   hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, 
	   featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swap, &mSwap, &mDev, &dx11, &mContext);

   ID3D11Resource* backBuffer;
   hr = mSwap->GetBuffer(0, __uuidof(backBuffer), (void**)&backBuffer);
   hr = mDev->CreateRenderTargetView(backBuffer, NULL, &mRTV);

   backBuffer->Release();


   mPort.Height = swap.BufferDesc.Height;
   mPort.Width = swap.BufferDesc.Width;
   mPort.TopLeftX = mPort.TopLeftY = 0;
   mPort.MinDepth = 0;
   mPort.MaxDepth = 1;

	mFirPort.Height = swap.BufferDesc.Height;
	mFirPort.Width = swap.BufferDesc.Width/2; 
	mFirPort.TopLeftX = mFirPort.TopLeftY = 0;
	mFirPort.MinDepth = 0; 
	mFirPort.MaxDepth = 1; 

	mSecPort.Height = swap.BufferDesc.Height;
	mSecPort.Width = swap.BufferDesc.Width/2;
	mSecPort.TopLeftX = swap.BufferDesc.Width / 2;
	mSecPort.TopLeftY = 0;
	mSecPort.MinDepth = 0;
	mSecPort.MaxDepth = 1;

   static const SkyBox skyBoxVerts[] =
   {
	   { XMFLOAT3(-1.0f, -1.0f, -1.0f) },
	   { XMFLOAT3(-1.0f, -1.0f,  1.0f) },
	   { XMFLOAT3(-1.0f,  1.0f, -1.0f) },
	   { XMFLOAT3(-1.0f,  1.0f,  1.0f) },
	   { XMFLOAT3(1.0f, -1.0f, -1.0f) },
	   { XMFLOAT3(1.0f, -1.0f,  1.0f) },
	   { XMFLOAT3(1.0f,  1.0f, -1.0f) },
	   { XMFLOAT3(1.0f,  1.0f,  1.0f) },
   };
   static const unsigned int skyIndices[] =
   {
	   3,7,5,
	   5,1,3,

	   7,6,4,
	   4,5,7,

	   6,2,0,
	   0,4,6,

	   2,3,1,
	   1,0,2,

	   2,6,7,
	   7,3,2,

	   5,4,0,
	   0,1,5,
   };
   static const D3D11_INPUT_ELEMENT_DESC skyBoxInputDesc[] =
   {
	   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   hr = mDev->CreateInputLayout(skyBoxInputDesc, 1, SkyVertexShader, sizeof(SkyVertexShader), &skyLayout);
   D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(SkyBox) * ARRAYSIZE(skyBoxVerts);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   subData.pSysMem = skyBoxVerts;
   mDev->CreateBuffer(&bDesc, &subData, &vSkyBuff);

   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(unsigned int)*ARRAYSIZE(skyIndices);
   subData.pSysMem = skyIndices;
   subData.SysMemPitch = 0;
   subData.SysMemSlicePitch = 0;
   mDev->CreateBuffer(&bDesc, &subData, &iSkyBuff);

   D3D11_SAMPLER_DESC sampleDesc;
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

   mDev->CreateSamplerState(&sampleDesc, &skyBoxSamplerState);
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/SkyboxOcean.dds", NULL, &skyBoxTextureRV);

   // compile the shaders
   hr = mDev->CreateVertexShader(SkyVertexShader, sizeof(SkyVertexShader), nullptr, &vSkyShader);
   hr = mDev->CreatePixelShader(SkyPixelShader, sizeof(SkyPixelShader), nullptr, &pSkyShader);

   SimpleVertex tri[] = // NDC
   {
	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {0.25f, -0.25f, -0.25f, 1},  {1, 0, 1, 1} },
	   { {-0.25f, -0.25f, -0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {0.25f, -0.25f, 0.25f, 1},  {1, 0, 1, 1} },
	   { {0.25f, -0.25f, -0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {-0.25f, -0.25f, 0.25f, 1},  {1, 0, 1, 1} },
	   { {0.25f, -0.25f, 0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {-0.25f, -0.25f, -0.25f, 1},  {1, 0, 1, 1} },
	   { {-0.25f, -0.25f, 0.25f, 1},  {1, 1, 1, 1} },
   };

   numVerts = ARRAYSIZE(tri);

   //LOAD THE TRI
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(SimpleVertex)*numVerts;
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   
   subData.pSysMem = tri;

   mDev->CreateBuffer(&bDesc, &subData, &vBuff);
  
   // compile the shaders
   hr=mDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
   hr=mDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);
   
   //Describe it to D3D11
   D3D11_INPUT_ELEMENT_DESC ieDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   hr= mDev->CreateInputLayout(ieDesc, 2, VertexShader, sizeof(VertexShader), &vLayout);


   //Constant Buffer
   ZeroMemory(&bDesc, sizeof(bDesc));

   bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bDesc.ByteWidth = sizeof(WVP);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DYNAMIC;

   hr= mDev->CreateBuffer(&bDesc, nullptr, &cBuff);

   //Lighting Buffer
   bDesc.ByteWidth = sizeof(LightingConstant);
   hr = mDev->CreateBuffer(&bDesc, nullptr, &cLightBuff);



   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   loadObject("Assets/Rock.obj", rockVertex, rockIndices, true);

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   bDesc.ByteWidth = sizeof(SimpleMesh)*rockVertex.size();
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   subData.pSysMem = rockVertex.data();
   hr=mDev->CreateBuffer(&bDesc, &subData, &vRockBuff);

   //Index Buffer mesh
   subData.pSysMem = rockIndices.data();
   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(unsigned int)*rockIndices.size();
   hr = mDev->CreateBuffer(&bDesc, &subData, &iRockBuff);

   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vRockShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pRockShader);
   D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/Rock_Diffuse.dds", (ID3D11Resource**)&rockTexture, &rockTextureRV);

   // Create the sample state
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampleDesc.MinLOD = 0;
   sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampleDesc, &rockSamplerState);
   hr = mDev->CreateInputLayout(meshInputDesc, 3, VertexMeshShader, sizeof(VertexMeshShader), &vRockLayout);

   CD3D11_TEXTURE2D_DESC zDesc;
   ZeroMemory(&zDesc, sizeof(zDesc));
   zDesc.ArraySize = 1;
   zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
   zDesc.Width = swap.BufferDesc.Width;
   zDesc.Height = swap.BufferDesc.Height;
   zDesc.Format = DXGI_FORMAT_D32_FLOAT;
   zDesc.Usage = D3D11_USAGE_DEFAULT;
   zDesc.MipLevels = 1;
   zDesc.SampleDesc.Count = 1;

   //Load StoneHenge Mesh

   ZeroMemory(&bDesc, sizeof(bDesc));
   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(StoneHenge_data);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;

   subData.pSysMem = StoneHenge_data;

   hr = mDev->CreateBuffer(&bDesc, &subData, &vStoneBuff);

   //Index Buffer mesh
   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(StoneHenge_indicies);
   subData.pSysMem = StoneHenge_indicies;
   hr = mDev->CreateBuffer(&bDesc, &subData, &iStoneBuff);

   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vStoneShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pStoneShader);
  // D3D11_INPUT_ELEMENT_DESC meshInputDesc2[] =
  // {
		//{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  // };
   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/StoneHenge.dds", (ID3D11Resource**)&stoneTexture, &stoneTextureRV);

   // Create the sample state
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampleDesc.MinLOD = 0;
   sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampleDesc, &stoneSamplerState);

   hr = mDev->CreateInputLayout(meshInputDesc, 3, VertexMeshShader, sizeof(VertexMeshShader), &vStoneLayout);


   ZeroMemory(&zDesc, sizeof(zDesc));
   zDesc.ArraySize = 1;
   zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
   zDesc.Width = swap.BufferDesc.Width;
   zDesc.Height = swap.BufferDesc.Height;
   zDesc.Format = DXGI_FORMAT_D32_FLOAT;
   zDesc.Usage = D3D11_USAGE_DEFAULT;
   zDesc.MipLevels = 1;
   zDesc.SampleDesc.Count = 1;

   hr = mDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);
   hr = mDev->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);

   //Directional Light
   XMStoreFloat4(&myLighting.dLightDir, dLightD);
   //Point Light
   XMStoreFloat4(&myLighting.pLightPos, pLightPosD);
   XMStoreFloat(&myLighting.pLightRadius, pLightRadiusD);
   //Spot Light
   XMStoreFloat4(&myLighting.sLightPos, sLightPosD);
   XMStoreFloat4(&myLighting.sLightDir, sLightDirD);
   XMStoreFloat(&myLighting.innerAngle, innerAngleD);
   XMStoreFloat(&myLighting.outerAngle, outerAngleD);
   myLighting.lightingMode = 1;
   myLighting.worldTime = mTimer.TotalTime();
   // Initialize the world matrices
   XMMATRIX temp = XMMatrixIdentity();
   XMStoreFloat4x4(&myMatricies.g_World,temp);

   // Initialize the view matrix
    Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
    At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   temp = XMMatrixLookAtLH(Eye, At, Up);
   XMStoreFloat4x4(&myMatricies.g_View, temp);

   temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
   XMStoreFloat4x4(&myMatricies.g_Projection, temp);


   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_SIZE:
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		WindowResize(width,height);
		break;
	}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Render()
{
	D3D11_MAPPED_SUBRESOURCE gpuBuffer = {};
	//HRESULT  hr;
	//XMMATRIX temp = XMMatrixIdentity();

	mContext->OMSetRenderTargets(1, &mRTV, zBufferView);
	mContext->ClearRenderTargetView(mRTV, DirectX::Colors::BurlyWood);
	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	// rasterizer


	if (multiviewPort)
	{
		//aspectRatio = mFirPort.Width/mFirPort.Height;
		//XMStoreFloat4x4(&myMatricies.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
		mContext->RSSetViewports(1, &mFirPort);
		postRender(gpuBuffer);
		

		mContext->RSSetViewports(1, &mSecPort);
		postRender(gpuBuffer);
	}
	else
	{

		mContext->RSSetViewports(1, &mPort);
		postRender(gpuBuffer);

	}
	mSwap->Present(1, 0);


}
void postRender(D3D11_MAPPED_SUBRESOURCE gpuBuffer)
{

	HRESULT  hr;
	XMMATRIX temp = XMMatrixIdentity();

	UINT skyStrides[] = { sizeof(SkyBox) };
	UINT skyoffsets[] = { 0 };
	mContext->PSSetShaderResources(0, 1, &skyBoxTextureRV);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mContext->IASetVertexBuffers(0, 1, &vSkyBuff, skyStrides, skyoffsets);
	mContext->IASetIndexBuffer(iSkyBuff, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vSkyShader, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(pSkyShader, 0, 0);
	mContext->IASetInputLayout(skyLayout);
	mContext->PSSetSamplers(0, 1, &skyBoxSamplerState);

	temp = XMMatrixIdentity();
	temp = XMMatrixMultiply(XMMatrixScaling(200.0f, 200.0f, 200.0f), temp);
	XMStoreFloat4x4(&myMatricies.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);
	mContext->DrawIndexed(36, 0, 0);
	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);



	UINT strides[] = { sizeof(SimpleVertex) };
	UINT offsets[] = { 0 };
	mContext->IASetVertexBuffers(0, 1, &vBuff, strides, offsets);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mContext->IASetInputLayout(vLayout);
	mContext->VSSetShader(vShader, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(pShader, 0, 0);
	temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(10, 0, 4);
	XMMATRIX temp2 = XMMatrixRotationY(delta_time);
	temp = XMMatrixMultiply(temp2, temp);
	XMStoreFloat4x4(&myMatricies.g_World, temp);

	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(numVerts, 0);




	if (myLighting.lightingMode == 1)
	{
		//Directional Light
		dLight = XMVector4Transform(dLight, XMMatrixRotationY(XMConvertToRadians(10 * delta_time)));
		XMStoreFloat4(&myLighting.dLightDir, dLight);
		//Point Light
		pLightPos = XMVector4Transform(pLightPos, XMMatrixRotationY(XMConvertToRadians(10 * delta_time)));
		XMStoreFloat4(&myLighting.pLightPos, pLightPos);
		//Spot Light 
		XMFLOAT4 slightTemp;

		XMStoreFloat4(&slightTemp, sLightPos);
		if (flag)
		{
			sLightPos = XMVector4Transform(sLightPos, XMMatrixTranslation(0, 0, 10 * delta_time));
			if (slightTemp.z > 10)
				flag = false;
		}
		else
		{
			sLightPos = XMVector4Transform(sLightPos, XMMatrixTranslation(0, 0, -10 * delta_time));
			if (slightTemp.z < -10)
				flag = true;
		}
		XMStoreFloat4(&myLighting.sLightPos, sLightPos);
		sLightDir = XMVector4Transform(sLightDir, XMMatrixRotationY(XMConvertToRadians(10 * -delta_time)));
		XMStoreFloat4(&myLighting.sLightDir, sLightDir);
	}
	D3D11_MAPPED_SUBRESOURCE LightingBuffer;
	hr = mContext->Map(cLightBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &LightingBuffer);
	*((LightingConstant*)(LightingBuffer.pData)) = myLighting;
	mContext->Unmap(cLightBuff, 0);

	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	//Draw Rock

	mContext->PSSetShaderResources(0, 1, &rockTextureRV);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);

	//Set Pipline
	UINT mesh_strides[] = { sizeof(SimpleMesh) };
	UINT mesh_offsets[] = { 0 };
	ID3D11Buffer* meshVB[] = { vRockBuff };
	mContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
	mContext->IASetIndexBuffer(iRockBuff, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vRockShader, 0, 0);
	mContext->PSSetShader(pRockShader, 0, 0);
	mContext->IASetInputLayout(vRockLayout);

	temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(-10, 0, -2);
	XMStoreFloat4x4(&myMatricies.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);
	mContext->DrawIndexed(rockIndices.size(), 0, 0);




	//Draw StoneHenge
	mContext->PSSetShaderResources(0, 1, &stoneTextureRV);
	mContext->PSSetSamplers(0, 1, &stoneSamplerState);

	//Set Pipline
	mesh_strides[0] = { sizeof(_OBJ_VERT_) };
	mesh_offsets[0] = { 0 };
	meshVB[0] = { vStoneBuff };
	mContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
	mContext->IASetIndexBuffer(iStoneBuff, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vStoneShader, 0, 0);
	mContext->PSSetShader(pStoneShader, 0, 0);
	mContext->IASetInputLayout(vStoneLayout);

	temp = XMMatrixIdentity();
	XMStoreFloat4x4(&myMatricies.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);
	mContext->DrawIndexed(2532, 0, 0);

	//mSwap->Present(1, 0);
}

void Update()
{
	
	delta_time=mTimer.Delta();
	if (GetAsyncKeyState('W') )
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -cameraSpeed *delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('S') )
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, cameraSpeed *delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('D') )
	{

		XMMATRIX translation = XMMatrixTranslation(-cameraSpeed *delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('A') )
	{
		XMMATRIX translation = XMMatrixTranslation(cameraSpeed *delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('X') )
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -cameraSpeed * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}

	if (GetAsyncKeyState(VK_SPACE) )
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, cameraSpeed *delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState(VK_NUMPAD7) )
	{
		fPlane+= 5*delta_time;

		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD1) )
	{
		fPlane -=  10*delta_time;
		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		if (fPlane <=10)
		{ 
			fPlane = 10.0f;
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		}
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	
	}
	if (GetAsyncKeyState(VK_NUMPAD8) )
	{
		XMMATRIX temp;
		FOV += 10*delta_time;
		if (FOV >= 120.0f)
		{
			FOV = 120.0f;
		}
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD2) )
	{
		XMMATRIX temp;
		FOV -= 10*delta_time;
		if (FOV <= 30.0f)
		{
			FOV = 30.0f;
		}
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD9))
	{
		nPlane +=  5*delta_time;

		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD3) )
	{
		XMMATRIX temp;
		nPlane -=  delta_time;
		
		if (nPlane <= 0)
		{
			nPlane = 0.01f;
		}
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD0) & 0x1)
	{
		FOV = 75.0f;
		nPlane = 0.01f;
		fPlane = 200.0f;
		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_F1) & 0x1)
	{
		multiviewPort = !multiviewPort;
	}
}

void CreateSphere(int LatLines, int LongLines)
{
	int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<SimpleMesh> vertices(NumSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].Pos.x = 0.0f;
	vertices[0].Pos.y = 0.0f;
	vertices[0].Pos.z = 1.0f;

	for (DWORD i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * LongLines + j + 1].Pos.x = XMVectorGetX(currVertPos);
			vertices[i * LongLines + j + 1].Pos.y = XMVectorGetY(currVertPos);
			vertices[i * LongLines + j + 1].Pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices - 1].Pos.x = 0.0f;
	vertices[NumSphereVertices - 1].Pos.y = 0.0f;
	vertices[NumSphereVertices - 1].Pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleMesh) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT hr = mDev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);


	std::vector<DWORD> indices(NumSphereFaces * 3);

	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;

		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	mDev->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);

}

bool loadObject(const char* path, std::vector <SimpleMesh>& outVertices, std::vector <unsigned int>& outIndicies,bool isRHCoord)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector<XMFLOAT3> tempVerts;
	std::vector<XMFLOAT3> tempUVs;
	std::vector<XMFLOAT3> tempNormals;
	FILE* file;
    fopen_s(&file,path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (true)
	{
		char line[128];
		int res = fscanf_s(file, "%s",line,sizeof(line));
		if (res == EOF)
			break;
		if (strcmp(line, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			if(isRHCoord)
			vertex.z = -vertex.z;
			tempVerts.push_back(vertex);
		}
		else if (strcmp(line, "vt") == 0)
		{
			XMFLOAT3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;
			tempUVs.push_back(uv);
		}
		else if (strcmp(line, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf_s(file, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
			if (isRHCoord)
				norm.z = -norm.z;
			tempNormals.push_back(norm);
		}
		else if (strcmp(line, "f") == 0) 
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[0], &uvIndex[0], &normalIndex[0]);
			if (matches != 9) {
				printf("File can't be read by this simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{

		SimpleMesh temp;
		temp.Pos = tempVerts[vertexIndices[i] - 1];
		temp.Tex = tempUVs[uvIndices[i] - 1];
		temp.Norm = tempNormals[normalIndices[i] - 1];
		
		outVertices.push_back(temp);
		outIndicies.push_back(i);
	}

	return true;
}
void WindowResize(UINT _width, UINT _height)
{

	aspectRatio = _width/(float)_height;
	if (multiviewPort)
		aspectRatio = (_width / 2) / (float)_height;
	XMStoreFloat4x4(&myMatricies.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
}

void CleanupDevice()
{
	if(mRTV)mRTV->Release();
	if(mContext)mContext->Release();
	if(mSwap)mSwap->Release();
	if (mDev)mDev->Release();

	if (zBuffer)zBuffer->Release();
	if (zBufferView)zBufferView->Release();
	if (cLightBuff)cLightBuff->Release();
	if (cBuff)cBuff->Release();

	if(vBuff)vBuff->Release();
	if (vShader)vShader->Release();
	if (pShader)pShader->Release();
	if (vLayout)vLayout->Release();

	//skybox
	if (vSkyBuff)vSkyBuff->Release();
	if (iSkyBuff)iSkyBuff->Release();
	if (vSkyShader)vSkyShader->Release();
	if (pSkyShader)pSkyShader->Release();
	if (skyLayout)skyLayout->Release();
	if (skyBoxTextureRV)skyBoxTextureRV->Release();
	if (skyBoxSamplerState)skyBoxSamplerState->Release();

	//rock
	if (vRockBuff)vRockBuff->Release();
	if (iRockBuff)iRockBuff->Release();
	if (vRockShader)vRockShader->Release();
	if (pRockShader)pRockShader->Release();
	if (rockSamplerState)rockSamplerState->Release();
	if (rockTextureRV)rockTextureRV->Release();
	if (rockTexture)rockTexture->Release();
	if (vRockLayout)vRockLayout->Release();

	//stone
	if (vStoneBuff)vStoneBuff->Release();
	if (iStoneBuff)iStoneBuff->Release();
	if (vStoneShader)vStoneShader->Release();
	if (pStoneShader)pStoneShader->Release();
	if (stoneTextureRV)stoneTextureRV->Release();
	if (stoneTexture)stoneTexture->Release();
	if (stoneSamplerState)stoneSamplerState->Release();
	if (vStoneLayout)vStoneLayout->Release();


}