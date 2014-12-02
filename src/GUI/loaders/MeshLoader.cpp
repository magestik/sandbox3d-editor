﻿#include "MeshLoader.h"
#include "TextureLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../Rendering/Rendering.h"

#include <QDebug>

vec3 qrot(vec4 q, vec3 v)
{
	return v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
}

inline vec4 MatrixToQuaternion(const mat3x3 & a)
{
	vec4 q;

	float trace = a[0][0] + a[1][1] + a[2][2]; // I removed + 1.0f; see discussion with Ethan

	if (trace > 0.0f) // I changed M_EPSILON to 0
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (a[2][1] - a[1][2]) * s;
		q.y = (a[0][2] - a[2][0]) * s;
		q.z = (a[1][0] - a[0][1]) * s;
	}
	else
	{
		if (a[0][0] > a[1][1] && a[0][0] > a[2][2] )
		{
			float s = 2.0f * sqrtf(1.0f + a[0][0] - a[1][1] - a[2][2]);
			q.w = (a[2][1] - a[1][2]) / s;
			q.x = 0.25f * s;
			q.y = (a[0][1] + a[1][0]) / s;
			q.z = (a[0][2] + a[2][0]) / s;
		}
		else if (a[1][1] > a[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + a[1][1] - a[0][0] - a[2][2]);
			q.w = (a[0][2] - a[2][0]) / s;
			q.x = (a[0][1] + a[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (a[1][2] + a[2][1] ) / s;
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + a[2][2] - a[0][0] - a[1][1]);
			q.w = (a[1][0] - a[0][1]) / s;
			q.x = (a[0][2] + a[2][0]) / s;
			q.y = (a[1][2] + a[2][1]) / s;
			q.z = 0.25f * s;
		}
	}
}

void loadAllMaterials(const aiScene * scene, const QDir & dir)
{
	aiTextureType aSupportedTextureTypes [] = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_HEIGHT };

	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial * material = scene->mMaterials[i];

		for (aiTextureType type : aSupportedTextureTypes)
		{
			aiString str;

			material->GetTexture(type, 0, &str);

			if (g_Textures.find(str.C_Str()) == g_Textures.end())
			{
				QString texture_filename(str.C_Str());
				GPU::Texture<GL_TEXTURE_2D> * texture = loadTexture(dir, texture_filename);

				if (texture != nullptr)
				{
					g_Textures.insert(std::pair<std::string, GPU::Texture<GL_TEXTURE_2D> *>(texture_filename.toStdString(), texture));
				}
			}
		}
	}
}

