#pragma once
#include"Renderer.h"
#include<vector>
#include<string>
#include<fstream>

struct aData
{
	float *vData[5];
	unsigned int dSize[5];
	unsigned int num_wrds;
	aData()
	{
		for (unsigned int i = 0; i < 5; ++i)
		{
			dSize[i] = 0;
		}
		num_wrds = 0;
	}
};

class VertexBuffer
{
private:
	unsigned int m_RendererID;
	aData dObj;
	bool priority[3];
	std::vector<float> data[3];
	std::vector<float> tangents;
	std::vector<float> bitangents;
	void VertexData(const std::string& arrayName)
	{
		std::fstream file;
		file.open("src/VertexData.txt", std::ios::in);
		if (file.fail())
		{
			std::cout << "Vertex data file didn't open" << std::endl;
			file.clear();
		}
		std::string line;
		bool Flag[4] = { false,false,false,false }; //objData,pos,norm,tex
		while (std::getline(file, line))
		{
			if (line.find(("~"+arrayName).c_str()) != std::string::npos)
			{
				Flag[0] = true;
			}
			else if (Flag[0] && line.find("#pos") != std::string::npos)
			{
				Flag[1] = true;
			}
			else if (Flag[0] && line.find("#norm") != std::string::npos)
			{
				Flag[1] = false;
				Flag[2] = true;
			}
			else if (Flag[0] && line.find("#tex") != std::string::npos)
			{
				Flag[1] = false;
				Flag[2] = false;
				Flag[3] = true;
			}
			else if (Flag[0] && line.find("~") != std::string::npos)
			{
				break;
			}
			else if (Flag[0] && Flag[1])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
				{
					data[0].push_back(atof(wrd.c_str()));
					dObj.num_wrds++;
				}
			}
			else if (Flag[0] && Flag[2])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
					data[1].push_back(atof(wrd.c_str()));
			}
			else if (Flag[0] && Flag[3])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
					data[2].push_back(atof(wrd.c_str()));
			}
		}
		for (unsigned int i = 0; i < 3; ++i)
		{
			if (data[i].empty())
				priority[i] = false;
			else
				priority[i] = true;
		}
		for (unsigned int i = 0, j=0; i < 3; ++i)
		{
			if (priority[i])
			{
				dObj.vData[j] = data[i].data();
				dObj.dSize[j++] = data[i].size()*sizeof(float);
			}
		}
	}
	void VertexDataTNB(const std::string& arrayName)
	{
		std::fstream file;
		file.open("src/VertexData.txt", std::ios::in);
		if (file.fail())
		{
			std::cout << "Vertex data file didn't open" << std::endl;
			file.clear();
		}
		std::string line;
		bool Flag[4] = { false,false,false,false }; //objData,pos,norm,tex
											
		while (std::getline(file, line))
		{
			if (line.find(("~" + arrayName).c_str()) != std::string::npos)
			{
				Flag[0] = true;
			}
			else if (Flag[0] && line.find("#pos") != std::string::npos)
			{
				Flag[1] = true;
			}
			else if (Flag[0] && line.find("#norm") != std::string::npos)
			{
				Flag[1] = false;
				Flag[2] = true;
			}
			else if (Flag[0] && line.find("#tex") != std::string::npos)
			{
				Flag[1] = false;
				Flag[2] = false;
				Flag[3] = true;
			}
			else if (Flag[0] && line.find("~") != std::string::npos)
			{
				break;
			}
			else if (Flag[0] && Flag[1])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
				{
					data[0].push_back(atof(wrd.c_str()));
					dObj.num_wrds++;
				}
			}
			else if (Flag[0] && Flag[2])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
					data[1].push_back(atof(wrd.c_str()));
			}
			else if (Flag[0] && Flag[3])
			{
				std::string wrd;
				std::stringstream ss(line);
				while (ss >> wrd)
					data[2].push_back(atof(wrd.c_str()));
			}
		}
		for (unsigned int rect = 0; rect < data[0].size() / 18.0f; ++rect)
		{
			glm::vec3 tangent[2];
			glm::vec3 bitangent[2];
			for (unsigned int tri = 0; tri < 2; ++tri)
			{
				glm::vec3 pos[3];
				glm::vec2 uv[3];
				glm::vec3 norm;
				glm::vec3 edge[2];
				glm::vec2 deltaUV[2];
				for (unsigned int line = 0; line < 3; ++line)
				{
					pos[line] = glm::vec3(data[0][18 * rect + 9 * tri + 3 * line], data[0][18 * rect + 9 * tri + 3 * line + 1], data[0][18 * rect + 9 * tri + 3 * line + 2]);
					uv[line] = glm::vec2(data[2][12 * rect + 6 * tri + 2 * line], data[2][12 * rect + 6 * tri + 2 * line + 1]);
				}
				norm = glm::vec3(data[1][18 * rect + 9 * tri], data[1][18 * rect + 9 * tri + 1], data[1][18 * rect + 9 * tri + 2]);
				if (tri == 0)
				{
					edge[0] = pos[1] - pos[0];
					edge[1] = pos[2] - pos[0];
					deltaUV[0] = uv[1] - uv[0];
					deltaUV[1] = uv[2] - uv[0];
				}
				else
				{
					edge[0] = pos[0] - pos[2];
					edge[1] = pos[1] - pos[2];
					deltaUV[0] = uv[0] - uv[2];
					deltaUV[1] = uv[1] - uv[2];
				}
				const float f = 1.0f / (deltaUV[0].x*deltaUV[1].y - deltaUV[1].x*deltaUV[0].y);
				tangent[tri].x = f * (deltaUV[1].y * edge[0].x - deltaUV[0].y * edge[1].x);
				tangent[tri].y = f * (deltaUV[1].y * edge[0].y - deltaUV[0].y * edge[1].y);
				tangent[tri].z = f * (deltaUV[1].y * edge[0].z - deltaUV[0].y * edge[1].z);
				tangent[tri] = glm::normalize(tangent[tri]);

				bitangent[tri].x = f * (-deltaUV[1].x * edge[0].x + deltaUV[0].x * edge[1].x);
				bitangent[tri].y = f * (-deltaUV[1].x * edge[0].y + deltaUV[0].x * edge[1].y);
				bitangent[tri].z = f * (-deltaUV[1].x * edge[0].z + deltaUV[0].x * edge[1].z);
				bitangent[tri] = glm::normalize(bitangent[tri]);
			}
			glm::vec3 tempTang = glm::normalize(tangent[0] + tangent[1]);
			glm::vec3 tempBitT = glm::normalize(bitangent[0] + bitangent[1]);
			for (unsigned int i = 0; i < 6; ++i)
			{
				tangents.push_back(tempTang.x);
				tangents.push_back(tempTang.y);
				tangents.push_back(tempTang.z);
				bitangents.push_back(tempBitT.x);
				bitangents.push_back(tempBitT.y);
				bitangents.push_back(tempBitT.z);
			}
		}

		for (unsigned int i = 0; i < 3; ++i)
		{
			if (data[i].empty())
				priority[i] = false;
			else
				priority[i] = true;
		}
		for (unsigned int i = 0, j = 0; i < 3; ++i)
		{
			if (priority[i])
			{
				dObj.vData[j] = data[i].data();
				dObj.dSize[j++] = data[i].size() * sizeof(float);
			}
		}
		dObj.vData[3] = tangents.data();
		dObj.dSize[3] = tangents.size() * sizeof(float);
		dObj.vData[4] = bitangents.data();
		dObj.dSize[4] = bitangents.size() * sizeof(float);
	}
	unsigned int dSizeCalc(unsigned int index)
	{
		unsigned int size = 0;
		for (unsigned int i = 0; i < index; ++i)
		{
			size += dObj.dSize[i];
		}
		return size;
	}
public:
	VertexBuffer(const void* data,size_t size,bool tanNbit)
	{
		GLCall(glGenBuffers(1, &m_RendererID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
	VertexBuffer(const std::string& arrayName,bool tanNbit)
	{
		if (!tanNbit)
			VertexData(arrayName);
		else
			VertexDataTNB(arrayName);
		unsigned int num = int(priority[0]) + int(priority[1]) + int(priority[2]) + int(tanNbit)*2;
		GLCall(glGenBuffers(1, &m_RendererID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, dSizeCalc(5), NULL, GL_STATIC_DRAW));
		for (unsigned int i = 0; i < (3 + tanNbit * 2); ++i)
		{
			GLCall(glBufferSubData(GL_ARRAY_BUFFER, dSizeCalc(i), dObj.dSize[i], dObj.vData[i]));
		}
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	~VertexBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RendererID));
	}
	unsigned int GetWrdNum() const
	{
		return dObj.num_wrds;
	}
	float GetSizeFactor(unsigned int index) const
	{
		return (float)dObj.dSize[index] / dObj.dSize[0];
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void Bind() const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	}
	void Unbind() const 
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
};
