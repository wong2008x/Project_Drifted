#include "GameObject.h"

bool LoadObj(const char* path, std::vector <SimpleMesh>& outVertices)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector<XMFLOAT3> tempVerts;
	std::vector<XMFLOAT3> tempUVs;
	std::vector<XMFLOAT3> tempNormals;
	FILE* file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (true)
	{
		char line[128];
		int res = fscanf_s(file, "%s", line, sizeof(line));
		if (res == EOF)
			break;
		if (strcmp(line, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
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
	}

	return true;
}


GameObject::GameObject()
{

}

void GameObject::CreateGameObject(ID3D11Device* _device, const char* _filepath, const BYTE* _VS, unsigned int _VSize)
{
	LoadObj(_filepath, GO_vertex);


	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.ByteWidth = sizeof(SimpleMesh) * GO_vertex.size();
	BufferDesc.CPUAccessFlags = NULL;
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA VerticesData;
	ZeroMemory(&VerticesData, sizeof(VerticesData));
	VerticesData.pSysMem = GO_vertex.data();
	_device->CreateBuffer(&BufferDesc, &VerticesData, &pGO_Vbuff);

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	_device->CreateInputLayout(InputLayout, 4, _VS, _VSize, &pGO_inputLayout);

}


GameObject::~GameObject()
{
	if(pGO_Vbuff)pGO_Vbuff->Release();
	if(pGO_inputLayout)pGO_inputLayout->Release();
	if(pGO_PS) pGO_PS->Release();
	if(pGO_VS) pGO_VS->Release();
	if (pGO_SRV_Texture)pGO_SRV_Texture->Release();
	if (pGO_SRV_secTexture)pGO_SRV_secTexture->Release();
	if (pGO_SRV_NormTex)pGO_SRV_NormTex->Release();
}