Mesh loadMesh(const QDir & dir, const QString & filename)
{
	std::string filepath = dir.filePath(filename).toStdString();

	Assimp::Importer importer;

	const aiScene * scene = importer.ReadFile(filepath.data(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (!scene)
	{
		return(Mesh());
	}

	loadAllMaterials(scene, dir);

	std::vector<SubMesh*> meshes;

	uint NumVertices = 0;
	uint NumIndices = 0;

	// Count the number of vertices and indices
	for (int i = 0 ; i < scene->mNumMeshes ; i++)
	{
		NumVertices += scene->mMeshes[i]->mNumVertices;
		NumIndices  += scene->mMeshes[i]->mNumFaces;
	}

	// Reserve GPU memory for the vertex attributes and indices
	GPU::Buffer<GL_ARRAY_BUFFER> * vertexBuffer = new GPU::Buffer<GL_ARRAY_BUFFER>();
	GPU::Buffer<GL_ELEMENT_ARRAY_BUFFER> * indexBuffer = new GPU::Buffer<GL_ELEMENT_ARRAY_BUFFER>();

	GPU::realloc(*vertexBuffer, NumVertices * sizeof(SubMesh::VertexSimple));
	GPU::realloc(*indexBuffer, NumIndices * 3 * sizeof(unsigned int));

	void * pVertexGPU = GPU::mmap(*vertexBuffer, GL_WRITE_ONLY);
	void * pIndexGPU = GPU::mmap(*indexBuffer, GL_WRITE_ONLY);

	unsigned int vertex_offset = 0;
	unsigned int index_offset = 0;

	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh * mesh = scene->mMeshes[i];

		std::vector<SubMesh::VertexSimple> vertices;
		std::vector<unsigned int> triangles;

		vertices.reserve(mesh->mNumVertices);
		triangles.reserve(mesh->mNumFaces*3);

		// Populate the vertex attribute vectors
		for (int j = 0 ; j < mesh->mNumVertices ; ++j)
		{
			SubMesh::VertexSimple vertex;

			vertex.position.x = mesh->mVertices[j].x;
			vertex.position.y = mesh->mVertices[j].y;
			vertex.position.z = mesh->mVertices[j].z;

			vertex.normal.x = mesh->mNormals[j].x;
			vertex.normal.y = mesh->mNormals[j].y;
			vertex.normal.z = mesh->mNormals[j].z;

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent.x = mesh->mTangents[j].x;
				vertex.tangent.y = mesh->mTangents[j].y;
				vertex.tangent.z = mesh->mTangents[j].z;
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.uv.x = mesh->mTextureCoords[0][j].x;
				vertex.uv.y = mesh->mTextureCoords[0][j].y;
			}
			else
			{
				vertex.uv.x = 0.5f;
				vertex.uv.y = 0.5f;
			}

			vertices.push_back(vertex);
		}

		// Populate the index buffer
		for (int j = 0 ; j < mesh->mNumFaces ; ++j)
		{
			const aiFace & Face = mesh->mFaces[j];
			triangles.push_back(Face.mIndices[0]);
			triangles.push_back(Face.mIndices[1]);
			triangles.push_back(Face.mIndices[2]);
		}

		memcpy(pVertexGPU, (void *)vertices.data(), vertices.size() * sizeof(SubMesh::VertexSimple));
		memcpy(pIndexGPU, (void *)triangles.data(), triangles.size() * sizeof(unsigned int));

		pVertexGPU = (void*)(((char*)pVertexGPU) + vertices.size() * sizeof(SubMesh::VertexSimple));
		pIndexGPU = (void*)(((char*)pIndexGPU) + triangles.size() * sizeof(unsigned int));

		std::vector<SubMesh::VertexSpec> specs;

		SubMesh::VertexSpec SPEC_POS;
		SPEC_POS.index			= 0;
		SPEC_POS.size			= 3;
		SPEC_POS.type			= GL_FLOAT;
		SPEC_POS.normalized		= GL_FALSE;
		SPEC_POS.stride			= sizeof(SubMesh::VertexSimple);
		SPEC_POS.pointer		= BUFFER_OFFSET(vertex_offset);

		SubMesh::VertexSpec SPEC_UV;
		SPEC_UV.index			= 2;
		SPEC_UV.size			= 2;
		SPEC_UV.type			= GL_FLOAT;
		SPEC_UV.normalized		= GL_FALSE;
		SPEC_UV.stride			= sizeof(SubMesh::VertexSimple);
		SPEC_UV.pointer			= BUFFER_OFFSET(vertex_offset+sizeof(float)*3);

		SubMesh::VertexSpec SPEC_NORMAL;
		SPEC_NORMAL.index		= 1;
		SPEC_NORMAL.size		= 3;
		SPEC_NORMAL.type		= GL_FLOAT;
		SPEC_NORMAL.normalized	= GL_FALSE;
		SPEC_NORMAL.stride		= sizeof(SubMesh::VertexSimple);
		SPEC_NORMAL.pointer		= BUFFER_OFFSET(vertex_offset+sizeof(float)*5);

		SubMesh::VertexSpec SPEC_TANGENT;
		SPEC_TANGENT.index		= 3;
		SPEC_TANGENT.size		= 3;
		SPEC_TANGENT.type		= GL_FLOAT;
		SPEC_TANGENT.normalized	= GL_FALSE;
		SPEC_TANGENT.stride		= sizeof(SubMesh::VertexSimple);
		SPEC_TANGENT.pointer		= BUFFER_OFFSET(vertex_offset+sizeof(float)*8);

		specs.push_back(SPEC_POS);
		specs.push_back(SPEC_UV);
		specs.push_back(SPEC_NORMAL);
		specs.push_back(SPEC_TANGENT);

		SubMesh * submesh = SubMesh::Create(vertexBuffer, triangles.size(), GL_TRIANGLES, specs, indexBuffer, index_offset, GL_UNSIGNED_INT);
		meshes.push_back(submesh);

		if (mesh->HasTangentsAndBitangents())
		{
			aiString str;
			scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_HEIGHT, 0, &str);
			std::string texture_name(str.C_Str());

			if (g_Textures.find(texture_name) != g_Textures.end())
			{
				submesh->m_pNormalMap = g_Textures[texture_name];
			}
		}

		if (mesh->HasTangentsAndBitangents())
		{
			aiString str;
			scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &str);
			std::string texture_name(str.C_Str());

			if (g_Textures.find(texture_name) != g_Textures.end())
			{
				submesh->m_material.m_diffuse = g_Textures[texture_name];
			}
		}


		vertex_offset += vertices.size() * sizeof(SubMesh::VertexSimple);
		index_offset += triangles.size() * sizeof(unsigned int);
	}

	GPU::munmap(*vertexBuffer);
	GPU::munmap(*indexBuffer);

	return(Mesh(meshes));
}
