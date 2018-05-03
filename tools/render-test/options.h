// options.h
#pragma once

#include <stdint.h>

#include "../../source/core/slang-result.h"

#include "render.h"

namespace renderer_test {

struct Options
{
    enum
    {
        // maximum number of command-line arguments to pass along to slang
        kMaxSlangArgs = 16,
    };

    enum class InputLanguageID
    {
        // Slang being used as an HLSL-ish compiler
        Slang,

        // Raw HLSL or GLSL input, bypassing Slang
        Native,

        // Raw HLSL or GLSL input, passed through the Slang rewriter
        NativeRewrite
    };
   

    enum class ShaderProgramType
    {
        Graphics,
        Compute,
        GraphicsCompute
    };

    char const* appName = "render-test";
    char const* sourcePath = nullptr;
    char const* outputPath = nullptr;
	ShaderProgramType shaderType = ShaderProgramType::Graphics;

    RendererType rendererType = RendererType::Unknown;
    InputLanguageID inputLanguageID = InputLanguageID::Slang;

    char const* slangArgs[kMaxSlangArgs];
    int slangArgCount = 0;
};

extern Options gOptions;

extern int gWindowWidth;
extern int gWindowHeight;


SlangResult parseOptions(int* argc, char** argv);

} // renderer_test
