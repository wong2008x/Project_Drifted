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
void CleanupDevice();
void Render();
void UpdateCamera();


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
		

		UpdateCamera();
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
   D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

   //Load Mesh


   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(Rock_data);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;

   subData.pSysMem = Rock_data;

   hr=mDev->CreateBuffer(&bDesc, &subData, &vBuffMesh);

   //Index Buffer mesh
   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(Rock_indicies);
   subData.pSysMem = Rock_indicies;
   hr = mDev->CreateBuffer(&bDesc, &subData, &iBuffMesh);

   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vMeshShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pMeshShader);
   D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/Rock_Diffuse.dds", (ID3D11Resource**)&enviromentTexture, &vTextureRV);

   // Create the sample state
   D3D11_SAMPLER_DESC sampDesc = {};
   sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampDesc.MinLOD = 0;
   sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampDesc, &mSamplerLinear);

   hr = mDev->CreateInputLayout(meshInputDesc, 3, VertexMeshShader, sizeof(VertexMeshShader), &vMeshLayout);

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

   hr = mDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);
   hr = mDev->CreateDepthStencilView(zBuffer,nullptr,&zBufferView);

   // Initialize the world matrices
   XMMATRIX temp = XMMatrixIdentity();
   XMStoreFloat4x4(&myMatricies.g_World,temp);

   // Initialize the view matrix
   XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
   XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   temp = XMMatrixLookAtLH(Eye, At, Up);
   XMStoreFloat4x4(&myMatricies.g_View, temp);

   temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
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
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	float color[] = { 0, 0, 0, 1 };
	mContext->ClearRenderTargetView(mRTV, color);

	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
	//Setup the pipeline
	//output merger
	ID3D11RenderTargetView* tempRTV[] = { mRTV };
	mContext->OMSetRenderTargets(1, tempRTV, zBufferView);
	// rasterizer
	mContext->RSSetViewports(1, &mPort);
	// Input Assembler
	mContext->IASetInputLayout(vLayout);
	UINT strides[] = { sizeof(SimpleVertex) };
	UINT offsets[] = { 0 };
	ID3D11Buffer* tempVB[] = { vBuff };
	mContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mContext->VSSetShader(vShader, 0, 0);
	mContext->PSSetShader(pShader, 0, 0);

	XMMATRIX temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(10, 0, 4);
	XMMATRIX temp2 = XMMatrixRotationY(t);
	temp = XMMatrixMultiply(temp2, temp);
	XMStoreFloat4x4(&myMatricies.g_World, temp);




	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	HRESULT hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);

	ID3D11Buffer* constants[] = { cBuff };
	mContext->VSSetConstantBuffers(0, 1, constants);

	mContext->Draw(numVerts, 0);


	ID3D11ShaderResourceView* texView[] = { vTextureRV };
	mContext->PSSetShaderResources(0, 1, texView);
	mContext->PSSetSamplers(0, 1, &mSamplerLinear);

	//Set Pipline
	UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
	UINT mesh_offsets[] = { 0 };
	ID3D11Buffer* meshVB[] = { vBuffMesh };
	mContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
	mContext->IASetIndexBuffer(iBuffMesh, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vMeshShader, 0, 0);
	mContext->PSSetShader(pMeshShader, 0, 0);
	mContext->IASetInputLayout(vMeshLayout);

	temp = XMMatrixIdentity();
	temp2 = XMMatrixRotationY(t);
	temp = XMMatrixMultiply(temp2, temp);
	XMStoreFloat4x4(&myMatricies.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);


	mContext->DrawIndexed(1908, 0, 0);
	mSwap->Present(1, 0);
}
void UpdateCamera()
{
	const float delta_time = 0.8f;



	if (GetAsyncKeyState('W'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, cameraSpeed * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('S'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -cameraSpeed * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('D'))
	{

		XMMATRIX translation = XMMatrixTranslation(-cameraSpeed * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('A'))
	{
		XMMATRIX translation = XMMatrixTranslation(cameraSpeed * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('X'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -cameraSpeed * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, cameraSpeed * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
}

void CleanupDevice()
{
	if(mRTV)mRTV->Release();
	if(mContext)mContext->Release();
	if(mSwap)mSwap->Release();
	if (mDev)mDev->Release();

	if (zBuffer)zBuffer->Release();
	if (zBufferView)zBufferView->Release();
	if(vBuff)vBuff->Release();
	if(cBuff)cBuff->Release();
	if (vBuffMesh)vBuffMesh->Release();
	if (iBuffMesh)iBuffMesh->Release();

	if(vLayout)vLayout->Release();
	if (vMeshLayout)vMeshLayout->Release();

	if(vShader)vShader->Release();
	if(pShader)pShader->Release();

	if (vMeshShader)vMeshShader->Release();
	if (pMeshShader)pMeshShader->Release();

	if (vTextureRV)vTextureRV->Release();
	if (enviromentTexture)enviromentTexture->Release();
	if (mSamplerLinear)mSamplerLinear->Release();
}