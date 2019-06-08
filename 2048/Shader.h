#pragma once

struct Shader
{
	const char* const VertexShader = 
	R"(		
		#version 430 core
		precision highp float;	

		in layout(location=0) vec4 InVertex;
		in layout(location=1) vec3 InColor;
		in layout(location=2) vec2 tex;

		// MVP-t a CPU-n számoljuk
		uniform mat4 MVP;


		out vec2 TexCoords;


		void main()
		{
			gl_Position = MVP * InVertex;
			TexCoords = tex;
		}
	)";

	// Ha a fragment átmegy a mélységi teszten akkor lesz belõle pixel
	// Mélységi teszt: Z irányban nincsen kisebb koordinátájú fragment az adott x,y-ban.
	const char* const FragmentShader = 
	R"(
		#version 430 core
		precision highp float;	

		in vec2 TexCoords;
		out vec4 outColor;					

		uniform sampler2D smp;

		void main()
		{
			outColor = texture(smp, TexCoords);
		}
	)";
};
