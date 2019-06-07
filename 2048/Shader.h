#pragma once

struct Shader
{
	const char* const VertexShader = 
	R"(		
		#version 430
		precision highp float;	

		in layout(location=0) vec3 InVertex;
		in layout(location=1) vec3 InColor;
		in layout(location=2) vec2 tex;

		// a sorok azok a mátrix oszlopai..
		uniform mat4 M;
		uniform mat4 V;
		uniform mat4 P;


		out vec3 TransferColor;
		out vec2 TexCoords;


		void main()
		{
			gl_Position = M * vec4(InVertex, 1.0f) ;
			TransferColor = InColor;
			TexCoords = tex;
		}
	)";

	// Ha a fragment átmegy a mélységi teszten akkor lesz belõle pixel
	// Mélységi teszt: Z irányban nincsen kisebb koordinátájú fragment az adott x,y-ban.
	const char* const FragmentShader = 
	R"(
		#version 430
		precision highp float;	

		in vec3 TransferColor;
		in vec2 TexCoords;

		out vec4 outColor;

		uniform sampler2D ourTexture;
		uniform sampler2D ourTexture2;

		uniform float f = 0.8;

		void main()
		{
			outColor = mix(texture(ourTexture, TexCoords), texture(ourTexture2, TexCoords), f);

		}
	)";
};
