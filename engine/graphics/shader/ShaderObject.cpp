#include "ShaderObject.h"

using namespace engine::graphics;

const std::unique_ptr<CShader>& CShaderObject::getShader()
{
	return pShader;
}